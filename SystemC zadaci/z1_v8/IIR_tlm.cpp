#include "IIR_tlm.h"

using namespace std;
using namespace sc_core;

IIR_tlm :: IIR_tlm(sc_module_name name) : sc_module(name)
{
    cout << "Napravljen je IIR_tlm modul" << endl;
}

void IIR_tlm :: write(const std :: vector<float> &data)
{
    x.clear();
    y.clear();

    for(int i = 0; i < data.size() / 2; i++)
        x.push_back(data[i]);
    for(int i = data.size() / 2; i < data.size(); i++)
        y.push_back(data[i]);

    cout << "Upisani su podaci" << endl;
    filter_response(); // izracunaj novu vrednost izlaza
}

void IIR_tlm :: read(float &data)
{
    data = response; // Salje se poslednji podatak u y vektoru, jer je on uvek najnoviji
    cout << "Procitan je podatak " << response << endl;
}

void IIR_tlm :: filter_response()
{
    vector <float> a = {0.2, 0.132, 1.4, -0.32}; // koeficijenti uz y-ne veceg reda
    vector <float> b = {0.1, -2.2, 0.56, 0.198}; // koeficijenti uz x-eve

    response = 0;

    for(int i = 0; i < x.size(); i++)
        response += b[i] * x[i];
    for(int i = 0; i < y.size(); i++)
        response -= a[i] * y[i];

}
