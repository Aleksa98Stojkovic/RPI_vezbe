/* 1. Моделовати модул коjи садржи два процеса. Први процес ствара
кружну секвенцу првих 64 Фибоначиjевих броjева у случаjним
временским интервалима. Нови броj се ствара у интервалу од 10
ns до 100 ns. Други процес исписуjе броj створених броjева сваке
секунде. Симлуациjа треба да траjе 20 секунди. За комуникациjу
између процеса користити ред или вектор. */

#include <systemc>
#include <iostream>
#include <queue>
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
		queue<unsigned long long> q;
		unsigned long long n;
		void generate_fibonacci();
		void print_fibonacci();
};

fibonacci :: fibonacci(sc_module_name name)
{
	SC_THREAD(generate_fibonacci);
	SC_THREAD(print_fibonacci);
	q.push(0); // prvi element
	srand(time(NULL));
}

void fibonacci :: generate_fibonacci()
{
	unsigned long long f1 = 1, f2, num;
	while(1)
	{
		wait(rand() % 91 + 10, SC_NS); // cekamo izmedju 10 i 100ns
		if(num < 64)
		{
			num++;
			f2 = q.back();
			q.push(f1 + f2);
			f1 = f2;
		}
		else
		{
			num = 1;
			q.push(1);
			f1 = 1;
		}
	}
}

void fibonacci :: print_fibonacci()
{
	while(1)
	{
		wait(1, SC_SEC);
		cout << "Broj stvorenih brojeva je :" << to_string(q.size()) << ". Trenutno vreme simulacije je: " << sc_time_stamp() << endl;
	}
}

int sc_main(int argc, char* argv[])
{
	fibonacci fib("uut");
	sc_start(15, SC_SEC); // Stavio sam 15, jer za 20 mi puca program
	cout << "Simulacija zavrsena za: " << sc_time_stamp() << endl;
	return 0;
}