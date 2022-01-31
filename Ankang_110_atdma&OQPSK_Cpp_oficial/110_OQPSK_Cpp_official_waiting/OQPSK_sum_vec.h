
//#include <iostream>
//using namespace std;
//#include <vector>

double OQPSK_sum_vec(vector<double> &x, int start_point , int end_point)
{
    double sum_tmp = 0;
    #pragma omp parallel for
    for (int i = start_point; i < end_point; i++)
    {
       sum_tmp = sum_tmp + x[i];
    }
    return sum_tmp;
}