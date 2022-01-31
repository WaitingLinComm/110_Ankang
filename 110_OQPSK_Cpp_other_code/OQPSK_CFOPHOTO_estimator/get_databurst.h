//#include <stdio.h>
//#include <stdlib.h>

get_databurst(start_burst, end_burst, Sig_I, Sig_Q, syncSig_I, syncSig_Q, exact_pilot_position, new_sps);

void get_databurst(vector<double> &InData_I, vector<double> &InData_Q, vector<double> &OutData_I, vector<double> &OutData_Q, double CFO, int DataLength)
{
    double coscoff, sincoff;
    for (int i = 0; i < DataLength; i++){
    	coscoff = cos((i + 1) * CFO); 
    	sincoff = sin((i + 1) * CFO);
        OutData_I.push_back(InData_I[i] * coscoff - InData_Q[i] * sincoff);
        OutData_Q.push_back(InData_I[i] * sincoff + InData_Q[i] * coscoff);
    }    
}
/*
function [analysis_symbols_modified, Received_pilot, data_burst] = get_databurst_v2(start_burst, end_burst_original, Sig, syncSig, pilot_initial_possition, sps)
       
analysis_symbols_original = end_burst_original - start_burst;

% number of analysis_symbols should be an integer multiple of sps
if(mod(analysis_symbols_original, sps) ~= 0)
    new_end_burst = end_burst_original - mod(analysis_symbols_original, sps);
else
    new_end_burst = end_burst_original;
end
analysis_symbols_modified = new_end_burst - start_burst;

if analysis_symbols_modified < length(syncSig)
    analysis_symbols_modified = length(syncSig);
    new_end_burst = start_burst + length(syncSig);
end

data_burst = Sig(start_burst : new_end_burst - 1); 
data_burst_time_index = (start_burst : new_end_burst - 1);

Received_pilot = data_burst(1:length(syncSig));
Received_pilot_time_index = data_burst_time_index(1:length(syncSig));
*/