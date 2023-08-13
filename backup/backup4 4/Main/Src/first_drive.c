/*
 * first_drive.c
 */

#include "header_init.h"



__STATIC_INLINE void	First_Drive_Ctrl();
__STATIC_INLINE void	Set_First_Drive_Data();
static void				First_Drive_Data(uint8_t exitEcho);




//1차 주행
void First_Drive() {
	uint8_t	exitEcho = EXIT_ECHO_IDLE;

	Custom_OLED_Clear();

	//주행 전 변수값 초기화
	Pre_Drive_Setting(FIRST_DRIVE);

	Sensor_Start();
	Motor_Start();
	Speed_Control_Start();

	while (1) {

		//Drive_Test_Info_Oled();

		Drive_State_Machine();
		First_Drive_Ctrl();
		//Drive_Speed_Cntl();
		if ( EXIT_ECHO_IDLE != (exitEcho = Is_Drive_End()) ) {
			break;
		}
	}

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();


	After_Drive_Setting(FIRST_DRIVE);
	First_Drive_Data(exitEcho);
}






__STATIC_INLINE void	First_Drive_Ctrl() {

	// markState가 변경되었을 경우
	if (markState != driveDataPtr->markState) {

		// 크로스가 아닐 경우
		if (markState != MARK_CROSS) {

			// driveData 값 업데이트
			Set_First_Drive_Data();

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
}



__STATIC_INLINE void	Set_First_Drive_Data() {

	// 모터의 tick 값을 현재 인덱스의 구조체에 저장 (종료 시점을 저장 함)
	driveDataPtr->tickCnt = curTick;

	// 종료시점에서 크로스를 읽은 총 개수를 저장
	driveDataPtr->crossCnt = crossCnt;

	// drivePtr 값 인덱스 증가
	driveDataPtr += 1;

	// 증가된 구조체의 인덱스에 markState 저장
	driveDataPtr->markState = markState;

	// 증가된 인덱스의 구조체의 값이 존재함을 저장
	driveDataPtr->isExist = CUSTOM_TRUE;
}






static void First_Drive_Data(uint8_t exitEcho) {
	uint16_t markCnt_L = 0;
	uint16_t markCnt_R = 0;
	uint16_t markCnt_End = 0;
	uint16_t markCnt_Cross = 0;


	for (volatile t_driveData *ptr = (driveData + 0); ptr->isExist == CUSTOM_TRUE; ptr += 1) {

		// 현재상태가 좌측 곡선인 경우
		if (ptr->markState == MARK_CURVE_L) {

			// 다음 상태가 우측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->markState == MARK_CURVE_R) {
				markCnt_L += 1;
			}
			else {
				markCnt_L += 2;
			}
		}

		// 현재상태가 우측 곡선인 경우
		else if (ptr->markState == MARK_CURVE_R) {
			// 다음 상태가 좌측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->markState == MARK_CURVE_L) {
				markCnt_R += 1;
			}
			else {
				markCnt_R += 2;
			}
		}

		// 엔드마크
		else if (ptr->markState == MARK_END) {
			markCnt_End += 2;
		}
	}

	// 크로스
	markCnt_Cross = crossCnt;


	// OLED에 exitEcho 변수명 변수값 출력
	Custom_OLED_Clear();

	if (exitEcho == EXIT_ECHO_END_MARK) {
		Custom_OLED_Printf("/0end mark");
	}
	else if (exitEcho == EXIT_ECHO_LINE_OUT){
		Custom_OLED_Printf("/0line out");
	}

	Custom_OLED_Printf("/1mark L:   %d", markCnt_L);
	Custom_OLED_Printf("/2mark R:   %d", markCnt_R);
	Custom_OLED_Printf("/3cross:    %d", markCnt_Cross);
	Custom_OLED_Printf("/4end mark: %d", markCnt_End);

	while (CUSTOM_SW_BOTH != Custom_Switch_Read());

	Custom_OLED_Clear();
}

