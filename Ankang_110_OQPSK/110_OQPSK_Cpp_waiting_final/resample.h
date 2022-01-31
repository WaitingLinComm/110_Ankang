//////////////////////////////////////////////////////////////////
// resample
//
// Input variable:
// InSig_I, InSig_Q: signal before resample
// ResampledSig_I, ResampledSig_Q: signal after resample
//
// SigLength_beforeResample
// SigLength_afterResample
//
// N = new SamplePerSymbol / old SamplePerSymbol
// upsample by N. NOTE: N can be a non-integer number
//
// window: half-sided window length(used in SincInterpolation) 
//////////////////////////////////////////////////////////////////
void resample(vector<double> &ResampledSig_I, vector<double> &ResampledSig_Q, int SigLength_afterResample, vector<double> &InSig_I, vector<double> &InSig_Q, int SigLength_beforeResample, double N, double window)
{   
    double inner_sum_I = 0; double inner_sum_Q = 0;    
    for(int n2 = 1; n2 <= SigLength_afterResample; n2++){              
        inner_sum_I = 0; inner_sum_Q = 0;        
        int n1_match = round(n2 / N);  

        if(n1_match > window & n1_match < SigLength_beforeResample - window){
            for(int n1 = n1_match - window; n1 <= n1_match + window; n1++){
                inner_sum_I += InSig_I[n1 - 1] * sinc(n2/N - n1); // #include "sinc.h"
                inner_sum_Q += InSig_Q[n1 - 1] * sinc(n2/N - n1);   
            }   
            ResampledSig_I.push_back(inner_sum_I);
            ResampledSig_Q.push_back(inner_sum_Q);            
        }                     
        else if(n1_match <= window){
            for(int n1 = 1 ; n1 <= n1_match + window; n1++){
                inner_sum_I += InSig_I[n1 - 1] * sinc(n2/N - n1); // #include "sinc.h"
                inner_sum_Q += InSig_Q[n1 - 1] * sinc(n2/N - n1);
            }   
            ResampledSig_I.push_back(inner_sum_I);
            ResampledSig_Q.push_back(inner_sum_Q);
        }
        
        else if(n1_match >= window){    
            for(int n1 = n1_match - window ; n1 <= SigLength_beforeResample; n1++){ 
                inner_sum_I += InSig_I[n1 - 1] * sinc(n2/N - n1); // #include "sinc.h"
                inner_sum_Q += InSig_Q[n1 - 1] * sinc(n2/N - n1);            
            }   
            ResampledSig_I.push_back(inner_sum_I); ResampledSig_Q.push_back(inner_sum_Q);
        }
    }
}