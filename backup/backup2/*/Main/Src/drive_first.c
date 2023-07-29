/*
 * drive_first.c
 */

#include "header_init.h"


volatile int8_t			driveIdx;


//1차 주행
void Drive_First() {

	driveIdx = DRIVE_FIRST;
	Sensor_Start();

	//주행 전 변수값 초기화
	Drive_Preset();

	Motor_Start();
	Speed_Control_Start();

	while (state != 0) {

		Drive_Test_Info_Oled();

		Decision_Machine();
		Drive_Decision_Ctrl();

		if (curDecisionState == DECISION_END_MARK) {
			Drive_Fit_In(0.25f, 0.1f);
			while (currentSpeed > 0.5f) ;
			break ;
		}
	}
	Custom_Delay_ms(50);

	Motor_Stop();
	Speed_Control_Stop();
	Sensor_Stop();


	Custom_OLED_Clear();

}
