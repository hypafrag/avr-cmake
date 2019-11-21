#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC = 0xEF;
	while (1)
	{
		PORTC = 0x00;
		_delay_ms(1000);
		PORTC = 0xEF;
		_delay_ms(1000);
	}
}
