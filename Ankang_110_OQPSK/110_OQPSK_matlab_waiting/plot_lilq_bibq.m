function [] = plot_lilq_bibq(plotmod, b_n, l_hat, b_i_hat, b_q_hat)
%plot_lilq_bibq
%
%   Input variable: plotmod(1 or 2), b_n(a vector), l_hat(a vector), b_i_hat(a vector), b_q_hat(a vector)
%
%   plotmod = 1; % Plot received pilots only
%   plotmod = 2; % Plot all the received data, including received pilots 

if plotmod == 1
    %% Plot l_hat
    b_n_i = b_n(1:2:end);
    b_n_q = b_n(2:2:end);
    b_n_i_1 = find(b_n_i == 1);
    b_n_i_0 = find(b_n_i == 0);
    b_n_q_1 = find(b_n_q == 1);
    b_n_q_0 = find(b_n_q == 0);

    l_i_hat = l_hat(1:2:end);
    l_q_hat = l_hat(2:2:end);
    figure
    for i = 1: length(b_n_i_1)
        scatter(real(l_i_hat(b_n_i_1(i))),imag(l_i_hat(b_n_i_1(i))),'r.'); hold on
    end    
    for i = 1: length(b_n_i_0)
        scatter(real(l_i_hat(b_n_i_0(i))),imag(l_i_hat(b_n_i_0(i))), 'cyan.'); hold on
    end    
    for i = 1: length(b_n_q_1)
        scatter(real(l_q_hat(b_n_q_1(i))),imag(l_q_hat(b_n_q_1(i))),'b.'); hold on
    end   
    for i = 1: length(b_n_q_0)
        scatter(real(l_q_hat(b_n_q_0(i))),imag(l_q_hat(b_n_q_0(i))),'g.'); hold on
    end
    
    % Plot dot order
    title("$\hat{l}[n]$", 'Interpreter', 'latex');
    for i = 1:length(l_i_hat)
        s = {' ',num2str(2*i-2)}; % 使印出來的字不要與點黏在一起
        text(real(l_i_hat(i)), imag(l_i_hat(i)), s);
        %text(real(l_i_hat(i)), imag(l_i_hat(i)), num2str(i));    
    end
    for i = 1:length(l_q_hat)
        s = {' ',num2str(2*i-1)};
        text(real(l_q_hat(i)), imag(l_q_hat(i)), s);
        %text(real(l_q_hat(i)), imag(l_q_hat(i)), num2str(i));   
    end

    %% Plot b_i_hat, b_q_hat
    z = zeros(1, length(b_i_hat));
    figure
    for i = 1: length(b_n_i_1)
        scatter(b_i_hat(b_n_i_1(i)),z(b_n_i_1(i)),'r.'); hold on
    end  
    for i = 1: length(b_n_i_0)
        scatter(b_i_hat(b_n_i_0(i)),z(b_n_i_0(i)),'cyan.'); hold on
    end  
    for i = 1: length(b_n_q_1)
        scatter(z(b_n_q_1(i)), b_q_hat(b_n_q_1(i)), 'b.'); hold on
    end  
    for i = 1: length(b_n_q_0)
        scatter(z(b_n_q_0(i)), b_q_hat(b_n_q_0(i)),'g.'); hold on
    end
    title("$\hat{b_i}[n], \hat{b_q}[n]$", 'Interpreter', 'latex');
    xlabel("$\hat{b_i}[n]$", 'Interpreter', 'latex');
    ylabel("$\hat{b_q}[n]$", 'Interpreter', 'latex');
    
    %Plot dot order
    for i = 1:length(b_i_hat)
        s = {' ',num2str(2*i-2)};
        text(b_i_hat(i), z(i), s);
    end
    for i = 1:length(b_q_hat)
        s = {' ',num2str(2*i-1)};
        text(z(i), b_q_hat(i), s);
    end

elseif plotmod == 2
   %% Plot l_hat
    figure
    scatter(real(l_hat),imag(l_hat), '.'); 
    title("$\hat{l}[n]$", 'Interpreter', 'latex');
    %% Plot b_i_hat, b_q_hat  
    figure
    % case 1
    b_i_hat_positive = b_i_hat > 0;
    index1 = find(b_i_hat_positive ~= 0);
    scatter(b_i_hat(index1), zeros(length(index1), 1),'r.'); hold on
    
    % case 2
    b_i_hat_negative = b_i_hat < 0;
    index2 = find(b_i_hat_negative ~= 0);
    scatter(b_i_hat(index2), zeros(length(index2), 1),'cyan.'); hold on
    
    % case 3
    b_q_hat_positive = b_q_hat > 0;
    index3 = find(b_q_hat_positive ~= 0);
    scatter(zeros(length(index3), 1), b_q_hat(index3),'b.'); hold on
    
    % case 4
    b_q_hat_positive = b_q_hat < 0;
    index4 = find(b_q_hat_positive ~= 0);
    scatter(zeros(length(index4), 1), b_q_hat(index4),'g.'); hold on
    
    title("$\hat{b_i}[n], \hat{b_q}[n]$", 'Interpreter', 'latex');
    xlabel("$\hat{b_i}[n]$", 'Interpreter', 'latex');
    ylabel("$\hat{b_q}[n]$", 'Interpreter', 'latex');
end
end