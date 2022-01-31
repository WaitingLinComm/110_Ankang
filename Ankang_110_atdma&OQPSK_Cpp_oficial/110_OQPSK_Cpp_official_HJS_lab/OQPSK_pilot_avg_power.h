//////////////////////////////////////////////////////////////////
// OQPSK_pilot_avg_power
//  
// [Input] int pilotType(Type 1 ~ Type 6), vector<int> &pilot_manual(The manually input pilot symbol)
// [Output] double avg_power(average power of the pilot)
// NOTE:
// There are 5 kinds of pilots((Type 1 ~ Type 5) in OQPSK. % Type 5 is the correct pilot of the data with filename = "burst2_sr6.25_dtCplx_st8t". 
// If setting pilotType = 6, the code supports the manually input pilot symbol. 
//
// Input different types of pilots and calculate the average power of pilot through the peak found in MF function
// If the OQPSK pilot is mismatch with data burst: show message "This OQPSK pilot is not used for the data burst!" and return avg_power = 0.
// If the OQPSK pilot is match with data burst: show message "This OQPSK pilot is used for the data burst!" and return avg_power.
//////////////////////////////////////////////////////////////////
double OQPSK_pilot_avg_power(int pilotType, vector<int> &pilot_manual, double fc, double fsy, double fs, const char *filename, int filelength){ 
    /////////////////////// pilot ///////////////////////
    int Symbol_Length_1 = 27; // Symbol_Length = sizeof(pilotPattern)/sizeof(*pilotPattern);
    int pilotPattern_1[27] = {2,0,3,0,3,2,1,0,2,2,0,0,0,2,3,1,3,2,1,3,2,2,1,1,2,2,2};
    int Symbol_Length_2 = 54;
    int pilotPattern_2[54] = {0,2,1,2,1,1,3,3,1,1,3,1,1,0,1,3,1,0,0,0,0,2,3,2,2,0,1,1,1,2,3,2,2,1,0,2,3,0,0,2,3,0,1,2,1,2,0,3};
    int Symbol_Length_3 = 27;
    int pilotPattern_3[27] = {2,3,1,0,1,3,1,1,0,3,0,1,1,3,2,0,3,0,0,3,3,1,2,1,1,3,0};
    int Symbol_Length_4 = 27;
    int pilotPattern_4[27] = {2,0,3,3,2,0,3,2,3,2,0,3,0,1,1,1,3,0,3,2,2,3,0,1,3,0,2};
    int Symbol_Length_5 = 27;
    int pilotPattern_5[27] = {2,1,3,0,3,1,3,3,0,1,0,3,3,1,2,0,1,0,0,1,1,3,2,3,3,1,0};
    
    int Symbol_Length;
    if (pilotType == 1){
        Symbol_Length = Symbol_Length_1;
    }
    if (pilotType == 2){
        Symbol_Length = Symbol_Length_2;
    }
    if (pilotType == 3){
        Symbol_Length = Symbol_Length_3;
    }
    if (pilotType == 4){
        Symbol_Length = Symbol_Length_4;
    }
    if (pilotType == 5){
        Symbol_Length = Symbol_Length_5;
    }
    if (pilotType == 6){
        Symbol_Length = pilot_manual.size();
    }

    int pilotPattern[Symbol_Length];
    vector<int> pilotPattern_vec;
    pilotPattern_vec.reserve(Symbol_Length);
    int Pattern_Length;
    if(pilotType == 1){
        Pattern_Length = Symbol_Length_1 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilotPattern_1[i];
            pilotPattern[i] = pilotPattern_1[i];
        }
    }
    if(pilotType == 2){
        Pattern_Length = Symbol_Length_2 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilotPattern_2[i];
            pilotPattern[i] = pilotPattern_2[i];
        }
    }
    if(pilotType == 3){
        Pattern_Length = Symbol_Length_3 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilotPattern_3[i];
            pilotPattern[i] = pilotPattern_3[i];
        }
    }
    if(pilotType == 4){
        Pattern_Length = Symbol_Length_4 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilotPattern_4[i];
            pilotPattern[i] = pilotPattern_4[i];
        }
    }
    if(pilotType == 5){
        Pattern_Length = Symbol_Length_5 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilotPattern_5[i];
            pilotPattern[i] = pilotPattern_5[i];
        }
    }
    // The manually input pilot symbol
    if(pilotType == 6){
        Pattern_Length = Symbol_Length_5 * 2;
        for(int i = 0; i < Symbol_Length; i++){
            pilotPattern_vec[i] = pilot_manual[i];
            pilotPattern[i] = pilot_manual[i];
        }
    }
     

    // string DataType("OQPSK"); 
    // cout << "DataType: " << DataType << endl; 
    ///////////////////////   Data Paremeters   ////////////////////// 
    //double fc = -338745;                    // center frequency of subcarrier
    // Tx info.
    //double fsy = 0.256 * pow(10, 6);        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    // USRP info.
    //double fs_USRP = 6.25 * pow(10, 6);     // sampling frequency
    double fs_USRP = fs;
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    
    ///////////////////////   Read File   //////////////////////////// 
    //const char *filename;
    //filename = "burst2_sr6.25_dtCplx_st8t"; //110 OQPSK
    //int filelength = 12.5 * pow(10, 6);   // Number of used samples: 1s data 
    //int filelength = 1.25 * pow(10, 6);   // Number of used samples: 0.1s data 
    //int filelength = 125 * pow(10,6);       // Number of used samples 
    int SigLength = filelength/2;    
    vector<double> rawDataI, rawDataQ;
    rawDataI.reserve(SigLength); rawDataQ.reserve(SigLength); 
    OQPSK_DataConversion(filename, filelength, rawDataI, rawDataQ);
    //printf("Data conversion is done in %1f(sec).\n", (t2-t1)/(double)(CLOCKS_PER_SEC));
    ////////////////////////// DC Blocker (Remove DC offset)  /////////////////////// 
    vector<double> Data_withoutDC_I, Data_withoutDC_Q;
    Data_withoutDC_I.reserve(SigLength); Data_withoutDC_Q.reserve(SigLength); 

    OQPSK_DC_Blocker(Data_withoutDC_I, Data_withoutDC_Q, SigLength, rawDataI, rawDataQ);
    rawDataI.clear(); vector<double>(rawDataI).swap(rawDataI);
    rawDataQ.clear(); vector<double>(rawDataQ).swap(rawDataQ);
    //printf("DC offset of data is removed in %1f(sec).\n", (t4-t3)/(double)(CLOCKS_PER_SEC));

    //////////////////////////  Down Conversion  ///////////////////////////////////// 
    vector<double> DataDownconversion_I, DataDownconversion_Q;
    DataDownconversion_I.reserve(SigLength); DataDownconversion_Q.reserve(SigLength); 

    OQPSK_DownConversion(DataDownconversion_I, DataDownconversion_Q, Data_withoutDC_I, Data_withoutDC_Q, SigLength, fc, fs_USRP);
    Data_withoutDC_I.clear(); vector<double>(Data_withoutDC_I).swap(Data_withoutDC_I);
    Data_withoutDC_Q.clear(); vector<double>(Data_withoutDC_Q).swap(Data_withoutDC_Q);
    //printf("Down conversion is done in %1f(sec).\n", (t6-t5)/(double)(CLOCKS_PER_SEC));

    /////////////////////////  Lowpass Filter  //////////////////////////////////////
    //  n = 3; Wn = 2/39;
    //  [b,a] = butter(n, Wn, 'low');
    //  b = [0.0004    0.0013    0.0013    0.0004];
    //  a = [1.0000   -2.6781    2.4060   -0.7243];
    //  Transfer function:
    //  H(z) = {b(1) + b(2)z^{-1} + b(3)z^{-2} + b(4)z^{-3}}/{a(1) + a(2)z^{-1} + a(3)z^{-2} + a(4)z^{-3}}
    //  data_LP = filter(b, a, data_demod);
    //  
    vector<double> data_LP_I, data_LP_Q;
    data_LP_I.reserve(SigLength); data_LP_Q.reserve(SigLength); 

    vector<double> b, a;
    b.reserve(4); a.reserve(4); 
    //  We get the value of a, b from matlab
    b.push_back(0.000447748392604308);
    b.push_back(0.00134324517781292);
    b.push_back(0.00134324517781292);
    b.push_back(0.000447748392604308);
    
    a.push_back(1);
    a.push_back(-2.67812669726477);
    a.push_back(2.4059972477312);
    a.push_back(-0.724288563325587);
 
    OQPSK_Filter_TransferFunction(b, a, DataDownconversion_I, DataDownconversion_Q, data_LP_I, data_LP_Q);
    b.clear(); vector<double>(b).swap(b);
    a.clear(); vector<double>(a).swap(a);
    DataDownconversion_I.clear(); vector<double>(DataDownconversion_I).swap(DataDownconversion_I);
    DataDownconversion_Q.clear(); vector<double>(DataDownconversion_Q).swap(DataDownconversion_Q);
 
    /////////////////////////  Resample(Sinc Interpolation)  /////////////////////////
    double old_sps = sps_USRP;
    double new_sps = 26;            // Set by ourselves
    double N = new_sps / old_sps;   // upsample by N. NOTE: N can be non-integer 
    double window = 1;              //double window = 6; // half-sided window length(used in SincInterpolation)  
    int SigLength_afterResample = floor(SigLength * N);
    vector<double> Sig_I, Sig_Q;
    Sig_I.reserve(SigLength_afterResample); Sig_Q.reserve(SigLength_afterResample); 
    OQPSK_resample(Sig_I, Sig_Q, SigLength_afterResample, data_LP_I, data_LP_Q, SigLength, N, window);
    //printf("Data resample is done in %1f(sec).\n", (t8-t7)/(double)(CLOCKS_PER_SEC));
    data_LP_I.clear(); vector<double>(data_LP_I).swap(data_LP_I);
    data_LP_Q.clear(); vector<double>(data_LP_Q).swap(data_LP_Q);
    double new_fs = fs_USRP * N;  
    
    /////////////////////////  Pulse Shaping Filter  /////////////////////////
    int FilterSpanInSymbols = 6;
    float RolloffFactor = 0.3;
    int L_pulseshaping = FilterSpanInSymbols * new_sps + 1; //157
	vector<double> PulseshapingFilter;
    PulseshapingFilter.reserve(L_pulseshaping); 
    OQPSK_SRRC(PulseshapingFilter, FilterSpanInSymbols, RolloffFactor, new_sps, L_pulseshaping);
    
    // NOTE: Pulseshaping is a real value vector
    // Although Pulseshaping is a real value vector, we would like to set its imaginary part as 0
    // for the ease of later calculation of complex convolution.
    // vector<double> Pulseshaping_I, Pulseshaping_Q;
    // Pulseshaping_I.reserve(L_pulseshaping); Pulseshaping_Q.reserve(L_pulseshaping);
    // for(int i = 0; i < L_pulseshaping; i++){
    // Pulseshaping_I.push_back(PulseshapingFilter[i]);
    // // set the imaginary part of Pulseshaping as zero
    // Pulseshaping_Q.push_back(0);
    // }
    // PulseshapingFilter.clear(); 

    /////////////////  Bit sequence generating parameters  /////////////////
    // if bitorder = 1: even number of b represents real bit value
    //                  odd number of b represents imaginary bit value
    // if bitorder = 2: even number of b represents imaginary bit value
    //                  odd number of b represents real bit value
    // if bitmap = 1: change bit value(1 / 0) to voltage (-1 / 1)
    // if bitmap = 2: change bit value(1 / 0) to voltage (1 / -1)
    //
    // if IQinterlace = 1: Q leads I
    // if IQinterlace = 2: I leads Q
    
    int bitorder = 1;
    int bitmap = 2;
    int IQinterlace = 1;
    ///////////////////// Get syncSig from pilots  ////////////////////
    /////////////////////    1. symbol_mapping     ////////////////////
    vector<double> m_p;
    m_p.reserve(Pattern_Length); //the memory pre-allocate for symbol mapping
    vector<double> pilotSym_I, pilotSym_Q;
    pilotSym_I.reserve(Symbol_Length); pilotSym_Q.reserve(Symbol_Length);
    vector<double> syncSig_I, syncSig_Q; 
    // NOTE! The memory of syncSig_I, syncSig_Q will be allocated in "OQPSK_frontendTx.h", because the length of syncSig_I and syncSig_Q are not know yet.
    // and we can get the length of syncSig_I and syncSig_Q after "OQPSK_frontendTx.h""
    OQPSK_symbol_mapping(m_p, pilotSym_I, pilotSym_Q, pilotPattern, Symbol_Length);   //Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
    OQPSK_frontendTx(syncSig_I, syncSig_Q, m_p, PulseshapingFilter, L_pulseshaping, Pattern_Length, new_sps, bitorder, bitmap, IQinterlace);
    int L_syncSig = syncSig_I.size(); 
    //printf("OQPSK frontendTx process is done in %f(sec).\n", (t10-t9)/(double)(CLOCKS_PER_SEC)); 

    //////////////////////   Course TO estimation   //////////////////////  
    int L_r = SigLength_afterResample;
    int L_q = L_syncSig;
    int sps = new_sps;

    vector<double>  q_flip_I, q_flip_Q;
    q_flip_I.reserve(L_q); vector<double>(q_flip_I).swap(q_flip_I);
    q_flip_Q.reserve(L_q); vector<double>(q_flip_Q).swap(q_flip_Q);

    OQPSK_Flip_vec(q_flip_I, syncSig_I, L_q);
    OQPSK_Flip_vec(q_flip_Q, syncSig_Q, L_q);
    syncSig_I.clear(); vector<double>(syncSig_I).swap(syncSig_I);
    syncSig_Q.clear(); vector<double>(syncSig_Q).swap(syncSig_Q);

    // realize : conj(q(end : -1 : 1)
    vector<double>  conj_q_flip_Q;
    conj_q_flip_Q.reserve(L_q); 
    for(int i = 0; i < L_q; i++){ 
        conj_q_flip_Q.push_back(- q_flip_Q[i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }
    
    // NOTE!
    // If we would like to perform z = filter(x, 1, y), the output length Lz = Ly.
    // If we would like to perform z = conv(x, y) the output length Lz = Lx + Ly - 1.
    //
    ////////////////       realize : y1 = filter(conj(q(end : -1 : 1)), 1, r)     /////////////////////
    //int L_out = L_r + L_q - 1;   // realize y1 = conv(conj(q(end : -1 : 1)), 1, r)
    //int L_out = L_r;               // realize y1 = filter(conj(q(end : -1 : 1)), 1, r)
    //y1_I.reserve(L_out); y1_Q.reserve(L_out);
    
    vector<double> y1_norm2;
    y1_norm2.reserve(L_r);
    OQPSK_complex_convolution_with_norm(y1_norm2, L_r ,q_flip_I, conj_q_flip_Q, L_q, Sig_I, Sig_Q, L_r);
    q_flip_I.clear(); q_flip_Q.clear();
    vector<double>(q_flip_I).swap(q_flip_I); vector<double>(q_flip_Q).swap(q_flip_Q);
    conj_q_flip_Q.clear(); vector<double>(conj_q_flip_Q).swap(conj_q_flip_Q);
    Sig_I.clear(); Sig_Q.clear(); 
    //cout << "Vector 的 容量為" << Sig_I . capacity ( ) << endl ; 
    vector<double>(Sig_I).swap (Sig_I); vector<double>(Sig_Q).swap (Sig_Q);
    //cout << "Vector 的 容量為" << Sig_I . capacity ( ) << endl ; 

    vector<int> burst_interval;  
    OQPSK_find_high_pilot(burst_interval, y1_norm2, sps);

    int number_of_pilot = burst_interval.size();
    // vector<int> coarse_pilot_position;
    // coarse_pilot_position.reserve(number_of_pilot);
    // for (int i = 0; i < number_of_pilot; i++){
    //     coarse_pilot_position.push_back(burst_interval[i] - L_q + 1);
    // }
    
    double y1_abs_pilot_sum;
    double avg_power;
    int pilot_index;    
    if(number_of_pilot < 10)
    {
        //cout << "This OQPSK pilot is not used for the data burst!" <<"\n";
        avg_power = 0.0;
    }
    else
    {
        //cout << "This OQPSK pilot is used for the data burst." <<"\n";
        y1_abs_pilot_sum = 0;
        for (int i = 0; i < number_of_pilot; i++){
            pilot_index = burst_interval[i];
            y1_abs_pilot_sum = y1_abs_pilot_sum + sqrt(sqrt(y1_norm2[pilot_index]));
        }
        avg_power = y1_abs_pilot_sum/number_of_pilot;
    }
    burst_interval.clear(); vector<int>(burst_interval).swap(burst_interval);
    y1_norm2.clear();       vector<double>(y1_norm2).swap(y1_norm2);
    //cout << avg_power<<"\n";
    return avg_power;   
}
