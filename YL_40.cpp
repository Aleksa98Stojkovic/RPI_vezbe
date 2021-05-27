#include "YL_40.hpp"

YL_40::YL_40(char address)
{
	yl_40 = address;
}

bool YL_40::InitYL()
{
	if(wiringPiSetup() == -1)
		return false;
	
	fd = wiringPiI2CSetup(yl_40);
	return true;
}

int YL_40::get_fd() const
{
	return fd;
}

int YL_40::get_ADCval(int pin) const
{
	ADCval = wiringPiI2CReadReg8(fd, yl_40 + pin);
	return ADCval; 
}