function [decoded_symbol , decoded_bit_seq]  = psk8_demod( symbol_seq , const_ind )
%% input and output
% input
% symbol sequence is an 1-D array, ith element is ith symbol
% constellation_ind is the index of the 8-psk mapping. 
% There are 2 kinds of mapping with idex 1 and 2
% 1 for A type, or 101 for 1j. 2 for B type, or 011 for 1j.
% output
% the decoded_symbol is an array. ith element is the decoded symbol.
% the decoded_bit_seq is an array. ith element is the decoded bit sequence.

% This function uses minimum distance to demodulation the constellation set
decoded_symbol = zeros( 1 , length(symbol_sequence) ) ;
decoded_bit_seq = cell( 1 , length(symbol_sequence) ) ;


% comfirm constellatioon type
if const_ind == 1
    % start decode the sequence
    for i = 1 : length(symbol_sequence)
        if  -7*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -5*pi/8
            decoded_symbol(i) = ( -1 -1j)/ sqrt(2);
            decoded_bit_seq{i} = '111' ;
            
        elseif -5*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -3*pi/8      
            decoded_symbol(i) = -1j;
            decoded_bit_seq{i} = '011' ;            
            
        elseif -3*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -pi/8       
            decoded_symbol(i) = ( 1 -1j)/ sqrt(2);
            decoded_bit_seq{i} = '010' ;                      
            
        elseif -pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < pi/8       
            decoded_symbol(i) = 1;
            decoded_bit_seq{i} = '000' ;                    
            
        elseif pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 3*pi/8       
            decoded_symbol(i) = ( 1 + 1j)/ sqrt(2);
            decoded_bit_seq{i} = '001' ;                 
            
        elseif 3*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 5*pi/8            
            decoded_symbol(i) = 1j;
            decoded_bit_seq{i} = '101' ;                  
            
        elseif 5*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 7*pi/8
            decoded_symbol(i) = ( -1 + 1j)/ sqrt(2);
            decoded_bit_seq{i} = '100' ;                    
            
        else
            decoded_symbol(i) = -1;
            decoded_bit_seq{i} = '110' ;  
        end
    
    
    end
    
% comfirm constellatioon type    
elseif const_ind == 2
    % start decode the sequence
    for i = 1 : length(symbol_sequence)
        if  -7*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -5*pi/8
            decoded_symbol(i) = ( -1 -1j)/ sqrt(2);
            decoded_bit_seq{i} = '111' ;
            
        elseif -5*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -3*pi/8      
            decoded_symbol(i) = -1j;
            decoded_bit_seq{i} = '101' ;            
            
        elseif -3*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < -pi/8       
            decoded_symbol(i) = ( 1 -1j)/ sqrt(2);
            decoded_bit_seq{i} = '100' ;                      
            
        elseif -pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < pi/8       
            decoded_symbol(i) = 1;
            decoded_bit_seq{i} = '000' ;                    
            
        elseif pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 3*pi/8       
            decoded_symbol(i) = ( 1 + 1j)/ sqrt(2);
            decoded_bit_seq{i} = '001' ;                 
            
        elseif 3*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 5*pi/8            
            decoded_symbol(i) = 1j;
            decoded_bit_seq{i} = '011' ;                  
            
        elseif 5*pi/8 <= angle( symbol_sequence(i) ) && angle( symbol_sequence(i) ) < 7*pi/8
            decoded_symbol(i) = ( -1 + 1j)/ sqrt(2);
            decoded_bit_seq{i} = '010' ;                    
            
        else
            decoded_symbol(i) = -1;
            decoded_bit_seq{i} = '110' ;  
        end    
    
    
    
    end    
   
end










end
