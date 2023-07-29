/*
 * drive_execution.h
 */

#ifndef INC_DRIVE_EXECUTION_H_
#define INC_DRIVE_EXECUTION_H_


#include "main.h"
#include "drive_def_var.h"
#include "drive_tools.h"








__STATIC_INLINE void	Set_First_Drive_Data(uint8_t decisionState) {

	// decisionState가 변경되었을 경우
	if (curDecisionState != driveDataPtr->decisionState) {
		// 현재 인덱스의 구조체의 값이 존재함을 저장
		driveDataPtr->isExist = CUSTOM_TRUE;

		// 현재 모터의 tick 값을 구조체에 저장
		driveDataPtr->tickCnt = curTick;

		// drivePtr 값 인덱스 증가
		driveDataPtr += 1;

		// 증가된 구조체의 인덱스에 curDecisionState 저장
		driveDataPtr->decisionState = decisionState;
	}
}



__STATIC_INLINE void	Set_Second_Drive_Data() {

	// decisionState가 변경되었을 경우
	if (curDecisionState != driveDataPtr->decisionState) {

		// drivePtr 값 인덱스 증가
		driveDataPtr += 1;

		// 주행중 decisionState와 1차 주행에서 저장된 decisionState가 동일하다면 정상적으로 읽었다고 판단
		if (curDecisionState == driveDataPtr->decisionState) {

			// 이전의 주행에서 마크를 정상적으로 읽었는지 판단
			if ((driveDataPtr - 1)->isReadAllMark == CUSTOM_TRUE) {

				// 마크 인식 정상 여부를 업데이트
				driveDataPtr->isReadAllMark = CUSTOM_TRUE;
			}
		}
	}
}


__STATIC_INLINE void	Straight_Boost_Acceleing() {

	// 현재 상태에서 부스트 지시가 있다면
	if (driveData->instruct == INSTRUCT_ACCELE) {

		// 주행에서 마크를 정상적으로 읽었으면
		if (driveDataPtr->isReadAllMark == CUSTOM_TRUE) {

			// 이전 코스가 끝나는 지점으로 부터 30cm 이동한 거리에서 부스트 시작
			if (curTick > (driveDataPtr - 1)->tickCnt + 0.3f * TICK_PER_M) {

				// 부스트
				targetSpeed = boostSpeed;

				// 부스트 판단값 업데이트
				isBoost = CUSTOM_TRUE;
			}
		}
	}
}


__STATIC_INLINE void Straight_Boost_Deceleing() {

	// 부스트 중일 경우
	if (isBoost == CUSTOM_TRUE) {
		if (curTick > (driveDataPtr - 1)->tickCnt + (driveDataPtr->tickCnt - (driveDataPtr - 1)->tickCnt) * BOOST_DECELE_POINT_RATIO) {

			// 감속
			Drive_Fit_In((driveDataPtr->tickCnt - (driveDataPtr - 1)->tickCnt) * (0.95f - BOOST_DECELE_POINT_RATIO), straightSpeed);
		}
	}
}




__STATIC_INLINE void Decision_Execution_Curve(uint8_t decisionCurve) {

	// 속도/가속도 업데이트
	accele = accele_init;
	targetSpeed = curveSpeed;

	// 부스트 판단값 업데이트
	isBoost = CUSTOM_FALSE;


	// 2차 주행일 경우
	if (driveIdx == DRIVE_SECOND) {
		// driveData 값 업데이트
		Set_Second_Drive_Data();
	}
	// 1차 주행일 경우
	else {
		// driveData 값 업데이트
		Set_First_Drive_Data(decisionCurve);
	}
}






__STATIC_INLINE void Decision_Execution() {

	// 오른쪽 커브일 경우
	if (curDecisionState == DECISION_CURVE_R) {
		Decision_Execution_Curve(DECISION_CURVE_R);
	}


	// 왼쪽 커브일 경우
	else if (curDecisionState == DECISION_CURVE_L) {
		Decision_Execution_Curve(DECISION_CURVE_L);
	}


	// 직선일 경우
	else if (curDecisionState == DECISION_STRAIGHT) {

		// 부스트 중이 아니라면 속도 업데이트
		if (isBoost == CUSTOM_FALSE) {
			targetSpeed = straightSpeed;
		}


		// 2차 주행일 경우
		if (driveIdx == DRIVE_SECOND) {
			// driveData 값 업데이트
			// driveDataPtr가 이 함수에서 다음 인덱스로 넘어감
			Set_Second_Drive_Data();

			// 부스터 여부를 결정하고 부스터를 하는 함수
			// driveDataPtr가 다음 인덱스로 넘어가 있음
			Straight_Boost_Acceleing();

			// 부스터 종료 여부를 결정하고 감속하는 함수
			// driveDataPtr가 다음 인덱스로 넘어가 있음
			Straight_Boost_Deceleing();
		}
		// 1차 주행일 경우
		else {
			// driveData 값 업데이트
			Set_First_Drive_Data(DECISION_STRAIGHT);
		}
	}

	// end mark일 경우
	else if (curDecisionState == DECISION_END_MARK) {

		// 2차 주행일 경우
		if (driveIdx == DRIVE_SECOND) {
			// driveData 값 업데이트
			Set_Second_Drive_Data();
		}
		// 1차 주행일 경우 driveData 값 업데이트
		else {
			// driveData 값 업데이트
			Set_First_Drive_Data(DECISION_END_MARK);
		}
	}
}


#endif
