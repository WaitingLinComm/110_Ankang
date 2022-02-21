function pilot = find_burst_interval( MF , sps)
%find_burst_interval
% 
% Output variable: 
% pilot(a two-row vector): 
% 1st row: head position of the burst(i.e. the pilot position)
% 2nd row: end position of the burst

% use the mean of following 50 symbol as thresold to detect the pilot head
runnig_sum_temp = sum( MF( 1 : 40 * sps) ) ;
% save possible point for pilot head
detect_temp = zeros( 1 , 40 * sps ) ;
% save pilot position
pilot = [] ;
% set a switch
switch_for_detect_head = 1 ; 
switch_for_detect_end = 1 ; 
index_for_detect_head = 1 ; 
index_for_detect_end = 1 ; 
scalar1 = 10;

%for testing_index = 40 : length(MF)
for testing_index = 1 : length(MF) % test all MF point
    if 40 * sps * MF(testing_index) > scalar1 * runnig_sum_temp && switch_for_detect_head == 1 
        detect_temp = MF( testing_index : min(length(MF) , testing_index + 40 * sps - 1) );
        if sum(detect_temp > scalar1 * runnig_sum_temp ) > 0.1 * 40 * sps && switch_for_detect_end == 1 
            index_for_detect_end = testing_index + 50 * sps  ; 
            switch_for_detect_end = 0 ; 
        elseif sum(detect_temp > scalar1 * runnig_sum_temp ) < 0.1 * 40 * sps && 40 * sps * MF(testing_index) > scalar1 * sum(MF(testing_index- 40 * sps + 1 : testing_index))
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
end_signal = length(MF) ;
sing_end_index=[pilot,end_signal];
pilot = [pilot;zeros(1,length(pilot))] ;

MF_temp=conv(MF,ones(1,40));

scalar2 = 0.8;
number_of_pilot = length(pilot);
%number_of_pilot = 1;
    for i = 1:number_of_pilot
        mean_sum_temp = mean( MF_temp(pilot(1,i)+10*sps : min( length(MF), pilot(1,i)+40*sps)));
        end_position= sum(MF_temp(sing_end_index(i):sing_end_index(i+1)) > scalar2 * mean_sum_temp )+pilot(1,i);
        pilot(2,i)=end_position; 
    end
end












