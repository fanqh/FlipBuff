#ifndef  __HAL_DEBUG_H

#define  __HAL_DEBUG_H

extern int sendchar(int ch);
void UartInit(void);

#define __DEBUG_

//��֪��Ϊʲô����ĺ궨����벻ͨ��
//#ifdef 	__DEBUG_
//	#define DEBUGOUT(A)    printf(A)
//	#define DEBUGOUT(A,B)  printf(A,B)
//#else 
//	#define DEBUGOUT(A)
//	#define DEBUGOUT(A,B)
//#endif


//extern int getKey(void);

#endif

