/*
 * sensor.h
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_



void	Sensor_TIM5_IRQ();

void	Sensor_Start();
void	Sensor_Stop();

void	Sensor_Calibration();

void	Sensor_Test_Raw();
void	Sensor_Test_Normalized();
void	Sensor_Test_State();



#define	THRESHOLD_MAX 			125
#define	THRESHOLD_MIN			25
#define	THRESHOLD_CHANGE_VAL	5
#define	THRESHOLD_RESET_VAL		50

#define	STATE_WHITE				1
#define	STATE_BLACK				0

#define POSITION_COEF_INIT		0.00004f



extern uint8_t		sensorRawVals[8];
extern uint8_t		sensorNormVals[8];
extern uint8_t		normalizeCoef[8];
extern uint8_t		whiteMaxs[8];
extern uint8_t		blackMaxs[8];
extern uint8_t		state;

extern uint8_t		threshold;

extern int32_t		positionVal;
extern float		positionCoef;


#endif /* INC_SENSOR_H_ */
