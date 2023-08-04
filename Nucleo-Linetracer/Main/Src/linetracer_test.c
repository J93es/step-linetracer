/*
 * linetracer_test.c
 */

#include "header_init.h"







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
		Custom_OLED_Printf("/0threshold: %3d", threshold);

		Custom_OLED_Printf("/1%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
			(state >> 0) & 1, (state >> 1) & 1, (state >> 2) & 1, (state >> 3) & 1, \
			(state >> 4) & 1, (state >> 5) & 1, (state >> 6) & 1, (state >> 7) & 1);


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
	uint8_t	phaseL = 0;
	uint8_t	phaseR = 0;

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
	Custom_OLED_Printf("/0phaseL: %1x", phaseL);
	Custom_OLED_Printf("/1phaseR: %1x", phaseR);
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

		if (sw == CUSTOM_SW_1) {
			Custom_GPIO_Set_t(motorL + 0, (phases[7 - phaseL] >> 0) & 0x01);
			Custom_GPIO_Set_t(motorL + 1, (phases[7 - phaseL] >> 1) & 0x01);
			Custom_GPIO_Set_t(motorL + 2, (phases[7 - phaseL] >> 2) & 0x01);
			Custom_GPIO_Set_t(motorL + 3, (phases[7 - phaseL] >> 3) & 0x01);

			Custom_Delay_ms(100);
			Motor_Power_Off();

			Custom_OLED_Printf("/0phaseL: %1x", phaseL);
			phaseL = (phaseL + 1) & 0x07;
		}

		else if (sw == CUSTOM_SW_2) {
			Custom_GPIO_Set_t(motorR + 0, (phases[phaseR] >> 0) & 0x01);
			Custom_GPIO_Set_t(motorR + 1, (phases[phaseR] >> 1) & 0x01);
			Custom_GPIO_Set_t(motorR + 2, (phases[phaseR] >> 2) & 0x01);
			Custom_GPIO_Set_t(motorR + 3, (phases[phaseR] >> 3) & 0x01);

			Custom_Delay_ms(100);
			Motor_Power_Off();

			Custom_OLED_Printf("/1phaseR: %1x", phaseR);
			phaseR = (phaseR + 1) & 0x07;
		}
	}

	Custom_OLED_Clear();
}








void Motor_Test_Velocity() {
	uint8_t		sw = 0;
	float		speed = MIN_SPEED;
	float		maxSpeed = 2.5;
	float		minSpeed = 1.5;
	/*
	 * 모터 속도를 부드럽게 올렸다가 내리기를 반복한다.
	 */
	accele = ACCELE_INIT;

	Motor_Start();
	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Motor_L_Speed_Control(speed);
		Motor_R_Speed_Control(speed);
		if ( (speed + accele / 2000 > maxSpeed) || (speed + accele / 2000 < minSpeed) ) {
			accele *= -1;
		}
		Custom_Delay_ms(1);
		speed += accele / 2000;
	}
	Motor_Stop();
}














void Drive_Test_Position() {
	uint8_t	sw = 0;

	Custom_OLED_Clear();
	Sensor_Start();

	while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {
		Update_Position_Val();

		Custom_OLED_Printf("/0pos: %10d", positionVal);
		Custom_OLED_Printf("/2speedL: %5f", (1 + positionVal * positionCoef));
		Custom_OLED_Printf("/3speedR: %5f", (1 - positionVal * positionCoef));
	}

	Sensor_Stop();
	Custom_OLED_Clear();
}




void Drive_Test_Info_Oled() {
	Custom_OLED_Printf("/0target: %5f", targetSpeed);
	Custom_OLED_Printf("/1current: %5f", currentSpeed);
	Custom_OLED_Printf("/2accele: %5f", accele);
	Custom_OLED_Printf("/3speedL: %5f", currentSpeed * (1 + positionVal * positionCoef));
	Custom_OLED_Printf("/4speedR: %5f", currentSpeed * (1 - positionVal * positionCoef));
	Custom_OLED_Printf("/decision: %5d", markState);
}





void Drive_Test_First_Data() {
	uint16_t markCnt_L = 0;
	uint16_t markCnt_R = 0;
	uint16_t markCnt_End = 2;
	uint16_t markCnt_Cross = 0;

	for (volatile t_driveData *ptr = (driveData + 0); ptr->isExist == CUSTOM_TRUE; ptr += 1) {

		// 현재상태가 좌측 곡선인 경우
		if (ptr->markState == MARK_CURVE_L) {

			// 다음 상태가 우측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->markState == MARK_CURVE_R) {
				markCnt_L += 1;
			}
			else {
				markCnt_L += 2;
			}
		}

		// 현재상태가 우측 곡선인 경우
		else if (ptr->markState == MARK_CURVE_R) {
			// 다음 상태가 좌측 곡선이었을 경우 == 연속 커브
			if ((ptr + 1)->markState == MARK_CURVE_L) {
				markCnt_R += 1;
			}
			else {
				markCnt_R += 2;
			}
		}

		else if (ptr->markState == MARK_END) {
			markCnt_End += 2;
		}
	}

	// 크로스
	markCnt_Cross = crossCnt;


	// OLED에 변수명 변수값 출력
	Custom_OLED_Clear();
	Custom_OLED_Printf("/0mark L:   %d", markCnt_L);
	Custom_OLED_Printf("/1mark R:   %d", markCnt_R);
	Custom_OLED_Printf("/2cross:    %d", markCnt_Cross);
	Custom_OLED_Printf("/3end mark: %d", markCnt_End);

	while (CUSTOM_SW_BOTH != Custom_Switch_Read());

	Custom_OLED_Clear();
}



