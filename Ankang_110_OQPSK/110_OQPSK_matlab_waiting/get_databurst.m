function [Received_pilot, data_burst] = get_databurst(analysis_symbols, Sig, syncSig, pilot_initial_possition, sps)

data_burst = Sig(pilot_initial_possition:pilot_initial_possition + analysis_symbols * sps - 1);      
%data_burst = Sig(pilot_initial_possition:min(pilot_initial_possition + analysis_symbols * sps - 1, length(Sig)));          
data_burst_time_index = (pilot_initial_possition:pilot_initial_possition + analysis_symbols * sps - 1);            

Received_pilot = data_burst(1:length(syncSig));
Received_pilot_time_index = data_burst_time_index(1:length(syncSig));
end