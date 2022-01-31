////////////////////////////////////////////////////////////////// 
// SRRC
//
// Input variable: 
// span: FilterSpanInSymbols
// beta: RolloffFactor;
// sps: SamplerPerSymbol
// FilterLength: length of the pulse shaping filter
//////////////////////////////////////////////////////////////////
void SRRC(vector<double> &PulseshapingFilter, int span, float beta, int sps, int FilterLength)
{
    int filterOrder = sps * span;
    double delay = filterOrder/2;
    double *t = (double*)malloc(FilterLength * sizeof(double));
    double *p = (double*)malloc(FilterLength * sizeof(double));
    double FilterPower = 0;
   
    for(int i = 0; i < FilterLength; i++){   
        t[i] = (-delay + i)/sps; // time index of the filter 
        if(i == int(delay)){
            // SRRC Filter
            // The value of mid-point(n = 0). In matlab: p(delay + 1); In C: p[delay]. NOTE: the index of C is start from 0!
            p[int(delay)] = -1 / (pi * sps) * (pi * (beta - 1) - 4 * beta);
        }
        else{
            // SRRC Filter
            p[i] = -4 * beta/sps * ( cos((1 + beta) * pi * t[i]) + sin((1-beta) * pi * t[i])/(4 * beta * t[i]))/(pi * (pow(4 * beta * t[i], 2) - 1)); 
        }
        FilterPower = FilterPower + pow(p[i], 2); 
    }
    double FilterEnergy = sqrt(FilterPower);
    
    double max_p_SRRC = p[int(delay)] / FilterEnergy; //max_p_SRRC equals to the mid-point(n = 0) value of p
    for(int i = 0; i < FilterLength; i++){
        PulseshapingFilter.push_back(p[i] / FilterEnergy / max_p_SRRC);  // Normalize filter energy and impulse response
    }
}