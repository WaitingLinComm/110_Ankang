function [signal_segment , signal_pilot_pos , signal_processed  ] = find_burst_2021( MF1 , MF2 ) % , data_type)
%% Input part
% MF1 is the array convolving the signal and pilot sequence
% MF2 is used to located the signal position
% If needed, data_type is the array that stores all possible data type 
% in data_type , 0 for no info, 1 for "OQPSK" , 2 for "aTDMA_QPSK" , 3 for "aTDMA_8PSK" 
%% output part
% signal_processed  array ( 2 , # of burst ) is a cell
% signal_processed ( : , i ) = [ ith start_pos_burst ; ith end_pos_burst ]
% "data_type" haven't finished
% start_pos_burst is the first position of the burst
% end_pos_burst is the final position of the burst 
% (removed) "data_type" is a int , storing data type like , 1 for "OQPSK" , 2 for "aTDMA_QPSK" , 3 for "aTDMA_8PSK" 

% signal_pilot_pos is array storing starting point index of each burst

% signal_segment is the array with 0 for noise and 1 for signal
% Its index corresponds to MF1 or MF2

%% initialization
data_len = length(MF1) ; % length of data
signal_pilot_pos = [];
signal_processed = [];
%% use MF2 to find the signal part 
clear desire ;
desire = [] ;
% first select a range to find threshold
if data_len >= 1e6
    desire = MF2( 1 : 1e6 );
else
    desire = MF2 ;
end    

%method 1 for threshold : use the intutive way
% % find threshold : select the min 0.05%-0.1% values, threshold  = mean of these values
% % min_k , range_wanted should be well set to find signal
% min_k = 0.005 ; % find min_k value out of MF2 
% range_wanted = 0.003 ; %  find range_wanted value out of min_k value out of MF2  
% min_1percent = mink( desire , ceil( min_k * length(desire) ) ) ;
% threshold_before_process = min_1percent( ceil( range_wanted * length(desire) ) : ceil( min_k * length(desire) )) ;
% threshold  = mean(threshold_before_process) * 2.5  % signal is decided as noise if its energy is lower than 2.5*min 0.5%-1% value

%method 2 for threshold: use kmeans

[ index , cluster_mean ] = kmeans( desire , 5 ) ; % 5 for seperating the noise part and signal part with totally 5 groups
threshold = min(cluster_mean ) * 1.9 ; 

%% segment the signal part
signal_part_identify = MF2 > threshold ;
signal_segment = signal_part_identify ;

% set some noise part to 0
for i = 1 : length(signal_segment) % identify it as signal when it has more than 500 conti 1s
    signal_segment(i) = prod(  signal_segment( max( 1 , i - 500 ) : i )  ) | prod(  signal_segment( i : min( data_len , i + 500 ) )  ) ;   
end

% initialize the start position of each part of the signal
starting_signal = [];
ending_signal = [];
% find starting position and ending position of each conti 1s
for i = 1 : length(signal_segment) % signal condition :
    if signal_segment( max( i - 1 , 1 ) ) == 0  ...% previous index is 0
            & signal_segment(i) == 1 ...% this index is 1( if this is a signal , index is 1 )
            & prod( signal_segment(i : min( data_len , i + 50 ) ) ) == 1 % last 50 1s
        starting_signal = [ starting_signal , i ] ;
    end
    
    if signal_segment( min( i + 1 , data_len ) ) == 0  ...
            & signal_segment(i) == 1 ...
            & prod( signal_segment( max( 0 , i - 50 ) : i ) ) == 1
        ending_signal = [ ending_signal , i ] ;
    end
end
% find burst    
clear temp_signal;clear temp_mean;
if length(starting_signal) == length(ending_signal)
    
    for i = 1 : length(starting_signal)
        temp_signal = MF1( starting_signal(i) : ending_signal(i) ) ;
        temp_mean = mean(temp_signal) ;% avearge abs magnitude of signal 
        burst_cadi = find( temp_signal > 4 * temp_mean) ;% determine if this position is a pilot head and store the position      
        % delete the repeated neighboring part of burst
        burst_cadi_tmp1 = [];
        burst_cadi_tmp2 = [];
        for j = 1 : length(burst_cadi)
            if j < length(burst_cadi)
                if burst_cadi( j+1 ) - burst_cadi(j) <= 2000
                    burst_cadi_tmp1 = [burst_cadi_tmp1 , burst_cadi(j) ];
                elseif burst_cadi( j+1 ) - burst_cadi(j) >= 2000
                    [ max_value , max_index ] = max ( temp_signal( burst_cadi_tmp1 ) ) ;
                    burst_cadi_tmp2  = [burst_cadi_tmp2  , burst_cadi(  max_index ) ] ;
                    burst_cadi_tmp1 = [];
                end
            elseif j == length(burst_cadi)
                if burst_cadi( j ) - burst_cadi( j-1 ) <= 2000
                    burst_cadi_tmp1 = [burst_cadi_tmp1 , burst_cadi(j) ];
                    [ max_value , max_index ] = max ( temp_signal( burst_cadi_tmp1 ) ) ;
                    burst_cadi_tmp2  = [burst_cadi_tmp2  , burst_cadi(  max_index ) ] ;
                    burst_cadi_tmp1 = [];
                elseif burst_cadi( j ) - burst_cadi( j-1 ) >= 2000
                    burst_cadi_tmp2  = [burst_cadi_tmp2  ,  burst_cadi( j )] ;                
                end
            end
            
            
            
            
        end
        burst_cadi  = burst_cadi_tmp2 ;
        size_burst_cadi = size(burst_cadi);
        
        if size_burst_cadi(2) ~= 1
            burst_cadi = transpose(burst_cadi) ;
        end
        signal_pilot_pos = [ signal_pilot_pos ; burst_cadi + starting_signal(i) - 1 ];
        
        if length(burst_cadi) == 1
            signal_processed = [ signal_processed ,  [ burst_cadi + starting_signal(i) - 1 ; ending_signal(i) ] ];
        
        elseif length(burst_cadi) > 1
            
            for j = 1 : length(burst_cadi)
                
                if j == length(burst_cadi)
                    signal_processed = [ signal_processed , [ burst_cadi(j) + starting_signal(i) - 1 ;  ending_signal(i)  ] ];
                else
                    signal_processed = [ signal_processed ,  [ burst_cadi(j) + starting_signal(i) - 1 ; burst_cadi(j+1) + starting_signal(i) - 1 - 1 ] ];               
                end
                
            end
            
        end
        
        
        
    end
end

end