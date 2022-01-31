//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
void norm(vector<double> &vec1_I, vector<double> &vec1_Q, vector<double> &vec2_I, vector<double> &vec2_Q, int N)
{
    double sum_I = 0, sum_Q = 0, norm2 = 0;
    
    for(int i = 0; i < N; i++){
    sum_I = vec1_I[i] - vec2_I[i];
    sum_Q = vec1_Q[i] - vec2_Q[i];
    norm2 = norm2 + sqrt(sum_I * sum_I + sum_Q * sum_Q);
    sum_I = 0, sum_Q = 0; 
    }
    printf("two norm  = %f\n", norm2);
}    