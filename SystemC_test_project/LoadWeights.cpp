// Treba realizovati funkcije za ucitavanje tezina u jedan 4D niz

#include "LoadWeights.h"

void LoadWeights(string path, vector < vector < vector < vector <float>>>> &W)
{
    ifstream file(path);
    vector <float> numbers;
    string line;
    int n = 0;

    while(getline(file, line))
    {
        stringstream sline(line);
        n = 0;
        while(sline.good())
        {
            string str;
            getline(sline, str, ',');
            if(numbers.size() < W.size() * W[0].size() * W[0][0].size())
                numbers.push_back(stof(str));
        }

        for(int x = 0; x < (int)(W[0].size()); x++)
        {
            for(int y = 0; y < (int)(W[0][0].size()); y++)
            {
                for(int d = 0; d < (int)(W[0][0][0].size()); d++)
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


