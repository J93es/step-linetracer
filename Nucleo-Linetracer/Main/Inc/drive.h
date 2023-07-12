/*
 * drive.h
 */

#ifndef INC_DRIVE_H_
#define INC_DRIVE_H_



#include "motor.h"
#include "sensor.h"


#define DRIVE_VAR_NAME_MAX_LEN 10


extern void		Drive_TIM9_IRQ();
void			Drive_Test_Position();
void			Drive_First();


extern float	targetSpeed;
extern float	currentSpeed;


typedef struct	t_driveMenu {
		char	valName[DRIVE_VAR_NAME_MAX_LEN];
		float	*val;
		float	changeVar;
}				t_driveMenu;


__STATIC_INLINE void Drive_Position() {
	Motor_L_Speed_Control( currentSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( currentSpeed * (1 - positionCoef * positionVal) );
}


#endif /* INC_DRIVE_H_ */
