/*
 * second_drive.c
 */



#include "header_init.h"


__STATIC_INLINE void	Second_Drive_Ctrl();
__STATIC_INLINE void	Set_Second_Drive_Data();
__STATIC_INLINE void	Second_Drive_Straight_Boost_Cntl();
__STATIC_INLINE void	Second_Drive_Curve_Inline_Cntl();
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
		if ( EXIT_ECHO_IDLE != (exitEcho = Is_Drive_End(exitEcho)) ) {
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

	Custom_OLED_Printf("/1cross: %u", crossCnt);

	while (CUSTOM_SW_BOTH != Custom_Switch_Read());
	Custom_OLED_Clear();
}




__STATIC_INLINE void Second_Drive_Ctrl() {

	if (markState == MARK_LINE_OUT) {
		return ;
	}

	// markState가 변경되었을 경우
	else if (markState != driveData[driveDataIdx].markState) {

		// driveData 값 업데이트
		Set_Second_Drive_Data();
	}

	// 주행에서 마크를 정상적으로 읽었을 경우
	else if (isReadAllMark == CUSTOM_TRUE) {

		// 최적화 레벨이 직선 가속 이상 일 때
		if (optimizeLevel >= OPTIMIZE_LEVEL_STRAIGHT) {

			// 직선일 경우
			if (markState == MARK_STRAIGHT) {

				// 직선가속
				Second_Drive_Straight_Boost_Cntl();
			}
		}

		// 최적화 레벨이 곡선 가속 이상 일 때
		else if (optimizeLevel >= OPTIMIZE_LEVEL_CURVE) {

			// 커브일 경우
			if (markState == MARK_CURVE_L || markState == MARK_CURVE_R) {

				// 곡선 인라인
				Second_Drive_Curve_Inline_Cntl();
			}
		}
	}
}





__STATIC_INLINE void Set_Second_Drive_Data() {

	// 크로스, 엔드마크가 아닐 경우
	if (markState != MARK_CROSS && markState != MARK_END) {

		// 현재 마크가 시작된 틱값 변경
		markStartTick_L = curTick_L;
		markStartTick_R = curTick_R;

		// drivePtr 값 인덱스 증가
		driveDataIdx += 1;


		// 주행중 markState와 1차 주행에서 저장된 markState가 동일하지 않다면 비정상적으로 읽었다고 판단
		if (markState != driveData[driveDataIdx].markState) {

			// 마크 인식 정상 여부를 업데이트
			isReadAllMark = CUSTOM_FALSE;
		}
	}

	else {

		// 크로스일 경우
		if (markState == MARK_CROSS) {

			// 마크 복구
			if (isReadAllMark == CUSTOM_FALSE) {

				// crossCntTable의 crossCnt 번째의 인덱스가 비어있지 않음 경우
				if (crossCntTable[crossCnt] != 0) {

					/*
					 *    n번째 크로스(crossCnt)		0		1		...		50
					 *    m번째 마크(driveDataIdx)		4(3)	6(5)	...		98
					 *
					 *    (0번째 마크에서 크로스를 읽었을 때 1번째 마크로 저장되도록 함, 0은 값이 없는 상태를 나타냄)
					 */
					driveDataIdx = crossCntTable[crossCnt] - 1;

					// isReadAllMark update
					isReadAllMark = CUSTOM_TRUE;
				}
			}

			crossCnt += 1;
		}

		// 엔드마크일 경우
		else if (markState == MARK_END){

			endMarkCnt += 1;
		}

		// 크로스, 엔드마크는 읽은 후 이전 상태로 되돌림
		markState = driveData[driveDataIdx].markState;
	}
}



__STATIC_INLINE void Second_Drive_Straight_Boost_Cntl() {


	// 직선 가속
	switch (starightBoostCntl) {

			// 초기 상태
			case BOOST_CNTL_IDLE :

					// 최소 부스트 거리 이상일 때
					if (driveData[driveDataIdx].tickCnt_L > acceleStartTick + deceleEndTick \
							&& driveData[driveDataIdx].tickCnt_R > acceleStartTick + deceleEndTick) {


						// decele 이후 다시 가속하는 것을 방지
						if (curTick_L < markStartTick_L + driveData[driveDataIdx].tickCnt_L - deceleEndTick \
								&& curTick_R < markStartTick_R + driveData[driveDataIdx].tickCnt_R - deceleEndTick) {

							starightBoostCntl = BOOST_CNTL_ACCELE;
						}
					}


					break ;


			// 부스트 가속 컨드롤
			case BOOST_CNTL_ACCELE :

					// 직선 구간 진입 후 ACCELE_START_TICK만큼 지났을 때 부스트
					if (curTick_L > markStartTick_L + acceleStartTick \
							&& curTick_R > markStartTick_R + acceleStartTick) {

						// boostSpeed로 가속
						targetSpeed = boostSpeed;

						starightBoostCntl = BOOST_CNTL_DECELE;
					}

					break;


			// 부스트 감속 컨트롤
			case BOOST_CNTL_DECELE :

					// decel이 시작되었을 경우
					if (Is_Decele() == CUSTOM_TRUE) {

						starightBoostCntl = BOOST_CNTL_END;
					}

					break ;



			// 부스트가 종료되었을 때
			case BOOST_CNTL_END :

					// 직선이 10cm 남았을 경우
					if (curTick_L > markStartTick_L + driveData[driveDataIdx].tickCnt_L - 0.1f * TICK_PER_M \
							|| curTick_R > markStartTick_R + driveData[driveDataIdx].tickCnt_R - 0.1f * TICK_PER_M) {

						// 부스트 중 차체가 떳을 때 크로스를 못읽는 경우를 방지함
						crossCnt = driveData[driveDataIdx].crossCnt;

						starightBoostCntl = BOOST_CNTL_IDLE;
					}

					break ;
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


	if (    decele * 2 * ( driveData[driveDataIdx].tickCnt_L - deceleEndTick - (curTick_L - markStartTick_L) )	<=	\
			ABS( (targetSpeed_init - curSpeed) * (targetSpeed_init + curSpeed) ) * TICK_PER_M 						\
			\
			|| \
			\
			decele * 2 * ( driveData[driveDataIdx].tickCnt_R - deceleEndTick - (curTick_R - markStartTick_R) )	<=	\
			ABS( (targetSpeed_init - curSpeed) * (targetSpeed_init + curSpeed) ) * TICK_PER_M) {

		// targetSpeed_init로 감속
		targetSpeed = targetSpeed_init;

		return CUSTOM_TRUE;
	}

	return CUSTOM_FALSE;
}



__STATIC_INLINE void Second_Drive_Curve_Inline_Cntl() {

}


