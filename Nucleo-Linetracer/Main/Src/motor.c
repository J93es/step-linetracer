/*
 * motor.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "motor.h"
#include "custom_delay.h"
#include "custom_gpio.h"
#include "custom_oled.h"
#include "custom_switch.h"





Custom_GPIO_t motorL[4] = {
		{ Motor_L1_GPIO_Port, Motor_L1_Pin },
		{ Motor_L3_GPIO_Port, Motor_L3_Pin },
		{ Motor_L2_GPIO_Port, Motor_L2_Pin },
		{ Motor_L4_GPIO_Port, Motor_L4_Pin },
};

Custom_GPIO_t motorR[4] = {
		{ Motor_R1_GPIO_Port, Motor_R1_Pin },
		{ Motor_R3_GPIO_Port, Motor_R3_Pin },
		{ Motor_R2_GPIO_Port, Motor_R2_Pin },
		{ Motor_R4_GPIO_Port, Motor_R4_Pin },
};

volatile static uint8_t	phases[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };

uint32_t	phaseL  = 0;
uint32_t	phaseR  = 0;





void motorPowerOff() {
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

	motorPowerOff();
}



void Motor_L_TIM3_IRQ() {
	// motorL
	Custom_GPIO_Set_t(motorL + 0, 0x01 & phases[phaseL]);
	Custom_GPIO_Set_t(motorL + 1, 0x02 & phases[phaseL]);
	Custom_GPIO_Set_t(motorL + 2, 0x04 & phases[phaseL]);
	Custom_GPIO_Set_t(motorL + 3, 0x08 & phases[phaseL]);

	phaseL = (phaseL + 1) & 0x07;
}



void Motor_R_TIM4_IRQ() {
	// motorR
	Custom_GPIO_Set_t(motorR + 0, 0x01 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 1, 0x02 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 2, 0x04 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 3, 0x08 & phases[phaseR]);

	phaseR = (phaseR + 1) & 0x07;
}



void Motor_Test_Phase() {
	uint32_t	sw = 0;
	uint32_t	stateL = 0;
	uint32_t	stateR = 0;

	/*
	 * 모터의 각 상을 잠깐씩 잡아본다.
	 * 모터에 무리를 주지 않기 위해 100ms 동안만 상을 잡은 후 바로 놓는다.
	 */
	Custom_OLED_Clear();

	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

		if (sw == CUSTOM_SW_1) {
			Custom_GPIO_Set_t(motorL + 0, 0x01 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 1, 0x02 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 2, 0x04 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 3, 0x08 & phases[stateL]);

			Custom_Delay_ms(100);
			motorPowerOff();

			Custom_OLED_Printf("/0phaseL: %2x", stateL);
			stateL = (stateL + 1) & 0x07;
		}

		else if (sw == CUSTOM_SW_2) {
			Custom_GPIO_Set_t(motorR + 0, 0x01 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 1, 0x02 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 2, 0x04 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 3, 0x08 & phases[stateR]);

			Custom_Delay_ms(100);
			motorPowerOff();

			Custom_OLED_Printf("/1phaseR: %2x", stateR);
			stateR = (stateR + 1) & 0x07;
		}
	}

	Custom_OLED_Clear();
}



void Motor_Test_Velocity() {
	/*
	 * 모터 속도를 부드럽게 올렸다가 내리기를 반복한다.
	 */
	Motor_Start();
	// --- Write your code ---
	Motor_Stop();
}
