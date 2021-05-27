// hpp file

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio>
#include <stdlib>

class YL_40
{
	public:
		
		YL_40(char address);
		int get_fd() const;
		int get_ADCval(int pin) const;
		bool InitYL();
	
	private:
		
		int fd;
		int ADCval;
		char yl_40;
};