// 
/////////////////////////////////////////////////////////
// The function transfer array data type to vector data type
/////////////////////////////////////////////////////////
// Input:  array type
// Output: vector type
//
#include <vector>
#include <iostream>
using namespace std;

void arraytovec(vector<double> &vec_I, vector<double> &vec_Q, double *array_I, double *array_Q, int N)
{
    for(int i = 0; i < N; i++){
        vec_I.push_back(array_I[i]);// (NO!) vec_I[i] = array_I[i]; //vec_I.push_back(array_I[i]);
        vec_Q.push_back(array_Q[i]);// (NO!) vec_Q[i] = array_Q[i]; //vec_Q.push_back(array_Q[i]);
    }
}    

// NOTE!!! 第一次加值進去vector時，需用push_back, 不可用 vec_I[i] = array_I[i]
// 若用尋訪的方式加入vector, 雖然能印出值，但實際上這些數值沒有被給予空間!!!
