#ifndef PROGRAMDATAMENU_H_
#define PROGRAMDATAMENU_H_

#include "Menu.h"
#include "ProgramData.h"
#include "Blink.h"

#define PROGRAM_DATA_MENU_SIZE 8

class ProgramDataMenu : public Menu, public Blink {
public:
	ProgramDataMenu(const ProgramData &p, int programIndex):
			Menu(PROGRAM_DATA_MENU_SIZE), p_(p), programIndex_(programIndex){};
	virtual uint8_t printItem(int i);
	void editIndex(int i);
	bool editItem(int i, uint8_t key);

	bool edit();
	void editName();
	void createName();

	ProgramData p_;
	int programIndex_;
};

#endif /* PROGRAMDATAMENU_H_ */
