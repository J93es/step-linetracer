/*
 * second_drive.c
 */



#include "header_init.h"


__STATIC_INLINE void	Second_Drive_Ctrl();
__STATIC_INLINE void	Set_Second_Drive_Data();
__STATIC_INLINE void	Second_Drive_Boost();
__STATIC_INLINE void	Second_Drive_Restore_Mark();
__STATIC_INLINE uint8_t	Is_Decele();




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

	// markState가 변경되었을 경우
	if (markState != driveDataPtr->markState) {

		// driveData 값 업데이트
		Set_Second_Drive_Data();
	}

	// 주행에서 마크를 정상적으로 읽었을 경우
	else if (isReadAllMark == CUSTOM_TRUE) {

		// 직선가속
		Second_Drive_Boost();
	}

	// 주행에서 마크를 정상적으로 읽지 못했을 경우
	else {

		// isReadAllMark 보정
		Second_Drive_Restore_Mark();
	}
}





__STATIC_INLINE void Set_Second_Drive_Data() {

	// 크로스, 엔드마크가 아닐 경우
	if (markState != MARK_CROSS && markState != MARK_END) {

		// 현재 마크가 시작된 틱값 변경
		markStartTick = curTick;

		// drivePtr 값 인덱스 증가
		driveDataPtr += 1;


		// 주행중 markState와 1차 주행에서 저장된 markState가 동일하지 않다면 비정상적으로 읽었다고 판단
		if (markState != driveDataPtr->markState) {

			// 마크 인식 정상 여부를 업데이트
			isReadAllMark = CUSTOM_FALSE;
		}
	}

	else {

		// 크로스일 경우
		if (markState == MARK_CROSS) {

			crossCnt += 1;
		}

		// 엔드마크일 경우
		else if (markState == MARK_END){

			endMarkCnt += 1;
		}

		// 크로스, 엔드마크는 읽은 후 이전 상태로 되돌림
		markState = driveDataPtr->markState;
	}
}



__STATIC_INLINE void Second_Drive_Boost() {

	// 직선 가속
	switch (boostCntl) {

			// 초기 상태
			case BOOST_CNTL_IDLE :

					// 직선일 경우
					if (markState == MARK_STRAIGHT) {

						// 최소 부스트 거리 이상일 때
						if (driveDataPtr->tickCnt > ACCELE_START_TICK + MIN_BOOST_TICK + DECELE_END_TICK) {


							// decele 이후 다시 가속하는 것을 방지
							if (curTick < markStartTick + driveDataPtr->tickCnt - MIN_BOOST_TICK - DECELE_END_TICK) {

								boostCntl = BOOST_CNTL_ACCELE;
							}
						}
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

					// decel이 시작되었을 경우
					if (Is_Decele() == CUSTOM_TRUE) {

						boostCntl = BOOST_CNTL_END;
					}

					break ;



			// 부스트가 종료되었을 때
			case BOOST_CNTL_END :

					// 감속이 종료되었을 때
					if ( curTick > markStartTick + driveDataPtr->tickCnt - DECELE_END_TICK ) {

						boostCntl = BOOST_CNTL_IDLE;
					}

					break ;
	}
}



__STATIC_INLINE void Second_Drive_Restore_Mark() {

	if (crossCnt > driveData->crossCnt) {

	}

	else {

	}
}





/*
 * fit_in 함수를 역산
 * 현재 남은 감속 거리를 토대로 감속하는데에 필요한 감속속도을 계산
 * 계산을통해 나온 감속 속도가 현재 감속 속도보다 크거나 같을 때 감속
 */

__STATIC_INLINE uint8_t	Is_Decele() {

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


	if (    decele * 2 * ( driveDataPtr->tickCnt - DECELE_END_TICK - (curTick - markStartTick) )    <= \
			ABS( (targetSpeed_init - currentSpeed) * (targetSpeed_init + currentSpeed) ) * TICK_PER_M    ) {

		// targetSpeed_init로 감속
		targetSpeed = targetSpeed_init;

		return CUSTOM_TRUE;
	}

	return CUSTOM_FALSE;
}


