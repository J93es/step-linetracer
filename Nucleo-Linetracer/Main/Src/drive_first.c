/*
 * drive_first.c
 */

#include "header_init.h"



//1차 주행
void Drive_First() {
	Custom_OLED_Clear();
	Sensor_Start();

	Drive_Setting();

	Motor_Start();
	Accele_Control_Start();

	while (state != 0) {
		Drive_Test_Info_Oled();
		First_Drive_State_Machine();
		if (curStateIdx == STATE_END_MARK) {
			Drive_Fit_In(0.25, 0.f);
			break ;
		}
	}
	Drive_Test_Info_Oled();
	Custom_Delay_ms(500);
	Motor_Power_Off();

	Accele_Control_Stop();
	Motor_Stop();
	Sensor_Stop();
	Custom_OLED_Clear();

}
