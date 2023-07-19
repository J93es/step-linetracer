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


volatile t_driveData	driveData[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataPtr = driveData + 0;

volatile uint32_t		curTick = 0;

volatile uint8_t		isBoost = CUSTOM_FALSE;




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

		// 이전의 주행에서 마크를 정상적으로 읽었는지 판단
		if ((driveDataPtr - 1)->isReadAllMark == CUSTOM_TRUE) {

			// 주행중 decisionState와 1차 주행에서 저장된 decisionState가 동일하다면 정상적으로 읽었다고 판단
			if (curDecisionState == driveDataPtr->decisionState) {
				driveDataPtr->isReadAllMark = CUSTOM_TRUE;
			}
		}
	}
}




__STATIC_INLINE void	Straight_Boost_Aceleing() {

	// decisionState가 변경되었을 경우
	if (curDecisionState != driveDataPtr->decisionState) {

		// 주행에서 마크를 정상적으로 읽었으면 부스트
		if (driveDataPtr->isReadAllMark == CUSTOM_TRUE) {
			targetSpeed = targetSpeed_init + 8.f;
			isBoost = CUSTOM_TRUE;
		}
	}
}


__STATIC_INLINE void	Straight_Boost_Deceleing() {

	// 부스트 중일 경우
	if (isBoost == CUSTOM_TRUE) {
		if (curTick - driveDataPtr->tickCnt > (driveDataPtr->tickCnt - (driveDataPtr + 1)->tickCnt) * DECELE_POINT_RATIO) {

			// 감속
			Drive_Fit_In((driveDataPtr->tickCnt - (driveDataPtr + 1)->tickCnt) * (1 - DECELE_POINT_RATIO), targetSpeed_init);

			// 부스트 판단값 업데이트
			isBoost = CUSTOM_FALSE;
		}
	}
}





void Motor_L_TIM3_IRQ() {
	static uint8_t phaseL  = 0;


	// motorL phase 잡기
	Custom_GPIO_Set_t(motorL + 0, 0x01 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 1, 0x02 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 2, 0x04 & phases[7 - phaseL]);
	Custom_GPIO_Set_t(motorL + 3, 0x08 & phases[7 - phaseL]);

	phaseL = (phaseL + 1) & 0x07;



	// decision에 해당하는 속도 조정 및 거리 기록

	// 오른쪽 커브일 경우
	if (curDecisionState == DESISION_CURVE_R) {
		// tick 값 증가
		curTick++;

		// 속도/가속도 업데이트
		targetSpeed = targetSpeed_init + curveDecele;
		accele = accele_init;


		// 2차 주행일 경우 driveData 값 업데이트
		if (driveIdx == DRIVE_SECOND) {
			Set_Second_Drive_Data();
		}
		// 1차 주행일 경우 driveData 값 업데이트
		else {
			Set_First_Drive_Data(DESISION_CURVE_R);
		}
	}

	// 직선일 경우
	else if (curDecisionState == DESISION_STRAIGHT) {
		// tick 값 증가
		curTick++;

		// 부스트 중이 아니라면 속도/가속도 업데이트
		if (isBoost == CUSTOM_FALSE) {
			targetSpeed = targetSpeed_init;
			accele = accele_init;
		}


		// 2차 주행일 경우 driveData 값 업데이트
		if (driveIdx == DRIVE_SECOND) {
			// driveDataPtr가 다음 인덱스로 아직 안 넘어감
			Straight_Boost_Aceleing();

			// driveDataPtr가 이 함수에서 다음 인덱스로 넘어감
			Set_Second_Drive_Data();

			// driveDataPtr가 다음 인덱스로 넘어가 있음
			Straight_Boost_Deceleing();
		}
		// 1차 주행일 경우 driveData 값 업데이트
		else {
			Set_First_Drive_Data(DESISION_STRAIGHT);
		}
	}
}





void Motor_R_TIM4_IRQ() {
	static uint8_t phaseR  = 0;


	// motorR phase 잡기
	Custom_GPIO_Set_t(motorR + 0, 0x01 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 1, 0x02 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 2, 0x04 & phases[phaseR]);
	Custom_GPIO_Set_t(motorR + 3, 0x08 & phases[phaseR]);

	phaseR = (phaseR + 1) & 0x07;




	// decision에 해당하는 속도 조정 및 거리 기록
	if (curDecisionState == DESISION_CURVE_L) {
		// tick 값 증가
		curTick++;

		// 속도/가속도 업데이트
		targetSpeed = targetSpeed_init + curveDecele;
		accele = accele_init;


		// 2차 주행일 경우 driveData 값 업데이트
		if (driveIdx == DRIVE_SECOND) {
			Set_Second_Drive_Data();
		}
		// 1차 주행일 경우 driveData 값 업데이트
		else {
			Set_First_Drive_Data(DESISION_CURVE_L);
		}
	}

	// end mark일 경우
	else if (curDecisionState == DESISION_END_MARK) {
		// tick 값 증가
		curTick++;


		// 2차 주행일 경우 driveData 값 업데이트
		if (driveIdx == DRIVE_SECOND) {
			Set_Second_Drive_Data();
		}
		// 1차 주행일 경우 driveData 값 업데이트
		else {
			Set_First_Drive_Data(DESISION_END_MARK);
		}
	}
}

