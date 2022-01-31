//////////////////////////////////////////////////////////////////
// Flip_vec
//   x: input vector 
//   y: output vector
//   L: length(x) = length(y) 
//////////////////////////////////////////////////////////////////
void Flip_vec(vector<double> &y, vector<double> &x, int L)
{
    for(int i = 0; i < L; i++){
        y.push_back(x[L - (i + 1)]); //y[i] = x[L - (i + 1)];
    }
}
// NOTE!!! 第一次加值進去vector時，需用push_back, 不可用 vec_I[i] = array_I[i]
// 若用尋訪的方式加入vector, 雖然能印出值，但實際上這些數值沒有被給予空間!!!