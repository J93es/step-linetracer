/*
 * drive_tools.h
 */

#ifndef INC_DRIVE_TOOLS_H_
#define INC_DRIVE_TOOLS_H_





#include <drive_def_var.h>
#include "init.h"
#include "motor.h"
#include "linetracer_test.h"





typedef struct	s_driveMenu {
		volatile char		valName[MAX_OLED_LEN];
		volatile float		*val;
		volatile float		changeVar;
}				t_driveMenu;




void	Drive_TIM9_IRQ();
void	Drive_Setting();
void	Accele_Control_Start();
void	Accele_Control_Stop();




#endif /* INC_DRIVE_TOOLS_H_ */
