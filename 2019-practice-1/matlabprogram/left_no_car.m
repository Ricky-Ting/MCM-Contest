function z = left_no_car(cars,Nleft,current,length)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
z=1

    for i=1:1:Nleft
        if( abs(cars(i,2)-current) <= length )
            z=0;
            break;
        end
    end




end

