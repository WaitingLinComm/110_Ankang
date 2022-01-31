// Execute Command
// g++ -o d main.cpp -lfftw3 -lm 

// NOTE:
// The code adopts fft/ifft implementation in FFTW library to perform the convolution of two complex vectors.
// Recall: conv(x,y) = ifft(fft(x).*fft(y)) where x and y are two complex vectors.
// To #include <fftw3.h>, one should follow instruction in https://www.fftw.org to install FFTW library.

//#include <omp.h>
#include <iostream> // for file written
#include <fstream>  // for file written
#include <fftw3.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <string> 
#include <vector>
#include <time.h>
#include <math.h>
#define pi 3.141592653589793238462643383279502884

#include "OQPSK_DataConversion.h"                 // used in "OQPSK_main"
#include "OQPSK_DC_Blocker.h"                     // used in "OQPSK_main"
#include "OQPSK_DownConversion.h"                 // used in "OQPSK_main"
#include "OQPSK_complex_convolution.h"            // used in "OQPSK_main and "OQPSK_frontendTx.h"
#include "OQPSK_overlap_add.h"                    // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_Filter_TransferFunction.h"        // used in "OQPSK_main" 
#include "OQPSK_sinc.h"                           // used in "OQPSK_resample.h"
#include "OQPSK_resample.h"                       // used in "OQPSK_main"
#include "OQPSK_SRRC.h"                           // used in "OQPSK_main"
#include "OQPSK_symbol_mapping.h"                 // used in "OQPSK_main"

#include "OQPSK_bitReorganization.h"              // used in "OQPSK_frontendTx.h"
#include "OQPSK_bitMapping.h"                     // used in "OQPSK_frontendTx.h"
#include "OQPSK_I_Q_interlace.h"                  // used in "OQPSK_frontendTx.h"
#include "OQPSK_Upsample.h"                       // used in "OQPSK_frontendTx.h"
#include "OQPSK_frontendTx.h"                     // used in "OQPSK_main"

#include "OQPSK_sum_vec.h"                        // used in "OQPSK_find_high_pilot.h"
#include "OQPSK_MAX_vec.h"                        // used in "OQPSK_find_high_pilot.h"
#include "OQPSK_find_high_pilot.h"                // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_Flip_vec.h"                       // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_complex_convolution_with_norm.h"  // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_coarseTO_estimator.h"             // used in "OQPSK_main"

#include "OQPSK_tmp_cfo_compen.h"                 // used in "OQPSK_CFOPHOTO_estimator_v2.h"
// #include "OQPSK_CFOPHOTO_estimator.h"          // used in "OQPSK_main"
#include "OQPSK_CFOPHOTO_estimator_v2.h"          // used in "OQPSK_main"

#include "OQPSK_get_databurst.h"                  // used in "OQPSK_main"
#include "OQPSK_CFOPHOcompen.h"                   // used in "OQPSK_main"
#include "OQPSK_conv_vec.h"                       // used in "OQPSK_OQPSK_aftendRx.h"
#include "OQPSK_aftendRx.h"                       // used in "OQPSK_main"
#include "OQPSK_DecisionProcess.h"                // used in "OQPSK_main"
#include "OQPSK_bitReorganizationRx.h"            // used in "OQPSK_main"
#include "OQPSK_symbol_demapping.h"               // used in "OQPSK_main"

#include "SNRestimation.h"                        // used in "OQPSK_main"
#include "ErrorRateCalculation.h"                 // used in "OQPSK_main"

#include "OQPSK_main.h"                           // used in main
#include "OQPSK_main_2.h"                           // used in main
#include "OQPSK_pilot_avg_power.h"                // used in main

int main() 
{ 
    ///////////////////////   Data Paremeters   ////////////////////// 
    double fc = -338745;               // center frequency of subcarrier
    // Tx info.
    double fsy = 0.256 * pow(10, 6);   // symbol rate  
    // USRP info.
    double fs = 6.25 * pow(10, 6);     // sampling frequency

    ///////////////////////   Read File   //////////////////////////// 
    const char *filename;
    filename = "burst2_sr6.25_dtCplx_st8t"; //110 OQPSK
    //int filelength = 12.5 * pow(10, 6);   // Number of used samples: 1s data 
    //int filelength = 1.25 * pow(10, 6);     // Number of used samples: 0.1s data 
    //int filelength = 125 * pow(10,6);     // Number of used samples
    int filelength = pow(10, 6); 
    


    ///////////////////////   Types of pilots   //////////////////////////// 
    // There are 5 kinds of pilots in OQPSK. % Type 5 is the correct pilot of this data. 
    // int pilotPattern_1[27] = {2,0,3,0,3,2,1,0,2,2,0,0,0,2,3,1,3,2,1,3,2,2,1,1,2,2,2};
    // int pilotPattern_2[54] = {0,2,1,2,1,1,3,3,1,1,3,1,1,0,1,3,1,0,0,0,0,2,3,2,2,0,1,1,1,2,3,2,2,1,0,2,3,0,0,2,3,0,1,2,1,2,0,3};
    // int pilotPattern_3[27] = {2,3,1,0,1,3,1,1,0,3,0,1,1,3,2,0,3,0,0,3,3,1,2,1,1,3,0};
    // int pilotPattern_4[27] = {2,0,3,3,2,0,3,2,3,2,0,3,0,1,1,1,3,0,3,2,2,3,0,1,3,0,2};
    // int pilotPattern_5[27] = {2,1,3,0,3,1,3,3,0,1,0,3,3,1,2,0,1,0,0,1,1,3,2,3,3,1,0};
    // int Symbol_Length = sizeof(pilotPattern_5)/sizeof(*pilotPattern_5); //int Symbol_Length = 27;

    int pilotType =5;
    //vector<int> pilot_manual = {2,1,3,0,3,1,3,3,0,1,0,3,3,1,2,0,1,0,0,1,1,3,2,3,3,1,0};
    vector<int> pilot_manual;
    pilot_manual.push_back(1);
    
    ///////////////////////   Calculate OQPSK pilot average power   //////////////////////////// 
    // Input different types of pilots and calculate 
    // If the OQPSK pilot is mismatch with data burst: show message "This OQPSK pilot is not used for the data burst!" and return avg_power = 0.
    // If the OQPSK pilot is match with data burst: show message "This OQPSK pilot is used for the data burst!" and return avg_power.
    double avg_power = 0.0;
    avg_power = OQPSK_pilot_avg_power(pilotType, pilot_manual, fc, fsy, fs, filename, filelength);
    //avg_power = OQPSK_pilot_avg_power(fc, fsy, fs, filename, filelength);

    /////// avg_power result//////
    // pilotPattern_1: avg_power = 45.9492 // number_of_pilot = 1
    // pilotPattern_2: avg_power = nan     // number_of_pilot = 0
    // pilotPattern_3: avg_power = 55.7411 // number_of_pilot = 5
    // pilotPattern_4: avg_power = 62.6054 // number_of_pilot = 5 
    // pilotPattern_5: avg_power = 59.2187 // number_of_pilot = 22
    cout << avg_power<<"\n";
    
    ///////////////////////   OQPSK demod   //////////////////////////// 
    vector<double> ReceivedSym_d;
    // OQPSK_main(fc, fsy, fs, filename, filelength);
    //ReceivedSym_d = OQPSK_main(fc, fsy, fs, filename, filelength); //(OK)
    ReceivedSym_d = OQPSK_main_2(pilotType, pilot_manual, fc, fsy, fs, filename, filelength);
	// cout << ReceivedSym_d.size() <<"\n";
    // for( int i = 0 ; i < 50 ; i++ ){
    //     cout << ReceivedSym_d[i] <<"\n";
	// }


    /////////////////////// write OQPSK demod result into file  //////////////////////////// 
    ofstream OQPSK_demod_result;
    OQPSK_demod_result.open ("OQPSK_demod_result.txt");
    OQPSK_demod_result << "OQPSK_demod_result\n";
    int count = 0;
    for(int i = 0; i < ReceivedSym_d.size() ;i++){
        OQPSK_demod_result << ReceivedSym_d[i] << " ";
        count = count + 1;
        if(count == 50){
           OQPSK_demod_result << "\n"; 
           count = 0;
        }
    }
    OQPSK_demod_result.close();

    ReceivedSym_d.clear(); vector<double>(ReceivedSym_d).swap(ReceivedSym_d);
    return 0;
}
