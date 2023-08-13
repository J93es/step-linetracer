/*
 * drive_preset.c
 */

#include "header_init.h"


// 초기의 속도 값에 관한 변수
volatile float			targetSpeed_init = TARGET_SPEED_INIT;
volatile float			decele_init = DECELE_INIT;


// 좌우 모터 포지션에 관한 변수
volatile int32_t		positionVal = 0;
volatile int32_t		limitedPositionVal = 0;
volatile int32_t		absPositionVal = 0;
volatile float			positionCoef = POSITION_COEF_INIT;




// 속도 값에 관한 변수
volatile float			accele = ACCELE_INIT;
volatile float			decele = DECELE_INIT;

volatile float			targetSpeed = TARGET_SPEED_INIT;
volatile float			currentSpeed = MIN_SPEED;
volatile float			boostSpeed = BOOST_SPEED_INIT;

volatile float			curveDecelCoef = CURVE_DECEL_COEF_INIT;



//end mark를 몇 번 봤는지 카운트하는 변수
volatile uint8_t		endMarkCnt = 0;


// 현재 직진인지 커브인지 등을 저장하는 변수
volatile uint8_t		markState = MARK_STRAIGHT;


// 현재 모터에 몇번 상이 잡혔는 지를 카운트하는 변수
volatile uint32_t		curTick_L = 0;
volatile uint32_t		curTick_R = 0;
volatile uint32_t		curTick = 0;


// 시간 측정 (500us)
volatile uint32_t		curTime = 0;


// 2차주행에서 마크를 정확히 읽었는지 판단
volatile uint8_t		isReadAllMark = CUSTOM_TRUE;


// driveData를 저장하고 접근하게 해주는 변수들
volatile t_driveData	driveData[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataPtr = driveData + 1;


// 1차 주행 데이터 임시저장
volatile t_driveData	driveDataBuffer[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataBufferPtr = driveDataBuffer + 1;


// state machine 의 상태
volatile uint8_t		driveState = DRIVE_STATE_IDLE;


// 2차주행 컨트롤 변수
volatile uint8_t		boostCntl = BOOST_CNTL_IDLE;


// 현재까지 읽은 크로스 개수
volatile uint16_t		crossCnt = 0;


// 피트인 거리
volatile float			pitInLen = PIT_IN_LEN_INIT;





// 주행 전 초기값 조정
static void Pre_Drive_Var_Adjust();

// 주행 전 초기값 대입
static void Pre_Drive_Var_Init(uint8_t driveIdx);





//주행 전 상수값 변경 절차
void Pre_Drive_Setting(uint8_t driveIdx) {

	Pre_Drive_Var_Adjust();
	Pre_Drive_Var_Init(driveIdx);

}



// 주행 전 초기값 조정
static void Pre_Drive_Var_Adjust() {
	uint8_t	sw = 0;


	t_driveMenu_Int		intValues[] = {

			{ "Threshold",			&threshold,			5 },
	};
	uint8_t intValCnt = sizeof(intValues) / sizeof(t_driveMenu_Int);


	t_driveMenu_Float	floatValues[] = {

			{ "Pit In Len",			&pitInLen,			0.01f },
			{ "Target Speed",		&targetSpeed_init,	0.05f },
			{ "Boost Speed",		&boostSpeed,		0.5f },
			{ "Accele",				&accele,			0.5f },
			{ "Decele",				&decele_init,		0.5f },
			{ "CurveDecel Coef",	&curveDecelCoef,	50 },
			{ "Position Coef",		&positionCoef,		0.00001f },
	};
	uint8_t floatValCnt = sizeof(floatValues) / sizeof(t_driveMenu_Float);



	for (uint8_t i = 0; i < intValCnt; i++) {

		Custom_OLED_Clear();

		// 정수 변수 초기화
		if (i < intValCnt) {

			if (i == 0) {
				Sensor_Start();
			}

			while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

				// OLED에 변수명 변수값 출력
				Custom_OLED_Printf("/2%s", intValues[i].valName);
				Custom_OLED_Printf("/4%d", *(intValues[i].val));

				if (i == 0) {
					Custom_OLED_Printf("/0%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
						(state >> 7) & 1, (state >> 6) & 1, (state >> 5) & 1, (state >> 4) & 1, \
						(state >> 3) & 1, (state >> 2) & 1, (state >> 1) & 1, (state >> 0) & 1);
				}

				// 변수 값 빼기
				if (sw == CUSTOM_SW_1) {
					*(intValues[i].val) -= intValues[i].changeVal;
				}
				// 변수값 더하기
				else if (sw == CUSTOM_SW_2) {
					*(intValues[i].val) += intValues[i].changeVal;
				}
			}

			if (i == 0) {
				Sensor_Stop();
			}
		}
	}


	// float 변수 초기화
	for (uint8_t i = 0; i < floatValCnt; i++) {

		Custom_OLED_Clear();

		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/2%s", floatValues[i].valName);
			Custom_OLED_Printf("/4%f", *(floatValues[i].val));

			if (i == floatValCnt - 1) {
				Custom_OLED_Printf("/g/0Ready to Drive");
			}

			// 변수 값 빼기
			if (sw == CUSTOM_SW_1) {
				*(floatValues[i].val) -= floatValues[i].changeVal;
			}
			// 변수값 더하기
			else if (sw == CUSTOM_SW_2) {
				*(floatValues[i].val) += floatValues[i].changeVal;
			}
		}
	}

	Custom_OLED_Clear();
}


// 주행 전 초기값 대입
static void Pre_Drive_Var_Init(uint8_t driveIdx) {


	// 좌우모터 포지션 값을 0으로 초기화
	positionVal = 0;
	limitedPositionVal = 0;
	absPositionVal = 0;

	// 속도 관련 변수 초기화
	decele = decele_init;

	targetSpeed = targetSpeed_init;
	currentSpeed = MIN_SPEED;

	// 엔드마크 읽은 개수 초기화
	endMarkCnt = 0;

	// 현재 마크 인식 상태를 직선 주행으로 초기화
	markState = MARK_STRAIGHT;

	// 현재 모터가 상을 잡은 횟수 초기화
	curTick_L = 0;
	curTick_R = 0;
	curTick = (curTick_L + curTick_R) >> 2; // (curTick_L + curTick_R) / 2

	// 500us 단위의 타이머 업데이트
	curTime = 0;

	// state machine 의 상태 업데이트
	driveState = DRIVE_STATE_IDLE;

	// 현재까지 읽은 크로스 개수 업데이트
	crossCnt = 0;


	// 1차 주행에서만 초기화할 변수
	if (driveIdx == FIRST_DRIVE) {

		for (uint32_t i = 0; i < MAX_MARKER_CNT; i++) {
			t_driveData temp = T_DRIVE_DATA_INIT;

			driveDataBuffer[i] = temp;
		}

		// driveData의 0, 1번째 값 초기화
		// 0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠
		// 실질적으로 주행은 1번 인덱스부터 시작
		driveDataBuffer[0].markState = MARK_STRAIGHT;
		driveDataBuffer[1].markState = MARK_STRAIGHT;

		// driveDataBuffer에 접근하는 포인터 1번 인덱스로 초기화
		driveDataBufferPtr = driveDataBuffer + 1;

	}

	// 2차 주행에서만 초기화할 변수
	else if (driveIdx == SECOND_DRIVE) {

		// isReadAllMark 값 정상으로 변경
		isReadAllMark = CUSTOM_TRUE;

		// 부스트 컨트롤 상태 업데이트
		boostCntl = BOOST_CNTL_IDLE;

		// driveData에 접근하는 포인터 1번 인덱스로 초기화
		driveDataPtr = driveData + 1;
	}
}



