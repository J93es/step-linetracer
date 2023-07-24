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

volatile static uint8_t phases[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };

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

	for (int i = 0; i < 4; i++) {
		Custom_GPIO_Set_t(motorL + i, 0);
		Custom_GPIO_Set_t(motorR + i, 0);
	}
}

void Motor_L_TIM3_IRQ() {
	// --- Write your code ---
	// motorL
}

void Motor_R_TIM4_IRQ() {
	// --- Write your code ---
	// motorR
}

void Motor_Test_Phase() {
	/*
	 * 모터의 각 상을 잠깐씩 잡아본다.
	 * 모터에 무리를 주지 않기 위해 100ms 동안만 상을 잡은 후 바로 놓는다.
	 */
	// --- Write your code ---
}

void Motor_Test_Velocity() {
	/*
	 * 모터 속도를 부드럽게 올렸다가 내리기를 반복한다.
	 */
	Motor_Start();
	// --- Write your code ---
	Motor_Stop();
}
