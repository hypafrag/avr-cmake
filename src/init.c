#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define SegOne 0x01
#define SegTwo 0x02
#define SegThree 0x04

const uint8_t SegCode[] = {
	0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8,
	0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e
};

#define RefreshRate 45.0
#define OnDelay (1000.0 / (RefreshRate * 3))

void show(uint8_t v, double delay)
{
	uint8_t digit;

	digit = v / 100;
	v = v % 100;

	PORTB = SegOne;
	PORTD = SegCode[digit];
	_delay_ms(delay);

	digit = v / 10;

	PORTB = SegTwo;
	PORTD = SegCode[digit];
	_delay_ms(delay);

	digit = v % 10;

	PORTB = SegThree;
	PORTD = SegCode[digit];
	_delay_ms(delay);
}

int main(void)
{
	// Configure the ports as output
	DDRB = 0xff; // Control signal PORTB0-PORTB2
	DDRD = 0xff; // Data lines
	DDRC = 0x01; // Blinker
	PORTC = 0x06; // Button pull-up for C1 and C2
	while (1)
	{
		for (uint8_t i = 0; i <= 255; i++)
		{
			for (uint8_t t = 0; t < (uint8_t)(RefreshRate); t++)
			{
				double delay = OnDelay;
				if (bit_is_clear(PINC, PC1))
				{
					delay *= 0.5;
				}
				if (bit_is_clear(PINC, PC2))
				{
					delay *= 2;
				}
				show(i, delay);
				PORTC = ~PORTC;
			}
		}
	}
}
