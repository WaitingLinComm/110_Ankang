//////////////////////////////////////////////////////////////////
//  OQPSK_frontendTx
//
//  return L_OQPSKwaveform; // Length of OQPSKwaveform
//////////////////////////////////////////////////////////////////
//#include "bitReorganization.h"
//#include "bitMapping.h"
//#include "I_Q_interlace.h"
//#include "Upsample.h"
//#include "convolution.h"

void OQPSK_frontendTx(vector<double> &OQPSKwaveform_I, vector<double> &OQPSKwaveform_Q, vector<double> &m, vector<double> &Pulseshaping, int FilterLength, int Pattern_Length, int sps, int bitorder, int bitmap, int IQinterlace)
{
    //////////////////////  Reorganize bit order   /////////////////////
    vector<double> b;
    b.reserve(Pattern_Length);
    bitReorganization(bitorder, m, b, Pattern_Length);

    //////////////////////       Bit Mapping       /////////////////////
    //// Change bit value (1 / 0) to voltage (-1 / 1) or bit value (1 /  0) to voltage (1 / -1) ////
    vector<double> psi;
    psi.reserve(Pattern_Length);
    bitMapping(bitmap, b, psi, Pattern_Length); 
    
    /////////////////////     I Q interlace        /////////////////////
    //double *l_I = (double*)malloc(Pattern_Length * sizeof(double));
    //double *l_Q = (double*)malloc(Pattern_Length * sizeof(double));
    vector<double> l_I, l_Q;
    l_I.reserve(Pattern_Length); l_Q.reserve(Pattern_Length);
    I_Q_interlace(IQinterlace, psi, l_I, l_Q, Pattern_Length);

    /////////////////////    upsample by sps/2    /////////////////////
    int N = sps / 2;
    int L_u = ((N - 1) * Pattern_Length) + Pattern_Length; // Length of the upsample output: u
    vector<double> u_I, u_Q;
    u_I.reserve(L_u); u_Q.reserve(L_u);

    Upsample(l_I, u_I, N, L_u);
    Upsample(l_Q, u_Q, N, L_u);
    
    //// Employ pulse shaping filter (Tx) and create OQPSK waveform ////
    int L_OQPSKwaveform = L_u + FilterLength - 1; // Length of OQPSKwaveform
    OQPSKwaveform_I.reserve(L_OQPSKwaveform);
    OQPSKwaveform_Q.reserve(L_OQPSKwaveform);
    
    vector<double> Pulseshaping_Q(FilterLength,0);
    complex_convolution(OQPSKwaveform_I, OQPSKwaveform_Q, L_OQPSKwaveform ,Pulseshaping, Pulseshaping_Q, FilterLength, u_I, u_Q, L_u);
}