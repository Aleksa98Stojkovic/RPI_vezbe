#include "coll.hpp"

using namespace std;
using namespace sc_core;

coll::coll(sc_module_name name) : sc_module(name)
{

	sort_o.bind(fifo);
	SC_THREAD(process);

	cout << "Konstruisan je Coll" << endl;
}

// Komponenta za sortiranje
void coll::process()
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
