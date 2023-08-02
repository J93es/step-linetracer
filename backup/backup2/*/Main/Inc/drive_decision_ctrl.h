/*
 * drive_decision_ctrl.h
 */

#ifndef INC_DRIVE_DECISION_CTRL_H_
#define INC_DRIVE_DECISION_CTRL_H_


#include "drive_speed_ctrl.h"
#include "drive_def_var.h"
#include "main.h"




__STATIC_INLINE void	Set_First_Drive_Data() {

	// 현재 모터의 tick 값을 구조체에 저장
	driveDataPtr->tickCnt = curTick;

	// 현재 인덱스의 구조체의 값이 존재함을 저장
	driveDataPtr->isExist = CUSTOM_TRUE;

	// drivePtr 값 인덱스 증가
	driveDataPtr += 1;

	// 증가된 구조체의 인덱스에 curDecisionState 저장
	driveDataPtr->decisionState = curDecisionState;
}




__STATIC_INLINE void	First_Drive_Decision_Ctrl() {

	// decisionState가 변경되었을 경우
	if (curDecisionState != driveDataPtr->decisionState) {

		// driveData 값 업데이트
		Set_First_Drive_Data();
	}
}










__STATIC_INLINE void	Set_Second_Drive_Data() {

	// drivePtr 값 인덱스 증가
	driveDataPtr += 1;

	// 이전의 주행에서 마크를 정상적으로 읽었는지 판단
	if ((driveDataPtr - 1)->isReadAllMark == CUSTOM_TRUE) {

		// 주행중 decisionState와 1차 주행에서 저장된 decisionState가 동일하다면 정상적으로 읽었다고 판단
		if (curDecisionState == driveDataPtr->decisionState) {

			// 마크 인식 정상 여부를 업데이트
			driveDataPtr->isReadAllMark = CUSTOM_TRUE;
		}
	}
}






__STATIC_INLINE void	Straight_Boost_Aceleing() {

	// 주행에서 마크를 정상적으로 읽었을 경우
	if (driveDataPtr->isReadAllMark == CUSTOM_TRUE) {

		// 직선 구간 진입 후 0.3m 지났을 때 부스트
		if (curTick > (driveDataPtr-1)->tickCnt + 0.3 * TICK_PER_M) {

			// ACCELE_POINT_RATIO 까지 boostSpeed에 도달
			Drive_Fit_In( (driveDataPtr->tickCnt - (driveDataPtr-1)->tickCnt) * ACCELE_POINT_RATIO, boostSpeed );

			// 부스트 판단값 업데이트
			isBoost = CUSTOM_TRUE;
		}
	}
}


__STATIC_INLINE void	Straight_Boost_Deceleing() {

	// DECELE_POINT_RATIO 거리까지 왔을 때
	if (curTick > (driveDataPtr-1)->tickCnt + (driveDataPtr->tickCnt - (driveDataPtr-1)->tickCnt) * DECELE_POINT_RATIO) {

		// straightSpeed로 감속
		Drive_Fit_In( (driveDataPtr->tickCnt - (driveDataPtr-1)->tickCnt) * (0.95f - DECELE_POINT_RATIO), targetSpeed_init );
	}
}







__STATIC_INLINE void	Second_Drive_Decision_Ctrl() {

	// decisionState가 변경되었을 경우
	if (curDecisionState != driveDataPtr->decisionState) {

		// 가속도 부스트 여부 업데이트
		accele = accele_init;
		isBoost = CUSTOM_FALSE;

		// driveData 값 업데이트
		Set_Second_Drive_Data();
	}


	// 부스트 중일 경우
	if (isBoost == CUSTOM_TRUE) {

		// 부스터 종료 여부를 결정하고 감속하는 함수
		Straight_Boost_Deceleing();
	}
	// boost 상태가 아니고 현재 인덱스의 instruct가 부스트일 경우
	else if (driveDataPtr->instruct == INSTRUCT_BOOST){

		// 부스터 여부를 결정하고 부스터를 하는 함수
		Straight_Boost_Aceleing();
	}
}




#endif
