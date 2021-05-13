#include <wiringPi.h>
#include <lcd.h>

#define COLUMNS 16
#define LCD_RS 3
#define LCD_E 14
#define LCD_D4 4
#define LCD_D5 12
#define LCD_D6 13
#define LCD_D7 6

int initLCD()
{
	int lcd;
	
	if(lcd = lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4,
					  LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0))
	{
		printf ("lcdInit nije uspeo! \n");
		return -1 ;
	}
	else
		return lcd;
}

void Time2LCD(int lcd, char* time, bool row) // QString -> to_string() -> cstr(), to_string(Qstr).cstr()
{
	lcdClear(lcd);
	lcdPosition(lcd, 0, row);
	lcdPuts(time);
}

/*
QTime vreme.setHMS(sati, minuti, sekunde);

h:m:s a

QString format = "hh:mm:ss";
QString str = vreme.toString(format);
*/