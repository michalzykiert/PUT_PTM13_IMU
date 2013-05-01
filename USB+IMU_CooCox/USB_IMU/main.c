
#include <stdio.h>
#include "stm32f4_discovery.h"
#include "usbd_cdc_vcp.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#define LSM303DL_A_ADDRESS 0x30  // adres akcelerometru
#define LSM303DL_M_ADDRESS 0x3C  // adres magnetometru
#define LSM303DL_G_ADDRESS 0xD2  // adres zyroskopu
#define I2C_Speed 200000
#define I2C1_SLAVE_ADDRESS7 0x30


/* Private macro */
/* Private variables */
int xla, xha, yla, yha, zla, zha, ax, ay, az;
int xhm, xlm, yhm, ylm, zhm, zlm, mx, my, mz;
int xhg, xlg, yhg, ylg, zhg, zlg, gx, gy, gz;
int j;

uint32_t button_sts;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
uint32_t i = 0;
/* Private function prototypes */
/* Private functions */

void init_I2C1(void);
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_stop(I2C_TypeDef* I2Cx);
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
void Delay(__IO uint32_t nCount);
void I2C_slave_start(I2C_TypeDef* I2Cx);
void I2C_slave_stop(I2C_TypeDef* I2Cx);
uint8_t I2C_slave_read_ack(I2C_TypeDef* I2Cx);
uint8_t I2C_slave_read_nack(I2C_TypeDef* I2Cx);
void read_acc();
void read_mag();
void read_gyro();



/**
**===========================================================================
**  Main
**===========================================================================
*/

int main(void)
{

	SystemInit();

	STM32F4_Discovery_LEDInit(LED3); //Orange
	STM32F4_Discovery_LEDInit(LED4); //Green
	STM32F4_Discovery_LEDInit(LED5); //Red
	STM32F4_Discovery_LEDInit(LED6); //Blue

	STM32F4_Discovery_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_CDC_cb,&USR_cb);
	SystemCoreClockUpdate(); // inicjalizacja dystrybucji czasu procesora
		init_I2C1(); // na podstawie: http://eliaselectronics.com/stm32f4-tutorials/stm32f4-i2c-mastertutorial/

		//acc
			I2C_start(I2C1, LSM303DL_A_ADDRESS, I2C_Direction_Transmitter);
			I2C_write(I2C1,0x20);   // LSM303_CTRL_REG1_A  0x20
			I2C_write(I2C1,0x27);   // Enable Accelerometer
			  	  	  	  	  	  	// 0x27 = 0b00100111
									// Normal power mode, all axes enabled
			I2C_stop(I2C1); // stop the transmission
		//acc
		//mag
				I2C_start(I2C1, LSM303DL_M_ADDRESS, I2C_Direction_Transmitter);
				I2C_write(I2C1,0x02);	  //LSM303_MR_REG_M   0x02
				I2C_write(I2C1,0x00);     // Enable Magnetometer
										  // 0x00 = 0b00000000
				  	  	  	  	  	  	  // Continuous conversion mode
				I2C_stop(I2C1);
		//mag
		//gyro
				I2C_start(I2C1, LSM303DL_G_ADDRESS, I2C_Direction_Transmitter);
				I2C_write(I2C1, 0x20);   //L3G_CTRL_REG1 0x20
				I2C_write(I2C1, 0x0F);   // 0x0F = 0b00001111
					  	  	  	  	     // Normal power mode, all axes enabled
				I2C_stop(I2C1);
		//gyro
	char start='0';
	while(1)
	{
					Delay(5);
					read_acc();
					read_mag();
					read_gyro();
					start='0';

					while(1)
					{
						start = usb_cdc_getc();
						if(start=='1')
						{
							break;
						}
					}

	}
	/*while (1){

		if(usb_cdc_kbhit()){
			char c, buffer_out[15];
			c = usb_cdc_getc();
			switch(c){
				case '3':
					STM32F4_Discovery_LEDToggle(LED3);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,LED3_PIN));
					usb_cdc_printf(buffer_out);
					break;
				case '4':
					STM32F4_Discovery_LEDToggle(LED4);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,LED4_PIN));
					usb_cdc_printf(buffer_out);
					break;
				case '5':
					STM32F4_Discovery_LEDToggle(LED5);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,LED5_PIN));
					usb_cdc_printf(buffer_out);
					break;
				case '6':
					STM32F4_Discovery_LEDToggle(LED6);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,LED6_PIN));
					usb_cdc_printf(buffer_out);
					break;
			}
		}

		button_sts = STM32F4_Discovery_PBGetState(BUTTON_USER);

		if(button_sts){
			STM32F4_Discovery_LEDOff(LED3);
			STM32F4_Discovery_LEDOff(LED5);
			STM32F4_Discovery_LEDOff(LED3);
			STM32F4_Discovery_LEDOff(LED5);
		}
	}*/
}
void Delay(__IO uint32_t nCount)
	{
		while(nCount--)
		{
}
}
void init_I2C1(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB6 and SDA on PB7
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD; // set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct); // init GPIOB
	// Connect I2C1 pins to AF
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA
	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed = 100000; // 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; // I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7; // own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable; // disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct); // init I2C1
	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);
}
/* This function issues a start condition and
* transmits the slave address + R/W bit
*
* Parameters:
* I2Cx --> the I2C peripheral e.g. I2C1
* address --> the 7 bit slave address
* direction --> the tranmission direction can be:
* I2C_Direction_Tranmitter for Master transmitter mode
* I2C_Direction_Receiver for Master receiver
*/
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);
	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}
/* This function transmits one byte to the slave device
* Parameters:
* I2Cx --> the I2C peripheral e.g. I2C1
* data --> the data byte to be transmitted
*/


void I2C_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
/* This function reads one byte from the slave device
* and acknowledges the byte (requests another byte)
*/
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}
/* This function reads one byte from the slave device
* and doesn't acknowledge the recieved data
*/
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	// disabe acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}
/* This funtion issues a stop condition and therefore
* releases the bus
*/
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);

	//while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)); //spoko to ja dopisalem
}
void I2C_slave_start(I2C_TypeDef* I2Cx){
	/* Test on I2C1 EV1 and clear it */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED));
}
/* This funtion issues a stop condition and therefore
* releases the bus
*/
void I2C_slave_stop(I2C_TypeDef* I2Cx){
	/* Test on I2C2 EV4 and clear it */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_STOP_DETECTED));
	/* Clear I2C2 STOPF flag: read operation to I2C_SR1 followed by a
	write operation to I2C_CR1 */
	(void)(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF));
	I2C_Cmd(I2Cx, ENABLE);
}
/* This function reads one byte from the slave device
* and acknowledges the byte (requests another byte)
*/
uint8_t I2C_slave_read_ack(I2C_TypeDef* I2Cx){
	/* Test on I2C2 EV2 and clear it */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_BYTE_RECEIVED));
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}
/* This function reads one byte from the slave device
* and doesn't acknowledge the recieved data
*/
uint8_t I2C_slave_read_nack(I2C_TypeDef* I2Cx){
	/* Test on I2C2 EV2 and clear it */
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_BYTE_RECEIVED));
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}
void read_acc()
{
	//acc
			I2C_start(I2C1, LSM303DL_A_ADDRESS, I2C_Direction_Transmitter);
			I2C_write(I2C1,0x28| (1 << 7));
			I2C_stop(I2C1);
			I2C_start(I2C1, LSM303DL_A_ADDRESS, I2C_Direction_Receiver);

			 xla = I2C_read_ack(I2C1);
			 xha = I2C_read_ack(I2C1);
			 yla = I2C_read_ack(I2C1);
			 yha = I2C_read_ack(I2C1);
			 zla = I2C_read_ack(I2C1);
			 zha = I2C_read_nack(I2C1);

			I2C_stop(I2C1); // stop the transmission
			char buffer_out[100];
			ax=0,ay=0,az=0;
			ax = ((int16_t)(xha << 8 | xla)) >> 4;
			sprintf(buffer_out,"ax%i\r\n",ax);
			usb_cdc_printf(buffer_out);
			ay = ((int16_t)(yha << 8 | yla)) >> 4;
			buffer_out[0]='\0';
			sprintf(buffer_out,"ay%i\r\n",ay);
			usb_cdc_printf(buffer_out);
			az = ((int16_t)(zha << 8 | zla)) >> 4;
			buffer_out[0]='\0';
			sprintf(buffer_out,"az%i\r\n",az);
			usb_cdc_printf(buffer_out);
			//acc
}
void read_mag()
{
	//mag
			I2C_start(I2C1, LSM303DL_M_ADDRESS, I2C_Direction_Transmitter);
			I2C_write(I2C1,0x03); //LSM303_OUT_X_H_M    0x03
			I2C_stop(I2C1);
			I2C_start(I2C1, LSM303DL_M_ADDRESS, I2C_Direction_Receiver);

			 xhm = I2C_read_ack(I2C1);
			 xlm = I2C_read_ack(I2C1);
			 yhm = I2C_read_ack(I2C1);
			 ylm = I2C_read_ack(I2C1);
			 zhm = I2C_read_ack(I2C1);
			 zlm = I2C_read_nack(I2C1);

			 I2C_stop(I2C1);
			 char buffer_out[100];
			mx=0,my=0,mz=0;
			mx = (int16_t)(xhm << 8 | xlm);
			sprintf(buffer_out,"mx%i\r\n",mx);
			usb_cdc_printf(buffer_out);
			my = (int16_t)(yhm << 8 | ylm);
			buffer_out[0]='\0';
			sprintf(buffer_out,"my%i\r\n",my);
			usb_cdc_printf(buffer_out);
			mz = (int16_t)(zhm << 8 | zlm);
			buffer_out[0]='\0';
			sprintf(buffer_out,"mz%i\r\n",mz);
			usb_cdc_printf(buffer_out);
			//mag
	}
void read_gyro()
{
	//gyro
			I2C_start(I2C1, LSM303DL_G_ADDRESS, I2C_Direction_Transmitter);
			I2C_write(I2C1, 0x28| (1 << 7) ); //L3G_OUT_X_L   0x28
			I2C_stop(I2C1);
			I2C_start(I2C1, LSM303DL_G_ADDRESS, I2C_Direction_Receiver);

			xlg = I2C_read_ack(I2C1);
			xhg = I2C_read_ack(I2C1);
			ylg = I2C_read_ack(I2C1);
			yhg = I2C_read_ack(I2C1);
			zlg = I2C_read_ack(I2C1);
			zhg = I2C_read_nack(I2C1);

			I2C_stop(I2C1);
			char buffer_out[100];
			 gx=0,gy=0,gz=0;
			 gx = (int16_t)(xhg << 8 | xlg);
			 sprintf(buffer_out,"gx%i\r\n",gx);
			 usb_cdc_printf(buffer_out);
			 gy = (int16_t)(yhg << 8 | ylg);
			 buffer_out[0]='\0';
			 sprintf(buffer_out,"gy%i\r\n",gy);
			 usb_cdc_printf(buffer_out);
			 gz = (int16_t)(zhg << 8 | zlg);
			 buffer_out[0]='\0';
			 sprintf(buffer_out,"gz%i\r\n",gz);
			 usb_cdc_printf(buffer_out);
			//gyro
}
