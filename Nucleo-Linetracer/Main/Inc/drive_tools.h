/*
 * drive_tools.h
 */

#ifndef INC_DRIVE_TOOLS_H_
#define INC_DRIVE_TOOLS_H_




#include "init.h"
#include "motor.h"
#include "sensor.h"


#define POSITION_COEF_INIT			0.00004f
#define TARGET_SPEED_INIT			1.f
#define ACCELE_INIT					2.f					//	m/ms
#define	MAX_SPEED_INIT				2.f						//	m/s
#define MIN_SPEED_INIT				SPEED_COEF / 65535.f	//	SPEED_COEF / (65535)ARR_MAX (m/s) 의 근사값



extern int32_t		positionVal;
extern float		positionCoef;

extern float		targetSpeed;
extern float		currentSpeed;
extern float		minSpeed;
extern float		maxSpeed;
extern float		accele;





typedef struct	s_driveMenu {
		char		valName[MAX_OLED_LEN];
		float		*val;
		float		changeVar;
}				t_driveMenu;




void	Drive_TIM9_IRQ();
void	Drive_Setting();
void	Accele_Control_Start();
void	Accele_Control_Stop();





__STATIC_INLINE void	Drive_Fit_In(float s, float pinSpeed) {
	targetSpeed = pinSpeed;
	accele = (currentSpeed * currentSpeed) / (2 * s);
	while (currentSpeed > 0.5) ;
}



//선의 위치에 따라 좌우 모터의 속도를 변경하는 함수
__STATIC_INLINE void	Drive_Position() {
	Motor_L_Speed_Control( currentSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( currentSpeed * (1 - positionCoef * positionVal) );
}



#endif /* INC_DRIVE_TOOLS_H_ */
