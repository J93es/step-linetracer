/*
 * drive_tools.c
 */


#include <stdint.h>
#include <stdbool.h>
#include "motor.h"
#include "sensor.h"
#include "drive_tools.h"
#include "custom_oled.h"
#include "custom_switch.h"
#include "custom_exception.h"


int32_t	positionVal = 0;
float	positionCoef = POSITION_COEF_INIT;

float	targetSpeed = 0;
float	currentSpeed = MIN_SPEED_INIT;
float	minSpeed = MIN_SPEED_INIT;
float	maxSpeed = MAX_SPEED_INIT;
float	accele = ACCELE_INIT;





void Accele_Control_Start(){
	LL_TIM_EnableCounter(TIM9);
	LL_TIM_EnableIT_UPDATE(TIM9);
}

void Accele_Control_Stop(){
	LL_TIM_DisableIT_UPDATE(TIM9);
	LL_TIM_DisableCounter(TIM9);
}





// 500us마다 호출됨.
void Drive_TIM9_IRQ() {
	static int32_t	i = 0;
	static int32_t	j = 0;
	static int32_t	temp = 0;
	static uint8_t	positionIdxMax = 7;
	static uint8_t	positionIdxMin = 1;
	static int32_t	positionBuffer[8] = { 0, };
	static int32_t	sensorNormValsBuffer[8] = { 1, };
	static int32_t	getPositionCoef[8] = { -14000, -10000, -6000, -2000, 2000, 6000, 10000, 14000 };

	/*
	 * 이 인터럽트가 필요한 이유는 가속도 제어 때문이다.
	 * while문을 사용하면 특정 시간마다 한 번씩 제어하기가 어려운 반면(불가능한 것은 아님)
	 * 인터럽트를 사용하면 그런 작업이 쉽게 가능하다.
	 * 그러나 인터럽트 함수인 만큼 너무 많은 작업을 하면 안 된다.
	 */
	// 가속도 조절
	if (targetSpeed > currentSpeed) {
		currentSpeed += accele / 2000;
		if (targetSpeed < currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}
	else {
		currentSpeed -= accele / 2000;
		if (targetSpeed > currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}

	// i == 7 일 때 position 값 업데이트
	if (i == 7) {

		//현재의 읽은 센서 위치에서 4개만 선별
		positionIdxMax = 5;
		positionIdxMin = 2;
		if (positionVal < -2000) {
			positionIdxMax = 4;
			positionIdxMin = 1;
		}
		else if (positionVal > 2000){
			positionIdxMax = 6;
			positionIdxMin = 3;
		}

		temp = 0;
		for (j = positionIdxMin; j != positionIdxMax + 1; j++) {
			temp += positionBuffer[j];
			i += sensorNormValsBuffer[j];						//i를 임시변수로 이용
		}
		positionVal = temp / (i + 1);							//divide by zero 방지하기 위해 i + 1로 나눔
		i = 0;
	}
	// position 값 누적
	else {
		positionBuffer[i] = getPositionCoef[i] * sensorNormVals[i];
		sensorNormValsBuffer[i] = sensorNormVals[i];
		i++;
	}
}







//주행 전 상수값 변경 절차
void Drive_Setting() {
	accele = ACCELE_INIT;
	targetSpeed = TARGET_SPEED_INIT;
	maxSpeed = MAX_SPEED_INIT;
	minSpeed = MIN_SPEED_INIT;

	uint8_t	sw = 0;
	t_driveMenu	values[] = {
			{ "accele      ", &accele, 0.0005},
			{ "targetSpeed ", &targetSpeed, 0.1 },
			{ "max speed   ", &maxSpeed, 0.05 },
			{ "min speed   ", &minSpeed, 0.05 },
	};
	uint8_t menuCnt = sizeof(values) / sizeof(t_driveMenu);

	currentSpeed = minSpeed;

	for (int i = 0; i < menuCnt; i++) {
		Custom_OLED_Clear();
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



