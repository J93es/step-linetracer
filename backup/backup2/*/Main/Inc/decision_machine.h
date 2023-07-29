/*
 * decision_machine.h
 */

#ifndef INC_DECISION_MACHINE_H_
#define INC_DECISION_MACHINE_H_


#include "drive_def_var.h"
#include "sensor.h"
#include "main.h"





// line sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t	Get_Line_Sensor_Cnt() {
	return ((state >> 6) & 0x01) + ((state >> 5) & 0x01) + ((state >> 4) & 0x01) + \
			((state >> 3) & 0x01) + ((state >> 2) & 0x01) + ((state >> 1) & 0x01);
}


// marker sensor가 읽은 값을 개수를 리턴함
__STATIC_INLINE uint8_t	Get_Marker_Sensor_Cnt() {
	return ((state >> 7) & 0x01) + ((state >> 0) & 0x01);
}





// end line, right mark, left mark, cross를 판별하고 정해진 동작을 실행하는 함수
__STATIC_INLINE void	Decision(uint8_t sensorStateSum) {

	// cross
	// 라인/마크 센서가 모두 인식됐을 경우 cross로 판별
	if (~sensorStateSum == 0x00) {
		// ignore
	}

	// end line
	else if ( ((sensorStateSum >> 0) & 0x01) && ((sensorStateSum >> 7) & 0x01) ) {
		// end mark를 한번 이상 읽은 상태일 경우
		if (endMarkCnt != 0) {
			curDecisionState = DECISION_END_MARK;
		}
		// end mark를 한번도 안 읽은 상태일 경우
		else {
			curDecisionState = DECISION_STRAIGHT;
			endMarkCnt++;
		}
	}

	// right mark
	else if ( (sensorStateSum >> 0) & 0x01 ) {
		// 이전 마크가 오른쪽 곡선 마크였다면 곡선주행 종료
		if (curDecisionState == DECISION_CURVE_R) {
			curDecisionState = DECISION_STRAIGHT;
		}
		// 곡선주행 진입
		else {
			curDecisionState = DECISION_CURVE_R;
		}
	}

	// left mark
	else if ( (sensorStateSum >> 7) & 0x01 ) {
		// 이전 마크가 왼쪽 곡선 마크였다면 곡선주행 종료
		if (curDecisionState == DECISION_CURVE_L) {
			curDecisionState = DECISION_STRAIGHT;
		}
		// 곡선주행 진입
		else {
			curDecisionState = DECISION_CURVE_L;
		}
	}
}







__STATIC_INLINE void	Decision_Machine() {
	static uint8_t	sensorStateSum = 0x00;	//센서 값 누적

	sensorStateSum = 0x00;

	// 라인 센서 4개 이상 인식
	if (Get_Line_Sensor_Cnt() >= 4) {

		// 라인 센서가 읽은 값의 개수 3이하 일 때 센서값 누적 종료
		while (Get_Line_Sensor_Cnt() >= 4) {
			sensorStateSum |= state;
		}
		// cross
		// 0x7e == 0b01111110
		// 라인 센서가 모두 인식됐을 경우 cross로 판별
		if ( (sensorStateSum & 0x7e) == 0x7e ) {
			Decision(sensorStateSum);
		}
	}

	// 라인 센서 3개 이하 and 마크 센서 1개 이상
	else if (Get_Marker_Sensor_Cnt() != 0){

		// 마크 센서가 읽은 값의 개수가 0일 때 센서값 누적 종료
		while (Get_Marker_Sensor_Cnt() != 0) {
			sensorStateSum |= state;
		}
		Decision(sensorStateSum);
	}
}




#endif
