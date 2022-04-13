#include "led.h"
#include "middle.h"


void LED_ON(int led)
{
	switch(led)
	{
		case LED_GREEN:
			SetIOLevelLow(PORT_LED_GREEN, PIN_LED_GREEN);
			break;
		case LED_BLUE:
			SetIOLevelLow(PORT_LED_BLUE, PIN_LED_BLUE);
			break;
		case LED_RED:
			SetIOLevelLow(PORT_LED_RED, PIN_LED_RED);
			break;
		default:
			break;
	}
}

void LED_OFF(int led)
{
	switch(led)
	{
		case LED_GREEN:
			SetIOLevelHigh(PORT_LED_GREEN, PIN_LED_GREEN);
			break;
		case LED_BLUE:
			SetIOLevelHigh(PORT_LED_BLUE, PIN_LED_BLUE);
			break;
		case LED_RED:
			SetIOLevelHigh(PORT_LED_RED, PIN_LED_RED);
			break;
		default:
			break;
	}
}

