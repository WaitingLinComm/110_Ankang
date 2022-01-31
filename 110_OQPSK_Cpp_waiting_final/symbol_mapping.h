////////////////////////////////////////////////////////////////// 
//   symbol_mapping
//  
//   Input variable:
//   pilotPattern: pilotPattern = '213031330103312010011323310'; 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)      
//   pilotBit: the memory pre-allocate for symbol mapping
//   Symbol_Length: 27 * 2 = 54
////////////////////////////////////////////////////////////////// 
void symbol_mapping(vector<double> &pilotBit, vector<double> &pilotSym_I, vector<double> &pilotSym_Q, int *pilotPattern, int Symbol_Length)
{
    for(int i = 0; i <  Symbol_Length; i++){
        if(pilotPattern[i] == 0){
            //pilotBit[2 * i] = 0;
            //pilotBit[2 * i + 1] = 0;
            pilotBit.push_back(0);
            pilotBit.push_back(0);
            pilotSym_I.push_back(1);
            pilotSym_Q.push_back(1);
        }
        else if(pilotPattern[i] == 1){
            //pilotBit[2 * i] = 0;
            //pilotBit[2 * i + 1] = 1;
            pilotBit.push_back(0);
            pilotBit.push_back(1);
            pilotSym_I.push_back(-1);
            pilotSym_Q.push_back(1);
        }
        else if(pilotPattern[i] == 2){
            //pilotBit[2 * i] = 1;
            //pilotBit[2 * i + 1] = 1;
            pilotBit.push_back(1);
            pilotBit.push_back(1);
            pilotSym_I.push_back(-1);
            pilotSym_Q.push_back(-1);
        }
        else if(pilotPattern[i] == 3){
            //pilotBit[2 * i] = 1;
            //pilotBit[2 * i + 1] = 0;
            pilotBit.push_back(1);
            pilotBit.push_back(0);
            pilotSym_I.push_back(1);
            pilotSym_Q.push_back(-1);
        }
    }
}