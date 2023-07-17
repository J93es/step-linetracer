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





/*
//선의 위치에 따라 좌우 모터의 속도를 변경하는 함수
__STATIC_INLINE void	Drive_Position() {
	Motor_L_Speed_Control( currentSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( currentSpeed * (1 - positionCoef * positionVal) );
}
*/




__STATIC_INLINE void	Drive_Fit_In(float s, float pinSpeed) {
	targetSpeed = pinSpeed;
	accele =  ABS( (pinSpeed - currentSpeed) * (pinSpeed + currentSpeed) / (2 * s) );
	Drive_Test_Info_Oled();
	while (currentSpeed > 0.1f + pinSpeed) ;
}



#endif /* INC_DRIVE_TOOLS_H_ */
