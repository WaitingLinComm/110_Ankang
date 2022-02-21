function y = convert_to_index(type,data)
y = zeros(1,length(data(:,1)));

if type == "qpsk"
    for i  = 1: length(y)
        if -pi < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= -pi/2
            y(i)=3;
        elseif -pi/2 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= 0
            y(i)=2;
        elseif 0 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi/2
            y(i)=0;
        elseif pi/2 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi
            y(i)=1;
        end
        
    end

elseif type== "8psk"
    for i  = 1: length(y)
        if -pi*7/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= -pi*5/8
            y(i)=7;
        elseif -pi*5/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= -pi*3/8
            y(i)=3;
        elseif -pi*3/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= -pi/8
            y(i)=2;
        elseif -pi/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi/8
            y(i)=0;
        elseif pi/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi*3/8
            y(i)=1;
        elseif pi*3/pi < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi*5/8
            y(i)=5;
        elseif pi*5/8 < angle(data(i,1)+1j*data(i,2)) &&  angle(data(i,1)+1j*data(i,2)) <= pi*7/8
            y(i)=4;
        else
            y(i)=6;
        end
        
    end
        
        
end
    
end