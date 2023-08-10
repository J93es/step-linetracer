/*
 * motor.h
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_





#include "drive_def_var.h"
#include "main.h"
#include "custom_gpio.h"




extern volatile Custom_GPIO_t	motorL[4];
extern volatile Custom_GPIO_t	motorR[4];

extern volatile uint8_t	phases[8];





void	Motor_Power_Off();
void	Motor_Start();
void	Motor_Stop();

void	Speed_Control_Start();
void	Speed_Control_Stop();






__STATIC_INLINE void	Motor_L_TIM3_IRQ() {
	static uint8_t phaseL  = 0;


	// motorL phase 잡기
	Custom_GPIO_Set_t(motorL + 0, (phases[7 - phaseL] >> 0) & 0x01);
	Custom_GPIO_Set_t(motorL + 1, (phases[7 - phaseL] >> 1) & 0x01);
	Custom_GPIO_Set_t(motorL + 2, (phases[7 - phaseL] >> 2) & 0x01);
	Custom_GPIO_Set_t(motorL + 3, (phases[7 - phaseL] >> 3) & 0x01);

	phaseL = (phaseL + 1) & 0x07;


	// tick 값 증가
	curTick++;
}





__STATIC_INLINE void	Motor_R_TIM4_IRQ() {
	static uint8_t phaseR  = 0;


	// motorR phase 잡기
	Custom_GPIO_Set_t(motorR + 0, (phases[phaseR] >> 0) & 0x01);
	Custom_GPIO_Set_t(motorR + 1, (phases[phaseR] >> 1) & 0x01);
	Custom_GPIO_Set_t(motorR + 2, (phases[phaseR] >> 2) & 0x01);
	Custom_GPIO_Set_t(motorR + 3, (phases[phaseR] >> 3) & 0x01);

	phaseR = (phaseR + 1) & 0x07;
}





#endif /* INC_MOTOR_H_ */
