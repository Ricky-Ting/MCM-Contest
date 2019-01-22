function num = carsmodeling2(cars,Nleft,Nright,T,dT,s0,a,S,ratio)
persistent point;
    for i=Nright-1:-1:1
        for j=1:1:i
            if(cars(j,4)<cars(j+1,4))
                tmp_v=cars(j,3);
                tmp_x=cars(j,4);
                cars(j,3)=cars(j+1,3);
                cars(j,4)=cars(j+1,4);
                cars(j+1,3)=tmp_v;
                cars(j+1,4)=tmp_x;
             end
         end
    end

    while T>0
        T=T-dT;
        for i=1:1:Nright
           if i==1   % the lead
               cars(i,4)=cars(i,4)+cars(i,3)*dT;
           else
               
              if (cars(i-1,4)-cars(i,4) <ratio*s0 ) && (cars(i-1,3)<cars(i,3)) && left_no_car(cars,Nleft,cars(i,4),ratio*s0) %超车
                  point=Nleft+1
                  for j=1:1:Nleft
                      if(cars(j,2)<cars(i,4))    % 找到待插入的点
                          point=j;
                          break;
                      end    
                  end
                  Nleft=Nleft+1;
                  for j=Nleft:-1:point+1
                      cars(j,1)=cars(j-1,1);
                      cars(j,2)=cars(j-1,2);
                  end
                  
                  cars(point,1)=cars(i,3);
                  cars(point,2)=cars(i,4);
                  
                  Nright=Nright-1;
                  
                  for j=i:1:Nright
                      cars(j,3)=cars(j+1,3);
                      cars(j,4)=cars(j+1,4);
                  end
                  
                  
                  
              else    %不超车
                 tmp_a=sign(cars(i-1,4)-cars(i,4)-s0) * a;
                 cars(i,3)=cars(i,3) + tmp_a * dT;
                 cars(i,4)=cars(i,4) + cars(i,3) * dT;
                 
              end
           end
        end
        
        for i=Nright-1:-1:1
            for j=1:1:i
                if(cars(j,4)<cars(j+1,4))
                    tmp_v=cars(j,3);
                    tmp_x=cars(j,4);
                    cars(j,3)=cars(j+1,3);
                    cars(j,4)=cars(j+1,4);
                    cars(j+1,3)=tmp_v;
                    cars(j+1,4)=tmp_x;
                end
            end
        end
        
        
        for i=1:1:Nleft
           if i==1   % the lead
               cars(i,2)=cars(i,2)+cars(i,1)*dT;
               %cars(i,2)
           else
               
% %               if (cars(i-1,2)-cars(i,2) <ratio*s0 ) && (cars(i-1,3)<cars(i,3)) && right_no_car(cars,Nleft,cars(i,2),ratio*s0) 
% %                   for j=1:1:Nleft
% %                       if(cars(j,2)<cars(i,4))    
% %                           point=j;
% %                           break;
% %                       end    
% %                   end
% %                   Nleft=Nleft+1;
% %                   for j=Nleft:-1:point+1
% %                       cars(j,1)=cars(j-1,1);
% %                       cars(j,2)=cars(j-1,2);
% %                   end
% %                   
% %                   cars(point,1)=cars(i,3);
% %                   cars(point,2)=cars(i,4);
% %                   
% %                   Nright=Nright-1;
% %                   
% %                   for j=i:1:Nright
% %                       cars(j,3)=cars(j+1,3);
% %                       cars(j,4)=cars(j+1,4);
% %                   end
% %         
% %                   
% %               else      %}
                 tmp_a=sign(cars(i-1,2)-cars(i,2)-s0) * a;
                 cars(i,1)=cars(i,1) + tmp_a * dT;
                 cars(i,2)=cars(i,2) + cars(i,1) * dT;
                 
%              end
           end
        end
        
        for i=Nleft-1:-1:1
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
    for i=1:1:Nleft
        if cars(i,2)>=S
            num=num+1;
        end
    end
    for i=1:1:Nright
        if cars(i,4)>=S
            num=num+1;
        end
    end
    
end
