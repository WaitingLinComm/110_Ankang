function [pilot] = find_high_pilot( MF , sps)
%find_high_pilot
% 
% Output variable: 
% pilot(a one-row vector): head position of the burst(i.e. the pilot position)

% use the mean of following 50 symbol as thresold to detect the pilot head
runnig_sum_temp = sum( MF( 1 : 40 * sps) ) ;
% save possible point for pilot head
detect_temp = zeros( 1 , 40 * sps ) ;
% save pilot position
pilot = [] ;
endburst_position = [];
% set a switch
switch_for_detect_head = 1 ; 
switch_for_detect_end = 1 ; 
index_for_detect_head = 1 ; 
index_for_detect_end = 1 ; 
scalar = 10;

for testing_index = 1 : length(MF)% test all MF point
    if 40 * sps * MF(testing_index) > scalar * runnig_sum_temp && switch_for_detect_head == 1 
        detect_temp = MF( testing_index : min(length(MF) , testing_index + 40 * sps - 1) );
        if sum(detect_temp > scalar * runnig_sum_temp ) > 0.1 * 40 * sps && switch_for_detect_end == 1 
            index_for_detect_end = testing_index + 50 * sps  ; 
            switch_for_detect_end = 0 ; 
        elseif sum(detect_temp > scalar * runnig_sum_temp ) < 0.1 * 40 * sps &&  40 * sps * MF(testing_index) > scalar * sum(MF(testing_index- 40 * sps + 1 : testing_index))
        % or elseif 40 * sps * MF(testing_index) > scalar * sum(MF(testing_index- 40 * sps + 1 : testing_index))
            index_for_detect_head = testing_index + 50 * sps  ;
            [max_value , max_index] = max(detect_temp) ;
            pilot = [pilot , max_index+testing_index-1] ;
            switch_for_detect_head = 0 ; 
        end
    end

    if testing_index > index_for_detect_head 
        switch_for_detect_head = 1 ; 
    end
    
    if testing_index > index_for_detect_end 
        switch_for_detect_end = 1 ; 
    end
    runnig_sum_temp = runnig_sum_temp - MF(testing_index) + MF(min( length(MF), testing_index + 40 * sps - 1) ) ;

end
end