/*
 * motor.h
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_



#include "main.h"


void				Motor_Power_Off();
void				Motor_Start();
void				Motor_Stop();

void				Motor_Test_Velocity();
void				Motor_Test_Phase();
void				Motor_L_TIM3_IRQ();
void				Motor_R_TIM4_IRQ();


#define TIRE_RADIUS	0.025f					// m
#define	SPEED_COEF	15707.f * TIRE_RADIUS
/*
 * (2 * l(m) * 3.141592) / (t(s) * 400) = v(m/s) * (arr+1)
 *
 * t =  1 / 1Mhz = 1 / 1,000,000 = 타이머 주
 * 1 / (t * 400) = 2,500
 *
 * l(m) = 타이어 반지름
 * 2 * l * 3.14
 *
 * v * (arr + 1) = SPEED_COEF
 */

#define ACCELE_CHANGE_INIT	0.001f					//	m/ms
#define	MAX_SPEED_INIT		2.f						//	m/s
#define MIN_SPEED_INIT		SPEED_COEF / 65535.f	//	SPEED_COEF / (65535)ARR_MAX (m/s) 의 근사값


__STATIC_INLINE void		Motor_L_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM3, SPEED_COEF / speed - 1);
}

__STATIC_INLINE void		Motor_R_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM4, SPEED_COEF / speed - 1);
}


extern float		minSpeed;
extern float		maxSpeed;
extern float		acceleChange;



#endif /* INC_MOTOR_H_ */
