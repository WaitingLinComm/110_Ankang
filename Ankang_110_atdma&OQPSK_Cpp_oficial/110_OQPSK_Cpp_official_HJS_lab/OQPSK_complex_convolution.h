// The code implement complex convolution 
// NOTE!
// If we would like to perform z = filter(x, 1, y), the output length Lz = Ly.
// If we would like to perform z = conv(x, y) the output length Lz = Lx + Ly - 1.
// 
// input:  x_I, x_Q, Lx, y_I, y_Q, Ly
// output: z_I, z_Q, Lz
// datatype: vector<double> 
//
// NOTE!
// Lx >= Ly
// 
// Suppose we would like to calculate z = conv(x, y), where x, y are complex values.           
// The code implement the convolution as sol1 which is much faster than using sol2
// sol1:
// t = fft(x, Lz) * fft(y, Lz), where Lz = Lx + Ly -1
// z = ifft(t, Lz)
// sol2:
// Re_z = conv(Re_x, Re_y) - conv(Im_x, Im_y); 
// Im_z = conv(Re_x, Im_y) + conv(Im_x, Re_y);

void OQPSK_complex_convolution(vector<double> &z_filter_I, vector<double> &z_filter_Q, int Lz,  vector<double> &x_I, vector<double> &x_Q, int Lx, vector<double> &y_I, vector<double> &y_Q, int Ly)
{    
    int N = Lx + Ly - 1; // do N-point fft, ifft
    ////////////////////// Declare fftw_complex //////////////////////
    fftw_complex *q, *q_fft;
    fftw_complex *r, *r_fft;
    fftw_complex *z, *z_ifft_conv;
    ////////////////////// set fftw_plan //////////////////////
    fftw_plan p1;
    fftw_plan p2;
    fftw_plan p3;
    ////////////////////////  realize q_fft = fft(q, Lz)   ////////////////////////
    q = fftw_alloc_complex(N);
    q_fft = fftw_alloc_complex(N);
    for(int i = 0; i < Lx; i++){
        q[i][0] = x_I[i];
        q[i][1] = x_Q[i];
    }                
    for(int i = Lx; i < N; i++){
        q[i][0] = 0; q[i][1] = 0; 
    }
    p1 = fftw_plan_dft_1d(N, q, q_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p1);
    fftw_destroy_plan(p1);
    fftw_free(q);
 
    ////////////////////////  realize r_fft = fft(r, Lz)   ////////////////////////
    r = fftw_alloc_complex(N);
    r_fft = fftw_alloc_complex(N);
    for(int i = 0; i < Ly; i++){
        r[i][0] = y_I[i];
        r[i][1] = y_Q[i];
    }
    for(int i = Ly; i < N; i++){
        r[i][0] = 0; r[i][1] = 0; 
    }
    p2 = fftw_plan_dft_1d(N, r, r_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p2);
    fftw_destroy_plan(p2);
    fftw_free(r);

    ////////////////////////  realize z1_ifft = fft(r, Lz) * fft(conj(flip_q), Lz)   ////////////////////////
    z = fftw_alloc_complex(N);
    z_ifft_conv = fftw_alloc_complex(N);
    for (int i = 0; i < N; i++){
        z[i][0] = r_fft[i][0] * q_fft[i][0] - r_fft[i][1] * q_fft[i][1]; 
        z[i][1] = r_fft[i][0] * q_fft[i][1] + r_fft[i][1] * q_fft[i][0]; 
    }
    fftw_free(q_fft);
    fftw_free(r_fft);
    p3 = fftw_plan_dft_1d(N, z, z_ifft_conv, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p3); //repeat as needed 
    fftw_destroy_plan(p3);
    fftw_free(z);

    for (int i = 0; i < Lz; i++) {
        z_filter_I.push_back(z_ifft_conv[i][0] * 1./N);
        z_filter_Q.push_back(z_ifft_conv[i][1] * 1./N);
    }
    fftw_free(z_ifft_conv);

    // printf the result
    /*
    for (int i = 0; i < 20; i++){
        printf("[%d]: %f + j*%f\n", i, z1_ifft_conv[i][0], z1_ifft_conv[i][1]);
    }
    */
}