%%Data Acquirement
close all
clear
clc
n = 2496;  % # of samples x measurements
m = 3;     % # of measurements
Max1s_path = ["Datasets/Dataset2/Max1_1s.txt" "Datasets/Dataset2/Max2_1s.txt" "Datasets/Dataset2/Max3_1s.txt"];
Max5s_path = [ "Datasets/Dataset2/Max1_5s.txt" "Datasets/Dataset2/Max2_5s.txt" "Datasets/Dataset2/Max3_5s.txt"];
Min_path = ["Datasets/Dataset2/Min1_5s.txt" "Datasets/Dataset2/Min2_5s.txt" "Datasets/Dataset2/Min3_5s.txt"];
DSUp_path = ["Datasets/Dataset2/DiscreteSweepUp1.txt" "Datasets/Dataset2/DiscreteSweepUp2.txt" "Datasets/Dataset2/DiscreteSweepUp3.txt"];
DSDown_path = ["Datasets/Dataset2/DiscreteSweepDown1.txt" "Datasets/Dataset2/DiscreteSweepDown2.txt" "Datasets/Dataset2/DiscreteSweepDown3.txt"];
SSDown_path = ["Datasets/Dataset2/SemidisSweepDown1.txt" "Datasets/Dataset2/SemidisSweepDown2.txt" "Datasets/Dataset2/SemidisSweepDown3.txt"];
SSUp_path = ["Datasets/Dataset2/SemidisSweepUp1.txt" "Datasets/Dataset2/SemidisSweepUp2.txt" "Datasets/Dataset2/SemidisSweepUp3.txt"];

Max1s  = zeros(n,m);
Max5s  = zeros(n,m);
Min    = zeros(n,m);
DSUp   = zeros(n,m);
DSDown = zeros(n,m);
SSUp   = zeros(n,m);
SSDown = zeros(n,m);
temp   = zeros (n,m);
% matrix creation with padding
for i = 1 : 3
    temp1 = readmatrix(Max1s_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    Max1s(:,i) = temp(:,1);
    temp = zeros (n,m);
    
     temp1 = readmatrix(Max5s_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    Max5s(:,i) = temp(:,1);
    temp = zeros (n,m);

    temp1 = readmatrix(Min_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    Min(:,i) = temp(:,1);
    temp = zeros (n,m);

    temp1 = readmatrix(DSUp_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    DSUp(:,i) = temp(:,1);
    temp = zeros (n,m);

    temp1 = readmatrix(DSDown_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    DSDown(:,i) = temp(:,1);
    temp = zeros (n,m);

    temp1 = readmatrix(SSUp_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    SSUp(:,i) = temp(:,1);
    temp = zeros (n,m);

    temp1 = readmatrix(SSDown_path(i));
    temp (1:length(temp1), 1:3)=temp1;
    SSDown(:,i) = temp(:,1);
    temp = zeros (n,m);
end    
%removing useless firmaware data stabilization
for i = 1:n
    for j = 1:m
        if rem(Max1s(i,j),1) ~= 0
            Max1s(i,j) = 0;
        end
        if rem(Max5s(i,j),1) ~= 0
            Max5s(i,j) = 0;
        end
        if rem(Min(i,j),1) ~= 0
            Min(i,j) = 0;
        end
        if rem(DSUp(i,j),1) ~= 0
            DSUp(i,j) = 0;
        end
        if rem(DSDown(i,j),1) ~= 0
            DSDown(i,j) = 0;
        end
        if rem(SSUp(i,j),1) ~= 0
            SSUp(i,j) = 0;
        end
        if rem(SSDown(i,j),1) ~= 0
            SSDown(i,j) = 0;
        end
    end
end


Ts = 0.01;                  %t sample = 10 ms
fs = 1/Ts;
ts = Ts * (0:n-1);

% plot of every dataset
figure
for i = 1:3
    subplot (2,3,i)
    plot (ts, Max1s(:,i));
    title ("Max1s");
    
    subplot (2,3,i+3)
    plot (ts, Max5s(:,i));
    title ("Max5s");
end 

figure
for i = 1:3
    subplot (1,3,i)
    plot (ts, Min(:,i));
    title ("Min");  
end 

figure
for i = 1:3
   subplot (2,3,i)
    plot (ts, DSUp(:,i));
    title ("DSUp");
    
    subplot (2,3,i+3)
    plot (ts, SSUp(:,i));
    title ("SSUp"); 
end 

figure
for i = 1:3
   subplot (2,3,i)
    plot (ts, DSDown(:,i));
    title ("DSDown");
    
    subplot (2,3,i+3)
    plot (ts, SSDown(:,i));
    title ("SSDown"); 
end 
%% FIR Design
clc

fc = [1 2 3 4 5 6 7 8 9 10];             %various cutoff frequencies of the filter
N  = [4 8 16 32 64 128 256];             %various filter order

%every combination of fc and N will be a different filter with N(k) coeffs
%h is a 3D matrix containing:
% for every row (i) a different fc
% for every column (j) a different N
% for every "depth" a coefficient
% !!! padding will be necessary since every filter has a different # of
% coeffs
h = zeros (10,7,256); 

for i = 1 : 10
    for j = 1 : 7
        h(i,j,1:N(j)) = fir1(N(j)-1, fc(i)/(fs/2), 'low', hamming(N(j)));
    end 
end