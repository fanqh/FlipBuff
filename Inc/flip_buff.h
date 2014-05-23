#ifndef __FLIP_BUFF_H
#define __FLIP_BUFF_H


#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"

typedef struct
{
	UART_HandleTypeDef 	UartHandle;
	uint16_t UpBufferCount;
	uint8_t* UpBufferPtr;
}USER_UART_HandleTypeDef;    //fan


typedef enum 
{
  BUFFA       = 0x00,
  BUFFB       = 0x01,
  ERR       = 0x02,
}FLIPBUFF_TABLE;

typedef enum 
{
  FALSE      = 0x00,
  TRUE       = 0x01,
}BOOL;


/*************added by fan for flip buffer*******************************************/

void HAL_UART_Open(USER_UART_HandleTypeDef* UartHandle, uint8_t* buffer, uint16_t len);
//uint8_t WhichBuffer(USER_UART_HandleTypeDef*  pUserUartHandle);
//uint16_t GetDataFromUpBuffer(USER_UART_HandleTypeDef* pHandle, uint8_t* pBuff, uint16_t len);
uint16_t HAL_UART_Read(USER_UART_HandleTypeDef* pUserUartHandle, uint8_t* buffer, uint16_t len);














#endif

