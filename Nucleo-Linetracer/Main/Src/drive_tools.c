/*
 * drive_tools.c
 */

#include "header_init.h"




volatile float		targetSpeed_init = TARGET_SPEED_INIT;
volatile float		currentSpeed_init = MIN_SPEED_INIT;
volatile float		minSpeed_init = MIN_SPEED_INIT;
volatile float		maxSpeed_init = MAX_SPEED_INIT;
volatile float		accele_init = ACCELE_INIT;
volatile float		curveDecele_init = CURVE_DECELE_INIT;


volatile int32_t	positionVal = 0;
volatile float		positionCoef = POSITION_COEF_INIT;

volatile float		targetSpeed;
volatile float		currentSpeed;
volatile float		minSpeed;
volatile float		maxSpeed;
volatile float		accele;
volatile float		curveDecele;

volatile int8_t		driveIdx;





void Accele_Control_Start(){
	LL_TIM_EnableCounter(TIM9);
	LL_TIM_EnableIT_UPDATE(TIM9);
}

void Accele_Control_Stop(){
	LL_TIM_DisableIT_UPDATE(TIM9);
	LL_TIM_DisableCounter(TIM9);
	positionVal = 0;
}





// 500us마다 호출됨.
void Drive_TIM9_IRQ() {
	static uint8_t	i = 0;
	static uint8_t	positionIdxMax = 5;
	static uint8_t	positionIdxMin = 2;
	static int32_t	positionBuffer = 0;
	static int32_t	sensorNormValsSum = 1;
	static int32_t	getPositionCoef[8] = { -14000, -10000, -6000, -2000, 2000, 6000, 10000, 14000 };




	// 가속도 조절
	if (targetSpeed > currentSpeed) {
		currentSpeed += accele / 2000;
		if (targetSpeed < currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}
	else {
		currentSpeed -= accele / 2000;
		if (targetSpeed > currentSpeed) {
			currentSpeed = targetSpeed;
		}
	}


	//현재의 읽은 센서 위치에서 4개만 선별
	positionIdxMax = 5;
	positionIdxMin = 2;
	if (positionVal < -2000) {
		positionIdxMax = 4;
		positionIdxMin = 1;
	}
	else if (positionVal > 2000){
		positionIdxMax = 6;
		positionIdxMin = 3;
	}

	for (i = positionIdxMin; i != positionIdxMax + 1; i++) {
		positionBuffer += getPositionCoef[i] * sensorNormVals[i];
		sensorNormValsSum += sensorNormVals[i];
	}

	//divide by zero 방지하기 위해 sensorNormValsSum + 1로 나눔
	positionVal = positionBuffer / (sensorNormValsSum + 1);
	positionBuffer = 0;
	sensorNormValsSum = 0;

	//position 값애 따른 좌우 모터 속도 조정
	Motor_L_Speed_Control( currentSpeed * (1 + positionCoef * positionVal) );
	Motor_R_Speed_Control( currentSpeed * (1 - positionCoef * positionVal) );
}





// 주행 전 초기값 대입
__STATIC_INLINE void Drive_Setting_Var() {

	// 속도 관련 변수 초기화
	accele = accele_init;
	targetSpeed = targetSpeed_init;
	maxSpeed = maxSpeed_init;
	minSpeed = minSpeed_init;
	curveDecele = curveDecele_init;

	currentSpeed = minSpeed_init;


	// 엔드마크 읽은 개수 초기화
	endMarkCnt = 0;

	// 현재 마크 인식 상태를 직선 주행으로 초기화
	curDecisionState = DESISION_STRAIGHT;

	// 좌우모터 포지션 값을 0으로 초기화
	positionVal = 0;

	// 현재 모터가 상을 잡은 횟수 초기화
	curTick = 0;

	// 부스트 판단 값 초기화
	isBoost = CUSTOM_FALSE;

	// driveData에 접근하는 포인터 0번 인덱스로 초기화 (0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠)
	driveDataPtr = driveData + 0;

	// 1차 주행에서만 초기화할 변수
	if (driveIdx == DRIVE_FIRST) {

		// driveData 값 초기화
		for (uint8_t i = 0; i < MAX_MARKER_CNT; i++) {
			t_driveData temp = T_DRIVE_DATA_INIT;

			driveData[i] = temp;
		}

		// driveData의 0번째 값 초기화 (0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠)
		driveData[0].decisionState = DESISION_STRAIGHT;
		driveData[0].isExist = CUSTOM_TRUE;
		driveData[0].tickCnt = 0;
	}

	// 2차 주행에서만 초기화할 변수
	else {
		// driveData 값 초기화
		for (uint8_t i = 0; i < MAX_MARKER_CNT; i++) {
			driveData[i].isReadAllMark = CUSTOM_FALSE;
		}

		// driveData의 0번째 isReadAllMark 값 정상으로 변경
		driveData[0].isReadAllMark = CUSTOM_TRUE;
	}
}




//주행 전 상수값 변경 절차
void Drive_Setting() {
	uint8_t	sw = 0;


	t_driveMenu	values[] = {
			{ "accele      ", &accele_init, 0.1},
			{ "targetSpeed ", &targetSpeed_init, 0.1 },
			{ "max speed   ", &maxSpeed_init, 0.1 },
			{ "min speed   ", &minSpeed_init, 0.1 },
			{ "curve decele", &curveDecele_init, 0.1 },
	};
	uint8_t valCnt = sizeof(values) / sizeof(t_driveMenu);


	for (uint8_t i = 0; i < valCnt; i++) {
		Custom_OLED_Clear();
		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/0%s", values[i].valName);
			Custom_OLED_Printf("/1%5f", *(values[i].val));

			// 변수 값 빼기
			if (sw == CUSTOM_SW_1) {
				*(values[i].val) -= values[i].changeVar;
			}
			// 변수값 더하기
			else if (sw == CUSTOM_SW_2) {
				*(values[i].val) += values[i].changeVar;
			}
		}
	}

	Drive_Setting_Var();

	Custom_OLED_Clear();
}



