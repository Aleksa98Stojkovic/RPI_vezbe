/*
Моделовати нуклеарни процес коjи се састоjи од 1000 честица.
Све честице моделовати истим модулом коjи ће се инстанцирати
1000 пута и садржаће jедан процес. Интеракциjу међу честицама
моделовати помоћу jедног догађаjа на коjи ће све честице бити
осетљиве. Честица може бити у три стања: неактивно, активно,
експлодирано. Са вероватноћом од 1% честица ће постати активна
уколико се активира догађаj. Када честица постане активна
она експлодира у случаном тренутку између 1 ns и 10 ns при
чему активира исти догађаj. Када честица експлодира ниjе више
осетљива на догађаj. Исписати броj честива коjе су експлодирале
100 ns након почетка симулациjе. У почетом тренутку, само jедна
честица jе у активном стању, док су све остале у неактивном 
*/

#include <iostream>
#include <systemc>
#include <vector>
#include <ctime>
#include <string>

using namespace std;
using namespace sc_core;

enum state_type {Inactive, Active, Exploded};

class Particle :  public sc_module
{
	public:
		SC_HAS_PROCESS(Particle);
		Particle(sc_module_name, state_type);
		static int exploded;
		state_type get_state() const; 
	protected:
		state_type state;
		void process();
		
};

int Particle :: exploded = 0;

Particle :: Particle(sc_module_name name, state_type s) : sc_module(name)
{
	srand(time(NULL));
	state = s;
	SC_THREAD(process);
}

void Particle :: process()
{
	static sc_event event;
	unsigned char time, prob;
	
	while(state != Exploded)
	{
		if(state == Active)
		{
			time = rand() % 10 + 1;
			wait(time, SC_NS);
			exploded++;
			state = Exploded;
			event.notify();
			wait();
		}
		else if(state == Inactive)
		{
			wait(event);
			prob = rand() % 100;
			if(prob == 1)
				state = Active;
		}
	}
}

state_type Particle :: get_state() const
{
	return state;
}

int sc_main( int argc , char* argv[])
{
	
	vector <Particle*> particles;
	Particle* P = new Particle(("Particle_" + to_string(0)).c_str(), Active);
	particles.push_back(P);
	
	for(int i = 1; i <= 999; i++)
	{
		P = new Particle(("Particle_" + to_string(i)).c_str(), Inactive);
		particles.push_back(P);
	}
	
	sc_start(100, SC_NS);
	cout << "Simulacija je zavrsena za: " << sc_time_stamp() << endl;
	cout << "Broj eksplodiranih cestica je: " << particles[0] -> exploded << endl;
	
	return 0;
}