/*
 * drive_preset.c
 */

#include "header_init.h"



/*
 * 인터럽트에서 쓰는 변수
 */


// 초기의 속도 값에 관한 변수
volatile float			targetSpeed_init = TARGET_SPEED_INIT;
volatile float			accele_init = ACCELE_INIT;
volatile float			decele_init = DECELE_INIT;


// 좌우 모터 포지션에 관한 변수
volatile int32_t		positionVal = 0;
volatile int32_t		absPositionVal = 0;
volatile float			positionCoef = POSITION_COEF_INIT;
volatile int32_t		limitedPositionVal = 0;

volatile int32_t		positionTable[8] = { -1400, -1000, -600, -200, 200, 600, 1000, 1400 };




// 속도 값에 관한 변수
volatile float			targetAccele = ACCELE_INIT;
volatile float			curAccele = ACCELE_INIT;
volatile float			decele = DECELE_INIT;

volatile float			targetSpeed = TARGET_SPEED_INIT;
volatile float			curSpeed = MIN_SPEED;
volatile float			boostSpeed = BOOST_SPEED_INIT;

volatile float			curveDecelCoef = CURVE_DECEL_COEF_INIT;


// 현재 모터에 몇번 상이 잡혔는 지를 카운트하는 변수
volatile uint32_t		curTick = 0;

// 라인 아웃 시간
volatile uint32_t		lineOutTime = 0;







/*
 * 주행문에서 쓰는 변수
 */


// 현재 직진인지 커브인지 등을 저장하는 변수
volatile uint8_t		markState = MARK_STRAIGHT;


// state machine 의 상태
volatile uint8_t		driveState = DRIVE_STATE_IDLE;


// 2차주행 컨트롤 변수
volatile uint8_t		boostCntl = BOOST_CNTL_IDLE;


// 2차주행에서 마크를 정확히 읽었는지 판단
volatile uint8_t		isReadAllMark = CUSTOM_TRUE;


// driveData를 저장하고 접근하게 해주는 변수들
volatile t_driveData	driveData[MAX_DRIVE_DATA_LEN] = { T_DRIVE_DATA_INIT, };


// 1차 주행 데이터 임시저장
volatile t_driveData	driveDataBuffer[MAX_DRIVE_DATA_LEN] = { T_DRIVE_DATA_INIT, };


// driveData 인덱스
volatile uint32_t		driveDataIdx = 0;


// 2차 주행에서 사용하는 cross 테이블
/*
 *    n번째 크로스(crossCnt)		0		1		...		50
 *    m번째 마크(driveDataIdx)		4(3)	6(5)	...		98
 *
 *    (0번째 마크에서 크로스를 읽었을 때 1번째 마크로 저장되도록 함, 0은 값이 없는 상태를 나타냄)
 */
volatile uint16_t		crossCntTable[MAX_CROSS_CNT] = { 0, };


// 1차 주행에서 cross 테이블 임시 저장
volatile uint16_t		crossCntTableBuffer[MAX_CROSS_CNT] = { 0, };


// 현재까지 읽은 크로스 개수
volatile uint16_t		crossCnt = 0;


// 현재 마크가 시작된 tick
volatile uint32_t		markStartTick = 0;


//end mark를 몇 번 봤는지 카운트하는 변수
volatile uint8_t		endMarkCnt = 0;


// 피트인 거리
volatile float			pitInLen = PIT_IN_LEN_INIT;


// state machine 에서 사용
//센서 값 누적
volatile uint8_t		sensorStateSum;

// 라인아웃 시간 누적
volatile uint32_t		lineOutStartTime;





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
			{ "Boost Speed",		&boostSpeed,		0.25f },
			{ "Accele",				&accele_init,		0.25f },
			{ "Decele",				&decele_init,		0.5f },
			{ "CurveDecel Coef",	&curveDecelCoef,	100 },
			{ "Position Coef",		&positionCoef,		0.00001f },
	};
	uint8_t floatValCnt = sizeof(floatValues) / sizeof(t_driveMenu_Float);



	for (uint8_t i = 0; i < intValCnt; i++) {

		Custom_OLED_Clear();

		// 정수 변수 초기화
		if (i < intValCnt) {

//			if (i == 0) {
//				Sensor_Start();
//			}

			while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

				// OLED에 변수명 변수값 출력
				Custom_OLED_Printf("/2%s", intValues[i].valName);
				Custom_OLED_Printf("/A/4%d", *(intValues[i].val));

//				if (i == 0) {
//					Custom_OLED_Printf("/0%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w",
//						(state >> 7) & 1, (state >> 6) & 1, (state >> 5) & 1, (state >> 4) & 1,
//						(state >> 3) & 1, (state >> 2) & 1, (state >> 1) & 1, (state >> 0) & 1);
//				}

				// 변수 값 빼기
				if (sw == CUSTOM_SW_1) {
					*(intValues[i].val) -= intValues[i].changeVal;
				}
				// 변수값 더하기
				else if (sw == CUSTOM_SW_2) {
					*(intValues[i].val) += intValues[i].changeVal;
				}
			}

//			if (i == 0) {
//				Sensor_Stop();
//			}
		}
	}


	// float 변수 초기화
	for (uint8_t i = 0; i < floatValCnt; i++) {

		Custom_OLED_Clear();

		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			uint32_t num1 = (uint32_t)(*(floatValues[i].val));
			uint32_t num2 = (uint32_t)( *(floatValues[i].val) * 100000 - num1 * 100000 );

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/2%s", floatValues[i].valName);
			Custom_OLED_Printf("/A/4%u.%05u", num1, num2);

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


	/*
	 * 인터럽트에서 쓰는 변수
	 */

	// 가속도 변수 초기화
	targetAccele = accele_init;
	curAccele = 0;

	// 속도 관련 변수 초기화
	targetSpeed = targetSpeed_init;
	decele = decele_init;
	curSpeed = MIN_SPEED;

	// 좌우모터 포지션 값을 0으로 초기화
	positionVal = 0;
	absPositionVal = 0;
	limitedPositionVal = 0;

	// 현재 모터가 상을 잡은 횟수 초기화
	curTick = 0;

	// 라인 아웃 시간 계산
	lineOutTime = 0;



	/*
	 * 주행문에서 쓰는 변수
	 */

	// 현재 마크 인식 상태를 직선 주행으로 초기화
	markState = MARK_STRAIGHT;

	// state machine 의 상태 업데이트
	driveState = DRIVE_STATE_IDLE;

	// 현재까지 읽은 크로스 개수 업데이트
	crossCnt = 0;

	// 현재 마크가 시작된 tick
	markStartTick = 0;

	// 엔드마크 읽은 개수 초기화
	endMarkCnt = 0;

	// driveData 인덱스 초기화
	driveDataIdx = 0;



	// 1차 주행에서만 초기화할 변수
	if (driveIdx == FIRST_DRIVE) {

		for (uint32_t i = 0; i < MAX_DRIVE_DATA_LEN; i++) {
			t_driveData temp = T_DRIVE_DATA_INIT;

			driveDataBuffer[i] = temp;
		}

		// driveData의 0, 1번째 값 초기화
		// 0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠
		// 실질적으로 주행은 1번 인덱스부터 시작
		driveDataBuffer[0].markState = MARK_STRAIGHT;


		for (uint32_t i = 0; i < MAX_CROSS_CNT; i++) {

			crossCntTableBuffer[i] = 0;
		}
	}

	// 2차 주행에서만 초기화할 변수
	else if (driveIdx == SECOND_DRIVE) {

		// isReadAllMark 값 정상으로 변경
		isReadAllMark = CUSTOM_TRUE;

		// 부스트 컨트롤 상태 업데이트
		boostCntl = BOOST_CNTL_IDLE;
	}
}


