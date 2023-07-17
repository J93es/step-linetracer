/*
 * speed_def_var.h
 */


#ifndef INC_SPEED_DEF_VAR_H_
#define INC_SPEED_DEF_VAR_H_


#include <stdint.h>
#include <stdbool.h>



#define CURVE_SPEED				1.f
#define STRAIGHT_SPEED			2.f

#define CURVE_MEASURE_SPEED		1.f
#define STRAIGHT_MEASURE_SPEED	2.f


#define STATE_END_MARK			0
#define STATE_CROSS				1
#define STATE_STRAIGHT			2
#define STATE_CURVE_R			3
#define STATE_CURVE_L			4
#define STATE_MEASURE_STRAIGHT	5
#define STATE_MEASURE_CURVE_R	6
#define STATE_MEASURE_CURVE_L	7






#define TIRE_RADIUS	0.025f					// m
#define	SPEED_COEF	15707.f * TIRE_RADIUS






#define POSITION_COEF_INIT			0.00004f
#define TARGET_SPEED_INIT			1.f
#define ACCELE_INIT					4.f						//	m/ms
#define	MAX_SPEED_INIT				2.f						//	m/s
#define MIN_SPEED_INIT				SPEED_COEF / 65535.f	//	SPEED_COEF / (65535)ARR_MAX (m/s) 의 근사값
/*
 * (2 * l(m) * 3.141592) / (t(s) * 400) = v(m/s) * (arr+1)
 *
 * t =  1 / 1Mhz = 1 / 1,000,000 = 타이머 주
 * 1 / (t * 400) = 2,500
 *
 * l(m) = 타이어 반지름
 * 2 * l * 3.14
 *
 * v * (arr + 1) = SPEED_COEF
 */




extern volatile int32_t		positionVal;
extern volatile float		positionCoef;

extern volatile float		targetSpeed;
extern volatile float		currentSpeed;
extern volatile float		minSpeed;
extern volatile float		maxSpeed;
extern volatile float		accele;

extern volatile uint8_t		curStateIdx;



#endif //INC_SPEED_DEF_VAR_H_
