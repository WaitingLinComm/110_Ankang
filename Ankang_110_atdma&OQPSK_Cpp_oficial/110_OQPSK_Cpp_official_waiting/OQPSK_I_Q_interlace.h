/* 
 * I_Q_interlace
 *  
 *  if IQinterlace = 1: Let Q leads I
 *  if IQinterlace = 2: Let I leads Q
 */

void OQPSK_I_Q_interlace(int IQinterlace, vector<double> &InBit, vector<double> &OutBit_I, vector<double> &OutBit_Q, int Pattern_Length)
{  
    if(IQinterlace == 1) {
        for(int i = 0; i <  Pattern_Length; i++){
            if((i + 1) % 2 == 1){
                //OutBit_I[i] = InBit[i];
                //OutBit_Q[i] = 0;
                OutBit_I.push_back(InBit[i]);
                OutBit_Q.push_back(0);
            }
            if((i + 1) % 2 == 0)
            {
                //OutBit_I[i] = 0;
                //OutBit_Q[i] = InBit[i];
                OutBit_I.push_back(0);
                OutBit_Q.push_back(InBit[i]);
            }
        }
    }
    
    else if(IQinterlace == 2){
        for(int i = 0; i <  Pattern_Length; i++){
            if((i + 1) % 2 == 1){
                //OutBit_I[i] = 0;
                //OutBit_Q[i] = InBit[i];
                OutBit_I.push_back(0);
                OutBit_Q.push_back(InBit[i]);
            }
            if((i + 1) % 2 == 0){
                //OutBit_I[i] = InBit[i];
                //OutBit_Q[i] = 0;
                OutBit_I.push_back(InBit[i]);
                OutBit_Q.push_back(0);
            }
        }
    }   
}