#include "main.h"
#include "stm32f1xx_hal.h"
#include "move.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define STOP 0
#define GUIDE 1
#define RIGHTSEEK 2
#define LEFTSEEK 3
#define TURNRATIO 0.5
//״̬0����ͣ��ֹͣ 1��Ŀ�굼�� 2����ѭ�� 3����ѭ�� 
//1��2��3״̬�ĺ�������Ϊ����Ŀ�����꣬������ʱ�ص���ѭ����ȡ��������Ҫ�Ĳ���:ת��ֱ��һС�Ρ���һ״̬

#define VALUE 7//��ֵ
#define OFFX 0
#define OFFY 0

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int GetValidData = 0;
volatile uint8_t messageReceive[72];
volatile uint8_t whole_message[72];
uint8_t huanchong[1];
char debug[5];
volatile int message_index=0;

volatile int myX=0;
volatile int myY=0;
int targetX,targetY;
volatile int myX_last=0;
volatile int myY_last=0;
int oppoX=0,oppoY=0;
int mystate=0,mystate_last=0,mystate_next;
uint8_t mynum;//Ϊ0ʱB����1ʱA��

uint8_t gamestate=0;
extern int PointX[44];
extern int PointY[44];
int passengersize=0;
int passengerX[5];
int passengerY[5];
int passengerXTo[5];
int passengerYTo[5];
uint8_t passengerstate[5];//�˿�״̬
int mypassenger=0;//���ҳ��ϻ�����׼��ȥ�ӵĳ˿ͱ��
int oppopassenger=-1;//�ڶԷ����ϵĳ˿ͱ�ţ�-1��ʾ�Է�û�ӿ�

volatile int status=0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
void bluetoothInit(UART_HandleTypeDef*huart);
void WifiInit(UART_HandleTypeDef*huart);
void SerialPrintChar(char* output, UART_HandleTypeDef*huart);
int checkBeginning(void);

void getMessage(void);
void chooseTarget(void);//ÿ����һ��״̬�յ����һ��targetX,targetY
double vecDiff(int x1,int y1,int x2,int y2);
double vecLen(int x,int y);
float angDiff(double a,double b);
uint8_t onMe(int i);//i�˿��Ƿ����ҳ���
int getNearPassenger(void);//����õ���������ĳ˿�
void shortForward(int t);//ֱ��һС��
void targetGuide(int x,int y,int delay);//Ŀ�굼����delay���յ���Ϣ�������
void rightSeek(int x,int y,int delay);//��ѭ��
void leftSeek(int x,int y,int delay);//��ѭ��
void leftTurn(int ang);
void rightTurn(int ang);//ת��
void DEBUG(int x)
{
  sprintf(debug,"%d ",x);
	SerialPrintChar(debug,&huart2);
}

void DEBUGS(char s[])
{
	SerialPrintChar(s,&huart2);
}

extern void initEdge(void);
extern void initOperation(void);
extern int getNextStep(int from, int current, int X, int Y);
extern int getOperation(int from, int current, int target);
void operationDecoder(int value,int t){
    int turnDeg, operation;
    operation = value % 10;
    turnDeg = value / 10 - 180;
		DEBUGS("A=");DEBUG(turnDeg);DEBUGS("OP=");DEBUG(operation);
		DEBUGS("ST=");DEBUG(mystate_last);DEBUG(mystate);DEBUG(t);
		DEBUGS("TAR=");DEBUG(targetX);DEBUG(targetY);DEBUGS("\n");
    //printf("%d %d\n", turnDeg, operation);
		if(turnDeg>0) rightTurn(turnDeg*TURNRATIO);
		if(turnDeg<0) leftTurn(-turnDeg*TURNRATIO);
		switch(operation)
		{
			case 0:
				targetGuide(PointX[t],PointY[t],0);
				break;
			case 1:
				targetGuide(PointX[t],PointY[t],20);
				break;
			case 2:
				targetGuide(PointX[t],PointY[t],10000);
				break;
			case 3:
				leftSeek(PointX[t],PointY[t],0);
				break;
			case 4:
				rightSeek(PointX[t],PointY[t],0);
				break;
			default:
				break;
		}
}

uint8_t onMe(int i)
{
	if(mynum)
	{
		if(passengerstate[i]==2) return 1;
		return 0;
	}
	else
	{
		if(passengerstate[i]==1) return 1;
		return 0;
	}
}

int getNearPassenger(void)
{
	int i=0,mindis=1000,min,dis;
	for(i=0;i<passengersize;i++)
	{
		if(passengerstate[i] != 0) continue;//�����˿Ͳ��ǿ��У���һ���ڶԷ����ϣ����ɽ�
		dis=vecLen(myX-passengerX[i],myY-passengerY[i]);
		if(dis<mindis) {min=i;mindis=dis;}
	}
	return min;
}

void chooseTarget(void)
{
	int i;
	for(i=0;i<passengersize;i++)
	{
		if(onMe(i))
		{
			targetX=passengerXTo[i];
			targetY=passengerYTo[i];
			return;
		}//����г˿����ҵĳ��ϣ���ɽ�������
	}
	i=getNearPassenger();
	targetX=passengerX[i];targetY=passengerY[i];//����ѡ��һ������˿�
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
	moveInit();
	MPU6050Init();
	initEdge();
	initOperation();
	WifiInit(&huart1);HAL_UART_Receive_IT(&huart1, huanchong, 1);
  /* Infinite loop */
	SerialPrintChar("loop",&huart2);
	//HAL_Delay(1000);
	//rightSeek(1000,1000,0);
	while(gamestate!=1)
	{
		if(GetValidData==1)
		{
			GetValidData=0;
			getMessage();
			chooseTarget();
		}
	}
	while(1)
	{
		chooseTarget();
		mystate_next=getNextStep(mystate_last,mystate,targetX,targetY);
		//DEBUG(mystate_next);
		while(mystate_next==-1)
		{
			if(GetValidData==1)
			{
				GetValidData=0;
				getMessage();
				chooseTarget();
				mystate_next=getNextStep(mystate_last,mystate,targetX,targetY);
			}
		}
		operationDecoder(getOperation(mystate_last,mystate,mystate_next),mystate_next);
		mystate_last=mystate;
		mystate=mystate_next;
		/*I2C_Start();
		I2C_SingleWrite(0xD0);
		debug[0]=I2C_WaitAck();
		I2C_Stop();
		debug[1]=MPU6050SingleRead(0x19);
		SerialPrintChar(debug,&huart2);
		HAL_Delay(500);*/
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, WIFI1_Pin|WIFI2_Pin|WIFI3_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = WIFI1_Pin|WIFI2_Pin|WIFI3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	mynum=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14);
	mystate=mynum;mystate_last=mynum;
}

/* USER CODE BEGIN 4 */
//����һ�����жϣ�ÿ�ӵ�һ�ֽ�����ִ��һ�Ρ����Ż���
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		whole_message[message_index]= huanchong[0];
		message_index++;		
		if(checkBeginning() == 1) //���ֿ�ͷ�ļ����ֽ�����
		{
			message_index = 0;
		}
		if(message_index >= 2 //���ֽ�β�����ֽ�����
			&& message_index < 64
			&& whole_message[message_index-2]==0x0D
			&& whole_message[message_index-1]==0x0A)
		{
			message_index = 0;
		}		
		if(message_index >= 64)
		{			
			message_index = 0;
			if(whole_message[62]==0x0D
				&& whole_message[63]==0x0A)
			{
				GetValidData = 1; //��ʾ�ӵ�һ����Ч��Ϣ
				for(int i = 0;i < 64;i++) //����Ч��Ϣ���Ƶ�messageReceive����Ż�
				{
					messageReceive[i] = whole_message[i];
				}
			}
		}
		HAL_UART_Receive_IT(&huart1, huanchong, 1);
	}
}

int checkBeginning(void)
{
	if(message_index >= 8
		&& whole_message[message_index - 8] == 0x2B
		&& whole_message[message_index - 7] == 0x49
		&& whole_message[message_index - 6] == 0x50
		&& whole_message[message_index - 5] == 0x44
		&& whole_message[message_index - 4] == 0x2C
		/*&& whole_message[message_index - 3] == 0x36
		&& whole_message[message_index - 2] == 0x34
		&& whole_message[message_index - 1] == 0x3A*/)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void SerialPrintChar(char* output, UART_HandleTypeDef*huart)
{
	unsigned int lenth = strlen(output)+1;
	HAL_UART_Transmit(huart, (uint8_t*)output, lenth, 0xFFFF);
}

//���Ż�
void WifiInit(UART_HandleTypeDef*huart)
{
	SerialPrintChar("AT\r\n", huart);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, WIFI1_Pin|WIFI2_Pin|WIFI3_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	SerialPrintChar("AT\r\n", huart);
	HAL_Delay(1000);
	//SerialPrintChar("start.\n", &huart2);
	SerialPrintChar("AT+CWMODE=1\r\n", huart);
	HAL_Delay(1000);
	//SerialPrintChar("reseting...\n", &huart2);
	SerialPrintChar("AT+RST\r\n", huart);
	HAL_Delay(1000);
	//SerialPrintChar("connecting...\n", &huart2);
	SerialPrintChar("AT+CWJAP=\"EDC20\",\"12345678\"\r\n", huart);
	HAL_Delay(3000);
	SerialPrintChar("AT+CIPSTART=\"TCP\",\"192.168.1.124\",20000\r\n", huart);
	HAL_Delay(2000);
	SerialPrintChar("AT+CIPSTART=\"TCP\",\"192.168.1.124\",20000\r\n", huart);
	HAL_Delay(2000);
	//SerialPrintChar("wifiinit", &huart2);
}

void getMessage(void)
{
	int high,low,i;
	gamestate=messageReceive[0]>>6;
	//if(gamestate!=0x01) stop();
	if(!mynum){
		high=(messageReceive[2]>>5) & 0x01;
		low=messageReceive[7];
		myX_last=myX;
		myX=(high<<8) | low;
		high=(messageReceive[2]>>4) & 0x01;
		low=messageReceive[8];
		myY_last=myY;
		myY=(high<<8) | low;//B
		
		high=(messageReceive[2]>>7) & 0x01;
		low=messageReceive[5];
		oppoX=(high<<8) | low;
		high=(messageReceive[2]>>6) & 0x01;
		low=messageReceive[6];
		oppoY=(high<<8) | low;//A
	}
	else{
		high=(messageReceive[2]>>7) & 0x01;
		low=messageReceive[5];
		myX_last=myX;
		myX=(high<<8) | low;
		high=(messageReceive[2]>>6) & 0x01;
		low=messageReceive[6];
		myY_last=myY;
		myY=(high<<8) | low;//A
		
		high=(messageReceive[2]>>5) & 0x01;
		low=messageReceive[7];
		oppoX=(high<<8) | low;
		high=(messageReceive[2]>>4) & 0x01;
		low=messageReceive[8];
		oppoY=(high<<8) | low;//B
	}
	myX-=OFFX;myY-=OFFY;
	oppoX-=OFFX;oppoY-=OFFY;//��ȡ����λ��
	passengersize=messageReceive[9]>>2;
	passengerstate[0]=messageReceive[9] & 0x03;
	passengerstate[1]=(messageReceive[10]>>6) & 0x03;
	passengerstate[2]=(messageReceive[10]>>4) & 0x03;
	passengerstate[3]=(messageReceive[10]>>2) & 0x03;
	passengerstate[4]=messageReceive[10] & 0x03;
	for(i=0;i<5;i++)
	{
		passengerX[i]=messageReceive[4*i+11];
		passengerY[i]=messageReceive[4*i+12];
		passengerXTo[i]=messageReceive[4*i+13];
		passengerYTo[i]=messageReceive[4*i+14];
	}
}

double vecDiff(int x1,int y1,int x2,int y2)
{
	double sin = x1 * y2 - y1 * x2;
	double cos = x1 * x2 + y1 * y2;
	return 180*atan2(sin, cos)/3.14;
}

double vecLen(int x,int y)
{
	return sqrt(x*x+y*y);
}

float angDiff(double a,double b)
{
	double tmp;
	tmp = a - b;
	if (tmp < -180) tmp += 360;
	if (tmp > 180) tmp -= 360;
	if (tmp < 0) return -tmp;
	return tmp;
}

void shortForward(int t)
{
	PWM_Out(RMIDFAST,0,LMIDFAST,0);
	HAL_Delay(t);
	return;
}

void leftTurn(int ang)
{
	float angle_last=getAngle();
	PWM_Out(RFAST,0,0,LFAST);
	while(angDiff(angle_last,getAngle())<ang);
	PWM_Out(0,RFAST,LFAST,0);delay(300);
	stop();
}

void rightTurn(int ang)
{
	float angle_last=getAngle();
	PWM_Out(0,RFAST,LFAST,0);
	while(angDiff(angle_last,getAngle())<ang);
	PWM_Out(RFAST,0,0,LFAST);delay(300);
	stop();
}

void targetGuide(int x,int y,int delay)
{
	float diff;int times=0;int i=0;
	while(vecLen(myX-x,myY-y)>VALUE)
	{
		if(i==0)
		{
			DEBUGS("my: ");DEBUG(myX);DEBUG(myY);i=100;
		}
		else i--;
		diff=vecDiff(x-myX_last,y-myY_last,myX-myX_last,myY-myY_last);
		if(diff<-3) PWM_Out(RMIDFAST,0,LMIDSLOW,0);
		else if(diff>3) PWM_Out(RMIDSLOW,0,LMIDFAST,0);
		else PWM_Out(RMIDFAST,0,LMIDFAST,0);
		//��diff̫�󣬿���ת��
		if(times>=delay)
		{
			if(isBlackLeft1() || isBlackLeft2()) PWM_Out(RSLOW,0,LFAST,0);
			else if(isBlackRight1() || isBlackRight2()) PWM_Out(RFAST,0,LSLOW,0);
			else PWM_Out(RMIDFAST,0,LMIDFAST,0);
		}
		if(GetValidData == 1)
		{
			GetValidData = 0;
			getMessage();
			times++;
		}
	}
	stop();
}

void rightSeek(int x,int y,int delay)
{
	int times=0;int i=0;
	while(vecLen(myX-x,myY-y)>VALUE)
	{
		if(i==0)
		{
			DEBUGS("my: ");DEBUG(myX);DEBUG(myY);i=100;
		}
		else i--;
		if(times>delay)
		{
			if(isBlackRight1() || isBlackRight2()) PWM_Out(RFAST,0,LSLOW,0);
			else PWM_Out(0,RSLOW,LFAST,0);
		}
		else
		{
			if(isBlackRight1() || isBlackRight2()) PWM_Out(RFAST,0,LSLOW,0);
			else PWM_Out(RMIDSLOW,0,LMIDFAST,0);
			times++;
		}
		if(GetValidData == 1)
		{
			GetValidData = 0;
			getMessage();
		}
	}
	stop();
}

void leftSeek(int x,int y,int delay)
{
	int times=0;
	while(vecLen(myX-x,myY-y)>VALUE)
	{
		if(times>delay)
		{
			if(isBlackLeft1() || isBlackLeft2()) PWM_Out(RSLOW,0,LFAST,0);
			else PWM_Out(RFAST,0,LMIDSLOW,0);
		}
		else
		{
			if(isBlackLeft1() || isBlackLeft2()) PWM_Out(RSLOW,0,LFAST,0);
			else PWM_Out(RMIDFAST,0,LMIDSLOW,0);
			times++;
		}
		if(GetValidData == 1)
		{
			GetValidData = 0;
			getMessage();
		}
	}
	stop();
}

/* USER CODE END 4 */

void _Error_Handler(char *file, int line)
{

  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */
