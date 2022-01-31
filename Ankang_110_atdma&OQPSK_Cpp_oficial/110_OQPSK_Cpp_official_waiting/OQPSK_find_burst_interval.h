//////////////////////////////////////////////////////////////////
// find_burst_interval
//  
// return:
// count_pilot: number of counted pilots 
// 
//////////////////////////////////////////////////////////////////
//#include <vector>
//#include "sum_vec.h"
//#include "MAX_vec.h"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

void OQPSK_find_burst_interval(vector<int> &pilot_position ,vector<int> &end_burst_position , vector<double> &MF, int sps)
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
        if( ((S * sps * MF[testing_index]) > (scalar * runnig_sum_temp)) && switch_for_detect_head == 1){
            for (int i = 0; i < S * sps ; i++){
                detect_temp[i] = MF[ testing_index + i ];
            }  
            
            double sum_detect_1 = 0;
            for (int i = 1; i <= S; i++){
                sum_detect_1 = sum_detect_1 + MF[i - 1];
            }
            double sum_detect_2 = 0;
            for (int i = testing_index - 40 * sps + 1; i <= testing_index; i++){
                sum_detect_2 = sum_detect_2 + MF[i - 1];
            }

            if(sum_detect_1 > 0.1 * S * sps && switch_for_detect_end == 1){
                index_for_detect_end = testing_index + 50 * sps  ; 
                switch_for_detect_end = 0 ; 
            }
            else if (40 * sps * MF[testing_index] > scalar * sum_detect_2){
                index_for_detect_head = testing_index + 50 * sps;
                double max_value;
                int max_index;
                OQPSK_MAX_vec(detect_temp, &max_value, &max_index);
                pilot_position.push_back( max_index + testing_index + 1);
                switch_for_detect_head = 0 ; 
            }
        }    
        if(testing_index > index_for_detect_head){
            switch_for_detect_head = 1 ; 
        } 
        if(testing_index > index_for_detect_end){
            switch_for_detect_end = 1 ; 
        } 
        runnig_sum_temp = runnig_sum_temp - MF[testing_index] + MF[min(L_MF, testing_index + 40 * sps - 1)];    
    }

    int end_signal = MF.size();
    vector<vector<int> > pilot(2, vector<int>(pilot_position.size()));       // 定義二維動態陣列 pilot[2][Lv]

    for(int j = 0; j < pilot_position.size(); j++){
        pilot[0][j] = pilot_position[j];
    }
    // printf 2D array  
    // for(int i=0; i< 2; i++) 
    // {
    //     for(int j = 0; j < pilot_position.size(); j++){    
    //         printf("[%d,%d]: %d ", i, j, pilot[i][j]);
    //     }
    //     cout<<"\n";
    // }

    int i, j, i1;
	double tmp;
    vector<double> ones; ones.reserve(40);
    for (i = 0; i < 40; i++){
        ones.push_back(1);
    }
    vector<double> MF_temp;
    int L_MFtemp = MF.size() + ones.size() - 1;
    MF_temp.reserve(L_MFtemp);

    vector<double> MF_Q(MF.size());
    vector<double> ones_Q(40);
    vector<double> MF_temp_Q(L_MFtemp);
    OQPSK_complex_convolution(MF_temp, MF_temp_Q, L_MFtemp, MF, MF_Q, MF.size(), ones, ones_Q, 40);

    // for (i = 0+1860; i < 4+1860; i++){
    //     cout << MF_temp[i] <<"\n";
    // }

    vector<int> sing_end_index; // 將訊號的最尾端index加入pilot_position，以利下方迴圈中的計算
    int L_sing_end_index = pilot_position.size() + 1;
    sing_end_index.reserve(L_sing_end_index);
    for (i = 0; i < pilot_position.size(); i++){
        sing_end_index.push_back(pilot_position[i]);
    }
    sing_end_index.push_back(MF.size());
    
    //double scalar2 = 2.8; // 1.8 // 2.8(最接近)
    double scalar2 = 0.02;
    int start_sum; int end_sum; int L_sum;
    double sum_temp; double mean_sum_temp;
//    double end_position;
    double sum_MFtemp = 0;

    for (i = 0; i < pilot_position.size(); i++){
        start_sum = pilot[0][i] + 10 * sps;
        end_sum = min(MF.size() , pilot[0][i] + 40 * sps); 
        L_sum = end_sum - start_sum + 1;
        for (int j = start_sum - 1; j < end_sum; j++){
            sum_temp = sum_temp + MF_temp[j];
        }    
        mean_sum_temp = sum_temp/L_sum;

        sum_MFtemp = 0;
        for (int j = sing_end_index[i] - 1; j < sing_end_index[i+1]; j++){
            if (MF_temp[j] > scalar2 * mean_sum_temp){
                sum_MFtemp = sum_MFtemp + 1;
            }
        } 
        pilot[1][i] = sum_MFtemp + pilot[0][i];
        end_burst_position.push_back(pilot[1][i]);
    }
    //printf 2D array  
    // for(int i=0; i< 2; i++)
    // {
    //     for(int j = 0; j < pilot_position.size(); j++){    
    //         printf("[%d,%d]: %d ", i, j, pilot[i][j]);
    //     }
    //     cout<<"\n";
    // }    
}
