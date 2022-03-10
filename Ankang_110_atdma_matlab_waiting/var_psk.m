function var = var_psk(cluster , pilot)
var = 0 ;
[h,w] = size(cluster) ;
temp = zeros(1, h) ;
for i = length(pilot)
    for j = 1 : h
        temp(j) = abs( pilot(i) - cluster( j , 1 ) - 1i * cluster( j , 2 ) )^2 ;
    end
    var = var + min(temp) ;
end

end