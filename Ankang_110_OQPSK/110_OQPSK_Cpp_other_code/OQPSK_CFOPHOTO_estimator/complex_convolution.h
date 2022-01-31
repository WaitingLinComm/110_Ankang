// The code implement complex convolution: filter(x, 1, y)
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
// We can implement the convolution as the following way
// t = fft(x, Lz) * fft(y, Lz), where Lz = Lx + Ly -1
// z = ifft(t, Lz)
//

void complex_convolution(vector<double> &z_filter_I, vector<double> &z_filter_Q, int Lz,  vector<double> &x_I, vector<double> &x_Q, int Lx, vector<double> &y_I, vector<double> &y_Q, int Ly)
{
    int N = Lx + Ly - 1; // do N-point fft, ifft
    fftw_complex *q, *q_fft;
    q = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    q_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    for(int i = 0; i < Lx; i++){
        q[i][0] = x_I[i];
        q[i][1] = x_Q[i];
    }
    fftw_plan p2;
    p2 = fftw_plan_dft_1d(N, q, q_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p2);
    fftw_destroy_plan(p2);
    fftw_free(q);   
    
    /*
    for (int i = 0; i < 20; i++){
        printf("[%d]: %f + j*%f\n", i, q[i][0], q[i][1]);
    }
    */
    ////////////////////////  realize r_fft = fft(r, Lz)   ////////////////////////
    fftw_complex *r, *r_fft;
    r = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    r_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

    for(int i = 0; i < Ly; i++){
        r[i][0] = y_I[i];
        r[i][1] = y_Q[i];
    }
    fftw_plan p;
    p = fftw_plan_dft_1d(N, r, r_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(r);
    
    /*
    for (int i = 0; i < 20; i++){
        printf("[%d]: %f + j*%f\n", i, r_fft[i][0], r_fft[i][1]);
    }
    */
    ////////////////////////  realize z1_ifft = fft(r, Lz) * fft(conj(flip_q), Lz)   ////////////////////////
    fftw_complex *z1, *z1_ifft_conv;
    z1 = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    z1_ifft_conv = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    
    for (int i = 0; i < N; i++){
        z1[i][0] = r_fft[i][0] * q_fft[i][0] - r_fft[i][1] * q_fft[i][1]; 
        z1[i][1] = r_fft[i][0] * q_fft[i][1] + r_fft[i][1] * q_fft[i][0]; 
    }
    fftw_free(r_fft);
    fftw_free(q_fft);

    fftw_plan p3;
    p3 = fftw_plan_dft_1d(N, z1, z1_ifft_conv, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p3); //repeat as needed 
    
    for (int i = 0; i < Lz; i++) {
        z_filter_I.push_back(z1_ifft_conv[i][0] * 1./N);
        z_filter_Q.push_back(z1_ifft_conv[i][1] * 1./N);
    }
    /*
    for (int i = 0; i < 20; i++){
        printf("[%d]: %f + j*%f\n", i, z1_ifft_conv[i][0], z1_ifft_conv[i][1]);
    }
    */
    fftw_destroy_plan(p3);
    fftw_free(z1);
    fftw_free(z1_ifft_conv);
 
}