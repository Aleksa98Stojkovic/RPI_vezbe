#include <iostream>
#include <systemc>
#include <ctime>
#include <vector>

using namespace std;
using namespace sc_core;

/* --------------------------------------------------------------------------- */

// Komponenta za generisanje brojeva
class RandGen : public sc_module
{
	public:
		SC_HAS_PROCESS(RandGen);
		RandGen(sc_module_name);
		sc_in <bool> gen;
		sc_out <int> num;
		sc_out <bool> start;
	protected:
		bool flag;
		void generate();
};

RandGen :: RandGen(sc_module_name name) : sc_module(name) 
{
	srand(time(NULL));
	flag = false;
	SC_METHOD(generate);
	sensitive << gen;
}

void RandGen :: generate()
{
	bool t_gen;
	int wait_time = 0;

	t_gen = gen -> read(); // 000011101101
	
	// Ovde ce da postavlja na izlaz
	if(flag)
	{
		start -> write(true);
		num -> write(rand() % 100);
	}
		
	if(t_gen)
	{
		wait_time = rand() % 6 + 5;
		sc_time t(wait_time, SC_NS);
		flag = true;
		next_trigger(t);
	}
	else
	{
		flag = false;
		start -> write(false);
		next_trigger();		
	}

}


/* --------------------------------------------------------------------------- */

class coll : public sc_module
{
	public:
		SC_HAS_PROCESS(coll);
		coll(sc_module_name);
		sc_in <int> num0;
		sc_in <int> num1;
		sc_in <int> num2;
		sc_export<sc_fifo_out_if<int>> sort_o;
	protected:
		void process();
		sc_fifo<int> fifo;
};

coll :: coll(sc_module_name name) : sc_module(name)
{
	sort_o.bind(fifo);
	SC_THREAD(process);
}

// Komponenta za sortiranje
void coll :: process()
{
	bool first = true;
	int num0_old, num1_old, num2_old;
	int count = 0;
	vector<int> vec(100);
	
	while(true)
	{
		if(first)
		{
			num0_old = num0 -> read();
			num1_old = num1 -> read();
			num2_old = num2 -> read();
			first = false;
		}
		else
		{
			if((num0 -> read() != num0_old) && (count < 100))
			{
				vec.push_back(num0 -> read());
				count++;
			}
			else if((num1 -> read() != num1_old) && (count < 100))
			{
				vec.push_back(num1 -> read());
				count++;
			}
			else if((num2 -> read() != num2_old) && (count < 100))
			{
				vec.push_back(num2 -> read());
				count++;
			}
			
			num0_old = num0 -> read();
			num1_old = num1 -> read();
			num2_old = num2 -> read();
		}
		
		if(count == 100)
		{
			sort(vec.begin(), vec.end());
			for(int i = 0; i < vec.size(); i++)
				fifo.nb_write(vec[0]);
		}
	}
	
}
/* --------------------------------------------------------------------------- */

class system : public sc_module
{
	public:
		system(sc_module_name);
		sc_in<bool> gen_i;
		sc_out<bool> start_o;
		sc_export<sc_fifo_out_if<int>> sort_out;
	protected:
		sc_signal<int> num0_s;
		sc_signal<int> num1_s;
		sc_signal<int> num2_s;
		sc_signal<bool> s0;
		sc_signal<bool> s1;
		RandGen rg0;
		RandGen rg1;
		RandGen rg2;
		coll c0;
		
};

system :: system(sc_module_name name) : sc_module(name), rg0("gen0"), rg1("gen1"), rg2("gen2"), c0("coll")
{
	gen_i.bind(rg1.gen);
	
	rg0.start.bind(s0);
	rg1.gen.bind(s0);
	rg1.start.bind(s1);
	rg2.gen.bind(s1);
	
	start_o.bind(rg2.start);
	
	rg0.num.bind(num0_s);
	rg1.num.bind(num1_s);
	rg2.num.bind(num2_s);
	
	c0.num0.bind(num0_s);
	c0.num1.bind(num1_s);
	c0.num2.bind(num2_s);
	
	sort_out.bind(c0.sort_o);
}
/* --------------------------------------------------------------------------- */

class tb : public  sc_module
{
	public:
		SC_HAS_PROCESS(tb);
		tb(sc_module_name);	
	protected:
		system uut;
		sc_signal<bool> sig;
		
		void driver();
		void monitor();
		
};

tb :: tb(sc_module_name name) : sc_module(name), uut("System")
{
	uut.gen_i(sig);
	SC_THREAD(driver);
	SC_METHOD(monitor);
	sensitive << uut.sort_out;
}

void tb :: driver()
{
	while(true)
	{
		sig = false;
		wait(11, SC_NS);
		sig = true;
		wait(11, SC_NS);
	}
}

void tb :: monitor()
{
	if(uut.sort_out.num_available() == 100)
	{
		cout << "Vrednosti su" << endl;
		for(int i = 0; i < 100; i++)
			cout << uut.sort_out.nb_read() << " ";
		cout << endl;
	}
}
/* --------------------------------------------------------------------------- */

int sc_main( int argc , char* argv[])
{
	tb t("TB");
	
	sc_start(1000, SC_NS);
	
	return 0;
}
