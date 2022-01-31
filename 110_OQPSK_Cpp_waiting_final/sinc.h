double sinc(double x)
{
    double y;
    if(x == 0){
        y = 1;
    }
    else{
        y =  sin(pi * x) / (pi * x);  
    } 
    return y;
}