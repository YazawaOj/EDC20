#ifndef MOVE_H
#define MOVE_H
#include "stm32f1xx_hal.h"
#include "mpu6050.h"

#define LFAST 900
#define LMIDFAST 700
#define LMID 500
#define LMIDSLOW 300
#define LSLOW 100

#define RFAST 900
#define RMIDFAST 700
#define RMID 500
#define RMIDSLOW 300
#define RSLOW 100
//PWM参数，可能因为电池电量改变

extern TIM_HandleTypeDef htimx;

void forward(int speed);
void stop(void);
uint8_t isBlackLeft1(void);
uint8_t isBlackRight1(void);
uint8_t isBlackLeft2(void);
uint8_t isBlackRight2(void);
void moveInit(void);
void PWM_Out(int ch1,int ch2,int ch3,int ch4);

#endif
