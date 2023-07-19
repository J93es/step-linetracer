/*
 * init.h
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_





void	Init(void);




#define MAX_OLED_LEN	15





typedef struct	s_menuData
{
	char menuName[MAX_OLED_LEN];
	void (*func)(void);
}				t_menuData;







#endif /* INC_INIT_H_ */
