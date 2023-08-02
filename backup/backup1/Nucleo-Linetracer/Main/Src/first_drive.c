/*
 * drive_first.c
 */

#include "header_init.h"







void Set_Drive_Instruct() {

	for (volatile t_driveData *ptr = driveData + 1; ptr->isExist == CUSTOM_TRUE; ptr += 1) {

		// 직선일 경우
		if (ptr->markState == MARK_STRAIGHT) {

			// MIN_BOOST_METER  * TICK_PER_M 이상 이동한 경우에 INSTRUCT_BOOST로 업데이트
			if (ptr->tickCnt - (ptr-1)->tickCnt > MIN_BOOST_METER * TICK_PER_M) {
				ptr->instruct = INSTRUCT_BOOST;
			}
		}
	}
}


//1차 주행
void First_Drive() {
	static uint8_t	exitEcho = 1;

	Custom_OLED_Clear();
	Sensor_Start();

	//주행 전 변수값 초기화
	Drive_Preset(FIRST_DRIVE);

	Motor_Start();
	Speed_Control_Start();

	while (state != 0) {

		//Drive_Test_Info_Oled();

		Drive_State_Machine();
		First_Drive_Decision_Ctrl();
		//Drive_Speed_Cntl();
		if (markState == MARK_END) {
			Drive_Fit_In(0.2f, 0.1f);
			while (currentSpeed > 0.5f) {
				//Drive_Speed_Cntl();
			}
			exitEcho = 0;
			break ;
		}
	}
	Custom_Delay_ms(100);

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();

	Set_Drive_Instruct();


	Custom_OLED_Clear();

	if (exitEcho == 1) {
		Custom_OLED_Printf("/0line out");
	}
	else {
		Custom_OLED_Printf("/0end mark");
	}
	Custom_OLED_Printf("/1%d", markState);
	Custom_Delay_ms(5000);

}
