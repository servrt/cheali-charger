/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2014  Paweł Stawicki. All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Program.h"
#include "ProgramMenus.h"
#include "StaticMenu.h"
#include "eeprom.h"
#include "ProgramDataMenu.h"
#include "memory.h"

// Program selection depending on the battery type

namespace ProgramMenus {

    const  PROGMEM Program::ProgramType programNoneMenu[] = {
            Program::EditBattery,
    };

    const  PROGMEM Program::ProgramType programLiXXMenu[] = {
            Program::Charge,
            Program::ChargeBalance,
            Program::Balance,
            Program::Discharge,
            Program::FastCharge,
            Program::Storage,
            Program::StorageBalance,
#ifdef ENABLE_PROGRAM_MENUS_LIXX_CYCLING
            Program::DischargeChargeCycle,
#endif
            Program::CapacityCheck,
            Program::EditBattery,
    };

    const PROGMEM Program::ProgramType programNiZnMenu[] = {
            Program::Charge,
            Program::ChargeBalance,
            Program::Balance,
            Program::Discharge,
            Program::FastCharge,
#ifdef ENABLE_PROGRAM_MENUS_NIZN_CYCLING
            Program::DischargeChargeCycle,
#endif
            Program::CapacityCheck,
            Program::EditBattery,
    };

    const PROGMEM Program::ProgramType programNiXXMenu[] = {
            Program::Charge,
            Program::Discharge,
            Program::DischargeChargeCycle,
            Program::CapacityCheck,
            Program::EditBattery,
    };

    const PROGMEM Program::ProgramType programPbMenu[] = {
            Program::Charge,
            Program::Discharge,
            Program::FastCharge,
            Program::DischargeChargeCycle,
            Program::CapacityCheck,
            Program::EditBattery,
    };

    const PROGMEM Program::ProgramType programLEDMenu[] = {
            Program::Charge,
            Program::EditBattery,
    };

// ProgramType -> strings
    const PROGMEM char * const programMenus_strings[] = {
            string_charge,
            string_chargeAndBalance,
            string_balance,
            string_discharge,
            string_fastCharge,
            string_storage,
            string_storageAndBalance,
            string_dcCycle,
            string_capacityCheck,
            string_editBattery,
    };


    struct ProgramMenus {
    	struct Menu::Menu menu;
        const Program::ProgramType * typeMenu_;
    };

    struct ProgramMenus programMenu;

    Program::ProgramType getProgramType(struct ProgramMenus *d, uint8_t i) {
        Program::ProgramType v;
        pgm_read(v, &d->typeMenu_[i]);
        return v;
    }

    void printItem(struct ProgramMenus *d, int8_t i) {
        STATIC_ASSERT(sizeOfArray(programMenus_strings)-1 == Program::EditBattery);
        lcdPrint_P(programMenus_strings, getProgramType(d, i));
    }

    uint8_t countElements(const Program::ProgramType typeMenu[]) {
        uint8_t retu = 0;
        Program::ProgramType type;
        do {
            pgm_read(type, &typeMenu[retu++]);
        } while(type != Program::EditBattery);
        return retu;
    }


    void initialize(const Program::ProgramType *typeMenu) {
        uint8_t size = countElements(typeMenu);
        programMenu.menu.size_ = size;
        programMenu.menu.render_ = true;
        programMenu.menu.waitRelease_ = true;
        programMenu.menu.printItem = (Menu::PrintMethod) printItem;
    }

    void changeSelectProgramMenu() {
        ProgramData::BatteryClass bc = ProgramData::getBatteryClass();
        const Program::ProgramType *typeMenu;

        STATIC_ASSERT(ProgramData::LAST_BATTERY_CLASS == 6);
        if(ProgramData::battery.type == ProgramData::None) {
            typeMenu = programNoneMenu;
        } else if(bc == ProgramData::ClassNiZn) {
            typeMenu = programNiZnMenu;
        } else if(bc == ProgramData::ClassLiXX) {
            typeMenu = programLiXXMenu;
        } else if(bc == ProgramData::ClassNiXX) {
            typeMenu = programNiXXMenu;
        } else if(bc == ProgramData::ClassLED) {
            typeMenu = programLEDMenu;
        } else {
            typeMenu = programPbMenu;
        }

        initialize(typeMenu);
    }
}

void ProgramMenus::selectProgram(int index)
{
    int8_t menuIndex;
    ProgramData::loadProgramData(index);
    do {
        changeSelectProgramMenu();
        menuIndex = Menu::runSimple(&programMenu.menu);
        if(menuIndex >= 0)  {
            Program::ProgramType prog = getProgramType(&programMenu, menuIndex);
            if(prog == Program::EditBattery) {
                ProgramDataMenu::run();
                ProgramData::saveProgramData(index);
            } else {
                Program::run(prog);
            }
        }
    } while(menuIndex >= 0);
}