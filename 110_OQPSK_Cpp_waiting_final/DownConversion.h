//////////////////////////////////////////////////////////////////
//  DownConversion
//  
//  Input variable:
//  int *InData_I: real part of InData before down convertor
//  int *InData_Q: imaginary part of InData before down convertor
//  
//  double *OutData_I: real part of OutData after down convertor
//  double *OutData_Q: imaginary part of OutData after down convertor
//   
//  int DataLength: length of InData_I and InData_Q
// 
//  fc: center frequency of subcarrier
//  fs: sampling frequency
//////////////////////////////////////////////////////////////////
void DownConversion(vector<double> &OutData_I, vector<double> &OutData_Q, vector<double> &InData_I, vector<double> &InData_Q, int DataLength, double fc, double fs)
{
    double ff = 2 * pi * fc / fs;
    double coscoff, sincoff;
    for (int i = 0; i < DataLength; i++)
    {
    	coscoff = cos((i + 1) * ff); // NOTE: in matlab: Sig_demod = InSig .* exp(-1i * 2 * pi * Ts * f_c * (1 : length(InSig)));
    	sincoff = sin((i + 1) * ff);

        OutData_I.push_back(InData_I[i] * coscoff + InData_Q[i] * sincoff);
        OutData_Q.push_back(- InData_I[i] * sincoff + InData_Q[i] * coscoff);
    }
}