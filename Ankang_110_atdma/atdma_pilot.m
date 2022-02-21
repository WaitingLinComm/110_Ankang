function y = atdma_pilot( mode_type , mode_index)
%% parameter setting 
% output y is the pilot symbol sequence stored in an array
% mode_type , mode_index ranges follows V2.xlsx
%% initialize the pilot sequence using mode_index and mode_type
P1 = '' ;%pilot sequence in hex form 
if mode_type == "atdma_qpsk"
    if mode_index == 1
        P1= '30CF300FF03CCC03030CFC00C0CCFCFFC';
    elseif mode_index ==2
        P1= '30CF300FF03CCC03030CFC00C0CCFC';
    elseif mode_index ==3
        P1= '33FC0F0FC03003C0C3CFFF3C3303FCCFF';
    elseif mode_index ==4
        P1= '33FC0F0FC03003C0C3CFFF3C3303FC';
    elseif mode_index ==5
        P1= '33FC0F0FC03003C0C3CFFF3C3303FCCFF300CFF033C3030C30C30F30FF333CF3CF3FC3F0';
    elseif mode_index ==6
        P1= '30CF300FF03CCC03030CFC00C0CCFCFFC3F330F00CFF0C3CFCF0C3CFC';
    elseif mode_index ==7
        P1= '30CF300FF03CCC03030CFC00C0CCFCFFC3F330F00CFF0C3CFCF0C3CFCFC33CCCCF00C0F0300';
    elseif mode_index ==8
        P1= '33FC0F0FC03003C0C3CFFF3C3303FCCFF300CFF033C3030C30C30F30FF333CF3CF3FC3F0C00F3C';
    elseif mode_index ==9% 1 2 6 7 
        P1='30CF300FF03CCC03030CFC00C0CCFC';
    elseif mode_index ==10% 3 4 5 8
        P1='33FC0F0FC03003C0C3CFFF3C3303FC';
    else
        disp("invalid mode_index in aTDMA_QPSK")
    end   
    
elseif mode_type == "atdma_8psk"
    if mode_index == 1
        P1= '186186D86C00030DB0006D80C360001800300361B0C06000';
    elseif mode_index ==2
        P1= '186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180C06036180DB61861B0D80';
    elseif mode_index ==3
        P1= '186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6D861B6C36';
    elseif mode_index ==4
        P1= '186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6D86';
    elseif mode_index ==5
        P1= '186186D86C00030DB0006D80C360001800300361B0C06000C00186C30D80DB6';
    elseif mode_index ==6 
        P1= '186186D86C00030DB0006D80C360001800300361B0000';
    elseif mode_index ==7
        P1= '186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180C06';
    elseif mode_index ==8
        P1= '186DB0036036C00180006C00C06C36DB61B0186006DB0C36D86000C36D80186C06006030180000';
    elseif mode_index ==9   % 2 7 8
        P1= '186DB0036036C00180006C00C06C36DB61B0186006';     
    elseif mode_index ==10 % 1 3 4 5 6 
        P1= '186186D86C00030DB0006D80C360001800300361B0';     
    else
        disp("invalid mode_index in aTDMA_8PSK")
    end 

else
    disp("invalid mode_type in aTDMA")

end
%% map the pilot sequence to binary sequence 
p1 = 0 ;
p11 = 0 ;
p111 = [] ;
for i=1:length(P1)
    p1=hex2dec(P1(i));% change pilot sequence from hex to dec
    p11=dec2bin(p1,4);% change pilot sequence from dec to binary
    p111=[p111 p11];
end
% p111
%% map these binary sequence to symbol
pilot = [] ;
if mode_type == "atdma_qpsk"
    for i=1:2:length(p111)
        if (p111(i)=='0' && p111(i+1)=='0')
            pilot=[pilot 1+j];
        elseif (p111(i)=='0' && p111(i+1)=='1')
            pilot=[pilot -1+j];            
        elseif (p111(i)=='1' && p111(i+1)=='0')
            pilot=[pilot 1-j];
        elseif (p111(i)=='1' && p111(i+1)=='1')
            pilot=[pilot -1-j];
        end
    end
    
    
elseif mode_type == "atdma_8psk"
    for i=1:3:length(p111)
%         %% BPSK pilot
%         if (p111(i)=='0' )
%             pilot=[pilot 1];
%         elseif (p111(i)=='1' )
%             pilot=[pilot -1];        
%         end
        %% QPSK pilot
%         if (p111(i)=='0' && p111(i+1)=='0')
%             pilot=[pilot 1+j];
%         elseif (p111(i)=='0' && p111(i+1)=='1')
%             pilot=[pilot -1+j];            
%         elseif (p111(i)=='1' && p111(i+1)=='0')
%             pilot=[pilot 1-j];
%         elseif (p111(i)=='1' && p111(i+1)=='1')
%             pilot=[pilot -1-j];
%         end
        %% 8PSK A type
        if (p111(i)=='0' && p111(i+1)=='0' &&  p111(i+2)=='0')% 000 -> 0
            pilot=[pilot 1];
        elseif (p111(i)=='0' && p111(i+1)=='0' &&  p111(i+2)=='1')% 001 -> 1
            pilot=[pilot (1+1j)/sqrt(2)];            
        elseif (p111(i)=='0' && p111(i+1)=='1' &&  p111(i+2)=='0')% 010 -> 2
            pilot=[pilot (1-1j)/sqrt(2)];                 
        elseif (p111(i)=='0' && p111(i+1)=='1' &&  p111(i+2)=='1')% 011 -> 3
            pilot=[pilot -1j];                 
        elseif (p111(i)=='1' && p111(i+1)=='0' &&  p111(i+2)=='0')% 100 -> 4
            pilot=[pilot (-1+1j)/sqrt(2)];                 
        elseif (p111(i)=='1' && p111(i+1)=='0' &&  p111(i+2)=='1')% 101 -> 5
            pilot=[pilot 1j];                 
        elseif (p111(i)=='1' && p111(i+1)=='1' &&  p111(i+2)=='0')% 110 -> 6
            pilot=[pilot -1];                 
        elseif (p111(i)=='1' && p111(i+1)=='1' &&  p111(i+2)=='1')% 111 -> 7
            pilot=[pilot (-1-1j)/sqrt(2)];                 
        end
        %% 8PSK B type
%         if (p111(i)=='0' && p111(i+1)=='0' &&  p111(i+2)=='0')% 000 -> 0
%             pilot=[pilot 1];
%         elseif (p111(i)=='0' && p111(i+1)=='0' &&  p111(i+2)=='1')% 001 -> 1
%             pilot=[pilot (1+1j)/sqrt(2)];            
%         elseif (p111(i)=='0' && p111(i+1)=='1' &&  p111(i+2)=='0')% 010 -> 2
%             pilot=[pilot (-1+1j)/sqrt(2)];                 
%         elseif (p111(i)=='0' && p111(i+1)=='1' &&  p111(i+2)=='1')% 011 -> 3
%             pilot=[pilot 1j];                 
%         elseif (p111(i)=='1' && p111(i+1)=='0' &&  p111(i+2)=='0')% 100 -> 4
%             pilot=[pilot (1-1j)/sqrt(2)];                 
%         elseif (p111(i)=='1' && p111(i+1)=='0' &&  p111(i+2)=='1')% 101 -> 5
%             pilot=[pilot -1j];                 
%         elseif (p111(i)=='1' && p111(i+1)=='1' &&  p111(i+2)=='0')% 110 -> 6
%             pilot=[pilot -1];                 
%         elseif (p111(i)=='1' && p111(i+1)=='1' &&  p111(i+2)=='1')% 111 -> 7
%             pilot=[pilot (-1-1j)/sqrt(2)];                 
%         end        
     end
%     
%     
else
    disp("invalid mode_type in aTDMA")
    
end


y = pilot ;

end