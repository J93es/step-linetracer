/*
 * drive_tools.h
 */

#ifndef INC_DRIVE_TOOLS_H_
#define INC_DRIVE_TOOLS_H_





#include "drive_def_var.h"
#include "init.h"
#include "motor.h"
#include "sensor.h"





typedef struct	s_driveMenu {
		volatile char		valName[MAX_OLED_LEN];
		volatile float		*val;
		volatile float		changeVar;
}				t_driveMenu;



void	Accele_Control_Start();
void	Accele_Control_Stop();
void	Drive_TIM9_IRQ();




// 피트인 함수
__STATIC_INLINE void Drive_Fit_In(float s, float pinSpeed) {
	targetSpeed = pinSpeed;
	accele = ABS( (pinSpeed - currentSpeed) * (pinSpeed + currentSpeed) / (2 * s) );
}





#endif /* INC_DRIVE_TOOLS_H_ */
