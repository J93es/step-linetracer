/*
 * drive_first.c
 */

#include "header_init.h"



//1차 주행
void Drive_First() {
	Custom_OLED_Clear();
	Sensor_Start();

	Drive_Setting();

	Accele_Control_Start();
	Motor_Start();

	while (state != 0) {
		Drive_Test_Info_Oled();
		First_Drive_Decision_Machine();
		if (curDecisionIdx == DESISION_END_MARK) {
			Drive_Fit_In(0.25f, 0.f);
			break ;
		}
	}
	Drive_Test_Info_Oled();
	Custom_Delay_ms(50);
	Motor_Power_Off();

	Motor_Stop();
	Accele_Control_Stop();
	Sensor_Stop();
	Custom_OLED_Clear();

}
