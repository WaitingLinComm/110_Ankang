// SNRestimation
//
//    Use pilot to estimate the SNR(Signal to Noise Ratio) of signal
//    [Input variable]: mode_type(a vector), received_pilot(a vector)
//      1. mod_type : "QPSK", "8PSK", "OQPSK"
//      2. received_pilot: total received pilot signal after CFO, PHO, TO compensation form each burst
//         NOTE: received_pilot is the result which can be visualized on constellation set.
//    [Output variable]: SNR_dB(a scalar; Unit: dB)  
//      NOTE
//      SNR = S/N0 % linear scale
//      S: power of received pilot
//      N0: noise power of received pilot
//      SNR_dB = 10 * log10(SNR); % dB

double SNRestimation(string &mode_type, vector<double> &received_pilot_I, vector<double> &received_pilot_Q)
{  
    // Calculate power of received pilot: S
    double L_received_pilot = received_pilot_I.size();
    // realize: S = norm(received_pilot,2)^2/length(received_pilot);    
    double norm_received_pilot;
    norm_received_pilot = 0;
    for(int i = 0; i < L_received_pilot; i++){
        norm_received_pilot = norm_received_pilot + received_pilot_I[i] * received_pilot_I[i] + received_pilot_Q[i] * received_pilot_Q[i]; 
    }    
    double S = norm_received_pilot / L_received_pilot;
    double a = sqrt(S/2);        // scalar of the desired pilot symbol on the constellation set

    // Divide received pilots into different quadrant
    double phase;
    vector<double> sp1_I, sp1_Q, sp2_I, sp2_Q, sp3_I, sp3_Q, sp4_I, sp4_Q;
    sp1_I.reserve(20); sp1_Q.reserve(20); sp2_I.reserve(20); sp2_Q.reserve(20); sp3_I.reserve(20); sp3_Q.reserve(20); sp4_I.reserve(20); sp4_Q.reserve(20);
    vector<double> sp5_I, sp5_Q, sp6_I, sp6_Q, sp7_I, sp7_Q, sp8_I, sp8_Q;
    sp5_I.reserve(20); sp5_Q.reserve(20); sp6_I.reserve(20); sp6_Q.reserve(20); sp7_I.reserve(20); sp7_Q.reserve(20); sp8_I.reserve(20); sp8_Q.reserve(20);
    
    int sn1 = 0, sn2 = 0, sn3 = 0, sn4 = 0, sn5 = 0, sn6 = 0, sn7 = 0, sn8 = 0;// number of pilots in each quadrant
    double N0;
    double norm1, norm2, norm3, norm4, norm5, norm6, norm7, norm8;
    if(mode_type == "QPSK" || mode_type == "OQPSK"){
        double z1_I, z1_Q, z2_I, z2_Q, z3_I, z3_Q, z4_I, z4_Q;
        z1_I = a * 1; z1_Q = a * 1;         // z1 = a * (1 + 1i); desired pilot symbol in 1st quadrant
        z2_I = a * (-1); z2_Q = a * 1;      // z2 = a * (-1 + 1i); desired pilot symbol in 2nd quadrant
        z3_I = a * (-1); z3_Q = a * (-1);   // z3 = a * (-1 - 1i); desired pilot symbol in 3rd quadrant
        z4_I = a * 1; z4_Q = a * (-1);      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant

        for(int i = 0; i < L_received_pilot; i++){
            // realize: phase = atan2(received_pilot(i)); // rad
            phase = atan2(received_pilot_Q[i], received_pilot_I[i]); // atan(Y,X) 
            // 1st quadrant
            if(phase > 0 && phase < pi/2){   
                sp1_I.push_back(received_pilot_I[i]); sp1_Q.push_back(received_pilot_Q[i]);
                sn1 = sn1 + 1;
            }
            // 2nd quadrant
            if(phase > pi/2 && phase <= pi){
                sp2_I.push_back(received_pilot_I[i]); sp2_Q.push_back(received_pilot_Q[i]);
                sn2 = sn2 + 1;
            }
            // 3rd quadrant
            if(phase > -pi && phase <= -pi/2){ 
                sp3_I.push_back(received_pilot_I[i]); sp3_Q.push_back(received_pilot_Q[i]);
                sn3 = sn3 + 1;
            }    
            // 4th quadrant
            if(phase > -pi/2 && phase <= 0){   
                sp4_I.push_back(received_pilot_I[i]); sp4_Q.push_back(received_pilot_Q[i]);
                sn4 = sn4 + 1;
            }
        } 
        
        // Calculate noise's variance (power)
        // realize: N0 = (1 / L_received_pilot) * (norm(sp1 - z1, 2) ^ 2 + ...
        // norm(sp2 - z2, 2) ^ 2 + norm(sp3 - z3, 2) ^ 2  + norm(sp4 - z4, 2) ^ 2 ); 
        norm1 = 0, norm2 = 0, norm3 = 0, norm4 = 0;    
        for(int i = 0; i < sn1; i++){
            norm1 = norm1 + (sp1_I[i] - z1_I) * (sp1_I[i] - z1_I) + (sp1_Q[i] - z1_Q) * (sp1_Q[i] - z1_Q);            
        }
        for(int i = 0; i < sn2; i++){
            norm2 = norm2 + (sp2_I[i] - z2_I) * (sp2_I[i] - z2_I) + (sp2_Q[i] - z2_Q) * (sp2_Q[i] - z2_Q);            
        }
        for(int i = 0; i < sn3; i++){
            norm3 = norm3 + (sp3_I[i] - z3_I) * (sp3_I[i] - z3_I) + (sp3_Q[i] - z3_Q) * (sp3_Q[i] - z3_Q);            
        }
        for(int i = 0; i < sn4; i++){
            norm4 = norm4 + (sp4_I[i] - z4_I) * (sp4_I[i] - z4_I) + (sp4_Q[i] - z4_Q) * (sp4_Q[i] - z4_Q);            
        }
        N0 = (1/L_received_pilot) * (norm1 + norm2 + norm3 + norm4);
    }  

    if(mode_type == "8PSK"){
        double z1_I, z1_Q, z2_I, z2_Q, z3_I, z3_Q, z4_I, z4_Q, z5_I, z5_Q, z6_I, z6_Q, z7_I, z7_Q, z8_I, z8_Q;
        z1_I = a * sqrt(2); z1_Q = a * 0;         // z1 = a * (1 + 1i); desired pilot symbol in 1st quadrant
        z2_I = a * 1; z2_Q = a * 1;      // z2 = a * (-1 + 1i); desired pilot symbol in 2nd quadrant
        z3_I = a * 0; z3_Q = a * sqrt(2);   // z3 = a * (-1 - 1i); desired pilot symbol in 3rd quadrant
        z4_I = a * (-1); z4_Q = a * 1;      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant
        z5_I = a * (-sqrt(2)); z5_Q = a * 0;      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant
        z6_I = a * (-1); z6_Q = a * (-1);      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant
        z7_I = a * 0; z7_Q = a * (-sqrt(2));      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant
        z8_I = a * 1; z8_Q = a * (-1);      // z4 = a * (1 - 1i);  desired pilot symbol in 4th quadrant

        for(int i = 0; i < L_received_pilot; i++){
            phase = atan2(received_pilot_Q[i], received_pilot_I[i]); // atan(Y,X) 
            // 1st quadrant
            if(phase > -pi/8 && phase <= pi/8){   
                sp1_I.push_back(received_pilot_I[i]); sp1_Q.push_back(received_pilot_Q[i]);
                sn1 = sn1 + 1;
            }
            // 2nd quadrant
            if(phase > pi/8 && phase <= 3*pi/8){
                sp2_I.push_back(received_pilot_I[i]); sp2_Q.push_back(received_pilot_Q[i]);
                sn2 = sn2 + 1;
            }
            // 3rd quadrant
            if(phase > 3*pi/8 && phase <= 5*pi/8){ 
                sp3_I.push_back(received_pilot_I[i]); sp3_Q.push_back(received_pilot_Q[i]);
                sn3 = sn3 + 1;
            }    
            // 4th quadrant
            if(phase > 5*pi/8 && phase <= 7*pi/8 ){   
                sp4_I.push_back(received_pilot_I[i]); sp4_Q.push_back(received_pilot_Q[i]);
                sn4 = sn4 + 1;
            }
            // 5th quadrant
            if(phase > 7*pi/8 || phase <= -7*pi/8){   
                sp5_I.push_back(received_pilot_I[i]); sp5_Q.push_back(received_pilot_Q[i]);
                sn5 = sn5 + 1;
            }
            // 6th quadrant
            if(phase > -7*pi/8 &&  phase <=  -5*pi/8){   
                sp6_I.push_back(received_pilot_I[i]); sp6_Q.push_back(received_pilot_Q[i]);
                sn6 = sn6 + 1;
            }
            // 7th quadrant
            if(phase >  -5*pi/8 && phase <= -3*pi/8){   
                sp7_I.push_back(received_pilot_I[i]); sp7_Q.push_back(received_pilot_Q[i]);
                sn7 = sn7 + 1;
            }
            // 8th quadrant
            if(phase >  -3*pi/8 & phase <= -1*pi/8){   
                sp8_I.push_back(received_pilot_I[i]); sp8_Q.push_back(received_pilot_Q[i]);
                sn8 = sn8 + 1;
            }
        }
        // Calculate noise's variance (power)
        // realize: N0 = (1 / L_received_pilot) * (norm(sp1 - z1, 2) ^ 2 + ...
        // norm(sp2 - z2, 2) ^ 2 + norm(sp3 - z3, 2) ^ 2  + norm(sp4 - z4, 2) ^ 2 ); 
        norm1 = 0, norm2 = 0, norm3 = 0, norm4 = 0, norm5 = 0, norm6 = 0, norm7 = 0, norm8 = 0;    
        for(int i = 0; i < sn1; i++){
            norm1 = norm1 + (sp1_I[i] - z1_I) * (sp1_I[i] - z1_I) + (sp1_Q[i] - z1_Q) * (sp1_Q[i] - z1_Q);            
        }
        for(int i = 0; i < sn2; i++){
            norm2 = norm2 + (sp2_I[i] - z2_I) * (sp2_I[i] - z2_I) + (sp2_Q[i] - z2_Q) * (sp2_Q[i] - z2_Q);            
        }
        for(int i = 0; i < sn3; i++){
            norm3 = norm3 + (sp3_I[i] - z3_I) * (sp3_I[i] - z3_I) + (sp3_Q[i] - z3_Q) * (sp3_Q[i] - z3_Q);            
        }
        for(int i = 0; i < sn4; i++){
            norm4 = norm4 + (sp4_I[i] - z4_I) * (sp4_I[i] - z4_I) + (sp4_Q[i] - z4_Q) * (sp4_Q[i] - z4_Q);            
        }
        for(int i = 0; i < sn5; i++){
            norm5 = norm5 + (sp5_I[i] - z5_I) * (sp5_I[i] - z5_I) + (sp5_Q[i] - z5_Q) * (sp5_Q[i] - z5_Q);            
        }
        for(int i = 0; i < sn6; i++){
            norm6 = norm6 + (sp6_I[i] - z6_I) * (sp6_I[i] - z6_I) + (sp6_Q[i] - z6_Q) * (sp6_Q[i] - z6_Q);            
        }
        for(int i = 0; i < sn7; i++){
            norm7 = norm7 + (sp7_I[i] - z7_I) * (sp7_I[i] - z7_I) + (sp7_Q[i] - z7_Q) * (sp7_Q[i] - z7_Q);            
        }
        for(int i = 0; i < sn8; i++){
            norm8 = norm8 + (sp8_I[i] - z8_I) * (sp8_I[i] - z8_I) + (sp8_Q[i] - z8_Q) * (sp8_Q[i] - z8_Q);            
        }
        N0 = (1/L_received_pilot) * (norm1 + norm2 + norm3 + norm4 + norm5 + norm6 + norm7 + norm8);
    } // end if       
    
    // Calculate the SNR of the received pilots
    double SNRlin = S / N0;              // linear scale
    double SNR_dB = 10 * log10(SNRlin);  // dB
    
    return SNR_dB;
}