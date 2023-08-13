/*
 * drive_speed_ctrl.h
 */

#ifndef INC_DRIVE_SPEED_CTRL_H_
#define INC_DRIVE_SPEED_CTRL_H_





#include "drive_def_var.h"
#include "init.h"
#include "main.h"
#include "drive_position.h"
#include "custom_delay.h"





__STATIC_INLINE void	Motor_L_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM3, SPEED_COEF / speed - 1);
}


__STATIC_INLINE void	Motor_R_Speed_Control(float speed) {
	LL_TIM_SetAutoReload(TIM4, SPEED_COEF / speed - 1);
}




__STATIC_INLINE void	Drive_Speed_Cntl() {

	static float	finalSpeed;

	// 포지션 값에 따른 감속
	finalSpeed = currentSpeed * (1 - ABS(positionVal) / curveDecelCoef);

	//position 값에 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( finalSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( finalSpeed * (1 - positionCoef * positionVal) );
}




// 500us마다 호출됨.
__STATIC_INLINE void	Drive_TIM9_IRQ() {

	// 가속도 조절
	if (targetSpeed > currentSpeed) {

		currentSpeed += accele / 2000;
		if (targetSpeed < currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}
	else {

		currentSpeed -= decele / 2000;
		if (targetSpeed > currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}

	// positionVal 값 업데이트
	Update_Position_Val();

	// 속도 컨트롤
	Drive_Speed_Cntl();

	// lineOut 판단 시간 업데이트
	if (driveState == DRIVE_DECISION_LINE_OUT) {

		curTime++;
	}
}





// 피트인 함수
__STATIC_INLINE void	Drive_Fit_In(float s, float pinSpeed) {

	targetSpeed = pinSpeed;
	decele = ABS( (pinSpeed - currentSpeed) * (pinSpeed + currentSpeed) / (2 * s) );
}





__STATIC_INLINE uint8_t	Is_Drive_End() {

	static uint8_t exitEcho;

	exitEcho = EXIT_ECHO_IDLE;

	// endMark || lineOut
	if (endMarkCnt >= 2 || markState == MARK_LINE_OUT) {

		Drive_Fit_In(pitInLen, PIT_IN_TARGET_SPEED);

		while (currentSpeed > DRIVE_END_DELAY_SPEED) {
			//Drive_Speed_Cntl();
		}

		Custom_Delay_ms(DRIVE_END_DELAY_TIME);

		if (endMarkCnt >= 2) {

			exitEcho = EXIT_ECHO_END_MARK;
		}
		else {

			exitEcho = EXIT_ECHO_LINE_OUT;
		}
	}

	return exitEcho;
}





#endif /* INC_DRIVE_SPEED_CTRL_H_ */
