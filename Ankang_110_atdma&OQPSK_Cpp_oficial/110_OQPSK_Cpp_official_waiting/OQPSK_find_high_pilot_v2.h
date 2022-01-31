
//   find_high_pilot
  
//    return:
//    count_pilot: number of counted pilots 
 

//#include <vector>
//#include "sum_vec.h"
//#include "MAX_vec.h"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

void OQPSK_find_high_pilot_v2(vector<int> &pilot_start_position, vector<int> &pilot_end_position, vector<double> &MF, int sps)
{
    //use the mean of following 50 symbol as thresold to detect the pilot head
    int S = 40; 
    double runnig_sum_temp = sum_vec(MF, 0 ,S * sps);
    //save possible point for pilot head
    vector<double> detect_temp(S * sps);
    // MF length
    int L_MF = MF.size();
    //set a switch
    int switch_for_detect_head = 1 ; 
    int index_for_detect_head = 1 ; 
    int switch_for_detect_end = 1 ; 
    int index_for_detect_end = 1 ; 
    int scalar = 10;// MF value should be higher than scalar*running_mean to be candidate for pilot head
    int count_pilot = 0;
    for (int testing_index = 0 ; testing_index < L_MF - S * sps; testing_index++) //test all MF point
    {
        if( ((S * sps * MF[testing_index]) > (scalar * runnig_sum_temp)) && switch_for_detect_head == 1)
        {
           
            for (int i = 0; i < S * sps ; i++)
            {
                detect_temp[i] = MF[ testing_index + i ];
            }  
            
            double sum_detect_1 = 0;
            for (int i = 1; i <= S; i++)
            {
                sum_detect_1 = sum_detect_1 + MF[i - 1];
            }
            double sum_detect_2 = 0;
            for (int i = testing_index - 40 * sps + 1; i <= testing_index; i++)
            {
                sum_detect_2 = sum_detect_2 + MF[i - 1];
            }

            if(sum_detect_1 > 0.1 * S * sps && switch_for_detect_end == 1)
            {
                index_for_detect_end = testing_index + 50 * sps  ; 
                switch_for_detect_end = 0 ; 
            }
            else if (40 * sps * MF[testing_index] > scalar * sum_detect_2)
            {
                index_for_detect_head = testing_index + 50 * sps;
                double max_value;
                int max_index;
                MAX_vec(detect_temp, &max_value, &max_index);
                pilot_start_position.push_back( max_index + testing_index + 1);
                switch_for_detect_head = 0 ; 
            }
        }    
        if(testing_index > index_for_detect_head)
        {
            switch_for_detect_head = 1 ; 
        } 
        if(testing_index > index_for_detect_end)
        {
            switch_for_detect_end = 1 ; 
        } 
        runnig_sum_temp = runnig_sum_temp - MF[testing_index] + MF[min(L_MF, testing_index + 40 * sps - 1)];    
    }

int end_signal = L_MF;
//realize: sing_end_index=[pilot,end_signal];
vector<int>  sing_end_index;
sing_end_index.reserve(24);
sing_end_index.assign(pilot_start_position.begin(), pilot_start_position.end()); 
sing_end_index.push_back(end_signal);
/*
for (int i = 0; i < pilot_start_position.size(); i++){
    printf("[%d]: %f\n", i, pilot_start_position[i]);
} 
*/
//for(i = 0; i< end_signal-1; i++){
//pilot_end_position.push_back(pilot_start_position[i]);
//}

//pilot = [pilot;zeros(1,length(pilot))] ;

// realize: MF_temp = conv(MF,ones(1,40));
vector<double> MF_Q;
//MF_Q.assign(L_MF, 0);  // MF is real, so 將其虛部補 0
for(int i = 1; i <= L_MF; i++){
    MF_Q.push_back(0);
}

vector<double> ones_I, ones_Q;
int L_ones = 40;
ones_I.reserve(L_ones); ones_Q.reserve(L_ones);
for(int i = 1; i <= L_ones; i++){
    ones_I.push_back(1); ones_Q.push_back(0);
}

int L_MF_temp = L_MF + 40 -1;
vector<double> MF_temp_I, MF_temp_Q;
MF_temp_I.reserve(L_MF_temp); //MF_temp_Q.reserve(L_MF_temp);
MF_temp_Q.assign(L_MF_temp, 0);

complex_convolution(MF_temp_I, MF_temp_Q, L_MF_temp ,ones_I, ones_Q, L_ones, MF, MF_Q, L_MF);
/*
for (int i = 0; i < 30; i++){
    printf("[%d]: %f\n", i, MF_temp_I[i], MF_temp_Q[i]);
}  
cout << MF_temp_Q.size() << "\n";
*/



double scalar2 = 0.8;
int number_of_pilot = pilot_start_position.size();
number_of_pilot = 2;
for(int i = 1; i <= number_of_pilot; i++){
    // realize: mean_sum_temp = mean( MF_temp(pilot(1,i)+10*sps : min( length(MF), pilot(1,i)+40*sps)));
    double tmp1, tmp2;
    tmp1 = pilot_start_position[i-1] + 10 * sps;
    tmp2 = min( L_MF, pilot_start_position[i-1] + 40 * sps);

    double tmp_sum, mean_sum_temp;
    tmp_sum = 0;
    for(int i = tmp1; i <= tmp2; i++){
        tmp_sum = tmp_sum + MF_temp_I[i - 1];
    }
    mean_sum_temp = tmp_sum/(tmp2 - tmp1 + 1) ;
    // realize: end_position= sum(MF_temp(sing_end_index(i):sing_end_index(i+1)) > scalar2 * mean_sum_temp )+pilot(1,i);
    //pilot_end_position = 

cout << tmp1 << "\n";
cout << tmp2 << "\n";
cout << mean_sum_temp << "\n";
}
//cout << tmp2 << "\n";

/*
scalar2 = 0.8;
for i = 1:length(pilot)
    mean_sum_temp = mean( MF_temp(pilot(1,i)+10*sps : min( length(MF), pilot(1,i)+40*sps)));
    end_position= sum(MF_temp(sing_end_index(i):sing_end_index(i+1)) > scalar2 * mean_sum_temp )+pilot(1,i);
    pilot(2,i)=end_position; 
end
*/

}


/*
function pilot = find_burst_interval( MF , sps)
%find_high_pilot
% 
%     Output variable: 
%     pilot(a two-row vector): 
%     1st row: head position of the burst(also the pilot position)
%     2nd row: end position of the burst

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
for i = 1:length(pilot)
    mean_sum_temp = mean( MF_temp(pilot(1,i)+10*sps : min( length(MF), pilot(1,i)+40*sps)));
    end_position= sum(MF_temp(sing_end_index(i):sing_end_index(i+1)) > scalar2 * mean_sum_temp )+pilot(1,i);
    pilot(2,i)=end_position; 
end


end
*/