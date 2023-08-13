/*
 * second_drive.c
 */



#include "header_init.h"


__STATIC_INLINE void	Second_Drive_Ctrl();
__STATIC_INLINE void	Set_Second_Drive_Data();
__STATIC_INLINE uint8_t	Is_Decele(uint32_t markStartTick);




//1차 주행
void Second_Drive() {
	uint8_t	exitEcho = EXIT_ECHO_IDLE;

	Custom_OLED_Clear();

	//주행 전 변수값 초기화
	Pre_Drive_Setting(SECOND_DRIVE);

	Sensor_Start();
	Motor_Start();
	Speed_Control_Start();

	while (1) {

		//Drive_Test_Info_Oled();

		Drive_State_Machine();
		Second_Drive_Ctrl();
		//Drive_Speed_Cntl();
		if ( EXIT_ECHO_IDLE != (exitEcho = Is_Drive_End()) ) {
			break;
		}
	}

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();



	Custom_OLED_Clear();

	After_Drive_Setting(SECOND_DRIVE);

	if (exitEcho == EXIT_ECHO_END_MARK) {
		Custom_OLED_Printf("/0end mark");
	}
	else {
		Custom_OLED_Printf("/0line out");
	}

	while (CUSTOM_SW_BOTH != Custom_Switch_Read());
	Custom_OLED_Clear();
}




__STATIC_INLINE void Second_Drive_Ctrl() {

	// 현재 마크가 시작된 tick
	static uint32_t	markStartTick = 0;



	// markState가 변경되었을 경우
	if (markState != driveDataPtr->markState) {

		// 크로스가 아닐 경우
		if (markState != MARK_CROSS) {

			// 부스트에서 쓰이는 startTick 값 변경
			markStartTick = curTick;

			// driveData 값 업데이트
			Set_Second_Drive_Data();

			// end mark는 한번만 기록하고 바로 직진 상태로 바꿈
			if (markState == MARK_END) {
				markState = MARK_STRAIGHT;
			}
		}

		// 크로스일 경우
		else {

			// crossCnt 증가
			crossCnt += 1;

			// 크로스는 한번만 기록하고 바로 직진 상태로 바꿈
			markState = MARK_STRAIGHT;
		}
	}

	// markState가 변경되지 않았을 경우
	else {

		switch (boostCntl) {

			// 초기 상태
			case BOOST_CNTL_IDLE :


					// 주행에서 마크를 정상적으로 읽었을 경우
					if (driveDataPtr->isReadAllMark == CUSTOM_TRUE) {

						// 현재 인덱스의 boostTick이 0보다 클 경우
						// boostTick이 0보다 클 경우는 straight 밖에 없음 (After_Drive_Setting 함수 참고)
						if (driveDataPtr->boostTick > 0) {
							boostCntl = BOOST_CNTL_ACCELE;
						}
					}

					// 커브일 때 (cross, endMark도 포함하지만 실질적인 값 업데이트는 이루어지지 않음)
					// && 주행에서 마크를 정상적으로 읽지 못했을 경우
					else if (markState != MARK_STRAIGHT) {

						// isReadAllMark 보정

					}

					break ;



			// 부스트 가속 컨드롤
			case BOOST_CNTL_ACCELE :

					// 직선 구간 진입 후 ACCELE_START_TICK만큼 지났을 때 부스트
					if (curTick > markStartTick + ACCELE_START_TICK) {

						// boostSpeed로 가속
						targetSpeed = boostSpeed;

						boostCntl = BOOST_CNTL_DECELE;
					}

					break;



			// 부스트 감속 컨트롤
			case BOOST_CNTL_DECELE :


					// 감속이 시작될 거리까지 왔을 때
					if (curTick > markStartTick + driveDataPtr->boostTick - DECELE_START_TICK) {

						// targetSpeed_init로 감속
						Drive_Fit_In( DECELE_LEN_M, targetSpeed_init );

						boostCntl = BOOST_CNTL_END;
					}
					/*
					if (Is_Decele(markStartTick) == CUSTOM_TRUE) {
						boostCntl = BOOST_CNTL_END;
					}
					*/

					break ;



			// 부스트가 종료되었을 때
			case BOOST_CNTL_END :

					// 감속이 종료되었을 때 || 새로운 곡선 마크를 읽었을 때
					if ( curTick > markStartTick + driveDataPtr->boostTick || driveDataPtr->boostTick == 0) {

						boostCntl = BOOST_CNTL_IDLE;
					}

					break ;
		}
	}
}





__STATIC_INLINE void Set_Second_Drive_Data() {

	// drivePtr 값 인덱스 증가
	driveDataPtr += 1;

	// 이전의 주행에서 마크를 정상적으로 읽었는지 판단
	if ((driveDataPtr - 1)->isReadAllMark == CUSTOM_TRUE) {

		// 주행중 markState와 1차 주행에서 저장된 markState가 동일하다면 정상적으로 읽었다고 판단
		if (markState == driveDataPtr->markState) {

			// 마크 인식 정상 여부를 업데이트
			driveDataPtr->isReadAllMark = CUSTOM_TRUE;
		}
	}
}




/*
 * fit_in 함수를 역산
 * 현재 남은 감속 거리를 토대로 감속하는데에 필요한 감속속도을 계산
 * 계산을통해 나온 감속 속도가 현재 감속 속도보다 크거나 같을 때 감속
 */

__STATIC_INLINE uint8_t	Is_Decele(uint32_t markStartTick) {

	/*

	// 남은 감속거리
	static uint32_t leftedDeceleLen;

	// 감속에 필요한 속도
	static uint32_t calculatedDecele;


	leftedDeceleLen = ( driveDataPtr->boostTick - DECELE_END_TICK - (curTick - markStartTick) ) / TICK_PER_M;

	calculatedDecele = ABS((targetSpeed_init - currentSpeed) * (targetSpeed_init + currentSpeed)) / (2 * leftedDeceleLen);

	if (decele <= calculatedDecele) {

		// targetSpeed_init로 감속
		targetSpeed = targetSpeed_init;

		return CUSTOM_TRUE;
	}

	return CUSTOM_FALSE;

	*/


	if (    decele * 2 * ( driveDataPtr->boostTick - DECELE_END_TICK - (curTick - markStartTick) )    <= \
			ABS( (targetSpeed_init - currentSpeed) * (targetSpeed_init + currentSpeed) ) * TICK_PER_M    ) {

		// targetSpeed_init로 감속
		targetSpeed = targetSpeed_init;

		return CUSTOM_TRUE;
	}

	return CUSTOM_FALSE;
}


