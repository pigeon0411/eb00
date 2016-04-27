#include "includes.h"
#include "stdlib.h"
#include    <math.h>

#include "Protocol.h"
#include "Queue.h"
#include "motorconfig.h"
#include "an41908a.h"


#include "bsp_spi_flash.h"

void timer0_int(void);
void timer0_initial(void);
void timer3_init(void);
u8 zf_key_handle(void);

u8 af_action_cnt = 0;
u8 af_action_onepush_cnt = 0;

#define CAMERA_POWER_PIN    GPIO_Pin_2
#define CAMERA_SET_KEY_PIN    GPIO_Pin_3

#define CAMERA_POWER_PORT    GPIOD
#define CAMERA_SET_KEY_PORT   GPIOB

void camera_power_key_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = CAMERA_SET_KEY_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAMERA_SET_KEY_PORT, &GPIOD_InitStructure);

	GPIOD_InitStructure.GPIO_Pin = CAMERA_POWER_PIN;
    GPIO_Init(CAMERA_POWER_PORT, &GPIOD_InitStructure);


	GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
	//GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	



	
	GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	



	PWR_BackupAccessCmd(ENABLE);//�����޸�RTC �ͺ󱸼Ĵ���
	RCC_LSEConfig(RCC_LSE_OFF);//�ر��ⲿ�����ⲿʱ���źŹ��� ��PC13 PC14 PC15 �ſ��Ե���ͨIO�á�
	BKP_TamperPinCmd(DISABLE);//�ر����ּ�⹦�ܣ�Ҳ���� PC13��Ҳ���Ե���ͨIO ʹ��
	PWR_BackupAccessCmd(DISABLE);//��ֹ�޸ĺ󱸼Ĵ���

	//RCC_LSICmd(DISABLE);
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_14);	
	GPIO_SetBits(GPIOC,GPIO_Pin_15);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);


	
	//GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_power_off_on(u8 mode)
{
    if(mode)//on
        GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
    else
        GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_sensor_format_set_with_long_key(void)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);

}

void camera_ntsc_pal_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_14);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_14);
}

void camera_hd_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void camera_cvbs_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_15);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_15);
}



void zoom_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = ZOOM_PIN_A1|ZOOM_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ZOOM_PORT, &GPIOD_InitStructure);	


    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

u8 iii = 2;

void focus_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = FOCUS_PIN_A1|FOCUS_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FOCUS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	


}


void iris_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_PIN_PLUS|IRIS_PIN_SUB;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	


}


void iris_auto_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_AUTO_PIN_A1|IRIS_AUTO_PIN_A2|IRIS_AUTO_PIN_B1|IRIS_AUTO_PIN_B2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_AUTO_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A2);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B2);	

//        md127_keepmode();
}



//���þ�ͷ�����������������ѹ
void motor_voltage_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = VOLTAGE_VL3|VOLTAGE_VL2|VOLTAGE_VL1;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOB,VOLTAGE_VL3);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL2);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL1);	
}

void camera_power_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_2);	

	

}

void camera_power_on_off(u8 mode)
{
    if(mode)
        GPIO_SetBits(GPIOC,GPIO_Pin_2);	
    else
        GPIO_ResetBits(GPIOC,GPIO_Pin_2);	   
}

void iris_ex_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_5);	

	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);	
}


void iris_ex_pin_set(u8 mode)
{
	if(mode > 1)
		return;

	    system_para.system_para.para_iris_ex_mode = mode;

	switch(mode)
	{
	case 1:
		GPIO_SetBits(GPIOC,GPIO_Pin_5);	
		GPIO_SetBits(GPIOB,GPIO_Pin_0);	
		break;
	case 0:
		GPIO_ResetBits(GPIOC,GPIO_Pin_5);	
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
		break;

	}

}

void extern_function_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    system_para.system_para.para_ex_io_1_mode = 1;   

    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
    
	extern_io2_output(0); 
	extern_io3_output(0); 

}

void key_long_osd_emu(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_8);	
    GPIO_SetBits(GPIOB,GPIO_Pin_9);	
    delay_X1ms(4000);   
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}

void key_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = KEY_LEFT_PIN|KEY_RIGHT_PIN|KEY_UP_PIN|KEY_DOWN_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIOD_InitStructure);	
}


// recieve datas from keyboard
void uart0_init(void)
{
// UCSR0B = 0x00; //disable while setting baud rate
// UCSR0A = 0x00;
// UCSR0C = 0x06;
//-----------�ȹرմ��ڹ���
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART1, DISABLE);


	BSP_USART1_Init();

	switch(Baud_rate)
	{
    case DOME_BAUDRATE_1200: 
				APP_USART1_Baudrate(1200);
			   break;    
	case DOME_BAUDRATE_2400: 
				APP_USART1_Baudrate(2400);
			   break;
	case DOME_BAUDRATE_4800: 
				APP_USART1_Baudrate(4800);
			   break;
	case DOME_BAUDRATE_9600: 
				APP_USART1_Baudrate(9600);
			   break;
	case DOME_BAUDRATE_14400:
		APP_USART1_Baudrate(14400);
	       break;
	case DOME_BAUDRATE_19200:
		APP_USART1_Baudrate(19200);
	       break;
	case DOME_BAUDRATE_115200:
		APP_USART1_Baudrate(115200);
	       break;	   
	default:   
		APP_USART1_Baudrate(2400);
	    break;
	}

	/* Enable USART1 receive intterrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  	/* Enable USART1 */
  	USART_Cmd(USART1, ENABLE);
	
	/* set the control pin at receive status */
	//GPIO_ResetBits(GPIOA, RS485Control);

	RS485_RX_ENABLE;
	//RS485_TX_ENABLE;

}

void serial_int1_send(void)	   //send data to USAR1		   
{
    if (cam_counter_send<cam_cmd_lenth)
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send++; 
    }
    else
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send=1;
    	txd1_buff_cFlag=1;
    	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
	if (cam_counter_send>=200)
		cam_counter_send=1; 
}

u8 cam_cmd_recieve_len = 0;
u8 cam_recieve_cnt_public=0;
void serial_int1_receive(uchar udr1)//receive data from USAR1
{
     
    rxd1_buffer[counter_receive] = udr1;

    if(counter_receive == 3)
    {
        cam_recieve_cnt_public=0;
        if(rxd1_buffer[2]==0x20)
        {
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;
        }
        else if(rxd1_buffer[2]==0x28)
        {
            cam_cmd_recieve_len = 7;
        }
        else
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;

		if(rxd1_buffer[2] == 0x10)
			cam_cmd_recieve_len--;
    }
    
   	counter_receive++;

   
    if (counter_receive >= cam_cmd_recieve_len && counter_receive>3)
   //if (counter_receive >= 6 && counter_receive>3)
   {
        cam_recieve_cnt_public = cam_cmd_recieve_len;
    	rxd1_buff_cFlag=1;
    	counter_receive=0;
        cam_cmd_recieve_len=0;
    }

    if (counter_receive >= 200)
    	counter_receive = 0;
}

// control the camera
void uart_to_camera_init(void)
{
    BSP_USART2_Init();
    APP_USART2_Baudrate(19200);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);// Enable USART2 receive intterrupt 
    USART_Cmd(USART2, ENABLE); // Enable USART2

}

//������Ƭ����
void cam_filt_cutter_int(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	


    GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

}

void cam_filt_cutter_set(u8 mode)
{
    if(mode)
    {// fog
        GPIO_SetBits(GPIOB,GPIO_Pin_13);	

    }
    else
    {// ir

        GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    }

}

void ports_initial(void)
{
	BSP_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��

	camera_power_key_pin_Init();

    //DC Motor driver ic: BA6208
	zoom_pin_Init();
	focus_pin_Init();
    iris_pin_Init();
    
	extern_function_pin_Init();
    //camera_power_pin_Init();
    motor_voltage_pin_Init();
    //iris_auto_pin_Init();
    
    
	timer0_initial();

	iris_ex_pin_Init();
	
//����UART4 PC10,PC11����ʱ���ΰ������� 
	//key_init();


	
    uart_to_camera_init();

	timer3_init();
	timer1_init();


	SPI_FLASH_Init();
//spi_flash_init();
    an41908a_spi_pin_init();
	
}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;


	#ifdef  VECT_TAB_RAM  
  		/* Set the Vector Table base location at 0x20000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH */
  		/* Set the Vector Table base location at 0x08000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  	
		
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI15_10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  



  /* Enable the TIM1 UP Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line17 Interrupt */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

//////////////////////////////////////////////////////////////////
#if 1//CAMERA_HITACHI_SC110

//return: 1,success;0,overtime
u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff)
{
	u32 overtime=0;
	u16 i;
	RS485_TX_ENABLE;
	for(i=0;i<bytes_size;i++)
	{
		overtime=0;
		USART1->DR = send_buff[i];
		while((USART1->SR&0x40)==0)
		{
			if(overtime>UART_SEND_OVERTIME)
				return 0;
			overtime++;
		}
	}
    RS485_RX_ENABLE;
	return 1;
}

#endif


void SerialPutString(unsigned char *Sent_value,int lenghtbuf)
{
    int k ;
//	RS485TX_Enable();
	 for (k=0;k<lenghtbuf;k++)   // ������λ��
	     {
	       USART_SendData(USART1, *(Sent_value+k));
		   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		  // delay_half_ms(12);
		 }
//      delay_half_ms(20);
//	 RS485TX_DisEnable() ;
   
}

void timer0_initial(void)
{
// TCCR0 = 0x00; //stop timer0
// TCNT0 = 0x83; //set count
// TIMSK |= 0x01; //timer interrupt sources
// TCCR0 = 0x06; //start timer0
	BSP_Timer2_Init();
}


//////////////////////////////////////////////////////////////
//TIMER3 initialisation - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 128uSec
// actual value: 128.000uSec (0.0%)
void timer3_init(void)
{	 
 /*TCCR3B = 0x00; //stop
 TCNT3H = 0xFF; //setup
 TCNT3L = 0xa0;
 TCCR3A = 0x00;          //���ﲻ������ʱ��
// TCCR3B = 0x02;*/
	BSP_Timer5_Init();
}

u16 key_check_period_cnt = 0;
u32 timer_cnt_x2ms	= 0;

void timer0_int(void)////2 ms timer
{
	if(key_check_period_cnt>0)
		key_check_period_cnt--;

    if(af_action_cnt < 0xff)
        af_action_cnt++;

    if(af_action_onepush_cnt < 0xff)
        af_action_onepush_cnt++;

	if(timer_cnt_x2ms < 0xffffffff)
		timer_cnt_x2ms++;
}

void read_protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{
	       FLASH_Unlock();//������FALSH���ܻᵼ���޷��������ö�����
	       FLASH_ReadOutProtection(ENABLE);
	}
}



u8 key_get(u16 *data)
{
	u16 temp = 0;
	u8 ret = 0;
	temp = (GPIO_ReadInputData(KEY_PORT)>>4&0x000f);
	if(temp)
	{
		delayms(3);
		if(temp == (GPIO_ReadInputData(KEY_PORT)>>4)&0x000f)
		{
			*data = temp;
			ret = 1;
		}
	}
	return ret;
}

u16 KeyPre = 0;//  �ϴΰ�����״̬
u16 KeyCurrent = 0;//  ���ΰ�����״̬
u16 KeyStbLast = 0;// ���˲�����ϴΰ�����״̬
u16 KeyStb = 0;//  ���˲���ı��ΰ�����״̬
u16 KeyAvlUp = 0;// ����̧����Ч����Ĵ���
u16 KeyAvlDown = 0;//  ����������Ч����Ĵ���

#define	KEY_DOWN_STATE		0

u16 key_val_table[] = {KEY_LEFT_PIN,KEY_RIGHT_PIN,KEY_UP_PIN,KEY_DOWN_PIN,KEY_SET_PIN};


// ������÷���ÿ��10MS ���� 20MS ����һ��������򼴿�
#if 1
u8 CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;
	u8 i =0;
		
	//if(!key_check_period_cnt)	
	if(1)
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //�����ϴΰ���״̬
		KeyCurrent = KeyStatu ;   //���浱ǰ����״̬

		delayms(10);

		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;	

		KeyCurrent = val ;
		

		key_check_period_cnt = 8;
		return 1;
	}

	return 0;
}


#else
void CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;

	if(!key_check_period_cnt)	
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //�����ϴΰ���״̬
		KeyCurrent = KeyStatu ;   //���浱ǰ����״̬
			
		KTmp1 = (KeyCurrent | KeyPre) ; //00 -> 0 ȡ�˲�ֵ0
		KTmp2 = (KeyCurrent & KeyPre) ; //11 -> 1 ȡ�˲�ֵ1
		
		KeyStbLast = KeyStb ;	//�����ϴξ��˲����ֵ	
		KeyStb &= KTmp1 ; //��ֵ��ǰ���˲����ֵ
		KeyStb |= KTmp2 ;

		//ȥ����ĵ�ǰ����״̬ 1��Ч
		KeyAvlUp = (KeyStb ^ KeyStbLast) & KeyStbLast; //�����ذ��±�־ ��ȡ1��0��״̬	
		KeyAvlDown = (KeyStb ^ KeyStbLast) & KeyStb ;	//�½��ذ��±�־ ��ȡ0��1��״̬		

		key_check_period_cnt = 8;
	}
	
}

#endif






#define		KEY_DEFAULT		0X1F00


u8 key_check(u16 key)
{

}

u16 key_pre = 0;



#define	KEY_PORT1		GPIOA
#define	KEY_PORT2		GPIOB
#define	KEY_PORT3		GPIOC


enum key_type
{
	KEY_NONE,
	SLPLUS,	
	SLSUB,	
	SRPLUS,	
	SRSUB,	
	BPPLUS,	
	BPSUB,	
	EN_P0,	
};

#define	key_to_long(val)	(val|0x9000)
#define	key_to_release(val)	(val|0x8000)


u32 key_merge(void)
{
	u32 data = 0,data2 = 0,data3 = 0;

	u32 key_tmp;
	
	//PA,PB,PC
	//data = GPIO_ReadInputData(KEY_PORT1);
	data2 = GPIO_ReadInputData(KEY_PORT2);	
	data3 = GPIO_ReadInputData(KEY_PORT3);	
		
	key_tmp = (data2>>14)&0x0003;//0-1
	key_tmp |= (data2>>1)&0x0004;//2//
	key_tmp |= (data2>>1)&0x0008;// 3
	key_tmp |= (data3>>1)&0x0010;// 4
	key_tmp |= (data2<<3)&0x0020;// 5
	key_tmp |= (data2<<7)&0x0040;// 6


	return key_tmp;
}


//����0Ϊ�ް��������ط�0ֵ����Ϊ��Ӧ�İ�����
static u16 key_ctl_check(void)
{
	u16 i;
	u32 key_tmp;
	static u32 long_press_cnt = 0;// 50ms
	
	key_tmp = key_merge();
	for(i=0;i<20;i++)
	{
		if(((key_tmp>>i)&0x0001)==0)
		{
			delay_X1ms(20);

			key_tmp = key_merge();

			if(((key_tmp>>i)&0x0001)==0)
			{
				if(key_pre == i+1)
				{
					if(long_press_cnt>40)
					{

						long_press_cnt=0;
						key_pre = 0;
						return ((i+1)|0x9000);
					}

					long_press_cnt++;
				}
				key_pre = i+1;
				//return (i+1);
				break;
			}
		}
	}


	if((key_pre && key_pre!=(i+1))||(key_pre && i==20))
	{
		i = key_pre|0x8000;
		key_pre = 0;
		return i;

	}
	return 0;
}




#define	IRIS_OPEN_DIR		0
#define	IRIS_CLOSE_DIR		1

#define	IRIS_STEPS_MAX		IRIS_LENS_STEPS_MAX

s16 iris_step_cnt = 0;//����CLOSE��Ϊ0����OPEN���ʱΪ���ֵ35

#define	 AN41908_STEPS_ONE_TIME (system_expand_para.system_para.para_stepsmotor_pulse_per_step)


//dir,1,left; 2,right; 0,stop 
//��ߵ��
void left_motor_run(u8 dir)
{
	if(dir == 1)
	{
		LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
	else if(dir == 2)
	{
		LenDrvZoomMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
	else if(dir == 0)
	{
		//LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
			
}


//dir,1,left; 2,right; 0,stop 
//��ߵ��
void right_motor_run(u8 dir)
{
	if(dir == 1)
	{
		LensDrvFocusMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
	else if(dir == 2)
	{
		LensDrvFocusMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
	else if(dir == 0)
	{
		//LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		delay_X1ms(80);

	}
			
}




void key_analyze(u16 val)
{

	switch(val)
	{
	case key_to_release(SLPLUS):
		left_motor_run(1);
		break;
	case key_to_release(SLSUB):
		left_motor_run(2);
		break;

	case key_to_release(SRPLUS):
		right_motor_run(1);
		break;
	case key_to_release(SRSUB):
		right_motor_run(2);
		break;

	case key_to_release(BPPLUS):


		break;
	case key_to_release(BPSUB):

			break;
			
	case key_to_release(EN_P0):

		break;
		
	default:
		break;
	}


}

u8 key_monitor(void)
{
	u16 key_tmp;
	
	key_tmp = key_ctl_check();
	if(key_tmp)
	{
		key_analyze(key_tmp);
		
	}
}




//u8 ttest[2000];
//double myd = 0.00078125;
u32 ttlen,ttlens,FlashID;
//#define VECT_TAB_RAM
extern void delay1Ms(int ms);


int main(void)
{	
    u16 k;


	ttlen = sizeof(system_para.system_para);
	
	InterruptConfig();
	ports_initial();
    delay_X1ms(600);


	while(1)
	{
			
        key_monitor();
		delay_X1ms(10);
		
	}
}

