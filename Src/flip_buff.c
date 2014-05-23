#include "stm32f4xx_hal.h"
#include "flip_buff.h"
#include<string.h>

/*hal buffer*/
#define HAL_BUFFER_SIZE	  256
uint8_t HalBufferA[HAL_BUFFER_SIZE]; 
uint8_t HalBufferB[HAL_BUFFER_SIZE]; 

uint8_t* pBuffTable[2];

/*
wrote by fan to build 2 buffer for a uart receive
*/
void HAL_UART_Open(USER_UART_HandleTypeDef* UartHandle, uint8_t* buffer, uint16_t len)
{

	buffer = buffer;
	len = len;

	pBuffTable[0] = HalBufferA;
	pBuffTable[1] = HalBufferB;
	UartHandle->UpBufferPtr = HalBufferB;
	UartHandle->UpBufferCount = 0;
	
	UartHandle->UartHandle.State = HAL_UART_STATE_READY;
	HAL_UART_Receive_IT((UART_HandleTypeDef*) &(UartHandle->UartHandle), HalBufferA, HAL_BUFFER_SIZE);
}
//uint8_t* pBuff[2] = {HalBufferA, HalBufferB}; 
//the buff of low
//return :
//0:  HalBUfferA
//1:  HalBufferB
//2:  error
static FLIPBUFF_TABLE WhichLowBuffer(USER_UART_HandleTypeDef*  pUserUartHandle)
{
	if(((pUserUartHandle->UartHandle.pRxBuffPtr) >= HalBufferA)&&((pUserUartHandle->UartHandle.pRxBuffPtr) <= (HalBufferA+HAL_BUFFER_SIZE)))	//&&(UartHandle->UpBufferPtr <= (HalBufferA+HAL_BUFFER_SIZE))
		return BUFFA;
	else if((pUserUartHandle->UartHandle.pRxBuffPtr >= HalBufferB)&&(pUserUartHandle->UartHandle.pRxBuffPtr <= (HalBufferB+HAL_BUFFER_SIZE)))	
		return BUFFB;
	else
		return ERR;	
}


static BOOL Hal_FlipBUff(USER_UART_HandleTypeDef *pHandle)
{
			uint8_t tmp;

			tmp = WhichLowBuffer(pHandle); 	
			if(tmp==ERR)
					return FALSE;
			//disable interrupt
			__HAL_UART_DISABLE_IT(&(pHandle->UartHandle), UART_IT_RXNE);  
			//save low buff to upbuff
			pHandle->UpBufferPtr = pBuffTable[tmp];  
			pHandle->UpBufferCount =  pHandle->UartHandle.RxXferSize - pHandle->UartHandle.RxXferCount;		
			//change the uart state
			if(pHandle->UartHandle.State==HAL_UART_STATE_BUSY_TX_RX) 
					pHandle->UartHandle.State = HAL_UART_STATE_BUSY_TX;
			else
					pHandle->UartHandle.State = HAL_UART_STATE_READY;
			
			//flip buffer
			if(tmp==0)
					HAL_UART_Receive_IT(&(pHandle->UartHandle), (uint8_t *)pBuffTable[1], HAL_BUFFER_SIZE);
			else
					HAL_UART_Receive_IT(&(pHandle->UartHandle), (uint8_t *)pBuffTable[0], HAL_BUFFER_SIZE);
			__HAL_UART_ENABLE_IT(&(pHandle->UartHandle), UART_IT_RXNE);
			
			return TRUE;
}
static uint16_t GetDataFromUpBuffer(USER_UART_HandleTypeDef* pHandle, uint8_t* pBuff, uint16_t len)
{
	uint16_t count = 0;
	
	if(pHandle->UpBufferCount > len)
	{
		memcpy(pBuff, pHandle->UpBufferPtr, len);
		pHandle->UpBufferPtr += len;
		pHandle->UpBufferCount -= len;
		return len;		
	}
	else
	{

		memcpy(pBuff, pHandle->UpBufferPtr, pHandle->UpBufferCount);
		pHandle->UpBufferPtr = pBuffTable[WhichLowBuffer(pHandle)];
		count = pHandle->UpBufferCount;
		pHandle->UpBufferCount = 0;
		/*insert flip buff*/
//		Hal_FlipBUff(pHandle);		
		return  count;
				
	}	
}

uint16_t HAL_UART_Read(USER_UART_HandleTypeDef* pUserUartHandle, uint8_t* buffer, uint16_t len)
{

	if((len==0)||(len>HAL_BUFFER_SIZE))
		 return 	0;
	if(pUserUartHandle->UpBufferCount > 0)
	{	
			return GetDataFromUpBuffer(pUserUartHandle, buffer,len);

	}
	else
	{ 
		if(pUserUartHandle->UartHandle.RxXferCount < pUserUartHandle->UartHandle.RxXferSize)	// have data in buff
		{
		  /**flip buff**/
			Hal_FlipBUff(pUserUartHandle);
			return GetDataFromUpBuffer(pUserUartHandle, buffer,len);
		}
		else
			return 0;
	}		
}


