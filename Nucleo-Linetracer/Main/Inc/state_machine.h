/*
 * state_machine.h
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_



#include "main.h"
#include "sensor.h"




// line sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t Get_Line_Sensor_Cnt() {
	return ((state << 6) >> 7) + ((state << 5) >> 7) + ((state << 4) >> 7) + \
			((state << 3) >> 7) + ((state << 2) >> 7) + ((state << 1) >> 7);
}


// marker sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t Get_Marker_Sensor_Cnt() {
	return ((state << 7) >> 7) + ((state << 0) >> 7);
}







// end line, right mark, left mark, cross를 판별하고 정해진 동작을 실행하는 함수
__STATIC_INLINE void Decision(uint8_t sensorStateSum) {
	// end line
	if ( (((sensorStateSum << 0) >> 7) == 1) && (((sensorStateSum << 7) >> 7) == 1) ) {

	}
	// right mark
	else if (((sensorStateSum << 0) >> 7) == 1) {

	}
	// left mark
	else if ((sensorStateSum << 7) >> 7) {

	}
	//cross는 무시
}







__STATIC_INLINE void State_Machine() {
	static uint8_t	sensorStateSum = 0x00;	//센서 값 누적

	while (1) {
		sensorStateSum = 0x00;

		if (Get_Line_Sensor_Cnt() >= 4) {

			// 라인 센서가 읽은 값의 개수 3이하 일 때 센서값 누적 종료
			while (Get_Line_Sensor_Cnt() >= 4) {
				sensorStateSum |= state;
			}
			Decision(sensorStateSum);
		}

		//라인 센서 4개이상 and 마크 센서 1개 이상
		else if (Get_Marker_Sensor_Cnt() != 0){

			// 마크 센서가 읽은 값의 개수가 0일 때 센서값 누적 종료
			while (Get_Marker_Sensor_Cnt() != 0) {
				sensorStateSum |= state;
			}
			Decision(sensorStateSum);
		}
	}
}





#endif
