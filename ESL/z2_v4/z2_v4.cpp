/*Процесом SC_METHOD врсте моделовати ситуациjу човека коjи
покушава да прође кроз троjе врата при следећим правилима
(слика 1.18). На почетку човек - H, чека испред врата A. Када прође
кроз врата A, треба да прође кроз врата B, па потом C. Уколико
испред било коjих врата човек чека дуже од 5 секунди, мора да
се врати испред врата A. Врата се отвараjу насумично у интервалу
од 3 до 7 секунди. Симулациjом одредити проценат успешности
проласка човека кроз сво троjе врата*/

#include <iostream>
#include <systemc>
#include <ctime>
#include <string>

using namespace std;
using namespace sc_core;

class human : sc_module
{
	public:
		SC_HAS_PROCESS(human);
		human(sc_module_name);
		int get_success() const;
		unsigned char door = 0;
	protected:
		static int success;
		void Door();
		bool first, help;
		int wait_time;
		sc_event event;
};

int human ::  success = 0;

int human :: get_success() const
{
	return success;
}

human :: human(sc_module_name name)
{
	SC_METHOD(Door);
	srand(time(NULL));
	door = 0;
	wait_time = 0;
	first = true;
	help = false;
}

void human :: Door()
{
	int time = 0; 

	time = rand() % 5 + 3;
	
	if(!first)
	{
		
		cout << "Cekao je " << wait_time << " sekundi, a vreme simulacije je: " << sc_time_stamp() << endl;
		cout << "Trenutno je ispred vrata " << to_string(this -> door) << endl;
		
		if(wait_time <= 5)
		{
			
			door++;
			if(door == 3)
			{
				cout << "Prosao je kroz sva vrata za: " << sc_time_stamp() << endl;
				success++;
				next_trigger();
				help = true;
			}
		}	
		else
			door = 0;
	}
	
	first = false;
	if(!help)
		next_trigger(time, SC_SEC);
	wait_time = time;
	
}


int sc_main(int argc, char* argv[])
{
	human h("Human");
	// int instances = 20;
	
	sc_start(50, SC_SEC);
	
	return 0;
}

