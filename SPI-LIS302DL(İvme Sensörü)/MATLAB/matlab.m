clear all;
clc;
close all;
delete(instrfindall);
comport=serial('COM9','BaudRate',115200,'DataBits',8);
set(comport,'Parity','none');

fopen(comport);
x=1;

while(1)
    
    axis([0 400 -4500 4500]);
    
    x=x+1;
    if(x>400)
       f=400;
    end 
    if(x<401)
       f=x;
    end 
    y1(f)=fscanf(comport,'%d');
    y2(f)=fscanf(comport,'%d');
    y3(f)=fscanf(comport,'%d');
    
    plot(y1,'b','linewidth',1);
    grid on;    
    hold on;
    plot(y2,'r','linewidth',1);
    grid on;
    hold on;
    plot(y3,'y','linewidth',1);
    grid on;
    hold on;    
    legend('X Ekseni','Y Ekseni','Z Ekseni');
    drawnow;
    
    hold off;
    if(x>=400)
       loop=0;
       while(loop<399)
           loop=loop+1;
          y1(loop)=y1(loop+1);
          y2(loop)=y2(loop+1);
          y3(loop)=y3(loop+1);
       end

    end
end
    
    fclose(comport);
    delete(comport);