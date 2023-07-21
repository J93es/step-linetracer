/*
 * drive_tools.c
 */

#include "header_init.h"




volatile int32_t		positionVal = 0;
volatile float			positionCoef = POSITION_COEF_INIT;


volatile t_driveData	driveData[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataPtr = driveData + 0;
volatile uint32_t		curTick = 0;
volatile uint8_t		isBoost = CUSTOM_FALSE;
volatile uint8_t		endMarkCnt = 0;
volatile uint8_t		curDecisionState = DECISION_STRAIGHT;





void Accele_Control_Start(){
	LL_TIM_EnableCounter(TIM9);
	LL_TIM_EnableIT_UPDATE(TIM9);
}

void Accele_Control_Stop(){
	LL_TIM_DisableIT_UPDATE(TIM9);
	LL_TIM_DisableCounter(TIM9);
	positionVal = 0;
}





__STATIC_INLINE void Set_Position() {
	static uint8_t	i = 0;
	static uint8_t	positionIdxMax = 5;
	static uint8_t	positionIdxMin = 2;
	static int32_t	positionBuffer = 0;
	static int32_t	sensorNormValsSum = 1;
	static int32_t	getPositionCoef[8] = { -14000, -10000, -6000, -2000, 2000, 6000, 10000, 14000 };


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

	for (i = positionIdxMin; i != positionIdxMax + 1; i++) {
		positionBuffer += getPositionCoef[i] * sensorNormVals[i];
		sensorNormValsSum += sensorNormVals[i];
	}

	//divide by zero 방지하기 위해 sensorNormValsSum + 1로 나눔
	positionVal = positionBuffer / (sensorNormValsSum + 1);
	positionBuffer = 0;
	sensorNormValsSum = 0;

	//position 값애 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( currentSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( currentSpeed * (1 - positionCoef * positionVal) );
}



// 500us마다 호출됨.
void Drive_TIM9_IRQ() {
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

	Set_Position();
}
