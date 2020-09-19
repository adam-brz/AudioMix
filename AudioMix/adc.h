/*
 * adc.h
 *
 * Created: 23.05.2020 12:33:20
 *  Author: Adam
 */ 


#ifndef ADC_H_
#define ADC_H_

inline void selectADC(uint8_t channel)
{
	ADMUX = (ADMUX & 0xf0) | channel;	// Select analog input (0.. 5)
}

inline void initADC()
{
	DIDR0 = 0x3f;												// Disable digital on ADC pins
	ADMUX |= BV(REFS0) | BV(ADLAR);								// External Vref, left adjust ADC
	ADCSRA |= BV(ADPS0) | BV(ADPS1) | BV(ADPS2) | BV(ADATE);	// Prescaler 128 - 125kHz, auto trigger
	ADCSRA |= BV(ADEN) | BV(ADIE) | BV(ADSC);					// Enable ADC, interupts, start conversion (free running)
}

#endif /* ADC_H_ */