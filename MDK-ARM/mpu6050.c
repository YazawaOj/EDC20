#include "mpu6050.h"

GPIO_InitTypeDef GPIO_InitStruct;
volatile double anglecom=0;//求得的角度
//float Angle=0;
//float GyroZ;

void I2CInit(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void SDA_Out(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//I2C_SCL_SET;
	//I2C_SDA_SET;
}

void SDA_In(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MPU6050Init(void)
{
	I2CInit();
	HAL_Delay(100);
	MPU6050SingleWrite(0x6b,0x00);
	HAL_Delay(5);
	MPU6050SingleWrite(0x6b,0x00);
	HAL_Delay(5);
	MPU6050SingleWrite(0x19,0x07);
	HAL_Delay(5);
	MPU6050SingleWrite(0x1a,0x06);
	HAL_Delay(5);
	MPU6050SingleWrite(0x1c,0x01);
	HAL_Delay(5);
	MPU6050SingleWrite(0x1b,0x18);
}

int16_t MPU6050GetValue(uint8_t addr)
{
	int16_t res,H,L;
	H=MPU6050SingleRead(addr);
	L=MPU6050SingleRead(addr+1);
	res=(H<<8)+L;
	return res;
}

void delay(int t)
{
	t=t*70;
	for(;t>0;t--);
}

void I2C_Start(void)
{
	SDA_Out();
	I2C_SDA_SET;
	delay(1);
	I2C_SCL_SET;
	delay(1);
	I2C_SDA_CLR;
	delay(1);
	I2C_SCL_CLR;
	delay(1);
}

void I2C_Stop(void)
{
	SDA_Out();
	I2C_SDA_CLR;
	delay(1);
	I2C_SCL_SET;
	delay(1);
	I2C_SDA_SET;
	delay(1);
}

void I2C_SingleWrite(uint8_t data)
{
	uint8_t i;
	SDA_Out();
	for(i=0;i<8;i++)
	{
		delay(1);
		if(data & 0x80) I2C_SDA_SET;
		else I2C_SDA_CLR;
		delay(1);
		I2C_SCL_SET;
		delay(1);
		I2C_SCL_CLR;
		data<<=1;
	}
	I2C_SDA_SET;
	while(I2C_WaitAck());
}

uint8_t I2C_SingleRead(uint8_t ack)
{
	uint8_t i;
	uint8_t data=0;
	I2C_SDA_SET;
	SDA_In();
	for(i=0;i<8;i++)
	{
		delay(1);
		I2C_SCL_SET;
		delay(1);
		data=data<<1;
		data |= I2C_SDA_IN;
		I2C_SCL_CLR;
	}
	if(ack) I2C_Ack();
	else I2C_NAck();
	return data;
}

uint8_t I2C_WaitAck(void)
{
	uint8_t status=0;
	//I2C_SDA_SET;
	SDA_In();
	delay(1);
	I2C_SCL_SET;
	delay(1);
	if(I2C_SDA_IN)
	{
		status=1;
		I2C_Stop();
	}
	else
	{
		status=0;
	}
	I2C_SCL_CLR;
	delay(1);
	return status;
}

void I2C_Ack(void)
{
	SDA_Out();
	I2C_SDA_CLR;
  delay(1);
  I2C_SCL_SET;
  delay(1);
  I2C_SCL_CLR;
  delay(1);
  I2C_SDA_SET;
}

void I2C_NAck(void)
{
	//I2C_SDA_SET;
	SDA_Out();
  delay(1);
  I2C_SCL_SET;
  delay(1);
  I2C_SCL_CLR;
  delay(1);
	I2C_SDA_SET;
}

void MPU6050SingleWrite(uint8_t addr,uint8_t data)
{
	I2C_Start();
	I2C_SingleWrite(MPU_ADDR);
	I2C_SingleWrite(addr);
	I2C_SingleWrite(data);
	I2C_Stop();
}

uint8_t MPU6050SingleRead(uint8_t addr)
{
	uint8_t data;
	I2C_Start();
	I2C_SingleWrite(MPU_ADDR);
	I2C_SingleWrite(addr);
	I2C_Start();
	I2C_SingleWrite(MPU_ADDR|1);
	data=I2C_SingleRead(0);
	I2C_Stop();
	return data;
}

float getAngle(void)
{
	int32_t value;
	value=MPU6050GetValue(GYRO_ZOUT_H)-offsetGZ;
	if(value<=5 && value>=-5) value=0;
	anglecom += value;
	if(anglecom<0) anglecom += ROUND;
	if(anglecom>ROUND) anglecom -= ROUND;
	return anglecom*360/ROUND;
	//return anglecom;
	//return value;
}
