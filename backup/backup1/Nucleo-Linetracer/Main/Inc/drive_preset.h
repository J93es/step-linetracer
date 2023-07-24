/*
 * drive_preset.h
 */

#ifndef INC_DRIVE_PRESET_H_
#define INC_DRIVE_PRESET_H_



void	Drive_Preset();




typedef struct	s_driveMenu {
		volatile char		valName[MAX_OLED_LEN];
		volatile float		*val;
		volatile float		changeVal;
}				t_driveMenu;

#endif
