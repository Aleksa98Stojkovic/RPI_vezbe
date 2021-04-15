#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#define MAX_VAL 25

unsigned char val = 0;
	
int main(void)
{
	if(!bcm2835_init())
		return 1;
	
	bcm2835_i2c_begin();
	bcm2835_i2c_setSlaveAddress(0x48);
	bcm2835_i2c_set_baudrate(10000);
	while(1)
	{
		val = (val + 1) % MAX_VAL;
		bcm2835_i2c_write((const char *)&val, 2);
		bcm2835_delay(50);
	}
	
	bcm2835_i2c_end();
	bcm2835_close();
	
	return 0;
}