/*
 * drive_preset.h
 */

#ifndef INC_DRIVE_PRESET_H_
#define INC_DRIVE_PRESET_H_



void	Drive_Preset();




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
