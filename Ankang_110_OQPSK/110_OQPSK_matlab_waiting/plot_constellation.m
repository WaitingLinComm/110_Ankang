function [U] = plot_constellation(b_i_hat, b_q_hat)

figure;
scatter(b_i_hat,b_q_hat,'.');
title("Constellation");
end
% Plot dot order
% for i = 1:length(b_i_hat)
%     s = {' ',num2str(i)};
%     text(b_i_hat(i), b_q_hat(i), s);
% end