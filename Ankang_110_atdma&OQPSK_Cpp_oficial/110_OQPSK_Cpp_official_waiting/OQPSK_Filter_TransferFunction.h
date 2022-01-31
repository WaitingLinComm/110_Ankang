// Filter_TransferFunction
// The code realize filter(b, a, X) in matlab

void OQPSK_Filter_TransferFunction(vector<double> &b, vector<double> &a, vector<double> &X_I, vector<double> &X_Q, vector<double> &Y_I, vector<double> &Y_Q)
{
    int n = a.size();
    vector<double> z_I(n);
    vector<double> z_Q(n);

    
    for(int i = 0; i < b.size(); i++){
        b[i] = b[i]/a[0];
    }
    for(int i = 0; i < a.size(); i++){
        a[i] = a[i]/a[0];
    }

    for(int m = 1; m <= X_I.size(); m++){
        Y_I.push_back(b[0] * X_I[m-1] + z_I[0]);
        Y_Q.push_back(b[0] * X_Q[m-1] + z_Q[0]);
        
        for(int i = 2; i <= n; i++){
            z_I[i-2] = b[i-1] * X_I[m-1] + z_I[i-1] - a[i-1] * Y_I[m-1];
            z_Q[i-2] = b[i-1] * X_Q[m-1] + z_Q[i-1] - a[i-1] * Y_Q[m-1];
        }        
    }

}