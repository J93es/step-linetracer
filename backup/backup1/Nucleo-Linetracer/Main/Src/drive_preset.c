/*
 * drive_preset.c
 */

#include "header_init.h"


// 초기의 속도 값에 관한 변수
volatile float			minSpeed_init = MIN_SPEED_INIT;
volatile float			maxSpeed_init = MAX_SPEED_INIT;
volatile float			accele_init = ACCELE_INIT;

volatile float			targetSpeed_init = TARGET_SPEED_INIT;

volatile float			boostSpeed_init = BOOST_SPEED_INIT;



// 좌우 모터 포지션에 관한 변수
volatile int32_t		positionVal = 0;
volatile float			positionCoef = POSITION_COEF_INIT;




// 주행 중 변하는 속도 값에 관한 변수
volatile float			targetSpeed;
volatile float			currentSpeed;

volatile float			minSpeed;
volatile float			maxSpeed;
volatile float			accele;

volatile float			boostSpeed;

volatile uint32_t		curveDecelCoef = CURVE_DECEL_COEF;



//end mark를 몇 번 봤는지 카운트하는 변수
volatile uint8_t		endMarkCnt = 0;

// 현재 직진인지 커브인지 등을 저장하는 변수
volatile uint8_t		curDecisionState = DECISION_STRAIGHT;

// 현재 모터에 몇번 상이 잡혔는 지를 카운트하는 변수
volatile uint32_t		curTick = 0;

// 부스트 중인지 저징하는 변수
volatile uint8_t		isBoost = CUSTOM_FALSE;

// driveData를 저장하고 접근하게 해주는 변수들
volatile t_driveData	driveData[MAX_MARKER_CNT] = { T_DRIVE_DATA_INIT, };
volatile t_driveData	*driveDataPtr = driveData + 0;


// state machine 의 상태
volatile uint8_t		driveState = DRIVE_STATE_IDLE;







// 주행 전 초기값 대입
__STATIC_INLINE void Drive_Preset_Var() {


	// 좌우모터 포지션 값을 0으로 초기화
	positionVal = 0;


	// 속도 관련 변수 초기화
	targetSpeed = targetSpeed_init;
	currentSpeed = minSpeed_init;

	accele = accele_init;
	maxSpeed = maxSpeed_init;
	minSpeed = minSpeed_init;

	boostSpeed = boostSpeed_init;

	// threshold 초기화
	threshold = (uint8_t)threshold_init;

	// 엔드마크 읽은 개수 초기화
	endMarkCnt = 0;

	// 현재 마크 인식 상태를 직선 주행으로 초기화
	curDecisionState = DECISION_STRAIGHT;


	// 현재 모터가 상을 잡은 횟수 초기화
	curTick = 0;

	// 부스트 판단 값 초기화
	isBoost = CUSTOM_FALSE;

	// driveData에 접근하는 포인터 0번 인덱스로 초기화
	driveDataPtr = driveData + 0;

	// state machine 의 상태 업데이트
	driveState = DRIVE_STATE_IDLE;

	// 1차 주행에서만 초기화할 변수
	if (driveIdx == DRIVE_FIRST) {

		// driveData 값 초기화
		for (uint16_t i = 0; i < MAX_MARKER_CNT; i++) {
			t_driveData temp = T_DRIVE_DATA_INIT;

			driveData[i] = temp;
		}

		// driveData의 0번째 값 초기화 (0번 인덱스는 할당되지 않은 포인터에 접근하지 않도록 고정시켜둠)
		driveData[0].decisionState = DECISION_STRAIGHT;
		driveData[0].isExist = CUSTOM_TRUE;
		driveData[0].tickCnt = 0;
	}

	// 2차 주행에서만 초기화할 변수
	else {
		// driveData 값 초기화
		for (uint16_t i = 0; i < MAX_MARKER_CNT; i++) {
			driveData[i].isReadAllMark = CUSTOM_FALSE;
		}

		// driveData의 0번째 isReadAllMark 값 정상으로 변경
		driveData[0].isReadAllMark = CUSTOM_TRUE;
	}
}




//주행 전 상수값 변경 절차
void Drive_Preset() {
	uint8_t	sw = 0;


	t_driveMenu_Float	floatValues[] = {
			{ "maxSpeed", &maxSpeed_init, SPEED_INIT_CHANGE_VAL },
			{ "minSpeed", &minSpeed_init, SPEED_INIT_CHANGE_VAL },

			{ "targetSpeed", &targetSpeed_init, SPEED_INIT_CHANGE_VAL },

			{ "positionCoef", &positionCoef, POSITION_COEF_CHANGE_VAL },
	};
	uint8_t floatValCnt = sizeof(floatValues) / sizeof(t_driveMenu_Float);


	t_driveMenu_Int		intValues[] = {
			{ "threshold", &threshold_init, THRESHOLD_CHANGE_VAL },
			{ "curveDecelCoef", &curveDecelCoef, CURVE_DECEL_COEF_CHANGE_VAL },
	};
	uint8_t intValCnt = sizeof(intValues) / sizeof(t_driveMenu_Int);



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
	for (uint8_t i = 0; i < intValCnt; i++) {
		Custom_OLED_Clear();
		while (CUSTOM_SW_BOTH != (sw = Custom_Switch_Read())) {

			// OLED에 변수명 변수값 출력
			Custom_OLED_Printf("/0%s", intValues[i].valName);
			Custom_OLED_Printf("/1%5d", *(intValues[i].val));

			// 변수 값 빼기
			if (sw == CUSTOM_SW_1) {
				*(intValues[i].val) -= intValues[i].changeVal;
			}
			// 변수값 더하기
			else if (sw == CUSTOM_SW_2) {
				*(intValues[i].val) += intValues[i].changeVal;
			}
		}
	}
	Custom_OLED_Clear();


	Drive_Preset_Var();

}



