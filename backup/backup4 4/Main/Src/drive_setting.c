/*
 * drive_setting.c
 */

#include "header_init.h"


// 초기의 속도 값에 관한 변수
volatile float			targetSpeed_init = TARGET_SPEED_INIT;
volatile float			decele_init = DECELE_INIT;


// 좌우 모터 포지션에 관한 변수
volatile int32_t		positionVal = 0;
volatile float			positionCoef = POSITION_COEF_INIT;




// 주행 중 변하는 속도 값에 관한 변수
volatile float			accele = ACCELE_INIT;
volatile float			decele = DECELE_INIT;

volatile float			targetSpeed = TARGET_SPEED_INIT;
volatile float			currentSpeed = MIN_SPEED;
volatile float			boostSpeed = BOOST_SPEED_INIT;

volatile uint32_t		curveDecelCoef = CURVE_DECEL_COEF_INIT;



//end mark를 몇 번 봤는지 카운트하는 변수
volatile uint8_t		endMarkCnt = 0;


// 현재 직진인지 커브인지 등을 저장하는 변수
volatile uint8_t		markState = MARK_STRAIGHT;


// 현재 모터에 몇번 상이 잡혔는 지를 카운트하는 변수
volatile uint32_t		curTick = 0;


// 시간 측정 (500us)
volatile uint32_t		curTime = 0;


// driveData를 저장하고 접근하게 해주는 변수들
volatile t_driveData	driveData[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataPtr = driveData + 0;


// state machine 의 상태
volatile uint8_t		driveState = DRIVE_STATE_IDLE;


// boost 컨트롤 변수
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

			{ "threshold",		&threshold,			5 },
			{ "curveDecelCoef",	&curveDecelCoef,	1000 },
	};
	uint8_t intValCnt = sizeof(intValues) / sizeof(t_driveMenu_Int);



	t_driveMenu_Float	floatValues[] = {

			{ "pit in",			&pitInLen,			0.01 },
			{ "targetSpeed",	&targetSpeed_init,	0.05 },
			{ "boostSpeed",		&boostSpeed,		0.1 },
			{ "accele",			&accele,			0.1 },
			{ "decele",			&decele_init,		0.1 },
			{ "positionCoef",	&positionCoef,		0.00001 },
	};
	uint8_t floatValCnt = sizeof(floatValues) / sizeof(t_driveMenu_Float);



	for (uint8_t i = 0; i < intValCnt; i++) {

		Custom_OLED_Clear();

		if (intValues[i].val == &threshold) {
			Sensor_Start();
		}

		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/0%s", intValues[i].valName);
			Custom_OLED_Printf("/1%5d", *(intValues[i].val));

			if (intValues[i].val == &threshold) {
				Custom_OLED_Printf("/3%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w%2x/r%2x/w", \
					(state >> 0) & 1, (state >> 1) & 1, (state >> 2) & 1, (state >> 3) & 1, \
					(state >> 4) & 1, (state >> 5) & 1, (state >> 6) & 1, (state >> 7) & 1);
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

		if (intValues[i].val == &threshold) {
			Sensor_Stop();
		}
	}



	for (uint8_t i = 0; i < floatValCnt; i++) {
		Custom_OLED_Clear();
		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/0%s", floatValues[i].valName);
			Custom_OLED_Printf("/1%5f", *(floatValues[i].val));

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

	// 속도 관련 변수 초기화
	decele = decele_init;

	targetSpeed = targetSpeed_init;
	currentSpeed = MIN_SPEED;

	// 엔드마크 읽은 개수 초기화
	endMarkCnt = 0;

	// 현재 마크 인식 상태를 직선 주행으로 초기화
	markState = MARK_STRAIGHT;

	// 현재 모터가 상을 잡은 횟수 초기화
	curTick = 0;

	// 500us 단위의 타이머 업데이트
	curTime = 0;

	// driveData에 접근하는 포인터 0번 인덱스로 초기화
	driveDataPtr = driveData + 0;

	// state machine 의 상태 업데이트
	driveState = DRIVE_STATE_IDLE;

	// 부스트 컨트롤 상태 업데이트
	boostCntl = BOOST_CNTL_IDLE;

	// 현재까지 읽은 크로스 개수 업데이트
	crossCnt = 0;


	// 1차 주행에서만 초기화할 변수
	if (driveIdx == FIRST_DRIVE) {

		// driveData 값 초기화
		for (uint16_t i = 0; i < MAX_MARKER_CNT; i++) {
			t_driveData temp = T_DRIVE_DATA_INIT;

			driveData[i] = temp;
		}

		// driveData의 0번째 값 초기화 (0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠)
		driveData[0].markState = MARK_STRAIGHT;
		driveData[0].isExist = CUSTOM_TRUE;
	}

	// 2차 주행에서만 초기화할 변수
	else if (driveIdx == SECOND_DRIVE) {

		// driveData 값 초기화
		for (uint16_t i = 1; i < MAX_MARKER_CNT; i++) {
			driveData[i].isReadAllMark = CUSTOM_FALSE;
		}

		// driveData의 0번째 isReadAllMark 값 정상으로 변경
		driveData[0].isReadAllMark = CUSTOM_TRUE;
	}
}





void After_Drive_Setting(uint8_t driveIdx) {

	// 1차 주행
	if (driveIdx == FIRST_DRIVE) {

		for (volatile t_driveData *p = driveData + 1; p->isExist == CUSTOM_TRUE; p += 1) {

			// 직선일 경우
			if (p->markState == MARK_STRAIGHT) {

				// MIN_BOOST_METER  * TICK_PER_M 이상 이동한 경우에 INSTRUCT_BOOST로 업데이트
				if ( p->tickCnt - (p-1)->tickCnt > MIN_BOOST_METER * TICK_PER_M ) {
					p->boostTick = p->tickCnt - (p-1)->tickCnt;
				}
				else {
					p->boostTick = 0;
				}
			}
		}
	}

	// 2차 주행
	else if (driveIdx == SECOND_DRIVE) {

	}
}



