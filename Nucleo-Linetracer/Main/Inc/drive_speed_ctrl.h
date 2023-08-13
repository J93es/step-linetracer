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




// 500us마다 호출됨.
__STATIC_INLINE void	Drive_TIM9_IRQ() {

	float	finalSpeed;


	// 가속도 및 속도 제어
	if (curSpeed <= targetSpeed) {

		// 가속도 제어
		curAccele += 0.003f;

		if (curAccele > targetAccele) {

			curAccele = targetAccele;
		}

		// 속도 제어
		curSpeed += curAccele / 2000;

		if (curSpeed >= targetSpeed) {

			curSpeed = targetSpeed;

			// 속도를 targetSpeed 까지 올린 후, curAccele을 0으로 변환
			curAccele = 0;
		}
	}
	else if (curSpeed > targetSpeed) {

		// 속도 제어
		curSpeed -= decele / 2000;

		if (curSpeed <= targetSpeed) {

			curSpeed = targetSpeed;

			// 직선 가속 후 targetSpeed 까지 도달하지 못하고 감속한 후 감속이 종료되었으면 , curAccele을 0으로 변환
			curAccele = 0;
		}
	}


	// positionVal 값 업데이트
	Update_Position_Val();

	// 포지션 값에 따른 감속
	finalSpeed = curSpeed * curveDecelCoef / (limitedPositionVal + curveDecelCoef);

	//position 값에 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( finalSpeed * (1 + positionVal * positionCoef) );
	Motor_R_Speed_Control( finalSpeed * (1 - positionVal * positionCoef) );

	if (driveState == DRIVE_DECISION_LINE_OUT) {

		lineOutTime += 1;
	}
}





// 피트인 함수
__STATIC_INLINE void	Drive_Fit_In(float s, float pinSpeed) {

	targetSpeed = pinSpeed;
	decele = ABS( (pinSpeed - curSpeed) * (pinSpeed + curSpeed) ) / (2 * s);
}





__STATIC_INLINE uint8_t	Is_Drive_End(uint8_t exitEcho) {

	// endMark || lineOut
	if (endMarkCnt >= 2 || markState == MARK_LINE_OUT) {

		Drive_Fit_In(pitInLen, PIT_IN_TARGET_SPEED);

		while (curSpeed > DRIVE_END_DELAY_SPEED) {
			//Drive_Speed_Cntl();
		}

		Custom_Delay_ms(DRIVE_END_DELAY_TIME_MS);

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
