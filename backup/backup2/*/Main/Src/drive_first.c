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
	Accele_Control_Start();

	while (state != 0) {

		//Drive_Test_Info_Oled();

		Decision_Machine();
		Decision_Execution();

		if (curDecisionState == DECISION_END_MARK) {
			Drive_Fit_In(0.25f, 0.1f);
			while (currentSpeed > 1.f) ;
			break ;
		}
	}
	Custom_Delay_ms(100);

	Motor_Stop();
	Accele_Control_Stop();
	Sensor_Stop();


	Custom_OLED_Clear();

}
