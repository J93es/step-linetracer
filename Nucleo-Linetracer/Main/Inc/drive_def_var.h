/*
 * speed_def_var.h
 */


#ifndef INC_DRIVE_DEF_VAR_H_
#define INC_DRIVE_DEF_VAR_H_


#include <stdint.h>
#include <stdbool.h>


// 공용 매크로
#define CUSTOM_FALSE				0
#define CUSTOM_TRUE					1
#define ABS(x) 						( ((x) > 0) ? (x) : (-1 * (x)))


// 1차 주행인지 2차주행 판단 매크로
#define DRIVE_FIRST					0
#define DRIVE_SECOND				1

// state machine에서 나온 상태
#define DRIVE_STATE_IDLE			0
#define DRIVE_STATE_CROSS			1
#define DRIVE_STATE_MARKER			2
#define DRIVE_STATE_DECISION		3

// 현재 decision의 상태값 매크로
#define DECISION_STRAIGHT			0
#define DECISION_END_MARK			1
#define DECISION_CURVE_R			2
#define DECISION_CURVE_L			3


// 속도와 관련된 매크로
#define MAX_SPEED_INIT				2.4f
#define MIN_SPEED_INIT				0.1f
#define ACCELE_INIT					4.0f

#define TARGET_SPEED_INIT			1.5f

#define BOOST_SPEED_INIT			2.0f

#define SPEED_INIT_CHANGE_VAL		0.1f

// 커브에서 어느정도 감속할지 결정하는 코드
#define CURVE_DECEL_COEF			22000
#define CURVE_DECEL_COEF_CHANGE_VAL	1000


// POSITION_COEF(포지션 상수)를 도출하기 위한 매크로
#define TIRE_RADIUS					0.025f					// m
#define	SPEED_COEF					15707.f * TIRE_RADIUS
#define POSITION_COEF_INIT			0.0001f
#define POSITION_COEF_CHANGE_VAL	0.00001f
/*
 * (2 * l(m) * 3.14159) / (t(s) * 400) = v(m/s) * (arr+1)
 *
 * t =  1 / 1Mhz = 1 / 1,000,000 = 타이머 주기
 * 1 / (t * 400) = 2,500
 *
 * l(m) = 타이어 반지름
 * 2 * l * 3.14159
 *
 * v * (arr + 1) = SPEED_COEF
 */


// 1 m 당 tick 개수
#define TICK_PER_M	400 * 1 / (2 * TIRE_RADIUS * 3.14159)
/*
 * 400(바퀴가 1바퀴 도는데 소요되는 tick 개수) * { 1(m) / (2 * TIRE_RADIUS * 3.14159)(1바퀴의 거리) }{ 1m 가는데 소요되는 바퀴 회전 횟수 }
 */



// 1차주행, 2차 주행의 driveData 관련 매크로
#define MAX_MARKER_CNT				1000
#define T_DRIVE_DATA_INIT			{ CUSTOM_FALSE, DECISION_STRAIGHT, INSTRUCT_NORMAL, CUSTOM_FALSE, 0 }


// 2차 주행에서 가속할지 말지를 판단하는 매크로
#define INSTRUCT_NORMAL				0
#define INSTRUCT_BOOST				1

// 최소 몇 미터 이상에서 부스트할지를 저장한 매크로
#define MIN_BOOST_METER				3


// 2차 주행에서 어느 정도 지나면 가감속할 지 결정하는 코드
// 예를 들어 10의 거리가 주어졌을 때 DECELE_POINT_RATIO가 0.7이라면 7에서 부스트 종료 후 감속
#define ACCELE_POINT_RATIO			0.3f
#define DECELE_POINT_RATIO			0.8f







// 1차주행, 2차 주행의 driveData 구조체
typedef struct	s_driveData {
		volatile uint8_t	isExist;
		volatile uint8_t	decisionState;
		volatile uint8_t	instruct;
		volatile uint8_t	isReadAllMark;
		volatile uint32_t	tickCnt;
}				t_driveData;






// 현재 1차주행, 2차주행의 여부를 저장하는 변수
extern volatile int8_t		driveIdx;


// 초기의 속도 값에 관한 변수
extern volatile float		targetSpeed_init;

extern volatile float		minSpeed_init;
extern volatile float		maxSpeed_init;
extern volatile float		accele_init;

extern volatile float		boostSpeed_init;


// 좌우 모터 포지션에 관한 변수
extern volatile int32_t		positionVal;
extern volatile float		positionCoef;


// 주행 중 변하는 속도 값에 관한 변수
extern volatile float		targetSpeed;
extern volatile float		currentSpeed;

extern volatile float		minSpeed;
extern volatile float		maxSpeed;
extern volatile float		accele;

extern volatile float		boostSpeed;

extern volatile uint32_t	curveDecelCoef;


//end mark를 몇 번 봤는지 카운트하는 변수
extern volatile uint8_t		endMarkCnt;


// 현재 직진인지 커브인지 등을 저장하는 변수
extern volatile uint8_t		curDecisionState;


// 현재 모터에 몇번 상이 잡혔는지를 카운트하는 변수
extern volatile uint32_t	curTick;


// 부스트 중인지 저징하는 변수
extern volatile uint8_t		isBoost;


// driveData를 저장하고 접근하게 해주는 변수들
extern volatile t_driveData	*driveDataPtr;
extern volatile t_driveData	driveData[MAX_MARKER_CNT];


// state machone 의 상태
extern volatile uint8_t		driveState;



#endif //INC_DRIVE_DEF_VAR_H_
