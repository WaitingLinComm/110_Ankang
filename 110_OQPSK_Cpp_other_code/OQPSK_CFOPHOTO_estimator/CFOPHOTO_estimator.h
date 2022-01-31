// 2021/10/19 
// 過 tmp_cfo_compen_vec()後，存在些數值誤差
// test_printf_2_vec(p_flip_I, conj_p_flip_Q, 20); // 存在些數值誤差, 但數值走勢還算對
// complex_convolution(y2_I, y2_Q, L_out ,p_flip_I, conj_p_flip_Q, Lp, v_I, v_Q, Lv);     
// 數值有誤

//#include <iostream>
//using namespace std;
//#include <vector>
//#include "tmp_cfo_compen.h"

void CFOPHOTO_estimator(vector<double> &v_I, vector<double> &v_Q, int Lv, vector<double> &q_I, vector<double> &q_Q, int Lq, int K, double Ts)
{   
    //Parameter setting
    int cfo_estimate = 0;
    int cfo_step = 1000;
    int nSteps = 10;
    int iterations = 0;
    
   /////////// start iteration //////////////
    iterations = iterations + 1;
    int L_vCFO = 2 * nSteps + 1; // length of vCFO vector
    vector<double> vCFO;
    vCFO.reserve(L_vCFO); 
    for (int i = 0; i < L_vCFO; i++){
        vCFO.push_back(cfo_estimate + (-nSteps + i) * cfo_step); // Hz 
    }
    //test_printf_1_vec(vCFO, 10); (OK)

    vector<double> y2_I, y2_Q;      // for 迴圈中再reserve y2 記憶體大小: y2_I.reserve(L_v), y2_Q.reserve(L_v); 
    vector<double> y2_norm2_buffer;
    y2_norm2_buffer.reserve(Lv);
    
    vector<vector<double> > y2_norm2(L_vCFO, vector<double>(Lv));  // 定義二維動態陣列 y2_norm2[L_vCFO][Lv]
    vector<vector<double> > pho(L_vCFO, vector<double>(Lv));       // 定義二維動態陣列 pho[L_vCFO][Lv]
    vector<double> CFO;
    CFO.reserve(L_vCFO); 

    for (int i = 1; i <= 2; i++) //for (int i = 1; i <= L_vCFO; i++)
    {
        vector<double> p_I, p_Q;
        int Lp = Lq;
        p_I.reserve(Lp); p_Q.reserve(Lp); 
        CFO.push_back(2 * pi * vCFO[i-1] * Ts); //CFO:[-pi,pi] (rad);
        tmp_cfo_compen(q_I, q_Q, p_I, p_Q, CFO[i-1], Lq);
        
        //test_printf_1_vec(CFO, 20); (OK)
        //test_printf_2_vec(p_I, p_Q, 20); // (same with matlab)

        ///// realize y2 = filter(conj(p(end : -1 : 1)), 1, v); % or y2 = fftconv((v),conj(p(end:-1:1))); ////
        // (OK)realize : q(end : -1 : 1)
        vector<double>  p_flip_I, p_flip_Q;
        p_flip_I.reserve(Lp); p_flip_Q.reserve(Lp); 
        Flip_vec(p_flip_I, p_I, Lp);
        Flip_vec(p_flip_Q, p_Q, Lp);
        //cout << q_flip_I.size() << "\n";
        //test_printf_2_vec(p_flip_I, p_flip_Q, 20); //(OK)

        // (OK)realize : conj(q(end : -1 : 1)
        vector<double>  conj_p_flip_Q;
        conj_p_flip_Q.reserve(Lp); 
        for(int i = 0; i < Lp; i++){ 
            conj_p_flip_Q.push_back(- p_flip_Q[i]); 
        }
        //test_printf_2_vec(p_flip_I, conj_p_flip_Q, 20); // (same with matlab)
        
        // NOTE!
        // If we would like to perform z = filter(x, 1, y), the output length Lz = Ly.
        // If we would like to perform z = conv(x, y) the output length Lz = Lx + Ly - 1.
        //
        ////////////////       realize : y1 = filter(conj(q(end : -1 : 1)), 1, r)     /////////////////////
        //int L_out = L_r + L_q - 1;   // realize y1 = conv(conj(q(end : -1 : 1)), 1, r)
        int L_out = Lv;                // realize y1 = filter(conj(q(end : -1 : 1)), 1, r)
        y2_I.reserve(L_out); y2_Q.reserve(L_out);    
        complex_convolution(y2_I, y2_Q, L_out ,p_flip_I, conj_p_flip_Q, Lp, v_I, v_Q, Lv);
        //test_printf_2_vec(y2_I, y2_Q, 20);  // (same with matlab)
                
        // problem: 當使用第二次 complex_convolution() 時，值會跑掉
        
        // for test (same with matlab)
        /*
        for (int j = 0; j < Lv; j++){
            y2_norm2_buffer.push_back(pow(y2_I[j],2) + pow(y2_Q[j],2));
        }
        //test_printf_1_vec(y2_norm2_buffer, 20);  // (same with matlab)
        */
        for(int j = 0; j < Lv; j++){ 
            y2_norm2[i - 1][j] = pow(y2_I[j],2) + pow(y2_Q[j],2);
        }
        for(int j = 0; j < Lv; j++){ 
            pho[i - 1][j] = atan2(y2_Q[j], y2_I[j]); // atan(Y,X)
        }

        p_flip_I.clear(); p_flip_Q.clear(); conj_p_flip_Q.clear();        
        y2_I.clear(); y2_Q.clear();
    }
    
    // printf 2D array
    
    for(int i=0; i< 2; i++)
    //for(int i=0; i< y2_norm2.size(); i++)//输出二维动态数组 
    {
        for(int j = 0; j < Lv; j++)
        {
            printf("[%d,%d]: %f ", i, j, y2_norm2[i][j]);
            //cout<<y2_norm2[i][j]<<" ";
        }
        cout<<"\n";
    }
    

    /*
    for i =1:length(vCFO)
        CFO(i) = 2 * pi * vCFO(i) * Ts; % CFO:[-pi,pi] (rad)
        p = q .* exp(1i * CFO(i) * (1 : length(q)));
        y2 = filter(conj(p(end : -1 : 1)), 1, v); % or y2 = fftconv((v),conj(p(end:-1:1))); 
        y2_norm2(i,:) = abs(y2).^2;
        pho(i,:) = angle(y2);
    end
*/

}