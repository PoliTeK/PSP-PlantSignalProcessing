
clear
clc
Max_path = ["DataSet2/Max1_1s.txt" "DataSet2/Max2_1s.txt" "DataSet/Max3_1s.txt" "DataSet2/Max1_5s.txt" "DataSet2/Max2_5s.txt" "DataSet2/Max3_5s.txt"];
Min_path = ["DataSet2/Min1_1s.txt" "DataSet2/Min2_1s.txt" "DataSet/Min3_1s.txt" "DataSet2/Min1_5s.txt" "DataSet2/Min2_5s.txt" "DataSet2/Min3_5s.txt"];
DSUp_path = ["Dataset2/DiscreteSweepUp1" "Dataset2/DiscreteSweepUp2" "Dataset2/DiscreteSweepUp3"];
DSDown_path = ["Dataset2/DiscreteSweepDown1" "Datset2/DiscreteSweepDown2" "Dataset2/DiscreteSweepDown3"];
SSDown_path = ["Dataset2/SemidisSweepDown1" "Dataset2/SemidisSweepDown2" "Dataset2/SemidisSweepDown3"];
SSUp_path = ["Dataset2/SemidisSweepUp1" "Dataset2/SemidisSweepUp2" "Dataset2/SemidisSweepUp3"];

Max_data = zeros(2500,18);
for i = 1:5
    fid = fopen(Max_path(i),'r');
    data = table2array(readtable(Max_path(i), 'Delimiter', ','));   % legge tutti i numeri come float
    Max_data(:,i) = data';
    fclose(fid);
end



% Number of samples
Ns = length(data);
% salmpling step length(10 ms = 0.01 s)
Ts = 0.01;
% Time Axis
t = (0:Ns-1) * Ts;
%%
%Plot raw data and uC filtered data
plot(t,data(1,:), "g", t, data(2,:), "r", t, data(3,:), "b");

xlabel('Time [s]');
ylabel('Value');
title('Datas with {\tau_s} = 10 ms');
grid on;
%%
%calculate optimal cutoff frequency
Der = zeros(Ns,1)
for i = 1: Ns-1
    Der(i) = data(1,i+1)- data(1,i);
end
plot(t,data(1,:), "g", t, Der, "r");

