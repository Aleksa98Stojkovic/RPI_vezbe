/* 1. Моделовати модул коjи садржи два процеса. Први процес ствара
кружну секвенцу првих 64 Фибоначиjевих броjева у случаjним
временским интервалима. Нови броj се ствара у интервалу од 10
ns до 100 ns. Други процес исписуjе поруку 5 ns након што jе
броj створен. За комуникациjу међу процесима користити догађаj.
Порука треба да садржи броj и тренутно време симулациjе. */

#include <systemc>
#include <iostream>
#include <ctime>
#include <string>

using namespace sc_core;
using namespace std;

SC_MODULE(fibonacci)
{
	public:
		SC_HAS_PROCESS(fibonacci);
		fibonacci(sc_module_name name);
	protected:
		sc_event f;
		unsigned long long f1;
		void generate_fibonacci();
		void print_fibonacci();
};

fibonacci :: fibonacci(sc_module_name name)
{
	SC_THREAD(generate_fibonacci);
	SC_THREAD(print_fibonacci);
	dont_initialize();
	sensitive << f;
	
	f1 = 1;
	srand(time(NULL));
}

void fibonacci :: generate_fibonacci()
{
	unsigned long long f2 = 0, num = 1, temp;
	while(1)
	{
		if(num < 64)	
		{
			num++;
			temp = f1; 
			f1 += f2;
			f2 = temp;
		}
		else
		{
			num = 1;
			f1 = 1;
			f2 = 0;
		}
		wait(rand() % 91 + 10, SC_NS);
		cout << "Broj je generisan u vremenu: " << sc_time_stamp() << endl;
		f.notify();
	}
}

void fibonacci :: print_fibonacci()
{
	while(1)
	{
		wait(f);
		wait(5, SC_NS);
		cout << "Generisani broj je :" << to_string(f1) << ". Trenutno vreme simulacije je: " << sc_time_stamp() << endl;
	}
}

int sc_main(int argc, char* argv[])
{
	fibonacci fib("uut");
	sc_start(10000, SC_NS);
	cout << "Simulacija zavrsena za: " << sc_time_stamp() << endl;
	return 0;
}