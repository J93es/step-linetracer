/*
 * motor.c
 */

#include "header_init.h"



volatile static Custom_GPIO_t	motorL[4] = {
		{ Motor_L1_GPIO_Port, Motor_L1_Pin },
		{ Motor_L3_GPIO_Port, Motor_L3_Pin },
		{ Motor_L2_GPIO_Port, Motor_L2_Pin },
		{ Motor_L4_GPIO_Port, Motor_L4_Pin },
};

volatile static Custom_GPIO_t	motorR[4] = {
		{ Motor_R1_GPIO_Port, Motor_R1_Pin },
		{ Motor_R3_GPIO_Port, Motor_R3_Pin },
		{ Motor_R2_GPIO_Port, Motor_R2_Pin },
		{ Motor_R4_GPIO_Port, Motor_R4_Pin },
};


volatile static uint8_t	phases[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };





void Motor_Power_Off() {
	Custom_GPIO_Set_t(motorL + 0, 0);
	Custom_GPIO_Set_t(motorL + 1, 0);
	Custom_GPIO_Set_t(motorL + 2, 0);
	Custom_GPIO_Set_t(motorL + 3, 0);

	Custom_GPIO_Set_t(motorR + 0, 0);
	Custom_GPIO_Set_t(motorR + 1, 0);
	Custom_GPIO_Set_t(motorR + 2, 0);
	Custom_GPIO_Set_t(motorR + 3, 0);
}







void Motor_Start() {
	LL_TIM_EnableCounter(TIM3);
	LL_TIM_EnableIT_UPDATE(TIM3);

	LL_TIM_EnableCounter(TIM4);
	LL_TIM_EnableIT_UPDATE(TIM4);
}





void Motor_Stop() {
	LL_TIM_DisableIT_UPDATE(TIM3);
	LL_TIM_DisableCounter(TIM3);

	LL_TIM_DisableIT_UPDATE(TIM4);
	LL_TIM_DisableCounter(TIM4);

	Motor_Power_Off();
}









void Motor_L_TIM3_IRQ() {
	static uint8_t			phaseL  = 0;


	// motorL phase 잡기
	Custom_GPIO_Set_t(motorL + 0, 0x01 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 1, 0x02 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 2, 0x04 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 3, 0x08 & phases[7 - phaseL]);

	phaseL = (phaseL + 1) & 0x07;



	// state 판단 및 기록
	if (curDecisionIdx == DESISION_STRAIGHT) {
		targetSpeed = targetSpeed_init;
	}
	else if (curDecisionIdx == DESISION_CURVE_L) {
		targetSpeed = targetSpeed_init + curveDecele;
	}
	else if (curDecisionIdx == DESISION_MEASURE_CURVE_L) {
		targetSpeed = targetSpeed_init + curveDecele;
	}
}





void Motor_R_TIM4_IRQ() {
	static uint8_t			phaseR  = 0;


	// motorR phase 잡기
	Custom_GPIO_Set_t(motorR + 0, 0x01 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 1, 0x02 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 2, 0x04 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 3, 0x08 & phases[phaseR]);

	phaseR = (phaseR + 1) & 0x07;




	// decision에 해당하는 속도 조정 및 거리 기록
	if (curDecisionIdx == DESISION_CURVE_R) {
		targetSpeed = targetSpeed_init + curveDecele;
	}
	else if (curDecisionIdx == DESISION_MEASURE_STRAIGHT) {
		targetSpeed = targetSpeed_init;
	}
	else if (curDecisionIdx == DESISION_MEASURE_CURVE_R) {
		targetSpeed = targetSpeed_init + curveDecele;
	}
}

