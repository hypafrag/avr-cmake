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

void show(uint8_t v)
{
	uint8_t digit;

	digit = v / 100;
	v = v % 100;

	PORTB = SegOne;
	PORTD = SegCode[digit];
	_delay_ms(OnDelay);

	digit = v / 10;

	PORTB = SegTwo;
	PORTD = SegCode[digit];
	_delay_ms(OnDelay);

	digit = v % 10;

	PORTB = SegThree;
	PORTD = SegCode[digit];
	_delay_ms(OnDelay);
}

int main(void)
{
	// Configure the ports as output
	DDRB = 0xff; // Control signal PORTB0-PORTB2
	DDRD = 0xff; // Data lines
	while (1)
	{
		for (uint8_t i = 0; i <= 255; i++)
		{
			for (uint8_t t = 0; t < (uint8_t)(RefreshRate); t++)
			{
				show(i);
			}
		}
	}
}
