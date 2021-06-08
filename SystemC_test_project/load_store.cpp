#include "load_store.h"

using namespace std;

void LoadInput(string path, float3D &IFM)
{
    ifstream file;
    vector <t> numbers;
    string line;
    int c = 0;
    file.open(path);
    while(getline(file, line))
    {
        stringstream sline(line);

        while(sline.good())
        {
            string str;
            getline(sline, str, ',');
            if(numbers.size() < IFM.size() * IFM[0].size())
                numbers.push_back(stof(str));
        }
        for(int x = 0; x < (int)(IFM.size()); x++)
        {
            for(int y = 0; y < (int)(IFM[0].size()); y++)
            {
                IFM[x][y][c] = numbers[0];
                numbers.erase(numbers.begin());
            }
        }
        c++;
    }
    file.close();
}

// Upisivanje rezultata, verifikovano **
void WriteFile(string path, const float3D &OFM)
{
    ofstream file;
    file.open(path);
    for(int c = 0; c < (int)OFM[0][0].size(); c++)
    {
        for(int x = 0; x < (int)OFM.size(); x++)
        {
            for(int y = 0; y < (int)OFM[0].size(); y++)
            {
                file << to_string(OFM[x][y][c]) + ",";
            }
        }
        file << endl;
    }
    file.close();
}

// Ucitavanje tezina, verifikovano **
// Fajl je sledeceg formata: Za svaki filtar stapic imamo jednu lniju
// Unutar linije imamo zaredjano 64 kernela, pri cemu je forma vidljiva na primeru
// [[1, 2, 3],  ovaj kernel ce se ispraviti u 1D niz 1,2,3,4,5,6,7,8,9
//  [4, 5, 6],
//  [7, 8, 9]]
// Ako je sledeci kernel unutar tog filter stapica bio
// [[3, 4, 1], onda se on spaja na prethodni, pa se u liniji nalazi 1,2,3,4,5,6,7,8,9,3,4,1,5,1,45,32,12,2
//  [5, 1, 45],
//  [32, 12, 2]]

void LoadFile(string path, float4D &W)
{
    ifstream file(path);
    vector <t> numbers;
    string line;
    int n = 0;
    while(getline(file, line))
    {
        stringstream sline(line);

        while(sline.good())
        {
            string str;
            getline(sline, str, ',');
            if(numbers.size() < W[0].size() * W[0][0].size() * W[0][0][0].size())
                numbers.push_back(stof(str));
        }

        for(int d = 0; d < (int)(W[0][0][0].size()); d++)
        {
            for(int x = 0; x < (int)(W[0].size()); x++)
            {
                for(int y = 0; y < (int)(W[0][0].size()); y++)
                {
                    W[n][x][y][d] = numbers[0];
                    numbers.erase(numbers.begin());
                }
            }
        }
        n++;
    }
    file.close();
}

void LoadBias(string path, vector <t> &b)
{
    ifstream file(path);
    string line;
    vector <t> numbers;
    while(getline(file, line))
    {
        stringstream sline(line);

        while(sline.good())
        {
            string str;
            getline(sline, str, ',');
            if(numbers.size() < b.size())
                numbers.push_back(stof(str));
        }
    }

    b.clear();
    b = numbers;
    numbers.clear();
}

