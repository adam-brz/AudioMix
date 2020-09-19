/*
 * toolbox.h
 *
 * Created: 18.05.2020 16:13:48
 *  Author: Adam
 */ 


#ifndef TOOLBOX_H_
#define TOOLBOX_H_

#define BV(x) (1 << (x))

inline uint8_t isDiffer(uint8_t old, uint8_t new)
{
	return (old & 0xfc) != (new & 0xfc);
}

#endif /* TOOLBOX_H_ */