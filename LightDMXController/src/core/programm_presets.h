/*
 * programm_presets.h
 *
 * Created: 21.12.2020 00:47:50
 *  Author: Franz Forster
 */ 


#ifndef PROGRAMM_PRESETS_H_
#define PROGRAMM_PRESETS_H_

#define PRESET_PROG_COUNT	5

#include "core/Programs_Task.h"

		tRgbwColor constColor[] = { {0x33cccc} };
		tRgbwColor rgbColors[] = { {0xFF}, {0xFF00},  {0xFF0000}, {0xFF000000}};					// RGB
		tRgbwColor warmColors[] = { {0xcccc00}, {0xff9966}, {0xff9900}, {0xff5050}, {0xcc3300}};		// warm colors
		tRgbwColor coldColors[] = { {0xcc66ff}, {0x009999}, {0x3366ff}, {0x000099}, {0x009999}};		// cold colors
			

const	tProgram constColorProgram = {constColor, 0, 0, 1, PRG_PROP_STATIC_COLOR, 0};
const	tProgram prg1 =	{rgbColors, 64, 255, 4, PRG_PROP_DYNAMIC | PRG_PROP_SIMULTAN, 0};
const	tProgram prg2 =	{rgbColors, 20, 255, 4, PRG_PROP_DYNAMIC , 2};
const	tProgram prg3 =	{warmColors, 35, 255, 5, PRG_PROP_DYNAMIC | PRG_PROP_SIMULTAN, 0};
const	tProgram prg4 =	{warmColors, 25, 255, 5, PRG_PROP_DYNAMIC , 2};
const	tProgram prg5 =	{coldColors, 20, 255, 5, PRG_PROP_DYNAMIC | PRG_PROP_SIMULTAN, 0};


tProgram const *programList[PRESET_PROG_COUNT] = {&prg1, &prg2, &prg3, &prg4, &prg5};



#endif /* PROGRAMM_PRESETS_H_ */