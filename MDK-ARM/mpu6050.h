#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f1xx_hal.h"
#include <math.h>

#define MPU_ADDR 0xD0
#define SCLPort GPIOB
#define SDAPort GPIOB
#define SCLPin GPIO_PIN_6
#define SDAPin GPIO_PIN_7

#define ACCEL_XOUT_H 0x3B 
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define TEMP_OUT_H  0x41
#define TEMP_OUT_L  0x42

#define GYRO_XOUT_H  0x43 
#define GYRO_XOUT_L  0x44 
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48

#define offsetAX 848
#define offsetAY 0
#define offsetAZ 0
#define offsetGX -66
#define offsetGY 0
#define offsetGZ 19
#define ROUND 576667

#define I2C_SCL_SET HAL_GPIO_WritePin(SCLPort,SCLPin,GPIO_PIN_SET)
#define I2C_SCL_CLR HAL_GPIO_WritePin(SCLPort,SCLPin,GPIO_PIN_RESET)
#define I2C_SDA_SET HAL_GPIO_WritePin(SDAPort,SDAPin,GPIO_PIN_SET)
#define I2C_SDA_CLR HAL_GPIO_WritePin(SDAPort,SDAPin,GPIO_PIN_RESET)
#define I2C_SDA_IN HAL_GPIO_ReadPin(SDAPort,SDAPin)

void I2CInit(void);
void MPU6050Init(void);
int16_t MPU6050GetValue(uint8_t addr);

void delay(int t);// ±–Ú—” ±
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_SingleRead(uint8_t ack);
void I2C_SingleWrite(uint8_t data);
uint8_t I2C_WaitAck(void);
void I2C_Ack(void);
void I2C_NAck(void);
void MPU6050SingleWrite(uint8_t addr,uint8_t data);
uint8_t MPU6050SingleRead(uint8_t addr);

float getAngle(void);
#endif
