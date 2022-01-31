// Ref: http://jashliao.eu/wordpress/2019/11/12/c-vector-容器淺析05-二維數組二維陣列兩種定義方法/

/////////////////////////////////////////////////////////

#include <iostream>
using namespace std;
#include <vector>
#include <math.h>

int main()
{
    // test: angle(1 + 1*j)
    double c;
    c = atan2(2,1); // atan(Y,X)
    cout << c << "\n";
 
    int N=5, M=6; 
    vector<vector<int> > obj(N, vector<int>(M)); //定义二维动态数组5行6列
	cout << obj.size() << "\n";    // 5 //number of row
    cout << obj[0].size() << "\n"; // 6 //number of col

    for(int i=0; i< obj.size(); i++)//init二维动态数组 
    {
        for(int j=0;j<obj[i].size();j++)
        {
            obj[i][j]=i+j;
            //obj[i][j]= 0;
            //obj[i].push_back(i+j);
        }

    }
    
    for(int i=0; i< obj.size(); i++)//输出二维动态数组 
    {
        for(int j=0;j<obj[i].size();j++)
        {
            printf("[%d,%d]: %d ", i, j, obj[i][j]);
            //cout<<obj[i][j]<<" ";
        }
        cout<<"\n";
    }
    cout << obj.size() << "\n";    // 5 //number of row
    cout << obj[0].size() << "\n"; // 6 //number of col
    return 0;
}
