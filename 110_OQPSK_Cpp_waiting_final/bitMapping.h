//////////////////////////////////////////////////////////////////
//  bitMapping
//  Change bit value (1 / 0) to voltage (-1 / 1) or bit value (1 /  0) to voltage (1 / -1) 
//////////////////////////////////////////////////////////////////
void bitMapping(int bitmap, vector<double> &InBit, vector<double> &OutBit, int Pattern_Length){      
    //bit value (1 / 0) to voltage (-1 / 1)
    if(bitmap == 1){
        for(int i = 0; i <  Pattern_Length; i++){
            OutBit.push_back(InBit[i] * (-2) + 1);
        }
    }
    //bit value (1 /  0) to voltage (1 / -1)
    else if(bitmap == 2){
        for(int i = 0; i <  Pattern_Length; i++){
            OutBit.push_back(InBit[i] * 2 - 1);
        }
    }
}