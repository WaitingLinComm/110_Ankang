/* By 煥霖
 * find_high_pilot
 * 
 *  return:
 *  count_pilot: number of counted pilots 
 */

//#include <vector>
//#include "sum_vec.h"
//#include "MAX_vec.h"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

void OQPSK_find_high_pilot(vector<int> &pilot_position , vector<double> &MF, int sps)
{
    //use the mean of following 50 symbol as thresold to detect the pilot head
    int S = 40; 
    double runnig_sum_temp = OQPSK_sum_vec(MF, 0 ,S * sps);
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
                OQPSK_MAX_vec(detect_temp, &max_value, &max_index);
                pilot_position.push_back( max_index + testing_index + 1);
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
}
