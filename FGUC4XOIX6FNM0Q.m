clc
clear all
close all
%%
% Save the serial port name in comPort variable.

comPort = 'COM5';
%% 
% It creates a serial element calling the function "setupSerial"
[arduino, serialFlag] = setupSerial(comPort);

%%
% Time to create our plot window in order to visualize data collectoed 
% from serial port readings

h = figure(1);% hold on
set(h,'UserData',1);
button = uicontrol('Style','togglebutton','String','Stop',...
    'Position',[0 0 50 25], 'parent',h);

buf_len = 500;
limits = [-60 180];
idx = (linspace(0,500,buf_len))';
data_plot = zeros(buf_len,5);

% 
myAxes = axes('Xlim',[0 buf_len],'Ylim',limits);
grid on;

lw = 1.5;
l1 = line(idx,data_plot(:,1), 'LineWidth',lw ,'Color','r');
l2 = line(idx,data_plot(:,2), 'LineWidth',lw ,'Color','b');
l3 = line(idx,data_plot(:,3), 'LineWidth',lw ,'Color','g');
l4 = line(idx,data_plot(:,4), 'LineWidth',lw ,'Color','m');
l5 = line(idx,data_plot(:,5), 'LineWidth',lw ,'Color','k');
drawnow;
xlabel('Number of reads [-]');
ylabel('Position [deg]');

%%

% Define Serial mode: R = Read
mode = 'R';     % Read

% Define robot Kinematics
myRobot = robotKinematicsFcn();
qf = zeros(1,4);
figure(2)
myRobot.plot(qf); 

aux = 0;
t = [];
data = [];
tic;
while (get(button,'Value') == 0 )
    
    val = readValue(arduino,mode);
    val = [180-val(1) val(2)+20 180-val(3)-32 val(4)-66 val(5)];
    
    % Plot signals
    figure(1)
    data_plot = [data_plot(2:end,:); val];    
    set(l1,'Ydata',data_plot(:,1));
    set(l2,'Ydata',data_plot(:,2));
    set(l3,'Ydata',data_plot(:,3));
    set(l4,'Ydata',data_plot(:,4));
    set(l5,'Ydata',data_plot(:,5));
    drawnow;        

    t = [t; toc];
    data = [data; val];
    aux = aux+1;
    
    % Plot Forward Kinematics
    figure(2)
    qf = val(1:4)*(pi/180);
    myRobot.plot(qf); 
    
%     pause(0.01);
end

fclose(arduino);    % close port
delete(arduino);    % delete port   

% Replay
myRobot.plot(data(:,1:4)*(pi/180));


