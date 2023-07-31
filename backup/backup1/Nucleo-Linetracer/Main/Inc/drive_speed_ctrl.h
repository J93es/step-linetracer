/*
 * drive_speed_ctrl.h
 */

#ifndef INC_DRIVE_SPEED_CTRL_H_
#define INC_DRIVE_SPEED_CTRL_H_





#include "drive_def_var.h"
#include "init.h"
#include "main.h"
#include "sensor.h"





__STATIC_INLINE void	Motor_L_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM3, SPEED_COEF / speed - 1);
}


__STATIC_INLINE void	Motor_R_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM4, SPEED_COEF / speed - 1);
}



__STATIC_INLINE void	Drive_Speed_Cntl() {
	static float	finalSpeed;
	static uint8_t	positionIdxMax = 5;
	static uint8_t	positionIdxMin = 2;
	static int32_t	positionValBuffer = 0;
	static int32_t	sensorNormValsSum = 1;
	static int32_t	getPositionCoef[8] = { -14000, -10000, -6000, -2000, 2000, 6000, 10000, 14000 };

	//positionVal 업데이트
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

	do {
		positionValBuffer += getPositionCoef[positionIdxMin] * sensorNormVals[positionIdxMin];
		sensorNormValsSum += sensorNormVals[positionIdxMin];
		positionIdxMin++;
	} while (positionIdxMin != positionIdxMax + 1);

	//divide by zero 방지하기 위해 sensorNormValsSum + 1로 나눔
	positionVal = positionValBuffer / (sensorNormValsSum + 1);

	// 포지션 값에 따른 감속
	finalSpeed = currentSpeed * (1 - ABS(positionVal) / CURVE_DECEL_COEF);

	//position 값에 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( finalSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( finalSpeed * (1 - positionCoef * positionVal) );


	positionValBuffer = 0;
	sensorNormValsSum = 0;
}




// 500us마다 호출됨.
__STATIC_INLINE void	Drive_TIM9_IRQ() {
	static float	finalSpeed;
	static uint8_t	positionIdxMax = 5;
	static uint8_t	positionIdxMin = 2;
	static int32_t	positionValBuffer = 0;
	static int32_t	sensorNormValsSum = 1;
	static int32_t	getPositionCoef[8] = { -14000, -10000, -6000, -2000, 2000, 6000, 10000, 14000 };


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


	//positionVal 업데이트
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

	do {
		positionValBuffer += getPositionCoef[positionIdxMin] * sensorNormVals[positionIdxMin];
		sensorNormValsSum += sensorNormVals[positionIdxMin];
		positionIdxMin++;
	} while (positionIdxMin != positionIdxMax + 1);

	//divide by zero 방지하기 위해 sensorNormValsSum + 1로 나눔
	positionVal = positionValBuffer / (sensorNormValsSum + 1);

	// 포지션 값에 따른 감속
	finalSpeed = currentSpeed * (1 - ABS(positionVal) / CURVE_DECEL_COEF);

	//position 값에 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( finalSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( finalSpeed * (1 - positionCoef * positionVal) );

	positionValBuffer = 0;
	sensorNormValsSum = 0;
}



// 피트인 함수
__STATIC_INLINE void	Drive_Fit_In(float s, float pinSpeed) {
	targetSpeed = pinSpeed;
	accele = ABS( (pinSpeed - currentSpeed) * (pinSpeed + currentSpeed) / (2 * s) );
}





#endif /* INC_DRIVE_SPEED_CTRL_H_ */
