function [ pilot , adaptivef , inpilot2 ] = pilot_map( mod_type , mode_index , IPOINT )
%% input : 
% mod_type : "atdma_8psk","atdma_qpsk","oqpsk"
% mode_index : the index corresponding to mod_type ( value is from spec )
% IPOINT : oversampling point
%% output: 
% pilot : array storing the pilot sequence
% adaptivef : indentifacation sequence of pilot symbol sequence after oversampling 
% inpilot2 : pilot symbol sequence after oversampling 

%% map pilot
pilot = [] ;
if mod_type == "atdma_8psk" || mod_type =="atdma_qpsk"% atdma part
    pilot = atdma_pilot( mod_type , mode_index) ;
elseif mod_type == "oqpsk"% OQPSK part
    pilot = OQPSK_pilot( mode_index ) ;
end
%% create pilot sequence with oversampling point
inpilot=[];
adaptive=ones(1,length(pilot));
adaptivef=[];


if mod_type == "atdma_8psk"  % atdma part
    if mode_index == 1 || mode_index == 3 || mode_index == 4 || mode_index == 5 || mode_index == 6|| mode_index == 10
        for i=1:4:length(pilot)
            inpilot=[inpilot pilot(1,i:i+3) zeros(1,26)] ; % every 4 pilot symbol follows 26 info symbols
            adaptivef=[adaptivef adaptive(1,i:i+3) zeros(1,26)];
        end
        inpilot2= oversamp( inpilot, length(inpilot) , IPOINT,1) ; % oversample the pilot to filter the ovesampling symbol sequence   
        adaptivef= oversamp( adaptivef, length(adaptivef) , IPOINT,1) ;
        
    elseif mode_index == 2 || mode_index == 7 || mode_index == 8|| mode_index == 9
        for i=1:2:length(pilot)
            inpilot=[inpilot pilot(1,i:i+1) zeros(1,28)] ; % every 2 pilot symbol follows 28 info symbols
            adaptivef=[adaptivef adaptive(1,i:i+1) zeros(1,28)];
        end
        inpilot2= oversamp( inpilot, length(inpilot) , IPOINT,1) ;% oversample the pilot to filter the ovesampling symbol sequence   
        adaptivef= oversamp( adaptivef, length(adaptivef) , IPOINT,1) ;
    end


elseif mod_type =="atdma_qpsk"
    if mode_index == 3 || mode_index == 4 || mode_index == 5 || mode_index == 8|| mode_index == 10
        for i=1:2:length(pilot)
            inpilot=[inpilot pilot(1,i:i+1) zeros(1,28)] ; % every 2 pilot symbol follows 28 info symbols
            adaptivef=[adaptivef adaptive(1,i:i+1) zeros(1,28)];
        end
        inpilot2= oversamp( inpilot, length(inpilot) , IPOINT,1) ;% oversample the pilot to filter the ovesampling symbol sequence   
        adaptivef= oversamp( adaptivef, length(adaptivef) , IPOINT,1) ;

    elseif mode_index == 1 || mode_index == 2 || mode_index == 6 || mode_index == 7 || mode_index == 9
        for i=1:3:length(pilot)
            inpilot=[inpilot pilot(1,i:i+2) zeros(1,27)] ; % every 3 pilot symbol follows 27 info symbols
            adaptivef=[adaptivef adaptive(1,i:i+2) zeros(1,27)];
        end
        inpilot2= oversamp( inpilot, length(inpilot) , IPOINT,1) ;% oversample the pilot to filter the ovesampling symbol sequence   
        adaptivef= oversamp( adaptivef, length(adaptivef) , IPOINT,1) ;
    end
    
    
elseif mod_type == "oqpsk"% OQPSK part
    inpilot2= oversamp( pilot, length(pilot) , IPOINT,1) ;% oversample the pilot to filter the ovesampling symbol sequence
    % since it's preamble sequence, you don't need insert 0s 
    adaptive=ones(1,length(pilot));% since it's preamble sequence, you don't need insert 0s
    adaptivef= oversamp( adaptive, length(adaptive), IPOINT,1) ;% oversample the pilot to filter the ovesampling symbol sequence    
end

% 
% 
% % DC2 unique word (CQPSK)
% DC2_uniq='00011110001000';
% DC2_pilot=[];
% for i=1:2:length(DC2_uniq)
%     if (DC2_uniq(i)=='0' && DC2_uniq(i+1)=='0')
%         DC2_pilot = [DC2_pilot 1+j];
%     elseif (DC2_uniq(i)=='0' && DC2_uniq(i+1)=='1')
%         DC2_pilot = [DC2_pilot -1+j];
%     elseif (DC2_uniq(i)=='1' && DC2_uniq(i+1)=='0')
%         DC2_pilot = [DC2_pilot 1-j];
%     elseif (DC2_uniq(i)=='1' && DC2_uniq(i+1)=='1')
%         DC2_pilot = [DC2_pilot -1-j];
%     end
% end
% DC2_inpilot = [zeros(1,26) DC2_pilot zeros(1,40)];
% 
% %% Timeslots per Burst = 3
% 
% % NT3 unique word for encoded speech (CQPSK)
% NT3_speech_uniq = '001010011110';
% NT3_speech_pilot = [];
% for i=1:2:length(NT3_speech_uniq)
%     if (NT3_speech_uniq(i)=='0' && NT3_speech_uniq(i+1)=='0')
%         NT3_speech_pilot = [NT3_speech_pilot 1+j];
%     elseif (NT3_speech_uniq(i)=='0' && NT3_speech_uniq(i+1)=='1')
%         NT3_speech_pilot = [NT3_speech_pilot -1+j];
%     elseif (NT3_speech_uniq(i)=='1' && NT3_speech_uniq(i+1)=='0')
%         NT3_speech_pilot = [NT3_speech_pilot 1-j];
%     elseif (NT3_speech_uniq(i)=='1' && NT3_speech_uniq(i+1)=='1')
%         NT3_speech_pilot = [NT3_speech_pilot -1-j];
%     end
% end
% NT3_speech_inpilot = [zeros(1,26) NT3_speech_pilot zeros(1,80)];
% 
% % NT3 unique word for FACCH0 (CBPSK)
% NT3_FACCH0_uniq = '10101010';
% NT3_FACCH0_pilot = [];
% for i=1:length(NT3_FACCH0_uniq)
%     if (NT3_FACCH0_uniq(i)=='0')
%         NT3_FACCH0_pilot = [NT3_FACCH0_pilot 1+j];
%     else (NT3_FACCH0_uniq(i)=='1')
%         NT3_FACCH0_pilot = [NT3_FACCH0_pilot -1-j];
%     end
% end
% NT3_FACCH0_inpilot = [zeros(1,26) NT3_FACCH0_pilot zeros(1,78)];
% 
% % NT3 unique word for FACCH1 (CBPSK)
% NT3_FACCH1_uniq = '11001001';
% NT3_FACCH1_pilot = [];
% for i=1:length(NT3_FACCH1_uniq)
%     if (NT3_FACCH1_uniq(i)=='0')
%         NT3_FACCH1_pilot = [NT3_FACCH1_pilot 1+j];
%     else (NT3_FACCH1_uniq(i)=='1')
%         NT3_FACCH1_pilot = [NT3_FACCH1_pilot -1-j];
%     end
% end
% NT3_FACCH1_inpilot = [zeros(1,26) NT3_FACCH1_pilot zeros(1,78)];
% 
% %% Timeslots per Burst = 6
% 
% % BCCH unique word
% BCCH_uniq = '0011110000001100111111111100111100';
% BCCH_pilot = [];
% for i=1:2:length(BCCH_uniq)
%     if (BCCH_uniq(i)=='0' && BCCH_uniq(i+1)=='0')
%         BCCH_pilot = [BCCH_pilot 1+1j];
%     elseif (BCCH_uniq(i)=='0' && BCCH_uniq(i+1)=='1')
%         BCCH_pilot = [BCCH_pilot -1+1j];
%     elseif (BCCH_uniq(i)=='1' && BCCH_uniq(i+1)=='0')
%         BCCH_pilot = [BCCH_pilot 1-1j];
%     elseif (BCCH_uniq(i)=='1' && BCCH_uniq(i+1)=='1')
%         BCCH_pilot = [BCCH_pilot -1-1j];
%     end
% end
% BCCH_inpilot = [zeros(1,26) BCCH_pilot(1:11) zeros(1,80) BCCH_pilot(12:14) zeros(1,75) BCCH_pilot(15:17) zeros(1,31)];
% 
% % NT6 User unique word
% NT6_user_uniq = '000000111100011000110110';
% NT6_user_pilot = [];
% for i=1:2:length(NT6_user_uniq)
%     if (NT6_user_uniq(i)=='0' && NT6_user_uniq(i+1)=='0')
%         NT6_user_pilot = [NT6_user_pilot 1+1j];
%     elseif (NT6_user_uniq(i)=='0' && NT6_user_uniq(i+1)=='1')
%         NT6_user_pilot = [NT6_user_pilot -1+1j];
%     elseif (NT6_user_uniq(i)=='1' && NT6_user_uniq(i+1)=='0')
%         NT6_user_pilot = [NT6_user_pilot 1-1j];
%     elseif (NT6_user_uniq(i)=='1' && NT6_user_uniq(i+1)=='1')
%         NT6_user_pilot = [NT6_user_pilot -1-1j];
%     end
% end
% NT6_user_inpilot = [zeros(1,26) NT6_user_pilot(1:6) zeros(1,85) NT6_user_pilot(7:9) zeros(1,75) NT6_user_pilot(10:12) zeros(1,31)];
% 
% % NT6 unique word(FACCH)
% NT6_FACCH_uniq = '001111101110000100111000';
% NT6_FACCH_pilot = [];
% for i=1:2:length(NT6_FACCH_uniq)
%     if (NT6_FACCH_uniq(i)=='0' && NT6_FACCH_uniq(i+1)=='0')
%         NT6_FACCH_pilot = [NT6_FACCH_pilot 1+1j];
%     elseif (NT6_FACCH_uniq(i)=='0' && NT6_FACCH_uniq(i+1)=='1')
%         NT6_FACCH_pilot = [NT6_FACCH_pilot -1+1j];
%     elseif (NT6_FACCH_uniq(i)=='1' && NT6_FACCH_uniq(i+1)=='0')
%         NT6_FACCH_pilot = [NT6_FACCH_pilot 1-1j];
%     elseif (NT6_FACCH_uniq(i)=='1' && NT6_FACCH_uniq(i+1)=='1')
%         NT6_FACCH_pilot = [NT6_FACCH_pilot -1-1j];
%     end
% end
% NT6_FACCH_inpilot = [zeros(1,26) NT6_FACCH_pilot(1:6) zeros(1,85) NT6_FACCH_pilot(7:9) zeros(1,75) NT6_FACCH_pilot(10:12) zeros(1,31)];
% 
% % DC6 unique word
% DC6_uniq = '00000011110011001000100101';
% DC6_pilot = [];
% for i=1:2:length(DC6_uniq)
%     if (DC6_uniq(i)=='0' && DC6_uniq(i+1)=='0')
%         DC6_pilot = [DC6_pilot 1+1j];
%     elseif (DC6_uniq(i)=='0' && DC6_uniq(i+1)=='1')
%         DC6_pilot = [DC6_pilot -1+1j];
%     elseif (DC6_uniq(i)=='1' && DC6_uniq(i+1)=='0')
%         DC6_pilot = [DC6_pilot 1-1j];
%     elseif (DC6_uniq(i)=='1' && DC6_uniq(i+1)=='1')
%         DC6_pilot = [DC6_pilot -1-1j];
%     end
% end
% DC6_inpilot = [zeros(1,26) DC6_pilot(1:7) zeros(1,84) DC6_pilot(8:10) zeros(1,75) DC6_pilot(11:13) zeros(1,31)];
% 
% % SDCCH (Type 0) (CBPSK)
% SDCCH0_uniq = '010101010101010101011';
% SDCCH0_pilot = [];
% for i=1:length(SDCCH0_uniq)
%     if (SDCCH0_uniq(i)=='0')
%         SDCCH0_pilot = [SDCCH0_pilot 1+j];
%     else (SDCCH0_uniq(i)=='1')
%         SDCCH0_pilot = [SDCCH0_pilot -1-j];
%     end
% end
% SDCCH0_inpilot = [zeros(1,26) SDCCH0_pilot(1:7) zeros(1,80) SDCCH0_pilot(8:14) zeros(1,75) SDCCH0_pilot(15:21) zeros(1,27)];
% 
% % SDCCH (Type 1) (CBPSK)
% SDCCH1_uniq = '001100110011001100111';
% SDCCH1_pilot = [];
% for i=1:length(SDCCH1_uniq)
%     if (SDCCH1_uniq(i)=='0')
%         SDCCH1_pilot = [SDCCH1_pilot 1+j];
%     else (SDCCH1_uniq(i)=='1')
%         SDCCH1_pilot = [SDCCH1_pilot -1-j];
%     end
% end
% SDCCH1_inpilot = [zeros(1,26) SDCCH1_pilot(1:7) zeros(1,80) SDCCH1_pilot(8:14) zeros(1,75) SDCCH1_pilot(15:21) zeros(1,27)];
% 
% % SDCCH (Type 2) (CBPSK)
% SDCCH2_uniq = '000011110000111100001';
% SDCCH2_pilot = [];
% for i=1:length(SDCCH2_uniq)
%     if (SDCCH2_uniq(i)=='0')
%         SDCCH2_pilot = [SDCCH2_pilot 1+j];
%     else (SDCCH2_uniq(i)=='1')
%         SDCCH2_pilot = [SDCCH2_pilot -1-j];
%     end
% end
% SDCCH2_inpilot = [zeros(1,26) SDCCH2_pilot(1:7) zeros(1,80) SDCCH2_pilot(8:14) zeros(1,75) SDCCH2_pilot(15:21) zeros(1,27)];
% 
% % SDCCH (Type 3) (CBPSK)
% SDCCH3_uniq = '011010010110100101101';
% SDCCH3_pilot = [];
% for i=1:length(SDCCH3_uniq)
%     if (SDCCH3_uniq(i)=='0')
%         SDCCH3_pilot = [SDCCH3_pilot 1+j];
%     else (SDCCH3_uniq(i)=='1')
%         SDCCH3_pilot = [SDCCH3_pilot -1-j];
%     end
% end
% SDCCH3_inpilot = [zeros(1,26) SDCCH3_pilot(1:7) zeros(1,80) SDCCH3_pilot(8:14) zeros(1,75) SDCCH3_pilot(15:21) zeros(1,27)];
% 
% %% Timeslots per Burst = 9
% 
% % NT9 User unique word
% NT9_user_uniq = '000000111100001100011000110110';
% NT9_user_pilot = [];
% for i=1:2:length(NT9_user_uniq)
%     if (NT9_user_uniq(i)=='0' && NT9_user_uniq(i+1)=='0')
%         NT9_user_pilot = [NT9_user_pilot 1+1j];
%     elseif (NT9_user_uniq(i)=='0' && NT9_user_uniq(i+1)=='1')
%         NT9_user_pilot = [NT9_user_pilot -1+1j];
%     elseif (NT9_user_uniq(i)=='1' && NT9_user_uniq(i+1)=='0')
%         NT9_user_pilot = [NT9_user_pilot 1-1j];
%     elseif (NT9_user_uniq(i)=='1' && NT9_user_uniq(i+1)=='1')
%         NT9_user_pilot = [NT9_user_pilot -1-1j];
%     end
% end
% NT9_user_inpilot = [zeros(1,26) NT9_user_pilot(1:6) zeros(1,85) NT9_user_pilot(7:9) zeros(1,75) NT9_user_pilot(10:12) zeros(1,75) NT9_user_pilot(13:15) zeros(1,70)];
% 
% % NT6 unique word(FACCH)
% NT9_FACCH_uniq = '001111101110011111000100111000';
% NT9_FACCH_pilot = [];
% for i=1:2:length(NT9_FACCH_uniq)
%     if (NT9_FACCH_uniq(i)=='0' && NT9_FACCH_uniq(i+1)=='0')
%         NT9_FACCH_pilot = [NT9_FACCH_pilot 1+1j];
%     elseif (NT9_FACCH_uniq(i)=='0' && NT9_FACCH_uniq(i+1)=='1')
%         NT9_FACCH_pilot = [NT9_FACCH_pilot -1+1j];
%     elseif (NT9_FACCH_uniq(i)=='1' && NT9_FACCH_uniq(i+1)=='0')
%         NT9_FACCH_pilot = [NT9_FACCH_pilot 1-1j];
%     elseif (NT9_FACCH_uniq(i)=='1' && NT9_FACCH_uniq(i+1)=='1')
%         NT9_FACCH_pilot = [NT9_FACCH_pilot -1-1j];
%     end
% end
% NT9_FACCH_inpilot = [zeros(1,26) NT9_FACCH_pilot(1:6) zeros(1,85) NT9_FACCH_pilot(7:9) zeros(1,75) NT9_FACCH_pilot(10:12) zeros(1,75) NT9_FACCH_pilot(13:15) zeros(1,70)];
% 
% % RACH unique word
% RACH_uniq = '0011110000001100111111111100111100';
% RACH_pilot = [];
% for i=1:2:length(RACH_uniq)
%     if (RACH_uniq(i)=='0' && RACH_uniq(i+1)=='0')
%         RACH_pilot = [RACH_pilot 1+1j];
%     elseif (RACH_uniq(i)=='0' && RACH_uniq(i+1)=='1')
%         RACH_pilot = [RACH_pilot -1+1j];
%     elseif (RACH_uniq(i)=='1' && RACH_uniq(i+1)=='0')
%         RACH_pilot = [RACH_pilot 1-1j];
%     elseif (RACH_uniq(i)=='1' && RACH_uniq(i+1)=='1')
%         RACH_pilot = [RACH_pilot -1-1j];
%     end
% end
% RACH_inpilot = [zeros(1,76) RACH_pilot zeros(1,160) RACH_pilot zeros(1,76)];
% 



end