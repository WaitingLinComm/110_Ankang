#pragma once
#include <vector>
//#include<bits/stdc++.h>
using namespace std;
#define sqrt2 0.70710678118 
void pilot(char type,vector<double> &pilot_r,vector<double> &pilot_i,vector<int> &pilot,vector<double> &inpilot_r,vector<double> &inpilot_i ,int &space_type_num){
    const char *a0;
    int N,data_type,space_type;
    // initialization
    /*
    	    pilot.clear();
    pilot.shrink_to_fit();
    pilot_r.clear();
    pilot_r.shrink_to_fit();
    pilot_i.clear();
    pilot_i.shrink_to_fit();
    inpilot_r.clear();
    inpilot_r.shrink_to_fit();
    inpilot_i.clear();
    inpilot_i.shrink_to_fit();
    // mapping pilot 
    	
	*/


    switch(type){
        // spacing_type 0 for (2,28) ; 1 for (3,27) ; 2 for (4,26)
        // data_type = 0 for QPSK ; data_type = 1 for 8PSK 
        case 'A': // atdma QPSK 1
            a0="30CF300FF03CCC03030CFC00C0CCFCFFC";
            N=33;
            space_type = 1 ;
            data_type = 0 ;
            break;
        case 'B': // atdma QPSK 2
            a0="30CF300FF03CCC03030CFC00C0CCFC";
            N=30;
            space_type = 1 ;
            data_type = 0 ;
            break;
        case 'C': // atdma QPSK 3
            a0="33FC0F0FC03003C0C3CFFF3C3303FCCFF";
            N=33;
            space_type = 0 ;
            data_type = 0 ;
            break;
        case 'D': // atdma QPSK 4
            a0="33FC0F0FC03003C0C3CFFF3C3303FC";
            N=30;
            space_type = 0 ;
            data_type = 0 ;
            break;
        case 'E': // atdma QPSK 5
            a0="33FC0F0FC03003C0C3CFFF3C3303FCCFF300CFF033C3030C30C30F30FF333CF3CF3FC3F0";
            N=72;
            space_type = 0 ;
            data_type = 0 ;
            break;
        case 'F': // atdma QPSK 6
            a0="30CF300FF03CCC03030CFC00C0CCFCFFC3F330F00CFF0C3CFCF0C3CFC";
            N=57;
            space_type = 1 ;
            data_type = 0 ;
            break;
        case 'G': // atdma QPSK 7
            a0="30CF300FF03CCC03030CFC00C0CCFCFFC3F330F00CFF0C3CFCF0C3CFCFC33CCCCF00C0F0300";
            N=75;
            space_type = 1 ;
            data_type = 0 ;
            break;
        case 'H': // atdma QPSK 8
            a0="33FC0F0FC03003C0C3CFFF3C3303FCCFF300CFF033C3030C30C30F30FF333CF3CF3FC3F0C00F3C";
            N=78;
            space_type = 0 ;
            data_type = 0 ;
            break;
        case 'I': // atdma 8PSK 1
            a0="186186D86C00030DB0006D80C360001800300361B0C06000";
            N=48;
            space_type = 2 ;
            data_type = 1 ;
            break;
        case 'J': // atdma 8PSK 2
            a0="186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180C06036180DB61861B0D80";
            N=96;
            space_type = 0 ;
            data_type = 1 ;
            break;
        case 'K': // atdma 8PSK 3
            a0="186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6D861B6C36";
            N=72;
            space_type = 2 ;
            data_type = 1 ;
            break;
        case 'L': // atdma 8PSK 4
            a0="186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6D86";
            N=66;
            space_type = 2 ;
            data_type = 1 ;
            break;
        case 'M': // atdma 8PSK 5
            a0="186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6";
            N=63;
            space_type = 2 ;
            data_type = 1 ;
            break;
        case 'N': // atdma 8PSK 6
            a0="186186D86C00030DB0006D80C360001800300361B0000";
            N=45;
            space_type = 2 ;
            data_type = 1 ;
            break;
        case 'O': // atdma 8PSK 7
            a0="186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180C06";
            N=78;
            space_type = 0 ;
            data_type = 1 ;
            break;
        case 'P': // atdma 8PSK 8
            a0="186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180000";
            N=78;
            space_type = 0 ;
            data_type = 1 ;
            break;
        case 'Q': // OPSK 1
            a0="203032102200023132132211222";
            N=27;
            space_type = 3 ;
            data_type = 2 ;
            break;
        case 'R': // OPSK 2
            a0= "021211331131101310000232201112322102300230121203";
            N=48;
            space_type = 3 ;
            data_type = 2 ;
            break;
        case 'S': // OPSK 3
            a0="231013110301132030033121130";
            N=27;
            space_type = 3 ;
            data_type = 2 ;
            break;
        case 'T': // OPSK 4
            a0="203320323203011130322301302";
            N=27;
            space_type = 3 ;
            data_type = 2 ;
            break;
        case 'U': // OPSK 5
            a0="213031330103312010011323310";
            N=27;
            space_type = 3 ;
            data_type = 2 ;
            break;
     }
     N=4*(N);
     char a[N];
// DEX to BIN
    if (space_type != 3){
        for(int i=0;i<N/4;i++){
            if(a0[i]=='3'){
                a[4*i]='0';
                a[4*i+1]='0';
                a[4*i+2]='1';
                a[4*i+3]='1';
            }else if(a0[i]=='F'){
                a[4*i]='1';
                a[4*i+1]='1';
                a[4*i+2]='1';
                a[4*i+3]='1';
            }else if(a0[i]=='C'){
                a[4*i]='1';
                a[4*i+1]='1';
                a[4*i+2]='0';
                a[4*i+3]='0';
            }else if(a0[i]=='0'){
                a[4*i]='0';
                a[4*i+1]='0';
                a[4*i+2]='0';
                a[4*i+3]='0';
            }else if(a0[i]=='1'){
                a[4*i]='0';
                a[4*i+1]='0';
                a[4*i+2]='0';
                a[4*i+3]='1';
            }else if(a0[i]=='6'){
                a[4*i]='0';
                a[4*i+1]='1';
                a[4*i+2]='1';
                a[4*i+3]='0';
            }else if(a0[i]=='B'){
                a[4*i]='1';
                a[4*i+1]='0';
                a[4*i+2]='1';
                a[4*i+3]='1';
            }else if(a0[i]=='D'){
                a[4*i]='1';
                a[4*i+1]='1';
                a[4*i+2]='0';
                a[4*i+3]='1';
            }else if(a0[i]=='8'){
                a[4*i]='0';
                a[4*i+1]='1';
                a[4*i+2]='0';
                a[4*i+3]='0';
            }else if(a0[i]=='2'){
                a[4*i]='0';
                a[4*i+1]='0';
                a[4*i+2]='1';
                a[4*i+3]='0';
            }
        }



    } 

// BIN to pilot symbol


    if (data_type == 0){ // QPSK
        for (int i=0;i<N/2;i++){
            if (a[2*i]=='0'&& a[2*i+1]=='0'){
                pilot_r.push_back(1);
                pilot_i.push_back(1);
                pilot.push_back(0);
            }else if (a[2*i]=='0'&& a[2*i+1]=='1'){
                pilot_r.push_back(-1);
                pilot_i.push_back(1);
                pilot.push_back(1);
            }else if (a[2*i]=='1'&& a[2*i+1]=='0'){
                pilot_r.push_back(1);
                pilot_i.push_back(-1);
                pilot.push_back(3);
            }else{
                pilot_r.push_back(-1);
                pilot_i.push_back(-1);
                pilot.push_back(2);
            }
        }
    }
    else if (data_type == 1 ){// 8PSK
        for (int i=0;i<N/3;i++){
            if (a[3*i]=='0'&& a[3*i+1]=='0'&& a[3*i+2]=='0'){
                pilot_r.push_back(1);
                pilot_i.push_back(0);
                pilot.push_back(0);
            }else if (a[3*i]=='0'&& a[3*i+1]=='0'&& a[3*i+2]=='1'){
                pilot_r.push_back(1/sqrt2);
                pilot_i.push_back(1/sqrt2);
                pilot.push_back(1);
            }else if (a[3*i]=='0'&& a[3*i+1]=='1'&& a[3*i+2]=='0'){
                pilot_r.push_back(1/sqrt2);
                pilot_i.push_back(-1/sqrt2);
                pilot.push_back(2);
            }else if (a[3*i]=='0'&& a[3*i+1]=='1'&& a[3*i+2]=='1'){
                pilot_r.push_back(0);
                pilot_i.push_back(-1);
                pilot.push_back(3);
            }else if (a[3*i]=='1'&& a[3*i+1]=='0'&& a[3*i+2]=='0'){
                pilot_r.push_back(-1/sqrt2);
                pilot_i.push_back(1/sqrt2);
                pilot.push_back(4);
            }else if (a[3*i]=='1'&& a[3*i+1]=='0'&& a[3*i+2]=='1'){
                pilot_r.push_back(0);
                pilot_i.push_back(1);
                pilot.push_back(5);
            }else if (a[3*i]=='1'&& a[3*i+1]=='1'&& a[3*i+2]=='0'){
                pilot_r.push_back(-1);
                pilot_i.push_back(0);
                pilot.push_back(6);
            }else{
                pilot_r.push_back(-1/sqrt2);
                pilot_i.push_back(-1/sqrt2);
                pilot.push_back(7);
            }
        }
    }
    else if (data_type == 2){ // OQPSK
        for (int i=0;i<N/2;i++){
            if (a0[i]=='0'){
                pilot_r.push_back(1);
                pilot_i.push_back(1);
                pilot.push_back(0);
            }else if (a0[i]=='1'){
                pilot_r.push_back(-1);
                pilot_i.push_back(1);
                pilot.push_back(1);
            }else if (a0[i]=='3'){
                pilot_r.push_back(1);
                pilot_i.push_back(-1);
                pilot.push_back(3);
            }else{
                pilot_r.push_back(-1);
                pilot_i.push_back(-1);
                pilot.push_back(2);
            }
        }
    }

// Generate synbol sequence 

if ( space_type == 0 ){
    for (int burst_index = 0 ; burst_index < pilot.size()/2 ; burst_index++){
      inpilot_i.push_back( pilot_i[burst_index*2] );
      inpilot_i.push_back( pilot_i[burst_index*2+1] );
      inpilot_r.push_back( pilot_r[burst_index*2] );
      inpilot_r.push_back( pilot_r[burst_index*2+1] );

      for (int i = 0 ; i<28 ; i++){
        inpilot_i.push_back( 0 );
        inpilot_r.push_back( 0 );

      }  

    }

}else if ( space_type == 1){
    for (int burst_index = 0 ; burst_index < pilot.size()/3 ; burst_index++){
      inpilot_i.push_back( pilot_i[burst_index*3] );
      inpilot_i.push_back( pilot_i[burst_index*3+1] );
      inpilot_i.push_back( pilot_i[burst_index*3+2] );
      inpilot_r.push_back( pilot_r[burst_index*3] );
      inpilot_r.push_back( pilot_r[burst_index*3+1] );
      inpilot_r.push_back( pilot_r[burst_index*3+2] );

      for (int i = 0 ; i<27 ; i++){
        inpilot_i.push_back( 0 );
        inpilot_r.push_back( 0 );

      }  
        
    }


}else if (space_type == 2){
    for (int burst_index = 0 ; burst_index < pilot.size()/4 ; burst_index++){
      inpilot_i.push_back( pilot_i[burst_index*4] );
      inpilot_i.push_back( pilot_i[burst_index*4+1] );
      inpilot_i.push_back( pilot_i[burst_index*4+2] );
      inpilot_i.push_back( pilot_i[burst_index*4+3] );
      inpilot_r.push_back( pilot_r[burst_index*4] );
      inpilot_r.push_back( pilot_r[burst_index*4+1] );
      inpilot_r.push_back( pilot_r[burst_index*4+2] );
      inpilot_r.push_back( pilot_r[burst_index*4+3] );

      for (int i = 0 ; i<26 ; i++){
        inpilot_i.push_back( 0 );
        inpilot_r.push_back( 0 );

      }  
        
    }


}else if (space_type == 3){
    for (int burst_index = 0 ; burst_index < pilot.size() ; burst_index++){
      inpilot_i.push_back( pilot_i[burst_index] );
      inpilot_r.push_back( pilot_r[burst_index] );      
    }


}


if (space_type == 0){
    space_type_num = 2 ;
}else if (space_type == 1){
    space_type_num = 3 ;
}else if (space_type == 2){
    space_type_num = 4 ;
}else if (space_type == 3){
    space_type_num = 0 ;
} 

        // for( int i = 0 ; i < pilot.size() ; i++ ){
        //     cout<< pilot[i] ;
        // }
        // cout<<endl ;






}
