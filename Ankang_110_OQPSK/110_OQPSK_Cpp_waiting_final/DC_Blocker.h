//////////////////////////////////////////////////////////////////
//   DC_Blocker
//  
//   [Input variable]
//   int *InData_I: real part of InData with DC
//   int *InData_Q: imaginary part of InData with DC
//  
//   double *OutData_I: real part of OutData without DC
//   double *OutData_Q: imaginary part of OutData without DC
//  
//   int DataLength: length of InData_I and InData_Q
//////////////////////////////////////////////////////////////////
void DC_Blocker(vector<double> &OutData_I, vector<double> &OutData_Q, int DataLength, vector<double> &InData_I, vector<double> &InData_Q)
{
    double DCvalue_I = 0;
    double DCvalue_Q = 0;  
    double avgDCvalue_I;
    double avgDCvalue_Q;
    int i;

    for(i = 0; i < DataLength; i++)
    {
        DCvalue_I = DCvalue_I + InData_I[i];
        DCvalue_Q = DCvalue_Q + InData_Q[i];
    }
    avgDCvalue_I = DCvalue_I / DataLength;
    avgDCvalue_Q = DCvalue_Q / DataLength;

    for(i = 0; i < DataLength; i++)
    {
        OutData_I.push_back(InData_I[i] - avgDCvalue_I);
        OutData_Q.push_back(InData_Q[i] - avgDCvalue_Q);
    }
}