/*
 * first_drive.c
 */

#include "header_init.h"



__STATIC_INLINE void	First_Drive_Ctrl();
__STATIC_INLINE void	Set_First_Drive_Data();
static void				First_Drive_Data_Cntl(uint8_t exitEcho);
static void				First_Drive_Data_Update_Cntl(uint8_t exitEcho);




//1차 주행
void First_Drive() {

	uint8_t exitEcho = EXIT_ECHO_IDLE;

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
		if ( EXIT_ECHO_IDLE != (exitEcho = Is_Drive_End(exitEcho)) ) {

			break;
		}
	}

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();


	First_Drive_Data_Cntl(exitEcho);
}






__STATIC_INLINE void First_Drive_Ctrl() {

	if (markState == MARK_LINE_OUT) {
		return ;
	}

	// markState가 변경되었을 경우
	else if (markState != driveDataBuffer[driveDataIdx].markState) {

		// driveData 값 업데이트
		Set_First_Drive_Data();
	}
}



__STATIC_INLINE void Set_First_Drive_Data() {

	// 크로스, 엔드마크가 아닐 경우
	if (markState != MARK_CROSS && markState != MARK_END) {

		// 현재마크에서 이동한 tick 값을 현재 인덱스의 구조체에 저장
		driveDataBuffer[driveDataIdx].tickCnt = curTick - markStartTick;

		// 종료 시점에서의 읽은 크로스의 개수
		driveDataBuffer[driveDataIdx].crossCnt = crossCnt;

		// drivePtr 값 인덱스 증가
		driveDataIdx += 1;

		// markStartTick 업데이트
		markStartTick = curTick;

		// 증가된 구조체의 인덱스에 markState 저장
		driveDataBuffer[driveDataIdx].markState = markState;
	}

	else {

		// 크로스일 경우
		if (markState == MARK_CROSS) {

			/*
			 *    n번째 크로스(crossCnt)		0		1		...		50
			 *    m번째 마크(driveDataIdx)		4(3)	6(5)	...		98
			 *
			 *    (0번째 마크에서 크로스를 읽었을 때 1번째 마크로 저장되도록 함, 0은 값이 없는 상태를 나타냄)
			 */
			crossCntTableBuffer[crossCnt] = driveDataIdx + 1;

			crossCnt += 1;
		}

		// 엔드마크일 경우
		else if (markState == MARK_END){

			endMarkCnt += 1;

			if (endMarkCnt >= 2) {

				// 현재마크에서 이동한 tick 값을 현재 인덱스의 구조체에 저장
				driveDataBuffer[driveDataIdx].tickCnt = curTick - markStartTick;

				// 종료 시점에서의 읽은 크로스의 개수
				driveDataBuffer[driveDataIdx].crossCnt = crossCnt;
			}
		}

		// 크로스, 엔드마크는 읽은 후 이전 상태로 되돌림
		markState = driveDataBuffer[driveDataIdx].markState;
	}

}






static void First_Drive_Data_Cntl(uint8_t exitEcho) {
	uint32_t i = 1;
	uint16_t markCnt_L = 0;
	uint16_t markCnt_R = 0;
	uint16_t crossCnt = 0;

	if (exitEcho == EXIT_ECHO_END_MARK) {

		// 마크 개수 세기
		for (i = 1; driveDataBuffer[i].markState != MARK_NONE; i += 1) {

			// 현재상태가 좌측 곡선인 경우
			if (driveDataBuffer[i].markState == MARK_CURVE_L) {

				markCnt_L += 1;
			}

			// 현재상태가 우측 곡선인 경우
			else if (driveDataBuffer[i].markState == MARK_CURVE_R) {

				markCnt_R += 1;
			}

			// 직선 (인덱스가 1부터 시작하기에 지정되지 않은 메모리에 접근하는 행동을 방지함)
			else if (driveDataBuffer[i].markState == MARK_STRAIGHT) {

				// 이전 상태가 좌측 곡선이었을 경우
				if (driveDataBuffer[i-1].markState == MARK_CURVE_L) {
					markCnt_L += 1;
				}

				// 이전 상태가 우측 곡선이었을 경우
				else if (driveDataBuffer[i-1].markState == MARK_CURVE_R) {
					markCnt_R += 1;
				}
			}
		}

		for (i = 0; crossCntTableBuffer[i] != 0 && i < MAX_CROSS_CNT; i++) {

			crossCnt++;
		}

		Custom_OLED_Clear();

		// OLED에 exitEcho 변수명 및 마크 개수 출력
		Custom_OLED_Printf("/0end mark");
		Custom_OLED_Printf("/1mark L:   %d", markCnt_L);
		Custom_OLED_Printf("/2mark R:   %d", markCnt_R);
		Custom_OLED_Printf("/3cross:    %d", crossCnt);

		while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;

		First_Drive_Data_Update_Cntl(exitEcho);
	}

	else if (exitEcho == EXIT_ECHO_LINE_OUT){

		Custom_OLED_Printf("/0line out");

		while (CUSTOM_SW_BOTH != Custom_Switch_Read()) ;
	}

	Custom_OLED_Clear();
}



static void First_Drive_Data_Update_Cntl(uint8_t exitEcho) {

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
	Custom_OLED_Clear();

	if (driveData[0].markState == MARK_NONE || isUpdate == CUSTOM_TRUE) {

		for (uint32_t i = 0; i < MAX_DRIVE_DATA_LEN; i++) {
			driveData[i].tickCnt = driveDataBuffer[i].tickCnt;
			driveData[i].markState = driveDataBuffer[i].markState;
		}

		for (uint32_t i = 0; i < MAX_CROSS_CNT; i++) {

			crossCntTable[i] = crossCntTableBuffer[i];
		}
	}
}

