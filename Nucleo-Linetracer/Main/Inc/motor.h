/*
 * motor.h
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_





#include "speed_def_var.h"
#include "main.h"
#include "custom_gpio.h"





void	Motor_Power_Off();
void	Motor_Start();
void	Motor_Stop();
void	Motor_L_TIM3_IRQ();
void	Motor_R_TIM4_IRQ();






__STATIC_INLINE void		Motor_L_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM3, SPEED_COEF / speed - 1);
}


__STATIC_INLINE void		Motor_R_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM4, SPEED_COEF / speed - 1);
}






#endif /* INC_MOTOR_H_ */
