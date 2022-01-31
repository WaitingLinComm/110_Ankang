void get_databurst(vector<double> &RegetReceived_pilot_I, vector<double> &RegetReceived_pilot_Q, vector<double> &Regetdata_burst_I, vector<double> &Regetdata_burst_Q, vector<double> &burst_interval, vector<double> &Sig_I, vector<double> &Sig_Q, vector<double> &syncSig_I,vector<double> &syncSig_Q, int exact_pilot_position, double new_sps)
{
    double start_burst = burst_interval[0];
    double end_burst_original = burst_interval[1];
    int L_syncSig = syncSig_I.size();
    double analysis_symbols_original = end_burst_original - start_burst;
    double new_end_burst;
    double analysis_symbols_modified; //number of analysis_symbols should be an integer multiple of sps
    
    // NOTE: The % operator is for integers. fmod() can use for doubles
    if(fmod(analysis_symbols_original, new_sps) != 0){
        new_end_burst = end_burst_original -  fmod(analysis_symbols_original, new_sps);
    }
    else{
        new_end_burst = end_burst_original;
    }
    analysis_symbols_modified = new_end_burst - start_burst;

    if(analysis_symbols_modified < L_syncSig){
        analysis_symbols_modified = L_syncSig;
        new_end_burst = start_burst + L_syncSig;
    }         
    int L_regetSig = new_end_burst - start_burst;

    Regetdata_burst_I.reserve(L_regetSig); Regetdata_burst_Q.reserve(L_regetSig); 
    for (int i = start_burst; i <= new_end_burst - 1; i++){
        Regetdata_burst_I.push_back(Sig_I[i-1]);
        Regetdata_burst_Q.push_back(Sig_Q[i-1]);
    }
    RegetReceived_pilot_I.reserve(L_syncSig); RegetReceived_pilot_Q.reserve(L_syncSig);     
    for (int i = 1; i <= L_syncSig; i++){
        RegetReceived_pilot_I.push_back(Regetdata_burst_I[i-1]);
        RegetReceived_pilot_Q.push_back(Regetdata_burst_Q[i-1]);
    }
}