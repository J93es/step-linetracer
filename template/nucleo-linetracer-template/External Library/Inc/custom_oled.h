/*
 * custom_oled.h
 * SSD1331 SPI Driver
 *
 *  Created on: Jun 27, 2023
 *      Author: Seongho Lee
 */

#ifndef INC_CUSTOM_OLED_H_
#define INC_CUSTOM_OLED_H_

#include <stdint.h>

/**
 * @brief OLED 화면을 모두 검은색으로 채우는 함수.
 */
void Custom_OLED_Clear();

/**
 * @brief SSD1331 OLED를 초기화하는 함수.
 *        SPI 통신 및 제어를 위한 GPIO 초기화는 STM32CubeIDE에서 진행하며, 이 함수는 명령어 모드로 SSD1331에 초기 사용을 위한 명령어를 전송하는 역할을 한다.
 */
void Custom_OLED_Init();

/*
 * OLED에서 한 픽셀은 16비트로 구성되며, 비트맵 구조는 아래와 같다. Green 색상은 5비트, Red 색상은 6비트, Blue는 5비트이다.
 *   Bit:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
 *   Map: G0 G1 G2 G3 G4 R0 R1 R2 R3 R4 R5 B0 B1 B2 B3 B4
 * 위의 비트맵 구조에 맞게 아래와 같은 색상 코드를 정의한다.
 */
enum Custom_OLED_Colors {
	OLED_COLOR_RED = 0x07E0,
	OLED_COLOR_GREEN = 0x001F,
	OLED_COLOR_BLUE = 0xF800,
	OLED_COLOR_YELLOW = OLED_COLOR_RED | OLED_COLOR_GREEN,
	OLED_COLOR_CYAN = OLED_COLOR_GREEN | OLED_COLOR_BLUE,
	OLED_COLOR_MAGENTA = OLED_COLOR_RED | OLED_COLOR_BLUE,
	OLED_COLOR_WHITE = OLED_COLOR_RED | OLED_COLOR_GREEN | OLED_COLOR_BLUE,
};

/**
 * @brief 지정된 위치에 문자 하나를 출력하는 함수.
 * 
 * @param c 출력할 문자
 * @param color 문자의 색상 (Custom_OLED_Colors 열거형 참고)
 * @param x 출력할 가로 위치
 * @param y 출력할 세로 위치
 * @return int 0 또는 -1
 *         0: 정상적으로 처리가 완료됨, -1: 문자의 출력 범위를 넘어 처리가 제대로 완료되지 않음
 */
int Custom_OLED_Putchar(char c, uint16_t color, uint8_t x, uint8_t y);

/**
 * @brief 서식을 포함한 문자열을 OLED에 출력하는 함수.
 * 
 * @param format 서식 문자열
 * @param ... 
 */
void Custom_OLED_Printf(const char *format, ...);

#endif /* INC_CUSTOM_OLED_H_ */
