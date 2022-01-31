//////////////////////////////////////////////////////////////////
// ErrorRateCalculation
//
//    Use pilot to calculate SER/BER 
//    [Input variable]: mode_type, pilotSym(a vector), ReceivedPilotSym(a vector)
//      1. mode_type : "QPSK", "8PSK", "OQPSK"
//      2. pilotSym: the pilot symbol which we know in advance
//              ex: for OQPSK: vector<double> pilotSym = [2, 1, 3, 0, 3, 1, 3, 3, 0, 1, 0, 
//                                          3, 3, 1, 2, 0, 1, 0, 0, 1, 1, 3, 2, 3, 3, 1, 0]
//      3. ReceivedPilotSym: total received pilot symbols form each burst
//         NOTE: ReceivedPilotSym is the result after symbol demapping.
//
//    [Output variable]: SER(a scalar), BER(a scalar)
//////////////////////////////////////////////////////////////////
void ErrorRateCalculation(vector<double> &SER_BER_errorpilot, string &mode_type, vector<double> &pilotSym, vector<double> &ReceivedPilotSym)
{  
    int M;  
    double k;
    if(mode_type == "QPSK" || mode_type == "OQPSK"){
        M = 4; 
        k = log2(M);
    }
    else if(mode_type == "8PSK"){
        M = 8; 
        k = log2(M);    
    }
    
    // Calculate BER/SER of the received pilots
    int error_pilot = 0;
    int L_pilotSym = pilotSym.size();
    int bursts = ReceivedPilotSym.size()/L_pilotSym;

    for (int i = 1; i < bursts; i++){
        for (int j = 1; j < L_pilotSym; j++){
            if(ReceivedPilotSym[(i-1) * L_pilotSym + j - 1] != pilotSym[j - 1]){
                error_pilot = error_pilot + 1;
            }    
        }         
    }
    double SER = double(error_pilot)/ReceivedPilotSym.size();
    double BER = k * SER;
    SER_BER_errorpilot.push_back(SER);
    SER_BER_errorpilot.push_back(BER);
    SER_BER_errorpilot.push_back(error_pilot);   
}