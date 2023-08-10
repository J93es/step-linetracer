/*
 * first_drive.c
 */

#include "header_init.h"



__STATIC_INLINE t_driveData *	First_Drive_Ctrl(t_driveData *driveDataBufferPtr);
__STATIC_INLINE t_driveData *	Set_First_Drive_Data(t_driveData *driveDataBufferPtr);
static void						First_Drive_Data(t_driveData driveDataBuffer[MAX_MARKER_CNT], uint8_t exitEcho);
static void						Drive_Date_Update_Cntl(t_driveData driveDataBuffer[MAX_MARKER_CNT], uint8_t exitEcho);




//1차 주행
void First_Drive() {

	uint8_t exitEcho = EXIT_ECHO_IDLE;

	t_driveData driveDataBuffer[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
	t_driveData *driveDataBufferPtr = driveDataBuffer + 0;

	// driveData의 0번째 값 초기화 (0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠)
	driveDataBufferPtr->markState = MARK_STRAIGHT;
	driveDataBufferPtr->isExist = CUSTOM_TRUE;

	Custom_OLED_Clear();

	//주행 전 변수값 초기화
	Pre_Drive_Setting(FIRST_DRIVE);

	Sensor_Start();
	Motor_Start();
	Speed_Control_Start();

	while (1) {

		//Drive_Test_Info_Oled();

		Drive_State_Machine();
		driveDataBufferPtr = First_Drive_Ctrl(driveDataBufferPtr);
		//Drive_Speed_Cntl();
		if ( EXIT_ECHO_IDLE != (exitEcho = Is_Drive_End()) ) {
			break;
		}
	}

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();


	First_Drive_Data(driveDataBuffer, exitEcho);
}






__STATIC_INLINE t_driveData * First_Drive_Ctrl(t_driveData *driveDataBufferPtr) {

	// markState가 변경되었을 경우
	if (markState != driveDataBufferPtr->markState) {

		// 크로스가 아닐 경우
		if (markState != MARK_CROSS) {

			// driveData 값 업데이트
			driveDataBufferPtr = Set_First_Drive_Data(driveDataBufferPtr);

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
	return driveDataBufferPtr;
}



__STATIC_INLINE t_driveData * Set_First_Drive_Data(t_driveData *driveDataBufferPtr) {

	// 모터의 tick 값을 현재 인덱스의 구조체에 저장 (종료 시점을 저장 함)
	driveDataBufferPtr->tickCnt = curTick;

	// 종료시점에서 크로스를 읽은 총 개수를 저장
	driveDataBufferPtr->crossCnt = crossCnt;

	// drivePtr 값 인덱스 증가
	driveDataBufferPtr += 1;

	// 증가된 구조체의 인덱스에 markState 저장
	driveDataBufferPtr->markState = markState;

	// 증가된 인덱스의 구조체의 값이 존재함을 저장
	driveDataBufferPtr->isExist = CUSTOM_TRUE;

	return driveDataBufferPtr;
}






static void First_Drive_Data(t_driveData driveDataBuffer[MAX_MARKER_CNT], uint8_t exitEcho) {
	uint16_t markCnt_L = 0;
	uint16_t markCnt_R = 0;
	uint16_t markCnt_End = 0;
	uint16_t markCnt_Cross = 0;

	if (exitEcho == EXIT_ECHO_END_MARK) {

		// 마크 개수 세기
		for (uint32_t i = 1; driveDataBuffer[i].isExist == CUSTOM_TRUE; i += 1) {

			// 현재상태가 좌측 곡선인 경우
			if (driveDataBuffer[i].markState == MARK_CURVE_L) {

				// 다음 상태가 우측 곡선이었을 경우 == 연속 커브
				if (driveDataBuffer[i+1].markState == MARK_CURVE_R) {
					markCnt_L += 1;
				}
				else {
					markCnt_L += 2;
				}
			}

			// 현재상태가 우측 곡선인 경우
			else if (driveDataBuffer[i].markState == MARK_CURVE_R) {
				// 다음 상태가 좌측 곡선이었을 경우 == 연속 커브
				if (driveDataBuffer[i+1].markState == MARK_CURVE_L) {
					markCnt_R += 1;
				}
				else {
					markCnt_R += 2;
				}
			}

			// 엔드마크
			else if (driveDataBuffer[i].markState == MARK_END) {
				markCnt_End += 2;

				// 크로스
				markCnt_Cross = driveDataBuffer[i-1].crossCnt;
			}
		}

		// OLED에 exitEcho 변수명 및 마크 개수 출력
		Custom_OLED_Printf("/0end mark");
		Custom_OLED_Printf("/1mark L:   %d", markCnt_L);
		Custom_OLED_Printf("/2mark R:   %d", markCnt_R);
		Custom_OLED_Printf("/3cross:    %d", markCnt_Cross);
		Custom_OLED_Printf("/4end mark: %d", markCnt_End);

		while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;

		Drive_Date_Update_Cntl(driveDataBuffer, exitEcho);
	}

	else if (exitEcho == EXIT_ECHO_LINE_OUT){

		Custom_OLED_Printf("/0line out");

		while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;
	}
}



void Drive_Date_Update_Cntl(t_driveData driveDataBuffer[MAX_MARKER_CNT], uint8_t exitEcho) {

	uint8_t sw;
	uint8_t isUpdate = CUSTOM_FALSE;

	Custom_OLED_Printf("/5update: NO");

	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

		// data 업데이트 함
		if (sw == CUSTOM_SW_1) {
			Custom_OLED_Printf("/5update: YES");
			isUpdate = CUSTOM_TRUE;
		}

		// data 업데이트 안함
		else if (sw == CUSTOM_SW_2) {
			Custom_OLED_Printf("/5update: NO ");
			isUpdate = CUSTOM_FALSE;
		}
	}

	if (driveData[0].isExist == CUSTOM_FALSE || isUpdate == CUSTOM_TRUE) {

		for (uint32_t i = 0; i < MAX_MARKER_CNT; i += 1) {
			driveData[i].boostTick = driveDataBuffer[i].boostTick;
			driveData[i].crossCnt = driveDataBuffer[i].crossCnt;
			driveData[i].isExist = driveDataBuffer[i].isExist;
			driveData[i].markState = driveDataBuffer[i].markState;
			driveData[i].tickCnt = driveDataBuffer[i].tickCnt;
		}

		for (uint32_t i = 1; driveData[i].isExist == CUSTOM_TRUE; i += 1) {

			// 직선일 경우
			if (driveData[i].markState == MARK_STRAIGHT) {

				// MIN_BOOST_METER  * TICK_PER_M 이상 이동한 경우에 INSTRUCT_BOOST로 업데이트
				if ( driveData[i].tickCnt - driveData[i-1].tickCnt > MIN_BOOST_METER * TICK_PER_M ) {
					driveData[i].boostTick = driveData[i].tickCnt - driveData[i-1].tickCnt;
				}
				else {
					driveData[i].boostTick = 0;
				}
			}
		}
	}


	Custom_OLED_Clear();
}

