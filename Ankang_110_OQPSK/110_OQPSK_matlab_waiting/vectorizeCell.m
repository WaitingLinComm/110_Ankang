function [vec_data] = vectorizeCell(cell_data)
%vectorizeCell
%     vectorize data from cell
% 
%     Input variable: cell_data
%     Output variable: vec_data(a vector)
%

vec_data = [];
for i = 1: length(cell_data)
    vec_data = [vec_data cell_data{i}];
end
end