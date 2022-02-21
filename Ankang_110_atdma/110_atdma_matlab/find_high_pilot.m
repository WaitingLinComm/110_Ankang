function pilot = find_high_pilot( MF , IPOINT)
% use the mean of following 50 symbol as thresold to detect the pilot head
runnig_sum_temp = sum( MF( 1 : 40 * IPOINT) ) ;
% save possible point for pilot head
detect_temp = zeros( 1 , 40 * IPOINT ) ;
% save pilot position
pilot = [] ;
% set a switch
switch_for_detect_head = 1 ; 
switch_for_detect_end = 1 ; 
index_for_detect_head = 1 ; 
index_for_detect_end = 1 ; 
for testing_index = 40 * IPOINT : length(MF)% test all MF point
    if 40 * IPOINT * MF(testing_index) > 4 * runnig_sum_temp && switch_for_detect_head == 1 
        detect_temp = MF( testing_index : min(length(MF) , testing_index + 40 * IPOINT - 1) );
        if sum(detect_temp > 4 * runnig_sum_temp ) > 0.1 * 40 * IPOINT && switch_for_detect_end == 1 
            index_for_detect_end = testing_index + 50 * IPOINT  ; 
            switch_for_detect_end = 0 ; 
        elseif sum(detect_temp > 4 * runnig_sum_temp ) < 0.1 * 40 * IPOINT && 40 * IPOINT * MF(testing_index) > 4 * sum(MF(testing_index- 40 * IPOINT + 1 : testing_index))
            index_for_detect_head = testing_index + 50 * IPOINT  ;
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
    runnig_sum_temp = runnig_sum_temp - MF(testing_index) + MF(min( length(MF), testing_index + 40 * IPOINT - 1) ) ;
    

end
end_signal = length(MF) ;
sing_end_index=[pilot,end_signal];
pilot = [pilot;zeros(1,length(pilot))] ;

MF_temp=conv(MF,ones(1,40));

for i = 1:length(pilot(1,:))
    mean_sum_temp = mean( MF_temp( pilot(1,i)+10*IPOINT  :    pilot(1,i)+40*IPOINT )  );
    end_position= sum(MF_temp(sing_end_index(i):sing_end_index(i+1))>0.9*mean_sum_temp )+pilot(1,i);
    pilot(2,i)=end_position; 
end

end