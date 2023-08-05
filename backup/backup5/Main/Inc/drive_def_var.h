/*
 * drive_def_var.h
 */


#ifndef INC_DRIVE_DEF_VAR_H_
#define INC_DRIVE_DEF_VAR_H_


#include <stdint.h>
#include <stdbool.h>


// 공용 매크로
#define CUSTOM_FALSE				0
#define CUSTOM_TRUE					1
#define ABS(x) 						( ((x) < 0) ? (-1 * (x)) : (x) )


// 1차 주행인지 2차주행 판단 매크로
#define FIRST_DRIVE					0
#define SECOND_DRIVE				1


// state machine에서 나온 상태
#define DRIVE_STATE_IDLE			0
#define DRIVE_STATE_CROSS			1
#define DRIVE_STATE_MARKER			2
#define DRIVE_STATE_DECISION		3


// 현재 mark의 상태값 매크로
#define MARK_STRAIGHT				0
#define MARK_END					1
#define MARK_CURVE_R				2
#define MARK_CURVE_L				3
#define MARK_CROSS					4


// 속도와 관련된 매크로
#define MIN_SPEED					0.01f

#define ACCELE_INIT					5.0f

#define TARGET_SPEED_INIT			1.7f

#define BOOST_SPEED_INIT			3.0f

#define SPEED_INIT_CHANGE_VAL		0.05f


// 커브에서 어느 정도 감속할지 결정하는 매크로
#define CURVE_DECEL_COEF_INIT		22000
#define CURVE_DECEL_COEF_CHANGE_VAL	1000


// POSITION_COEF(포지션 상수)를 도출하기 위한 매크로
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
#define TIRE_RADIUS					0.029f					// m
#define	SPEED_COEF					( 15707.f * TIRE_RADIUS )
#define POSITION_COEF_INIT			0.00007f
#define POSITION_COEF_CHANGE_VAL	0.00001f


// 1 m 당 tick 개수
/*
 * 400(바퀴가 1바퀴 도는데 소요되는 tick 개수) * { 1(m) / (2 * TIRE_RADIUS * 3.14159)(1바퀴의 거리) }{ 1m 가는데 소요되는 바퀴 회전 횟수 }
 */
#define TICK_PER_M					( 400.f * 1.f / (2.f * TIRE_RADIUS * 3.14159f) )


// 1차주행, 2차 주행의 driveData 관련 매크로
#define MAX_MARKER_CNT				1000
#define T_DRIVE_DATA_INIT			{ 0, MARK_STRAIGHT, 0, CUSTOM_FALSE, 0, CUSTOM_FALSE }


// 최소 몇 미터 이상에서 부스트할지를 저장한 매크로
#define MIN_BOOST_METER				0.7f


// 2차 주행에서 어느 정도 지나면 가감속할 지 결정하는 매크로

// 직선에 진입한 후 어느정도 이동한 후 가속할지
#define ACCELE_START_TICK			( 0.1f * TICK_PER_M )

// 가속 거리
#define ACCELE_LEN_TICK				( 0.4f * TICK_PER_M )

// 어느정도 직선이 남았으면 감속할 지
#define DECELE_START_TICK			( 0.5f * TICK_PER_M )

// 감속 거리
#define DECELE_LEN_TICK				( 0.3f * TICK_PER_M )


// 피트인 관련 매크로
#define PIT_IN_LEN					0.22f
#define PIT_IN_TARGET_SPEED			MIN_SPEED
#define PIT_IN_DELAY_SPEED			0.3f
#define DRIVE_END_DELAY				100


// exitEcho 관련 매크로
#define EXIT_ECHO_END_MARK			0
#define EXIT_ECHO_LINE_OUT			1


// 주행 컨트롤 매크로
#define BOOST_CNTL_IDLE				0
#define BOOST_CNTL_ACCELE			1
#define BOOST_CNTL_DECELE			2
#define BOOST_CNTL_END				3






// 1차주행, 2차 주행의 driveData 구조체
typedef struct	s_driveData {

		// 현재 인덱스의 마크가 종료된 시점에서의 curTick 값
		volatile uint32_t	tickCnt;

		// 현재 마크의 상태
		volatile uint8_t	markState;

		// 몇번째 크로스를 보았는지
		// 에를 들어 한 직선에서 30, 31, 32 번째 크로스가 관찰 되었다면 32만 저장
		volatile uint8_t	crossCnt;

		// 현재 인덱스의 구조체가 존재하는지
		volatile uint8_t	isExist;

		// 부스트 할 거리
		volatile uint32_t	boostTick;

		// 마크를 정상적으로 읽고 있는지
		volatile uint8_t	isReadAllMark;
}				t_driveData;







// 초기의 속도 값에 관한 변수
extern volatile float		targetSpeed_init;

extern volatile float		accele_init;

extern volatile float		boostSpeed_init;


// 좌우 모터 포지션에 관한 변수
extern volatile int32_t		positionVal;
extern volatile float		positionCoef;


// 주행 중 변하는 속도 값에 관한 변수
extern volatile float		targetSpeed;
extern volatile float		currentSpeed;

extern volatile float		accele;

extern volatile float		boostSpeed;

extern volatile uint32_t	curveDecelCoef;


//end mark를 몇 번 봤는지 카운트하는 변수
extern volatile uint8_t		endMarkCnt;


// 현재 직진인지 커브인지 등을 저장하는 변수
extern volatile uint8_t		markState;


// 현재 모터에 몇번 상이 잡혔는지를 카운트하는 변수
extern volatile uint32_t	curTick;


// driveData를 저장하고 접근하게 해주는 변수들
extern volatile t_driveData	*driveDataPtr;
extern volatile t_driveData	driveData[MAX_MARKER_CNT];


// state machone 의 상태
extern volatile uint8_t		driveState;


// 주행 컨트롤 변수
extern volatile uint8_t		boostCntl;


// 현재까지 읽은 크로스 개수
extern volatile uint16_t	crossCnt;



#endif //INC_DRIVE_DEF_VAR_H_
