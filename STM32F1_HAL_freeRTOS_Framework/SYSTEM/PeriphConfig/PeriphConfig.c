#include "PeriphConfig.h"

/*
______________________________【PIN MAP】__________________________________________
注：外设如串口、PWM等的IO初始化在其初始化函数内，不用单独再初始化
		[IO]											[描述]
外设：	* PA8										MCO输出，默认时钟源为HSE
		* CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1		TIM3默认PWM口
		  CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1		TIM3部分重映射PWM口
		  CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9		TIM3完全重映射PWM口
		* TX/PA9, RX/PA10 - TX/PB6, RX/PB7			USART1默认引脚和重映射引脚
		* TX/PA2, RX/PA3 - TX/PD5, RX/PD6			USART2默认引脚和重映射引脚
		* TX/PB10, RX/PB11 - TX/PD8,  RX/PD9		USART3默认引脚和重映射引脚
		* 											SPI1默认引脚和重映射引脚
		*...

用户：	*
*/

/*___________________________器件IO配置___________________________________________*/

/*STEP1:去.h文件里的"定义都有什么器件"里面写都有哪些器件*/

/*STEP2:定义一共有多少个器件*/
#define devicesNum	2

/*STEP3:定义每个器件所用到的IO和其配置*/
/*参数说明：
	PIN：	GPIO_PIN_0~GPIO_PIN_15，GPIO_PIN_All
	MODE：	GPIO_MODE_INPUT、GPIO_MODE_ANALOG、GPIO_MODE_AF_INPUT							输入
			GPIO_MODE_OUTPUT_PP、GPIO_MODE_OUTPUT_OD、GPIO_MODE_AF_PP、GPIO_MODE_AF_OD		输出
			GPIO_MODE_IT_RISING、GPIO_MODE_IT_FALLING、GPIO_MODE_IT_RISING_FALLING  		选用IT表示启用EXTI，最多16线EXTI，如PA0和PB0共用EXTI0
	上下拉：GPIO_NOPULL、GPIO_PULLUP、GPIO_PULLDOWN
	翻转速度：GPIO_SPEED_FREQ_LOW、GPIO_SPEED_FREQ_MEDIUM、GPIO_SPEED_FREQ_HIGH
	GPIOx：GPIOA~GPIOE
	默认状态:GPIO_PIN_SET（1u）或GPIO_PIN_RESET（0u）
	EXTI优先级：0~15
	启否EXTI：TRUE或者FALSE
*/
GPIO_Init_Struct LCD_IO_Struct[] = 
{	/*	PIN				MODE			  上下拉		翻转速度		  	GPIOx 	  默认状态     EXTI优先级	启否EXTI*/
	{{GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH},	GPIOA,  GPIO_PIN_SET,		2,		  TRUE},
	{{GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH},	GPIOB,  GPIO_PIN_SET,		2,		  TRUE}
};

GPIO_Init_Struct BUCK_IO_Struct[] =
{
	{{GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH},	GPIOA,  GPIO_PIN_SET,		2,		  TRUE},
	{{GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH},	GPIOB,  GPIO_PIN_SET,		2,		  TRUE}
};

/*STEP4:按照器件的数量，按照"定义都有什么器件"里面的顺序！完善每个器件的信息*/
Devices_Init_Struct UserDevices[devicesNum] = 
{
	{	
		.deviceIO_Struct 	= 	LCD_IO_Struct	,		//器件IO配置结构体
		.deviceIndex 		= 	LCD				,		//器件enum格式索引
		.deviceName 		= 	"LCD device"	,		//器件名称
		.device_IOnum 		= 	8						//器件有多少个IO口
	},
	{
		.deviceIO_Struct 	= 	BUCK_IO_Struct	,
		.deviceIndex		= 	BUCK			,
		.deviceName 		= 	"BUCK device"	,
		.device_IOnum 		= 	2
	}
};


/*___________________________器件IO配置函数___________________________________________*/
void Devices_Init(Devices_Init_Struct* Devices , enum devicesIndex_enum device2Init)
{
	u8 dIndex;
	if(device2Init == ALL)
	{
		for(dIndex = 0;dIndex < devicesNum;dIndex++)	//遍历所有器件
		{
			deviceIO_Init(Devices,(enum devicesIndex_enum)dIndex);
		}
	}else{
		switch(device2Init)								//特定器件初始化，按需增添		
		{
			case LCD:	deviceIO_Init(Devices,LCD);
				break;
			case BUCK:	deviceIO_Init(Devices,BUCK);
			default:break;
		}
	}
}

void deviceIO_Init(Devices_Init_Struct* Devices , enum devicesIndex_enum device2Init)
{
	u8 dIndex,iIndex;
	dIndex = (u8)device2Init;
	for(iIndex = 0;iIndex < Devices[dIndex].device_IOnum;iIndex++)	//遍历某一个器件的所有IO
	{
		//时钟
		if(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx == GPIOA)
			__HAL_RCC_GPIOA_CLK_ENABLE();
		else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx == GPIOB)
			__HAL_RCC_GPIOB_CLK_ENABLE();
		else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx == GPIOC)
			__HAL_RCC_GPIOC_CLK_ENABLE();
		else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx == GPIOD)
			__HAL_RCC_GPIOD_CLK_ENABLE();
		else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx == GPIOE)
			__HAL_RCC_GPIOE_CLK_ENABLE();
		
		//填入IO配置
		HAL_GPIO_Init(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx, \
			&Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure);
		
		//如果使用EXTI，配置中断线和优先级
		if( (Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Mode == GPIO_MODE_IT_RISING) \
			||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Mode == GPIO_MODE_IT_FALLING) \
			||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Mode == GPIO_MODE_IT_RISING_FALLING) )
		{		
			if(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_0)
			{	
				HAL_NVIC_SetPriority(EXTI0_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI0_IRQn); else HAL_NVIC_DisableIRQ(EXTI0_IRQn);
			}else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_1)
			{
				HAL_NVIC_SetPriority(EXTI1_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI1_IRQn); else HAL_NVIC_DisableIRQ(EXTI1_IRQn);
			}else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_2)
			{
				HAL_NVIC_SetPriority(EXTI2_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI2_IRQn); else HAL_NVIC_DisableIRQ(EXTI2_IRQn);
			}else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_3)
			{
				HAL_NVIC_SetPriority(EXTI3_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI3_IRQn); else HAL_NVIC_DisableIRQ(EXTI3_IRQn);
			}else if(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_4)
			{
				HAL_NVIC_SetPriority(EXTI4_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI4_IRQn); else HAL_NVIC_DisableIRQ(EXTI4_IRQn);
			}else if((Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_5) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_6) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_7) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_8) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_9) )
			{
				HAL_NVIC_SetPriority(EXTI9_5_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); else HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			}else if((Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_10) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_11) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_12) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_13) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_14) \
					||(Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin == GPIO_PIN_15) )
			{
				HAL_NVIC_SetPriority(EXTI15_10_IRQn, Devices[dIndex].deviceIO_Struct[iIndex].PreemptPriority, 0);
				if(Devices[dIndex].deviceIO_Struct[iIndex].isEnableExit)  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); else HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			}
		}
		
		//配置默认IO状态
		HAL_GPIO_WritePin(Devices[dIndex].deviceIO_Struct[iIndex].GPIOx, \
			Devices[dIndex].deviceIO_Struct[iIndex].GPIO_Initure.Pin, \
			Devices[dIndex].deviceIO_Struct[iIndex].defaultState);
	}
}


/*中断服务函数*/
void EXTI0_IRQHandler(void){ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);}
void EXTI1_IRQHandler(void){ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);}
void EXTI2_IRQHandler(void){ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);}
void EXTI3_IRQHandler(void){ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);}
void EXTI4_IRQHandler(void){ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);}
void EXTI9_5_IRQHandler(void)		//需要自己判断的！
{ 
//	if(PAin(5))		//提前配置PA5为外部中断时
//		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}
void EXTI15_10_IRQHandler(void)		//需要自己判断的！
{ 
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}


/*外部中断回调函数*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            break;
		case GPIO_PIN_1:
			break;
        case GPIO_PIN_2:
            break;
        case GPIO_PIN_3:
            break;
        case GPIO_PIN_4:
            break;
    }
}

/*________________________________________用户定时器2配置_________________________________________________________*/

#if STSTEM_TIM2_ENABLE

TIM_HandleTypeDef TIM2_Handler;

#if (!STSTEM_TIM2_asPWMorCap)
	TIM_OC_InitTypeDef 	TIM2_CH1Handler,TIM2_CH2Handler,TIM2_CH3Handler,TIM2_CH4Handler;
#else
	TIM_IC_InitTypeDef TIM2_CHxConfig;
#endif

void sys_TIM2_ENABLE(void)
{
	TIM2_Handler.Instance = 		TIM2;                          	//通用定时器2
    TIM2_Handler.Init.Prescaler=	(72-1);                    		//分频系数
    TIM2_Handler.Init.CounterMode=	TIM_COUNTERMODE_UP;    			//向上计数器
    TIM2_Handler.Init.Period=		tim2arr;                       	//自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;			//时钟分频因子
    
	#if STSTEM_TIM2_TI_ENABLE
		HAL_TIM_Base_Init(&TIM2_Handler);		//这个不影响PWM和Cap功能吧，不知道
		HAL_TIM_Base_Start_IT(&TIM2_Handler);
	#endif
	
	
	#if (STSTEM_TIM2_asPWMorCap == 0)
		/*作为PWM*/
		HAL_TIM_PWM_Init(&TIM2_Handler);       //初始化PWM
		if(STSTEM_TIM2PWM_CHANNEL_ENABLE & B0000_0001)
		{
			TIM2_CH1Handler.OCMode=TIM_OCMODE_PWM1; 		//模式选择PWM1
			TIM2_CH1Handler.Pulse=tim2arr/2;            	//设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
			TIM2_CH1Handler.OCPolarity=TIM_OCPOLARITY_LOW; 	//输出比较极性为低 
			HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH1Handler,TIM_CHANNEL_1);//配置TIM2通道1
			HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_1);	//开启PWM通道1
		}
		if(STSTEM_TIM2PWM_CHANNEL_ENABLE & B0000_0010)
		{
			TIM2_CH2Handler.OCMode=TIM_OCMODE_PWM1;
			TIM2_CH2Handler.Pulse=tim2arr/2;
			TIM2_CH2Handler.OCPolarity=TIM_OCPOLARITY_LOW;
			HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH2Handler,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_2);
		}
		if(STSTEM_TIM2PWM_CHANNEL_ENABLE & B0000_0100)
		{
			TIM2_CH3Handler.OCMode=TIM_OCMODE_PWM1;
			TIM2_CH3Handler.Pulse=tim2arr/2;
			TIM2_CH3Handler.OCPolarity=TIM_OCPOLARITY_LOW;
			HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH3Handler,TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_3);
		}
		if(STSTEM_TIM2PWM_CHANNEL_ENABLE & B0000_1000)
		{
			TIM2_CH4Handler.OCMode=TIM_OCMODE_PWM1;
			TIM2_CH4Handler.Pulse=tim2arr/2;
			TIM2_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW;
			HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH4Handler,TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_4);
		}
	#elif (STSTEM_TIM2_asPWMorCap == 1)
		/*作为Cap*/
		TIM2_Handler.Init.Period=0XFFFF;       //自动装载值
		HAL_TIM_IC_Init(&TIM2_Handler);			//初始化输入捕获时基参数
		
		if(STSTEM_TIM2_Cap_trigV == 1)
			TIM2_CHxConfig.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
		else if(STSTEM_TIM2_Cap_trigV == 0)
			TIM2_CHxConfig.ICPolarity=TIM_ICPOLARITY_FALLING;    //下降沿捕获
		else if(STSTEM_TIM2_Cap_trigV == 2)
			TIM2_CHxConfig.ICPolarity=TIM_ICPOLARITY_BOTHEDGE;    //双边沿捕获
		
		TIM2_CHxConfig.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
		TIM2_CHxConfig.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
		TIM2_CHxConfig.ICFilter=0;                          //配置输入滤波器，不滤波
		
		switch(STSTEM_TIM2_Cap_Channel)
		{
			case B0000_0001:				//选用通带1作为输入捕获
				HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CHxConfig,TIM_CHANNEL_1);//配置TIM2通道1
				HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);   //开启TIM2的捕获通道1，并且开启捕获中断
				break;
			case B0000_0010:				//选用通带2作为输入捕获
				HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CHxConfig,TIM_CHANNEL_2);
				HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_2);
				break;
			case B0000_0100:				//选用通带3作为输入捕获
				HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CHxConfig,TIM_CHANNEL_3);
				HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_3);
				break;
			case B0000_1000:				//选用通带4作为输入捕获
				HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CHxConfig,TIM_CHANNEL_4);
				HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_4);
				break;
			default:break;
		}
		__HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);   //使能更新中断
	#elif (STSTEM_TIM2_asPWMorCap == 2)
		
	#endif
}

#if (STSTEM_TIM2_asPWMorCap == 1)				//如果使用Cap功能

//定时器2的IC功能底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM2时钟
		__HAL_AFIO_REMAP_TIM2_ENABLE();			/*TIM2通道引脚完全重映射使能 (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)*/
		
		if(STSTEM_TIM2_Cap_trigV == 1)				//上升沿捕获
			GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
		else if(STSTEM_TIM2_Cap_trigV == 0)			//下降沿捕获
			GPIO_Initure.Pull=GPIO_PULLUP;        	//上拉
		else if(STSTEM_TIM2_Cap_trigV == 2)			//双边沿捕获
			GPIO_Initure.Pull=GPIO_NOPULL;        	//无拉
		
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 	//复用输入
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		
		switch(STSTEM_TIM2_Cap_Channel)
		{
			case B0000_0001:					//选用通带1作为输入捕获
				__HAL_RCC_GPIOA_CLK_ENABLE();	//开启GPIOA时钟
				GPIO_Initure.Pin=GPIO_PIN_15;            //PA15
				HAL_GPIO_Init(GPIOA,&GPIO_Initure);
				break;
			case B0000_0010:					//选用通带2作为输入捕获
				__HAL_RCC_GPIOB_CLK_ENABLE();
				GPIO_Initure.Pin=GPIO_PIN_3;            //PB3
				HAL_GPIO_Init(GPIOB,&GPIO_Initure);
				break;
			case B0000_0100:					//选用通带3作为输入捕获
				__HAL_RCC_GPIOB_CLK_ENABLE();
				GPIO_Initure.Pin=GPIO_PIN_10;            //PB10
				HAL_GPIO_Init(GPIOB,&GPIO_Initure);
				break;
			case B0000_1000:					//选用通带4作为输入捕获
				__HAL_RCC_GPIOB_CLK_ENABLE();
				GPIO_Initure.Pin=GPIO_PIN_11;            //PB11
				HAL_GPIO_Init(GPIOB,&GPIO_Initure);
				break;
			default:break;
		}
		
		HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //设置中断优先级，抢占优先级3，子优先级0
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启ITM2中断通道  
	}
}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
u8  TIM2CHx_CAPTURE_STA=0;							//输入捕获状态		    				
u16	TIM2CHx_CAPTURE_VAL;							//输入捕获值(TIM5是16位)

/*获取最近一次TIM2输入捕获的脉冲时间，单位 毫秒，按照设置的捕获沿进行，可以随时随地调用此函数*/
float Peek_TIM2_Cap_Val(void)
{
	u8 Tim2overFlowTimes = 0;
	float temp = 0;
	if(TIM2CHx_CAPTURE_STA&0X80)        			//成功捕获到了一次高电平
	{
		Tim2overFlowTimes = TIM2CHx_CAPTURE_STA & 0X3F;
		temp = (float)Tim2overFlowTimes;
		temp *= 65536.0;		 	    			//溢出时间总和
		temp += (float)TIM2CHx_CAPTURE_VAL;      	//加上当前计数值得到总的高电平时间
		TIM2CHx_CAPTURE_STA = 0;          			//开启下一次捕获
		temp /= 1000.0;								//单位 毫秒
		return temp;
	}else{
		return 0.0;
	}
}

//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if(htim==(&TIM2_Handler))
	{
			switch(STSTEM_TIM2_Cap_Channel)
			{
				case B0000_0001: //通道1
					Process_TIM2_IC_CallBack_Channel_1();
					break;
				case B0000_0010: //通带2
					Process_TIM2_IC_CallBack_Channel_2();
					break;
				case B0000_0100: //通道3
					Process_TIM2_IC_CallBack_Channel_3();
					break;
				case B0000_1000: //通带4
					Process_TIM2_IC_CallBack_Channel_4();
					break;
				default:break;
			}
	}	
}

void Process_TIM2_IC_CallBack_Channel_1(void)
{
	if((STSTEM_TIM2_Cap_trigV == 1)||(STSTEM_TIM2_Cap_trigV == 0))				//上升沿或者下降沿捕获
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)				//还未成功捕获
		{
			if(TIM2CHx_CAPTURE_STA&0X40)				//捕获到一个下降沿 		
			{	  			
				TIM2CHx_CAPTURE_STA|=0X80;				//标记成功捕获到一次高电平脉宽
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);//获取当前的捕获值.
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
				if(STSTEM_TIM2_Cap_trigV == 1)	//如果配置为上升沿捕获
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//配置TIM2通道1上升沿捕获
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//定时器2通道1设置为下降沿捕获
				}
				
			}else  										//还未开始,第一次捕获上升沿
			{
				TIM2CHx_CAPTURE_STA=0;					//清空
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;				//标记捕获到了上升沿
				__HAL_TIM_DISABLE(&TIM2_Handler);      	//关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
				
				if(STSTEM_TIM2_Cap_trigV == 1)	//如果配置为上升沿捕获
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//定时器2通道1设置为下降沿捕获
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//定时器2通道1设置为上升沿捕获
				}
				
				__HAL_TIM_ENABLE(&TIM2_Handler);		//使能定时器2
			}		    
		}
	}else if(STSTEM_TIM2_Cap_trigV == 2)				//双沿捕获
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)				//还未成功捕获
		{
			if(TIM2CHx_CAPTURE_STA&0X40)				//已经捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA|=0X80;				//标记成功捕获到一次脉宽
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);//获取当前的捕获值
			}else										//还未捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA=0;					//清空
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;				//标记捕获到了一个沿
				__HAL_TIM_DISABLE(&TIM2_Handler);      	//关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				__HAL_TIM_ENABLE(&TIM2_Handler);		//使能定时器2
			}
		}
	}
}

void Process_TIM2_IC_CallBack_Channel_2(void)
{
	if((STSTEM_TIM2_Cap_trigV == 1)||(STSTEM_TIM2_Cap_trigV == 0))
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)
		{
			if(TIM2CHx_CAPTURE_STA&0X40)	
			{	  			
				TIM2CHx_CAPTURE_STA|=0X80;	
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_2);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2); 
				if(STSTEM_TIM2_Cap_trigV == 1)
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);
				}
				
			}else 
			{
				TIM2CHx_CAPTURE_STA=0;
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;
				__HAL_TIM_DISABLE(&TIM2_Handler);
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2); 
				
				if(STSTEM_TIM2_Cap_trigV == 1)	
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);
				}
				
				__HAL_TIM_ENABLE(&TIM2_Handler);
			}		    
		}
	}else if(STSTEM_TIM2_Cap_trigV == 2)				//双沿捕获
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)				//还未成功捕获
		{
			if(TIM2CHx_CAPTURE_STA&0X40)				//已经捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA|=0X80;				//标记成功捕获到一次脉宽
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_2);//获取当前的捕获值
			}else										//还未捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA=0;					//清空
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;				//标记捕获到了一个沿
				__HAL_TIM_DISABLE(&TIM2_Handler);      	//关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				__HAL_TIM_ENABLE(&TIM2_Handler);		//使能定时器2
			}
		}
	}
}

void Process_TIM2_IC_CallBack_Channel_3(void)
{
	if((STSTEM_TIM2_Cap_trigV == 1)||(STSTEM_TIM2_Cap_trigV == 0))
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)
		{
			if(TIM2CHx_CAPTURE_STA&0X40)	
			{	  			
				TIM2CHx_CAPTURE_STA|=0X80;	
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_3);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3); 
				if(STSTEM_TIM2_Cap_trigV == 1)
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);
				}
				
			}else 
			{
				TIM2CHx_CAPTURE_STA=0;
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;
				__HAL_TIM_DISABLE(&TIM2_Handler);
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3); 
				
				if(STSTEM_TIM2_Cap_trigV == 1)	
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);
				}
				
				__HAL_TIM_ENABLE(&TIM2_Handler);
			}		    
		}
	}else if(STSTEM_TIM2_Cap_trigV == 2)				//双沿捕获
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)				//还未成功捕获
		{
			if(TIM2CHx_CAPTURE_STA&0X40)				//已经捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA|=0X80;				//标记成功捕获到一次脉宽
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_3);//获取当前的捕获值
			}else										//还未捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA=0;					//清空
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;				//标记捕获到了一个沿
				__HAL_TIM_DISABLE(&TIM2_Handler);      	//关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				__HAL_TIM_ENABLE(&TIM2_Handler);		//使能定时器2
			}
		}
	}
}
void Process_TIM2_IC_CallBack_Channel_4(void)
{
	if((STSTEM_TIM2_Cap_trigV == 1)||(STSTEM_TIM2_Cap_trigV == 0))
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)
		{
			if(TIM2CHx_CAPTURE_STA&0X40)	
			{	  			
				TIM2CHx_CAPTURE_STA|=0X80;	
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_4);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4); 
				if(STSTEM_TIM2_Cap_trigV == 1)
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);
				}
				
			}else 
			{
				TIM2CHx_CAPTURE_STA=0;
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;
				__HAL_TIM_DISABLE(&TIM2_Handler);
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4); 
				
				if(STSTEM_TIM2_Cap_trigV == 1)	
				{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);
				}else{
					TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);
				}
				
				__HAL_TIM_ENABLE(&TIM2_Handler);
			}		    
		}
	}else if(STSTEM_TIM2_Cap_trigV == 2)				//双沿捕获
	{
		if((TIM2CHx_CAPTURE_STA&0X80)==0)				//还未成功捕获
		{
			if(TIM2CHx_CAPTURE_STA&0X40)				//已经捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA|=0X80;				//标记成功捕获到一次脉宽
				TIM2CHx_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_4);//获取当前的捕获值
			}else										//还未捕获到一个沿
			{
				TIM2CHx_CAPTURE_STA=0;					//清空
				TIM2CHx_CAPTURE_VAL=0;
				TIM2CHx_CAPTURE_STA|=0X40;				//标记捕获到了一个沿
				__HAL_TIM_DISABLE(&TIM2_Handler);      	//关闭定时器2
				__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
				__HAL_TIM_ENABLE(&TIM2_Handler);		//使能定时器2
			}
		}
	}
}


#endif

//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_Handler);
}

#if (!STSTEM_TIM2_asPWMorCap)
	//设置TIM2通道的占空比
	//percent:占空比百分数
	void TIM2_set_Channel_Pulse(u8 channel,float percent)
	{
		float compare;
		if(percent < 0) percent = 0;
		if(percent > 100) percent = 100.0;
		percent /= 100.0;
		compare = (float)tim2arr * percent;
		switch(channel)
		{
			case TIM2PWM_Channel_1: TIM2->CCR1=(u32)compare;break;
			case TIM2PWM_Channel_2: TIM2->CCR2=(u32)compare;break;
			case TIM2PWM_Channel_3: TIM2->CCR3=(u32)compare;break;
			case TIM2PWM_Channel_4: TIM2->CCR4=(u32)compare;break;
			default:break;
		}
	}
#endif

#endif


/*________________________________________用户ADC1配置_________________________________________________________*/
#if SYSTEM_ADC1_ENABLE

ADC_HandleTypeDef ADC1_Handler;		//ADC句柄

void sys_ADC1_ENABLE(void)
{
	u8 i;
	i = 1;
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC外设时钟
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			//分频因子6时钟为72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//设置ADC时钟
	
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
	
	if(!SYSTEM_ADC1_useScan)
		ADC1_Handler.Init.ScanConvMode=DISABLE;                  //非扫描模式
	else ADC1_Handler.Init.ScanConvMode=ENABLE;					 //扫描模式
	
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;                //连续转换，开启则是，触发一次转换本组所有通道，否则触发一次只转换一次
    
	if(SYSTEM_ADC1_useScan)
		ADC1_Handler.Init.NbrOfConversion=SYSTEM_ADC1_useChanlNum;   	//n个转换在规则序列中
	else ADC1_Handler.Init.NbrOfConversion=1;							//只转换规则序列1
	
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
	ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;  	 //软件触发
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler);					 //校准ADC
	
	if(SYSTEM_ADC1_useScan)		//如果启用扫描，则把所有通道都加入到规则组里
	{
		if(SYSTEM_ADC1_useChanl & B0in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_0, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B1in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_1, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B2in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_2, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B3in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_3, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B4in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_4, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B5in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_5, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B6in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_6, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B7in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_7, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B8in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_8, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B9in16)	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_9, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B10in16)	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_10, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B11in16)	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_11, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B12in16)	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_12, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B13in16)	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_13, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B14in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_14, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		if(SYSTEM_ADC1_useChanl & B15in16) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_15, (uint32_t)(i++), ADC_SAMPLETIME_55CYCLES_5);
		
		if(SYSTEM_ADC1_useChanl & InrTemp) 	ADC_RegularChannelConfig(&ADC1_Handler, ADC_CHANNEL_TEMPSENSOR, (uint32_t)(i++), ADC_SAMPLETIME_239CYCLES_5);
	}
	
	if(SYSTEM_ADC1_useDMA1)
	{
		ADC_DMA_Cfg();
		HAL_ADC_Start_DMA(&ADC1_Handler, (uint32_t*)&adValue,12);	//开始DMA，最后一个参数为数据长度
	}
}

void ADC_RegularChannelConfig(ADC_HandleTypeDef *AdcHandle, uint32_t Channel, uint32_t Rank, uint32_t SamplingTime)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;      
    ADC1_ChanConf.Channel      = Channel;                      		//通道
    ADC1_ChanConf.Rank         = Rank;                          	//第几个序列
    ADC1_ChanConf.SamplingTime = SamplingTime;                  	//采样时间 
    HAL_ADC_ConfigChannel(AdcHandle,&ADC1_ChanConf);              	//通道配置
}

//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
/*	
	通道：	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	   16		     17
	IO	：	A0	A1	A2	A3	A4	A5	A6	A7	B0	B1	C0	C1	C2	C3	C4	C5	内部温度	内部参考电压
*/
	if(hadc->Instance == ADC1)
	{
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
		GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
		
		if(SYSTEM_ADC1_useChanl & B0in16) {GPIO_Initure.Pin=GPIO_PIN_0; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B1in16) {GPIO_Initure.Pin=GPIO_PIN_1; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B2in16) {GPIO_Initure.Pin=GPIO_PIN_2; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B3in16) {GPIO_Initure.Pin=GPIO_PIN_3; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B4in16) {GPIO_Initure.Pin=GPIO_PIN_4; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B5in16) {GPIO_Initure.Pin=GPIO_PIN_5; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B6in16) {GPIO_Initure.Pin=GPIO_PIN_6; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B7in16) {GPIO_Initure.Pin=GPIO_PIN_7; HAL_GPIO_Init(GPIOA,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B8in16) {GPIO_Initure.Pin=GPIO_PIN_0; HAL_GPIO_Init(GPIOB,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B9in16) {GPIO_Initure.Pin=GPIO_PIN_1; HAL_GPIO_Init(GPIOB,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B10in16) {GPIO_Initure.Pin=GPIO_PIN_0; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B11in16) {GPIO_Initure.Pin=GPIO_PIN_1; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B12in16) {GPIO_Initure.Pin=GPIO_PIN_2; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B13in16) {GPIO_Initure.Pin=GPIO_PIN_3; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B14in16) {GPIO_Initure.Pin=GPIO_PIN_4; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
		if(SYSTEM_ADC1_useChanl & B15in16) {GPIO_Initure.Pin=GPIO_PIN_5; HAL_GPIO_Init(GPIOC,&GPIO_Initure);}
	}
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
u16 Get_Adc(u32 ch)   
{
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	if (!SYSTEM_ADC1_useScan)
	{	
		ADC1_ChanConf.Channel=ch;                                   //通道
		ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
		ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //采样时间               
		HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置
	}
	
    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
    HAL_ADC_PollForConversion(&ADC1_Handler,2);                 //轮询转换
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        	//返回最近一次ADC1规则组的转换结果
}

void Get_Adc_Average(u32 ch,u8 times,u32* result)
{
	u32 temp_val[SYSTEM_ADC1_useChanlNum]= {0};
	u8 t,i;
	
	if (SYSTEM_ADC1_useScan)
	{
		for(t=0;t < times;t++)
		{
			for(i = 0;i < SYSTEM_ADC1_useChanlNum; i++)
			{
				temp_val[i] += Get_Adc(ch);
				delay_ms(1);
			}
		}
		
		for(i = 0;i < SYSTEM_ADC1_useChanlNum; i++)
		{
			result[i] = temp_val[i]/times;
		}
	}else{
		for(t=0;t < times;t++)
		{
			temp_val[0] += Get_Adc(ch);
			delay_ms(1);
		}
		result[0] = temp_val[0]/times;
	}
}

/*把AD采集温度通道的原始值，转化为温度值*/
float Get_Temprate(u32 adcx)
{
 	float temperate;
	temperate = ((float)adcx)*(3.3/4096.0);				//电压值
	temperate = (1.43-temperate)/0.0043 + 25.0;				//转换为温度值 	 
	return temperate;
}

#if SYSTEM_ADC1_useDMA1
DMA_HandleTypeDef  ADC1rxDMA_Handler; //DMA句柄

//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
void ADC_DMA_Cfg(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    ADC1rxDMA_Handler.Instance = DMA1_Channel1;                                 //DMA通道选择
    ADC1rxDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;                     //外设到存储器
    ADC1rxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                        //外设非增量模式
    ADC1rxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                             //存储器增量模式
    ADC1rxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;        //外设数据长度:16位
    ADC1rxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;          //存储器数据长度:16位
    ADC1rxDMA_Handler.Init.Mode=DMA_CIRCULAR;                                 //外设循环模式
    ADC1rxDMA_Handler.Init.Priority=DMA_PRIORITY_HIGH;                       //优先级

    ADC1rxDMA_Handler.XferCpltCallback = HAL_DMA_IRQHandler;
    
	HAL_DMA_DeInit(&ADC1rxDMA_Handler);                                      //DMA复位
    HAL_DMA_Init(&ADC1rxDMA_Handler);                                        //DMA初始化 
        
	__HAL_LINKDMA(&ADC1_Handler,DMA_Handle,ADC1rxDMA_Handler);               //将DMA与ADC联系起来(发送DMA)
        
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 0);                          //DMA中断优先级
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);                                  //使能DMA中断
        
}

// DMA通道1中断服务函数 （完成传送时的中断）
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&ADC1rxDMA_Handler);
	//printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",advalue[0],advalue[1],advalue[2],advalue[3],advalue[4],advalue[5],advalue[6],advalue[7],advalue[8],advalue[9],advalue[10],advalue[11]);        
	adValueDone = 1;
}

#endif


#endif

