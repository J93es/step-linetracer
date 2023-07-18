/*
 * decison_machine.c
 */

#include "header_init.h"




volatile uint8_t	endMarkCnt = 0;
volatile uint8_t	curDecisionIdx = DESISION_STRAIGHT;




// line sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t Get_Line_Sensor_Cnt() {
	return ((state >> 6) & 0x01) + ((state >> 5) & 0x01) + ((state >> 4) & 0x01) + \
			((state >> 3) & 0x01) + ((state >> 2) & 0x01) + ((state >> 1) & 0x01);
}


// marker sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t Get_Marker_Sensor_Cnt() {
	return ((state >> 7) & 0x01) + ((state >> 0) & 0x01);
}





// end line, right mark, left mark, cross를 판별하고 정해진 동작을 실행하는 함수
__STATIC_INLINE void First_Drive_Decision(uint8_t sensorStateSum) {
	// cross
	if (~sensorStateSum == 0x00) {
		curDecisionIdx = DESISION_MEASURE_STRAIGHT;
	}
	// end line
	else if ( ((sensorStateSum >> 0) & 0x01) && ((sensorStateSum >> 7) & 0x01) ) {
		if (endMarkCnt != 0) {
			curDecisionIdx = DESISION_END_MARK;
		}
		else {
			endMarkCnt++;
		}
	}
	// right mark
	else if ( (sensorStateSum >> 0) & 0x01 ) {
		if (curDecisionIdx == DESISION_MEASURE_CURVE_R) {
			curDecisionIdx = DESISION_MEASURE_STRAIGHT;
		}
		else {
			curDecisionIdx = DESISION_MEASURE_CURVE_R;
		}
	}
	// left mark
	else if ( (sensorStateSum >> 7) & 0x01 ) {
		if (curDecisionIdx == DESISION_MEASURE_CURVE_L) {
			curDecisionIdx = DESISION_MEASURE_STRAIGHT;
		}
		else {
			curDecisionIdx = DESISION_MEASURE_CURVE_L;
		}
	}
}







void First_Drive_Decision_Machine() {
	static uint8_t	sensorStateSum = 0x00;	//센서 값 누적

	sensorStateSum = 0x00;
	// 라인 센서 4개 이상 인식
	// 0x7e == 0b01111110
	if (Get_Line_Sensor_Cnt() >= 4) {

		// 라인 센서가 읽은 값의 개수 3이하 일 때 센서값 누적 종료
		while (Get_Line_Sensor_Cnt() >= 4) {
			sensorStateSum |= state;
		}
		// cross 상태가 아닐 때만 decision
		if (sensorStateSum != 0x7e) {
			First_Drive_Decision(sensorStateSum);
		}
	}

	// 라인 센서 3개 이하 and 마크 센서 1개 이상
	else if (Get_Marker_Sensor_Cnt() != 0){

		// 마크 센서가 읽은 값의 개수가 0일 때 센서값 누적 종료
		while (Get_Marker_Sensor_Cnt() != 0) {
			sensorStateSum |= state;
		}
		First_Drive_Decision(sensorStateSum);
	}
}


