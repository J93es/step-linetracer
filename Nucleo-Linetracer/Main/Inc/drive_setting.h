/*
 * drive_setting.h
 */

#ifndef INC_DRIVE_SETTING_H_
#define INC_DRIVE_SETTING_H_

#include <stdint.h>
#include <stdbool.h>
#include "init.h"



void	Pre_Drive_Setting(uint8_t driveIdx);
void	After_Drive_Setting(uint8_t driveIdx);




typedef struct	s_driveMenu_Float {
		volatile char		valName[MAX_OLED_LEN];
		volatile float		*val;
		volatile float		changeVal;
}				t_driveMenu_Float;


typedef struct	s_driveMenu_Int {
		volatile char		valName[MAX_OLED_LEN];
		volatile uint32_t	*val;
		volatile uint32_t	changeVal;
}				t_driveMenu_Int;

#endif
