/*
* AudioMix.c
*
* Created: 19.05.2020 22:10:19
* Author : Adam
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "vusb/usbdrv.h"
#include "HID_descriptor.h"

#include "toolbox.h"
#include "adc.h"
#include "usbFunctions.c"

#define DBG_BLINK() (PORTD ^= BV(PD1))
#define PROG_BLINK() (PORTD ^= BV(PD0))

#define READINGS_COUNT 16
#define POT_COUNT 6

static uint8_t reportBuffer[8];							// buffer for HID reports
static uint8_t potValues[POT_COUNT];					// ADC average, bit 0 - was Updated?
static uint8_t readings[POT_COUNT][READINGS_COUNT];		// all ADC
static uint16_t buttonsStatus;							// Bits 0.. 10 - buttons PB0-5, PD4-7

ISR(ADC_vect)
{
	static uint8_t adcChannel = 0;		// current ADC channel - 0.. 5
	static uint8_t reading = 0;

	sei();
	readings[adcChannel][reading++] = ADCH;
	
	if(reading >= READINGS_COUNT - 1)
	{
		reading = 0;
		adcChannel = (adcChannel + 1) % POT_COUNT;
		selectADC(adcChannel);
	}
}

inline uint16_t readButtons()
{
	return ((uint16_t)(PINB & 0x3f) << 4) | ((uint16_t)(PIND & 0xf0) >> 4);  // Pack buttons status
}

void updatePot(uint8_t pot)
{
	int16_t avg = 0;
	int16_t real = 0;
	uint8_t count = 0;
	uint8_t i;
	
	for(i = 0; i < READINGS_COUNT; ++i)
	{
		avg += readings[pot][i];
	}
	avg /= READINGS_COUNT;
	
	for(i = 0; i < READINGS_COUNT; ++i)
	{
		int8_t diff = (int16_t)(readings[pot][i]) - avg;
		if(diff < 7 && diff > -7);
		{
			real += readings[pot][i];
			++count;
		}
	}
	
	real /= count;
	
	if(isDiffer(potValues[pot], real))
		potValues[pot] = real | 0x01;
}

void sendReading(uint8_t channel)
{
	reportBuffer[0] = 0x0b;
	reportBuffer[1] = 0xb0;
	reportBuffer[2] = channel + 70;				// cc 70..75
	reportBuffer[3] = potValues[channel] >> 1;	// ADC value (0.. 127)
	potValues[channel] &= ~(0x01);				// Clear bit0 - set updated to false
	
	DBG_BLINK();
	usbSetInterrupt(reportBuffer, 4);			// Send report
}

void sendKeyStatus(uint8_t key, uint8_t status)
{
	reportBuffer[0] = 0x0b;
	reportBuffer[1] = 0xb0;
	reportBuffer[2] = 11 + key;
	reportBuffer[3] = status;
	
	DBG_BLINK();
	usbSetInterrupt(reportBuffer, 4);
}

void sendKeyChange()
{
	static uint8_t current = 0;
	uint16_t change, key;
	uint16_t newKeyStatus = readButtons();
	uint16_t diff = newKeyStatus ^ buttonsStatus;

	while(current < 10)
	{
		key = current++;
		
		if(diff & BV(key))
		{
			change = newKeyStatus & BV(key);
			buttonsStatus = (buttonsStatus & ~(BV(key))) | change;
			
			if(change)
				return sendKeyStatus(key, 0x00);
			return sendKeyStatus(key, 0x7f);	
		}
	}
	
	current = current % 10;
}

inline void initDevice()
{
	DDRB &= ~(0x3f); // PB0-5
	PORTB |= 0x3f;
	
	DDRD |= BV(PD0) | BV(PD1);
	DDRD &= ~(0xf0); // PD4-7
	PORTD |= 0xf0;

	wdt_enable(WDTO_1S);
}

int main(void)
{
	uint8_t i = 0;
	uint16_t counter = 0;
	
	initDevice();
	initADC();
	
	sei();
	setupUSB();
	
	buttonsStatus = readButtons();
	
	while(1)
	{
		wdt_reset();
	
		updatePot(i);
		usbPoll();

		if(usbInterruptIsReady()) 
		{
			if(potValues[i] & 0x01)	// was updated?
				sendReading(i);
			else
				sendKeyChange();
		}
		
		i = (i + 1) % POT_COUNT;

		if(!(++counter))
			PROG_BLINK();
	}
}

