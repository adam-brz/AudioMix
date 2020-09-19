/*
 * usbFunctions.c
 *
 * Created: 28.01.2018 15:03:29
 *  Author: Adam
 */ 

#include "HID_descriptor.h"

void setupUSB()
{
	usbInit();
	usbDeviceDisconnect();
	
	for(uint8_t i = 0; i < 250; ++i)
	{
		wdt_reset();
		_delay_ms(2);
	}
	
	usbDeviceConnect();
}

uchar usbFunctionDescriptor(usbRequest_t * rq)
{

	if (rq->wValue.bytes[1] == USBDESCR_DEVICE) 
	{
		usbMsgPtr = (uchar *) deviceDescrMIDI;
		return sizeof(deviceDescrMIDI);
	}
	else {		/* must be config descriptor */
		usbMsgPtr = (uchar *) configDescrMIDI;
		return sizeof(configDescrMIDI);
	}
}

uchar usbFunctionSetup(uchar data[8])
{
	return 0xff;
}

uchar usbFunctionRead(uchar * data, uchar len)
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	return 7;
}

uchar usbFunctionWrite(uchar * data, uchar len)
{
	return 1;
}

void usbFunctionWriteOut(uchar *data, uchar len)
{
	
}