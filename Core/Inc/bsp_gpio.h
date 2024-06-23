#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

/*宏定义是函数的临时演员，其会在编译后被替换调，所以不会过函数编译检查
，不能以函数的眼光看待宏定义！！！*/
#define LED_NUMBER_2	GPIOC,GPIO_PIN_2
#define LED_NUMBER_1	GPIOC,GPIO_PIN_3

#define LIGHT GPIO_PIN_SET
#define CLOSE GPIO_PIN_RESET

extern void bsp_InitGpio(void);


extern void bsp_GpioOn(unsigned char _no);

extern void bsp_GpioOff(unsigned char _no);

extern void bsp_GpioToggle(unsigned char _no);

extern unsigned char bsp_IsGpioOn(unsigned char _no);

#endif