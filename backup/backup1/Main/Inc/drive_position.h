/*
 * drive_position.h
 */

#ifndef INC_DRIVE_POSITION_H_
#define INC_DRIVE_POSITION_H_





#include "drive_def_var.h"
#include "init.h"
#include "main.h"
#include "sensor.h"



// 이전 주기에서 읽은 센서 위치에서 4개만 선별
__STATIC_INLINE int32_t	Window_Position_Val() {
	static uint8_t	positionIdxMax = 5;
	static uint8_t	positionIdxMin = 2;
	static int32_t	positionTable[8] = { -1400, -1000, -600, -200, 200, 600, 1000, 1400 };

	static int32_t	sensorNormVal = 0;
	static int32_t	positionSum = 0;
	static int32_t	sensorNormValsSum = 1;


	positionIdxMax = 5;
	positionIdxMin = 2;

	// positionVal이 -2000보다 작거나 2000 보다 클 때
	if (absPositionVal > positionTable[4]) {

		// positionVal이 -2000보다 작을 때
		if (positionVal < 0) {
			positionIdxMax = 4;
			positionIdxMin = 1;
		}
		// positionVal이 2000보다 클 때
		else {
			positionIdxMax = 6;
			positionIdxMin = 3;
		}
	}


	positionSum = 0;
	sensorNormValsSum = 0;


	do {

		// 중간에 센서 인터럽트가 있다면 값이 바뀔 수 있음으로 별도의 변수에 저장
		sensorNormVal = sensorNormVals[positionIdxMin];

		positionSum += positionTable[positionIdxMin] * sensorNormVal;
		sensorNormValsSum += sensorNormVal;

		positionIdxMin++;

	} while (positionIdxMin < positionIdxMax + 1);


	// positionValBuffer 값 return
	//divide by zero 방지하기 위해 sensorNormValsSum + 1로 나눔
	return positionSum / (sensorNormValsSum + 1);
}





// positionVal의 절대값이 9000 이상일 때 9800으로 고정시킴 (라인 센서가 읽어지지 않을 때 positionVal이 0인 상황을 방지)
__STATIC_INLINE int32_t	Stabilize_Position_Val(int32_t positionValBuffer) {

	/*
	 * positionVal == 이전 인터럽트에서 계산 한 값
	 * 0  <=  (정상)  <  900  <=  (비정상)
	 *
	 *
	 * positionValBuffer == 현재 주기에서 계산한 값
	 * 0  <=  (탈선 진행중)  <=  400  <  (탈선 후 정상으로 돌아옴)  <  900  <=  (탈선 하기 직전) < 9800
	 *
	 *
	 * positionVal로 (정상),  (비정상)에 해당하는지를 판단
	 * positionValBuffer로 (탈선 후 정상으로 돌아옴), (탈선 하기 직전), (탈선 진행중)에 해당하는지를 판단
	 *
	 *
	 * (정상) || (탈선 후 정상으로 돌아옴)일 경우 positionVal = positionValBuffer
	 * (탈선 진행중) || (탈선 하기 직전)일 경우 positionVal = 980
	 */

	// 정상
	if (absPositionVal < 900) {
		return positionValBuffer;
	}

	// 비정상
	else {

		// 탈선 후 정상으로 돌아온 상태
		if ( (400 < ABS(positionValBuffer) && ABS(positionValBuffer) < 900) ) {
			return positionValBuffer;
		}

		// 탈선 하기 직전의 상태 || 탈선 진행중
		else {
			if (positionValBuffer < 0) {
				return -1000;
			}

			else {
				return 1000;
			}
		}
	}
}



__STATIC_INLINE void	Update_Limited_Position_Val() {

	// 곡선에 진입을 시작했을 때 빠르게 curve decel을 해줌
	if (limitedPositionVal < absPositionVal) {

		limitedPositionVal += 20;
		if (limitedPositionVal > absPositionVal) {
			limitedPositionVal = absPositionVal;
		}
	}
	// 곡선에서 벗어날 때 천천히 속도를 올려줌
	else {

		limitedPositionVal -= 10;
		if (limitedPositionVal < absPositionVal) {
			limitedPositionVal = absPositionVal;
		}
	}
}





__STATIC_INLINE void	Update_Position_Val() {


	positionVal = Window_Position_Val();//Stabilize_Position_Val( Window_Position_Val() );

	absPositionVal = ABS(positionVal);

	Update_Limited_Position_Val();
}

#endif
