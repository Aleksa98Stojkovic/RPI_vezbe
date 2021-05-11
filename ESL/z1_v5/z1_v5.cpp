/*Моделовати проблем пет филозофа (слика 1.23): пет тихих
филозофа (слика - P) седи за округлим столом са тањирима
шпагета (слика - S). Виљушке (слика - F) су постављене
између сваког пара суседних филозофа. Сваки филозоф мора
размишљати и jести. Међутим, филозоф може jести шпагете само
ако има и леву и десну виљушку. Сваку виљушку може држати
само jедан филозоф. Након што поjединачни филозоф поjеде
шпагете спушта обе виљушке како би виљушке постале доступне
другима. Филозоф може узети леву или десну виљушку али не
могу почети да jеде пре него што добиjу обе виљушке.
Филозофе моделовати као модуле. Направити само jедан модел
филозова коjи ће се инстанцирати 5 пута у главном програму.
Виљушке моделовати као мутексе. Када филозоф размишља а када
покушава да jеде моделовати по жељи. Поруку о тренутном стању
филозова исписати сваке секунде симулациjе. Уколико се деси
ситуациjа да сваки филозоф држи само jедну виљушку, исписати
одговараjућу поруку и завршити симулациjу.*/

#include <systemc>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace sc_core;

SC_MODULE(filozof)
{
	public:
		SC_HAS_PROCESS(filozof);
		filozof(sc_module_name, sc_mutex*, sc_mutex*); // muteksi su za levu i desnu viljusku
		unsigned char forks;
		static int left, right;
	protected:
		sc_mutex *l, *r;
		void think_or_eat();
};

int filozof :: left = 5;
int filozof :: right = 5;

filozof :: filozof(sc_module_name name, sc_mutex* l, sc_mutex* r) : sc_module(name), l(l), r(r)
{
	SC_THREAD(think_or_eat);
	srand(time(NULL));
}

void filozof :: think_or_eat()
{
	unsigned char t1, t2;
	while(1)
	{
		
		// ovo vreme misli
		t1 = rand() % 5 + 1;
		for(int i = 0; i < t1; i++)
		{
			cout << "Filozof " << name() << " misli, a vreme je " << sc_time_stamp() << endl;
			wait(1, SC_SEC);
		}
		/*
		cout << "Filozof " << name() << " misli, a vreme je " << sc_time_stamp() << endl;
		wait(t1, SC_SEC);
		*/
		// probaj da zauzmes mutekse
		
		t1 = rand() % 2; // odredjuje da li ce se prvo uzeti desna ili leva viljuska
		
		if(t1 == 0)
		{
			l -> lock();
			left--;
			if(!left)
			{
				sc_stop();
				cout << "Svako drzi jednu viljusku" << endl;
			}
			forks++;
			r -> lock();
			right--;
			if(!right)
			{
				sc_stop();
				cout << "Svako drzi jednu viljusku" << endl;
			}
			forks++;
		}
		else
		{
			r -> lock();
			right--;
			if(!right)
			{
				sc_stop();
				cout << "Svako drzi jednu viljusku" << endl;
			}
			forks++;
			l -> lock();
			left--;
			if(!left)
			{
				sc_stop();
				cout << "Svako drzi jednu viljusku" << endl;
			}
			forks++;
		}
		
		
		
		// ovo vreme jede
		t2 = rand() % 5 + 1;
		for(int i = 0; i < t2; i++)
		{
			cout << "Filozof " << name() << " jede, a vreme je " << sc_time_stamp() << endl;
			wait(1, SC_SEC);
		}
		/*
		cout << "Filozof " << name() << " jede, a vreme je " << sc_time_stamp() << endl;
		wait(t2, SC_SEC);
		*/
		l -> unlock();
		left++;
		forks--;
		r -> unlock();
		right++;
		forks--;
	}
}

int sc_main( int argc , char* argv[])
{
	// muteksi
	sc_mutex f1, f2, f3, f4, f5;
	// filozofi
	filozof p1( "filozof1", &f1, &f2);
	filozof p2( "filozof2", &f2, &f3);
	filozof p3( "filozof3", &f3, &f4);
	filozof p4( "filozof4", &f4, &f5);
	filozof p5( "filozof5", &f5, &f1);
	
	sc_start(100, SC_SEC);
	cout << "Simulacija je zavrsena za " << sc_time_stamp() << endl;
	
	return 0;
}