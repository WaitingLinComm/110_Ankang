function [ pilot , adaptivef , inpilot2 ] = pilot_detection( signal , mod_type , IPOINT , burst_len_sample)
%% input parameter setting
% signal : an array of the undeteted signal
% data_type : "None" for detecting all kinds of pilot ; other possible input "OQPSK", "atdma_qpsk" , "atdma_8psk"
% IPOINT : interger , the oversampling point #
% burst_len_sample : int , 0 for test all 
%% output parameter setting
% pilot : array storing the pilot sequence
% adaptivef : indentifacation sequence of pilot symbol sequence after oversampling 
% inpilot2 : pilot symbol sequence after oversampling 


%% parameter set
pilot_temp = [] ; % temp pilot for testing the signal
inpilot = []; % temp pilot sequence for testing the signal

%% first select a range to detect threshold
if length(signal) >= 1e6
    signal = signal( 1 : 1e6 );
end


if mod_type == "None" % test all kinds of pilot
    mod_type ="oqpsk";
    for mode_index = 1 : 5 % 5 kinds of OQPSK pilot sequence
        [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type , mode_index , IPOINT );
        [ y , threshold ] = detect_success( signal , inpilot2 , IPOINT);
        if y
            mod_type , mode_index
            return
        end       
    
    end
    
    
    index_mode = [ 0 , 0 ] ;
    switch_ind = 0 ;
    mod_type = "atdma_qpsk";
    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, 9 , IPOINT );
    [ y_1 , threshold_1 ] = detect_success( signal , inpilot2, IPOINT );
    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, 10 , IPOINT );
    [ y_2 , threshold_2 ]  = detect_success( signal , inpilot2, IPOINT );
    if y_1 == 1 && y_2 == 1
        if  threshold_1 > threshold_2 % belong to type9 (3,27) ; 1 2 6 7
            if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
                
        elseif  threshold_1 < threshold_2 % belong to type10 (2 , 28)
             if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
        end
    elseif y_1 == 0 && y_2 == 1% belong to type10
              if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type ,pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
    elseif y_1 == 1 && y_2 == 0% belong to type9
            if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
        [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, index_mode(2)  , IPOINT );
        
    end
    
    
    
    
    
    

    mod_type ="atdma_8psk";
    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, 9 , IPOINT );
    [ y_1 , threshold_1 ] = detect_success( signal , inpilot2, IPOINT );
    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, 10 , IPOINT );
    [ y_2 , threshold_2 ]  = detect_success( signal , inpilot2, IPOINT );
    if y_1 == 1 && y_2 == 1
        if  threshold_1 > threshold_2 
            if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                        switch_ind = 1 ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx =  [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                            switch_ind = 1 ;
                        end
                    end 
                end
            end
                
        elseif  threshold_1 < threshold_2 % belong to type10 (2 , 28)
             if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                        switch_ind = 1 ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                            switch_ind = 1 ;
                        end
                    end
                end
                
                
            end     
        end
    elseif y_1 == 0 && y_2 == 1% belong to type10
              if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type ,pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                        switch_ind = 1 ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                            switch_ind = 1 ;
                        end
                    end
                end
                
                
            end     
    elseif y_1 == 1 && y_2 == 0% belong to type9
            if burst_len_sample == 0
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                        switch_ind = 1 ;
                    end               
                end          
            else
                %index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                            switch_ind = 1 ;
                        end
                    end 
                end
            end
        [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, index_mode(2)  , IPOINT );
        
        
    end
    

    
elseif mod_type == "oqpsk"
    
    for mode_index = 1 : 5 % 5 kinds of OQPSK pilot sequence
        [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type , mode_index , IPOINT );
        if  detect_success( signal , inpilot2 , IPOINT)
            mod_type , mode_index
            return
        end       
    
    end
    
elseif mod_type == "atdma_qpsk"
        [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, 9 , IPOINT );
    [ y_1 , threshold_1 ] = detect_success( signal , inpilot2, IPOINT );
    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, 10 , IPOINT );
    [ y_2 , threshold_2 ]  = detect_success( signal , inpilot2, IPOINT );
    if y_1 == 1 && y_2 == 1
        if  threshold_1 > threshold_2 % belong to type9 (3,27) ; 1 2 6 7
            if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
                
        elseif  threshold_1 < threshold_2 % belong to type10 (2 , 28)
             if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
        end
    elseif y_1 == 0 && y_2 == 1% belong to type10
              if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type ,pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [3 , 4 , 5 , 8 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
    elseif y_1 == 1 && y_2 == 0% belong to type9
            if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 , 2 , 6 , 7 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
        
        
    end
    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, index_mode(2)  , IPOINT );
    
    
    
    
    

    
elseif mod_type == "atdma_8psk"  
    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, 9 , IPOINT );
    [ y_1 , threshold_1 ] = detect_success( signal , inpilot2, IPOINT );
    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, 10 , IPOINT );
    [ y_2 , threshold_2 ]  = detect_success( signal , inpilot2, IPOINT );
    if y_1 == 1 && y_2 == 1
        if  threshold_1 > threshold_2 
            if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx =  [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
                
        elseif  threshold_1 < threshold_2 % belong to type10 (2 , 28)
             if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6 ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
        end
    elseif y_1 == 0 && y_2 == 1% belong to type10
              if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type ,pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [1 3 4 5 6  ]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end
                end
                
                
            end     
    elseif y_1 == 1 && y_2 == 0% belong to type9
            if burst_len_sample == 0
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type, pilot_idx  , IPOINT );
                    [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                    if threshold > index_mode(1)
                        index_mode(1) = threshold ;
                        index_mode(2) = pilot_idx ;
                    end               
                end          
            else
                index_mode = [ 0 , 0 ] ;% [ pilot_SNR , index ] 
                for pilot_idx = [2,7,8]
                    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, pilot_idx  , IPOINT );
                    if length(inpilot2)< 1.1*  burst_len_sample% detect only when burst len larger than spec len
                        [ y , threshold ] = detect_success( signal , inpilot2, IPOINT );    
                        if threshold > index_mode(1)
                            index_mode(1) = threshold ;
                            index_mode(2) = pilot_idx ;
                        end
                    end 
                end
            end
        
        
    end
    [ pilot , adaptivef , inpilot2 ] = pilot_map(mod_type, index_mode(2)  , IPOINT );


    
end

end