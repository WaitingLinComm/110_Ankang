#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

void str2pilot( char detection_type[] , int space_type   , string s , int length , vector<double> &pilot_r , vector<double> &pilot_i  , vector<double> &inpilot_r , vector<double> &inpilot_i , vector<int> &pilot )
{

    cout<<s[0]<<endl;
    if ( strcmp(detection_type , "QPSK") == 0 ){ // QPSK
        for (int i=0;i< length - 1 ;i++){
            if ( s[i] - '0' == 0 ){
                pilot_r.push_back(1);
                pilot_i.push_back(1);
                pilot.push_back(0);
            }else if ( s[i] - '0' == 1 ){
                pilot_r.push_back(-1);
                pilot_i.push_back(1);
                pilot.push_back(1);
            }else if (s[i] - '0' == 2){
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
    else if ( strcmp(detection_type , "8PSK") == 0 ){// 8PSK
        for (int i=0;i< length - 1 ;i++){
            if ( s[i] - '0' == 0 ){
                pilot_r.push_back(1);
                pilot_i.push_back(0);
                pilot.push_back(0);
            }else if ( s[i] - '0' == 1 ){
                pilot_r.push_back(1/sqrt2);
                pilot_i.push_back(1/sqrt2);
                pilot.push_back(1);
            }else if ( s[i] - '0' == 2 ){
                pilot_r.push_back(1/sqrt2);
                pilot_i.push_back(-1/sqrt2);
                pilot.push_back(2);
            }else if ( s[i] - '0' == 3 ){
                pilot_r.push_back(0);
                pilot_i.push_back(-1);
                pilot.push_back(3);
            }else if ( s[i] - '0' == 4 ){
                pilot_r.push_back(-1/sqrt2);
                pilot_i.push_back(1/sqrt2);
                pilot.push_back(4);
            }else if ( s[i] - '0' == 5 ){
                pilot_r.push_back(0);
                pilot_i.push_back(1);
                pilot.push_back(5);
            }else if ( s[i] - '0' == 6 ){
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
    else if ( strcmp(detection_type , "OQPSK") == 0 ){ // OQPSK
        for (int i=0;i<length-1;i++){
            if (s[i]=='0'){
                pilot_r.push_back(1);
                pilot_i.push_back(1);
                pilot.push_back(0);
            }else if (s[i]=='1'){
                pilot_r.push_back(-1);
                pilot_i.push_back(1);
                pilot.push_back(1);
            }else if (s[i]=='3'){
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

    if ( strcmp(detection_type , "QPSK") == 0 || strcmp(detection_type , "8PSK") == 0 ){
        for ( int i = 0 ; i < ( length -1 ) / space_type ; i++){
            for (int j = 0 ; j < space_type ; j++ ){
                inpilot_r.push_back( pilot_r[ space_type * i + j ] ) ;
                inpilot_i.push_back( pilot_i[ space_type * i + j ] ) ;
            }
            for (int j = 0 ; j < 30 - space_type ; j++ ){
                inpilot_r.push_back( 0 ) ;
                inpilot_i.push_back( 0 ) ;
            }
        }
    }
    else if ( strcmp(detection_type , "OQPSK") == 0 ){
        for ( int i = 0 ; i < ( length -1 ) / space_type ; i++){
            for (int j = 0 ; j < space_type ; j++ ){
                inpilot_r.push_back( pilot_r[ space_type * i + j ] ) ;
                inpilot_i.push_back( pilot_i[ space_type * i + j ] ) ;
            }
        }

    }
    // for( int i = 0 ; i < inpilot_i.size() ; i++ ){
    //     cout<< inpilot_i[i] ;
    // }
    // cout << endl ;
    // for( int i = 0 ; i < inpilot_r.size() ; i++ ){
    //     cout<< inpilot_r[i] ;
    // }
    // cout <<endl ;


	return;








}