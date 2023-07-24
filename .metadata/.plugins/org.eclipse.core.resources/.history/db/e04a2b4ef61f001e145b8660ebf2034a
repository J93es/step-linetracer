/*
 * drive.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "drive.h"
#include "motor.h"
#include "sensor.h"
#include "custom_oled.h"
#include "custom_switch.h"
#include "custom_exception.h"





float targetSpeed = 0;
float currentSpeed = 0;


// 500us마다 호출됨.
void Drive_TIM9_IRQ() {
	/*
	 * 이 인터럽트가 필요한 이유는 가속도 제어 때문이다.
	 * while문을 사용하면 특정 시간마다 한 번씩 제어하기가 어려운 반면(불가능한 것은 아님)
	 * 인터럽트를 사용하면 그런 작업이 쉽게 가능하다.
	 * 그러나 인터럽트 함수인 만큼 너무 많은 작업을 하면 안 된다.
	 */
	if (targetSpeed > currentSpeed) {
		currentSpeed += acceleChange;
		if (targetSpeed < currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}
	else {
		currentSpeed -= acceleChange;
		if (targetSpeed > currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}
}


void Drive_Test_Position() {
	uint8_t	sw = 0;

	Custom_OLED_Clear();
	Sensor_Start();
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Custom_OLED_Printf("/0pos: %7d", positionVal);
		Custom_OLED_Printf("/2speedL: %5f", 1 + positionVal * positionCoef);
		Custom_OLED_Printf("/3speedR: %5f", 1 - positionVal * positionCoef);
	}
	Sensor_Stop();
	Custom_OLED_Clear();
}



void Drive_Menu() {
	uint8_t	sw = 0;
	t_driveMenu	values[] = {
			{ "accele", &acceleChange, ACCELE_CHANGE_INIT},
			{ "target", &targetSpeed, 0.1 },
			{ "max speed", &maxSpeed, 0.1 },
			{ "min speed", &minSpeed, 0.1 },
	};
	uint8_t menuCnt = sizeof(values) / sizeof(t_driveMenu);

	targetSpeed = 1.f;
	currentSpeed = minSpeed;

	Custom_OLED_Clear();
	for (int i = 0; i < menuCnt; i++) {
		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
			Custom_OLED_Printf("/0%s", values[i].valName);
			Custom_OLED_Printf("/1%5f", *(values[i].val));

			if (sw == CUSTOM_SW_1) {
				*(values[i].val) -= values[i].changeVar;
			}
			else if (sw == CUSTOM_SW_2) {
				*(values[i].val) += values[i].changeVar;
			}
		}
	}
	Custom_OLED_Clear();
}



void Drive_First() {

	Custom_OLED_Clear();
	Sensor_Start();
	Motor_Start();
	LL_TIM_EnableCounter(TIM9);
	LL_TIM_EnableIT_UPDATE(TIM9);

	while (state != 0) {
		Drive_Position();
		Custom_OLED_Printf("/0target: %5f", targetSpeed);
		Custom_OLED_Printf("/1current: %5f", currentSpeed);
		Custom_OLED_Printf("/2speedL: %5f", currentSpeed * (1 + positionVal * positionCoef));
		Custom_OLED_Printf("/3speedR: %5f", currentSpeed * (1 - positionVal * positionCoef));
	}

	LL_TIM_DisableIT_UPDATE(TIM9);
	LL_TIM_DisableCounter(TIM9);
	Motor_Stop();
	Sensor_Stop();
	Custom_OLED_Clear();

}
