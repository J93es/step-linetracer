/*
 * linetracer_test.c
 */

#include "header_init.h"



static Custom_GPIO_t	motorL[4] = {
		{ Motor_L1_GPIO_Port, Motor_L1_Pin },
		{ Motor_L3_GPIO_Port, Motor_L3_Pin },
		{ Motor_L2_GPIO_Port, Motor_L2_Pin },
		{ Motor_L4_GPIO_Port, Motor_L4_Pin },
};


static Custom_GPIO_t	motorR[4] = {
		{ Motor_R1_GPIO_Port, Motor_R1_Pin },
		{ Motor_R3_GPIO_Port, Motor_R3_Pin },
		{ Motor_R2_GPIO_Port, Motor_R2_Pin },
		{ Motor_R4_GPIO_Port, Motor_R4_Pin },
};


volatile static uint8_t	phases[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };







void Sensor_Test_Raw() {
	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 Raw 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			sensorRawVals[0], sensorRawVals[1], sensorRawVals[2], sensorRawVals[3], \
			sensorRawVals[4], sensorRawVals[5], sensorRawVals[6], sensorRawVals[7]);
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}








void Sensor_Test_Normalized() {
	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 Normalized 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != Custom_Switch_Read()) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			sensorNormVals[0], sensorNormVals[1], sensorNormVals[2], sensorNormVals[3], \
			sensorNormVals[4], sensorNormVals[5], sensorNormVals[6], sensorNormVals[7]);
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}






void Sensor_Test_State() {
	uint8_t sw = 0;

	Sensor_Start();
	Custom_OLED_Clear();

	// 센서의 State 값을 디스플레이에 출력해 확인하기
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Custom_OLED_Printf("%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			(state >> 0) & 1, (state >> 1) & 1, (state >> 2) & 1, (state >> 3) & 1, \
			(state >> 4) & 1, (state >> 5) & 1, (state >> 6) & 1, (state >> 7) & 1);
		Custom_OLED_Printf("/1threshold: %3d", threshold);


		if (sw == CUSTOM_SW_1) {
			if (threshold > THRESHOLD_MIN) {
				threshold -= THRESHOLD_CHANGE_VAL;
			}
		}
		else if (sw == CUSTOM_SW_2) {
			if (threshold < THRESHOLD_MAX) {
				threshold += THRESHOLD_CHANGE_VAL;
			}
		}
	}

	Custom_OLED_Clear();
	Sensor_Stop();
}














void Motor_Test_Phase() {
	uint8_t	sw = 0;
	uint8_t	stateL = 0;
	uint8_t	stateR = 0;

	static Custom_GPIO_t	motorL[4] = {
			{ Motor_L1_GPIO_Port, Motor_L1_Pin },
			{ Motor_L3_GPIO_Port, Motor_L3_Pin },
			{ Motor_L2_GPIO_Port, Motor_L2_Pin },
			{ Motor_L4_GPIO_Port, Motor_L4_Pin },
	};

	static Custom_GPIO_t	motorR[4] = {
			{ Motor_R1_GPIO_Port, Motor_R1_Pin },
			{ Motor_R3_GPIO_Port, Motor_R3_Pin },
			{ Motor_R2_GPIO_Port, Motor_R2_Pin },
			{ Motor_R4_GPIO_Port, Motor_R4_Pin },
	};

	static uint8_t	phases[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };

	/*
	 * 모터의 각 상을 잠깐씩 잡아본다.
	 * 모터에 무리를 주지 않기 위해 100ms 동안만 상을 잡은 후 바로 놓는다.
	 */
	Custom_OLED_Clear();
	Custom_OLED_Printf("/0phaseL: %1x", stateL);
	Custom_OLED_Printf("/1phaseR: %1x", stateR);
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

		if (sw == CUSTOM_SW_1) {
			Custom_GPIO_Set_t(motorL + 0, 0x01 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 1, 0x02 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 2, 0x04 & phases[stateL]);
			Custom_GPIO_Set_t(motorL + 3, 0x08 & phases[stateL]);

			Custom_Delay_ms(100);
			Motor_Power_Off();

			Custom_OLED_Printf("/0phaseL: %1x", stateL);
			stateL = (stateL + 1) & 0x07;
		}

		else if (sw == CUSTOM_SW_2) {
			Custom_GPIO_Set_t(motorR + 0, 0x01 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 1, 0x02 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 2, 0x04 & phases[stateR]);
			Custom_GPIO_Set_t(motorR + 3, 0x08 & phases[stateR]);

			Custom_Delay_ms(100);
			Motor_Power_Off();

			Custom_OLED_Printf("/1phaseR: %1x", stateR);
			stateR = (stateR + 1) & 0x07;
		}
	}

	Custom_OLED_Clear();
}








void Motor_Test_Velocity() {
	uint8_t		sw = 0;
	float		speed = MIN_SPEED_INIT;
	/*
	 * 모터 속도를 부드럽게 올렸다가 내리기를 반복한다.
	 */
	accele = ACCELE_INIT;
	maxSpeed = MAX_SPEED_INIT;
	minSpeed = MIN_SPEED_INIT;

	Motor_Start();
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Motor_L_Speed_Control(speed);
		Motor_R_Speed_Control(speed);
		if ( (speed + accele > maxSpeed) || (speed + accele < minSpeed) ) {
			accele *= -1;
		}
		Custom_Delay_ms(1);
		speed += accele;
	}
	Motor_Stop();
}














void Drive_Test_Position() {
	uint8_t	sw = 0;

	Custom_OLED_Clear();
	Sensor_Start();
	Accele_Control_Start();

	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Custom_OLED_Printf("/0pos: %10d", positionVal);
		Custom_OLED_Printf("/2speedL: %5f", (1 + positionVal * positionCoef));
		Custom_OLED_Printf("/3speedR: %5f", (1 - positionVal * positionCoef));
	}

	Accele_Control_Stop();
	Sensor_Stop();
	Custom_OLED_Clear();
}




void Drive_Test_Info_Oled() {
	Custom_OLED_Printf("/0target: %5f", targetSpeed);
	Custom_OLED_Printf("/1current: %5f", currentSpeed);
	Custom_OLED_Printf("/2accele: %5f", accele);
	Custom_OLED_Printf("/3speedL: %5f", currentSpeed * (1 + positionVal * positionCoef));
	Custom_OLED_Printf("/4speedR: %5f", currentSpeed * (1 - positionVal * positionCoef));
	Custom_OLED_Printf("/decision: %5d", curDecisionState);
}





void Drive_Test_Data() {
	uint8_t sw = 0;
	uint8_t count = 0;
	uint8_t markCnt = 2;

	for (volatile t_driveData *ptr = (driveData + 0); ptr->isExist != CUSTOM_FALSE; ptr += 1) {

		// 현재상태가 좌측 곡선인 경우
		if (ptr->decisionState == DECISION_CURVE_L) {

			// 다음 상태가 우측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->decisionState == DECISION_CURVE_R) {
				markCnt += 1;
			}
			else {
				markCnt += 2;
			}
		}

		// 현재상태가 우측 곡선인 경우
		else if (ptr->decisionState == DECISION_CURVE_R) {
			// 다음 상태가 좌측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->decisionState == DECISION_CURVE_L) {
				markCnt += 1;
			}
			else {
				markCnt += 2;
			}
		}

		else if (ptr->decisionState == DECISION_END_MARK) {
			markCnt += 2;
		}
	}

	Custom_OLED_Clear();

	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

		if (sw == CUSTOM_SW_1) {

			// 첫번째 인덱스가 아닌 경우에만 카운트 감소
			if (count != 0) {
				count--;
			}
		}
		else if (sw == CUSTOM_SW_2) {

			// 마지막 인덱스가 아닌 경우에 카운트 증가
			if ((driveData + count)->isExist != CUSTOM_FALSE) {
				count++;
			}
		}

		// OLED에 변수명 변수값 출력
		Custom_OLED_Printf("/0markCnt:");
		Custom_OLED_Printf("/1%d", markCnt);
		Custom_OLED_Printf("/2tickCnt:  %d");
		Custom_OLED_Printf("/3%d", driveData[count].tickCnt);
		Custom_OLED_Printf("/4decision: %d");
		Custom_OLED_Printf("/5%d", driveData[count].decisionState);
	}
	Custom_OLED_Clear();
}



