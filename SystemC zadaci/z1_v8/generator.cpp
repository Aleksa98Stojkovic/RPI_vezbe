#include "generator.h"
#include <vector>
#include <ctime>

using namespace std;
using namespace sc_core;

generator::generator(sc_module_name name) :
	sc_module(name)
{
  cout << "Napravljen je generator" << endl;;
  SC_THREAD(test);
  srand(time(nullptr));
}

void generator::test()
{

    vector <float> a = {0.2, 0.132, 1.4, -0.32}; // koeficijenti uz y-ne veceg reda
    vector <float> b = {0.1, -2.2, 0.56, 0.198}; // koeficijenti uz x-eve

    vector <float> result, sim_result;

    int len = 8;
    vector<float> tran;

    for (int i=0; i != len; ++i)
    {
        float num = (float)(rand() % 100000) / 10000.0;
        tran.push_back(num);
    }

    for(int testnum = 0; testnum != 10; testnum++)
    {

        wr_port->write(tran);
        cout << "Poslata je transakcija" << endl;

        // Prvo racunam odziv koji bi trebalo da dobijem
        float response = 0;

        for(int i = 0; i < (int)tran.size() / 2; i++)
        {
            response += b[i] * tran[i];
        }
        cout << endl;
        for(int i = (int)tran.size() / 2; i < (int)tran.size(); i++)
        {
            response -= a[i - (int)tran.size() / 2] * tran[i];
        }

        result.push_back(response); // ovde cu da cuvam tacne vrednosti

        rd_port -> read(response);
        sim_result.push_back(response);

        tran.erase(tran.begin() + 4);
        tran.push_back(response);
        tran.erase(tran.begin());
        tran.insert(tran.begin() + 4 ,(float)(rand() % 100000) / 10000.0);

    }

    if(result.size() != sim_result.size())
    {
        cout << "Greska prilikom testiranja" << endl;
    }
    else
    {
        for(int i = 0; i < (int)result.size(); i++)
        {
            cout << "Pravi rezultat je: " << result[i] << ", a rezultat simulacije: " << sim_result[i] << endl;
        }
    }
}
