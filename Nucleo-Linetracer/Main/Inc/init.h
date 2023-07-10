/*
 * init.h
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_



void	Init(void);



#define MENU_TEXT_LENGTH	25



typedef struct	s_menuData
{
	char menuName[MENU_TEXT_LENGTH];
	void (*func)(void);
}				t_menuData;



#endif /* INC_INIT_H_ */
