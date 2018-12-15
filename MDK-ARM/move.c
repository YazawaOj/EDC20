#include "move.h"

TIM_HandleTypeDef htimx;
int CH[4]={0,0,0,0};

void stop(void)
{
	PWM_Out(0,0,0,0);
}

uint8_t isBlackLeft1(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6);
}

uint8_t isBlackRight1(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9);
}

uint8_t isBlackLeft2(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7);
}

uint8_t isBlackRight2(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8);
}

void moveInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
  TIM_MasterConfigTypeDef sMasterConfig;
	
	__HAL_RCC_TIM3_CLK_ENABLE();	
	htimx.Instance=TIM3;     
  htimx.Init.Prescaler=72;         
  htimx.Init.CounterMode=TIM_COUNTERMODE_UP;
  htimx.Init.Period=1000;             
  htimx.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	htimx.Init.AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&htimx);     
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htimx, &sMasterConfig);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();	
	GPIO_InitStruct.Pin=GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;  
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.Pin=GPIO_PIN_0|GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void PWM_Out(int ch1,int ch2,int ch3,int ch4)
{
	TIM_OC_InitTypeDef TIM_CHHandler;
	if(ch1==CH[0] && ch2==CH[1] && ch3==CH[2] && ch4==CH[3]) return;
	TIM_CHHandler.OCMode=TIM_OCMODE_PWM1;  
  TIM_CHHandler.OCPolarity=TIM_OCPOLARITY_HIGH;
	TIM_CHHandler.OCFastMode = TIM_OCFAST_DISABLE;
  TIM_CHHandler.Pulse=ch1; 
  HAL_TIM_PWM_ConfigChannel(&htimx,&TIM_CHHandler,TIM_CHANNEL_1);
	TIM_CHHandler.Pulse=ch2; 
	HAL_TIM_PWM_ConfigChannel(&htimx,&TIM_CHHandler,TIM_CHANNEL_2);
	TIM_CHHandler.Pulse=ch3; 
	HAL_TIM_PWM_ConfigChannel(&htimx,&TIM_CHHandler,TIM_CHANNEL_3);
	TIM_CHHandler.Pulse=ch4; 
	HAL_TIM_PWM_ConfigChannel(&htimx,&TIM_CHHandler,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htimx,TIM_CHANNEL_4);
	CH[0]=ch1;CH[1]=ch2;CH[2]=ch3;CH[3]=ch4;
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
 
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  
} 
