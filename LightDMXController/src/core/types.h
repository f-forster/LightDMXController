/*
 * linar_types.h
 *
 * Created: 20.03.2018 12:43:24
 *  Author: Franz
 */ 


#ifndef TYPES_H_
#define TYPES_H_


typedef union {
	uint32_t _dword;
	uint8_t  _byte[4];
	struct {
		uint8_t		r;
		uint8_t		g;
		uint8_t		b;
		uint8_t		w;
		};
	
} tRgbwColor;


typedef union {
	uint32_t	_dword;
	uint16_t	_word[2];
	uint8_t		_byte[4];
} tuConvertDWord;



#endif /* TYPES_H_ */