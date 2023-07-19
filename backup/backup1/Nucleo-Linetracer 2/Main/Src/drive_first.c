/*
 * drive_first.c
 */

#include "header_init.h"



//1차 주행
void Drive_First() {

	driveIdx = DRIVE_FIRST;

	//주행 전 변수값 초기화
	Drive_Setting();

	Sensor_Start();
	Accele_Control_Start();
	Motor_Start();

	while (state != 0) {

		//Drive_Test_Info_Oled();

		Decision_Machine();

		if (curDecisionState == DESISION_END_MARK) {
			Drive_Fit_In(0.25f, 0.f);
			while (currentSpeed > 0.1f) ;
			break ;
		}
	}
	Custom_Delay_ms(50);

	Drive_Test_Info_Oled();

	Motor_Power_Off();

	Motor_Stop();
	Accele_Control_Stop();
	Sensor_Stop();


	Custom_OLED_Clear();

}
