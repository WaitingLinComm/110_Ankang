#include <vector>
#include <iostream>
using namespace std;

int main()
{
    
    int N = 3;
    vector<double> x, y, z;
    x.push_back(1); x.push_back(2); x.push_back(3);
    y.push_back(1); y.push_back(2); y.push_back(4); 
    z.reserve(N);
    int Lx = N;
    int Ly = N;
    int Lz = N;

    int i, j, i1;
    double tmp;

    for (i = 0; i < Lz; i++)
    {
        i1 = i;
        tmp = 0;
        for (j=0; j < Ly; j++)
        {
            if(i1 >= 0 && i1 < Lx)
                tmp = tmp + (x[i1] * y[j]);

            i1 = i1-1;
            //z[i] = tmp;			
        }
        z.push_back(tmp); 
    }
    cout << z.size() << '\n';
    for (int i = 0; i < 3; i++){
        printf("%f\n", z[i]);
    }

    return 0;
}