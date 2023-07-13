/*
 * drive_first.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "motor.h"
#include "sensor.h"
#include "drive_tools.h"
#include "state_machine.h"
#include "custom_gpio.h"
#include "custom_oled.h"
#include "custom_switch.h"
#include "custom_exception.h"



//1차 주행
void Drive_First() {
	Custom_OLED_Clear();
	Sensor_Start();
	Drive_Setting();

	Motor_Start();
	Accele_Control_Start();

	while (state != 0) {
		Drive_Position();
		Custom_OLED_Printf("/0target: %5f", targetSpeed);
		Custom_OLED_Printf("/1current: %5f", currentSpeed);
		Custom_OLED_Printf("/2speedL: %5f", currentSpeed * (1 + positionVal * positionCoef));
		Custom_OLED_Printf("/3speedR: %5f", currentSpeed * (1 - positionVal * positionCoef));
	}
	Drive_Fit_In(0.25, 0.005);

	Custom_Delay_ms(500);
	Motor_Power_Off();

	Accele_Control_Stop();
	Motor_Stop();
	Sensor_Stop();
	Custom_OLED_Clear();

}
