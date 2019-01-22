function [num] = carsmodeling(cars,N,T,dT,s0,a,S)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    for i=N-1:-1:1
        for j=1:1:i
            if(cars(j,2)<cars(j+1,2))
                tmp_v=cars(j,1);
                tmp_x=cars(j,2);
                cars(j,1)=cars(j+1,1);
                cars(j,2)=cars(j+1,2);
                cars(j+1,1)=tmp_v;
                cars(j+1,2)=tmp_x;
             end
         end
    end
    while T>0
        T=T-dT;
        for i=1:1:N
           if i==1   % the lead
               cars(i,2)=cars(i,2)+cars(i,1)*dT;
           else
               tmp_a=sign(cars(i-1,2)-cars(i,2)-s0) * a;
               cars(i,1)=cars(i,1) + tmp_a * dT;
               cars(i,2)=cars(i,2) + cars(i,1) * dT;
           end
        end
        
        for i=N-1:-1:1
            for j=1:1:i
                if(cars(j,2)<cars(j+1,2))
                    tmp_v=cars(j,1);
                    tmp_x=cars(j,2);
                    cars(j,1)=cars(j+1,1);
                    cars(j,2)=cars(j+1,2);
                    cars(j+1,1)=tmp_v;
                    cars(j+1,2)=tmp_x;
                end
            end
        end
    end
    
    num=0;
    for i=1:1:N
        if cars(i,2)>=S
            num=num+1;
        end
    end
end

