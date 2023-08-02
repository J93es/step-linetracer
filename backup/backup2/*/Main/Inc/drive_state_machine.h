/*
 * drive_state_machine.h
 */

#ifndef INC_DRIVE_STATE_MACHINE_H_
#define INC_DRIVE_STATE_MACHINE_H_


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





// end line, right mark, left mark, straight를 판별하고 정해진 동작을 실행하는 함수
__STATIC_INLINE void	Decision(uint8_t sensorStateSum) {

	// end mark
//	if ( ((sensorStateSum >> 0) & 0x01) && ((sensorStateSum >> 7) & 0x01) ) {
	if ( (sensorStateSum & 0x81) == 0x81 ) {

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
	else if ( (sensorStateSum & 0x01) == 0x01 ) {

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
	else if ( (sensorStateSum & 0x80) == 0x80 ) {

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







__STATIC_INLINE void	Drive_State_Machine() {

	//센서 값 누적
	static uint8_t	sensorStateSum;
	//static uint8_t	accumStartTick;


	switch (driveState) {

		case DRIVE_STATE_IDLE:

			// 라인 센서 4개 이상 인식
			if (Get_Line_Sensor_Cnt() >= 4) {
				sensorStateSum = 0x00;
				driveState = DRIVE_STATE_CROSS;
			}

			// 라인 센서 4개 이하 and 마크 센서 1개 이상
			else if (Get_Marker_Sensor_Cnt() != 0) {
				sensorStateSum = 0x00;
				driveState = DRIVE_STATE_MARKER;
			}
			break;


		case DRIVE_STATE_CROSS:

			// accum
			sensorStateSum |= state;

			// 모든 센서를 읽었고 마크 센서가 선을 지나쳤거나, 15cm 이상 이동했을 때 IDLE
			if (sensorStateSum == 0xff && Get_Marker_Sensor_Cnt() == 0) {
//			if (Get_Line_Sensor_Cnt() < 4) {
				driveState = DRIVE_STATE_IDLE;
			}
			break;


		case DRIVE_STATE_MARKER:

			// accum
			sensorStateSum |= state;

			// 마커 센서가 0개 일 때
			if (Get_Marker_Sensor_Cnt() == 0) {
				driveState = DRIVE_STATE_DECISION;
			}
			break;


		case DRIVE_STATE_DECISION:

			Decision(sensorStateSum);
			driveState = DRIVE_STATE_IDLE;
			break;


	}
}



#endif
