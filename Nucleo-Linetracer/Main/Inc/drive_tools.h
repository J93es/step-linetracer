/*
 * drive_tools.h
 */

#ifndef INC_DRIVE_TOOLS_H_
#define INC_DRIVE_TOOLS_H_





#include "init.h"
#include "motor.h"
#include "speed_def_var.h"
#include "linetracer_test.h"




#define ABS(x) ( ((x) > 0) ? (x) : (-1 * (x)))





typedef struct	s_driveMenu {
		volatile char		valName[MAX_OLED_LEN];
		volatile float		*val;
		volatile float		changeVar;
}				t_driveMenu;




void	Drive_TIM9_IRQ();
void	Drive_Setting();
void	Accele_Control_Start();
void	Accele_Control_Stop();

void	Drive_Fit_In(float s, float pinSpeed);





#endif /* INC_DRIVE_TOOLS_H_ */
