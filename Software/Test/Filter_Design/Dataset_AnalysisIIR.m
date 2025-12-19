%% Data Acquirement
close all
clear
clc

n = 2496;  % # of samples x measurements
m = 3;     % # of measurements

Max1s_path = ["Datasets/Dataset2/Max1_1s.txt" "Datasets/Dataset2/Max2_1s.txt" "Datasets/Dataset2/Max3_1s.txt"];
Max5s_path = ["Datasets/Dataset2/Max1_5s.txt" "Datasets/Dataset2/Max2_5s.txt" "Datasets/Dataset2/Max3_5s.txt"];
Min_path   = ["Datasets/Dataset2/Min1_5s.txt" "Datasets/Dataset2/Min2_5s.txt" "Datasets/Dataset2/Min3_5s.txt"];
DSUp_path  = ["Datasets/Dataset2/DiscreteSweepUp1.txt" "Datasets/Dataset2/DiscreteSweepUp2.txt" "Datasets/Dataset2/DiscreteSweepUp3.txt"];
DSDown_path= ["Datasets/Dataset2/DiscreteSweepDown1.txt" "Datasets/Dataset2/DiscreteSweepDown2.txt" "Datasets/Dataset2/DiscreteSweepDown3.txt"];
SSDown_path= ["Datasets/Dataset2/SemidisSweepDown1.txt" "Datasets/Dataset2/SemidisSweepDown2.txt" "Datasets/Dataset2/SemidisSweepDown3.txt"];
SSUp_path  = ["Datasets/Dataset2/SemidisSweepUp1.txt" "Datasets/Dataset2/SemidisSweepUp2.txt" "Datasets/Dataset2/SemidisSweepUp3.txt"];

Max1s  = zeros(n,m);
Max5s  = zeros(n,m);
Min    = zeros(n,m);
DSUp   = zeros(n,m);
DSDown = zeros(n,m);
SSUp   = zeros(n,m);
SSDown = zeros(n,m);
temp   = zeros(n,m);

% Matrix creation with padding
for i = 1 : 3
    temp1 = readmatrix(Max1s_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    Max1s(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(Max5s_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    Max5s(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(Min_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    Min(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(DSUp_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    DSUp(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(DSDown_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    DSDown(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(SSUp_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    SSUp(:,i) = temp(:,1);
    temp = zeros(n,m);
    
    temp1 = readmatrix(SSDown_path(i));
    temp(1:length(temp1), 1:3) = temp1;
    SSDown(:,i) = temp(:,1);
    temp = zeros(n,m);
end    

% Removing useless firmware data stabilization
DUT_Raw = {Max1s, Max5s, Min, DSUp, DSDown, SSUp, SSDown};
for k = 1:length(DUT_Raw)
    currMat = DUT_Raw{k};
    for i = 1:n
        for j = 1:m
            if rem(currMat(i,j),1) ~= 0
                currMat(i,j) = 0;
            end
        end
    end
    DUT_Raw{k} = currMat;
end
Max1s = DUT_Raw{1}; Max5s = DUT_Raw{2}; Min = DUT_Raw{3};
DSUp = DUT_Raw{4}; DSDown = DUT_Raw{5}; SSUp = DUT_Raw{6}; SSDown = DUT_Raw{7};

Ts = 0.01;                  % t sample = 10 ms
fs = 1/Ts;
ts = Ts * (0:n-1);

%% Preparazione Coefficienti Filtri (Ordini 2, 4, 6, 8)
fc_val = 5; 
freq_norm = fc_val / (fs/2);

% Ordine 2 (NUOVO)
[b2, a2] = butter(2, freq_norm, 'low');
% Ordine 4
[b4, a4] = butter(4, freq_norm, 'low');
% Ordine 6
[b6, a6] = butter(6, freq_norm, 'low');
% Ordine 8
[b8, a8] = butter(8, freq_norm, 'low');

%% Elaborazione e Plotting (21 Grafici totali)
DUT = [Max1s, Max5s, Min, DSUp, DSDown, SSUp, SSDown];
num_signals = size(DUT, 2);

% Preallocazione vettori output
output2 = zeros(n,1);
output4 = zeros(n,1);
output6 = zeros(n,1);
output8 = zeros(n,1);

for k = 1:num_signals
    input = DUT(:,k);
    
    % --- APPLICAZIONE FILTRO ORDINE 2 ---
    state_buff = zeros(2,1); % Buffer dimensione 2
    for i = 1 : n
        % Direct Form II
        x1 = input(i) - (a2(2)*state_buff(1) + a2(3)*state_buff(2));
        output2(i)   = b2(1)*x1 + b2(2)*state_buff(1) + b2(3)*state_buff(2);
        state_buff(2) = state_buff(1);
        state_buff(1) = x1;
    end
    
    % --- APPLICAZIONE FILTRO ORDINE 4 ---
    state_buff = zeros(4,1);
    for i = 1 : n
        x1 = input(i) - (a4(2)*state_buff(1) + a4(3)*state_buff(2) + a4(4)*state_buff(3) + a4(5)*state_buff(4));
        output4(i)   = b4(1)*x1 + b4(2)*state_buff(1) + b4(3)*state_buff(2) + b4(4)*state_buff(3) + b4(5)*state_buff(4);
        state_buff(2:4) = state_buff(1:3);
        state_buff(1) = x1;
    end
    
    % --- APPLICAZIONE FILTRO ORDINE 6 ---
    state_buff = zeros(6,1);
    for i = 1 : n
        x1 = input(i) - (a6(2)*state_buff(1) + a6(3)*state_buff(2) + a6(4)*state_buff(3) + ...
                         a6(5)*state_buff(4) + a6(6)*state_buff(5) + a6(7)*state_buff(6));
        output6(i)   = b6(1)*x1 + b6(2)*state_buff(1) + b6(3)*state_buff(2) + b6(4)*state_buff(3) + ...
                         b6(5)*state_buff(4) + b6(6)*state_buff(5) + b6(7)*state_buff(6);
        state_buff(2:6) = state_buff(1:5);
        state_buff(1) = x1;
    end
    
    % --- APPLICAZIONE FILTRO ORDINE 8 ---
    state_buff = zeros(8,1);
    for i = 1 : n
        x1 = input(i) - (a8(2)*state_buff(1) + a8(3)*state_buff(2) + a8(4)*state_buff(3) + a8(5)*state_buff(4) + ...
                         a8(6)*state_buff(5) + a8(7)*state_buff(6) + a8(8)*state_buff(7) + a8(9)*state_buff(8));
        output8(i)   = b8(1)*x1 + b8(2)*state_buff(1) + b8(3)*state_buff(2) + b8(4)*state_buff(3) + b8(5)*state_buff(4) + ...
                         b8(6)*state_buff(5) + b8(7)*state_buff(6) + b8(8)*state_buff(7) + b8(9)*state_buff(8);
        state_buff(2:8) = state_buff(1:7);
        state_buff(1) = x1;
    end
    
    % --- PLOTTING ---
    figure(100 + k); 
    
    % Originale
    plot(input, 'Color', [0.7 0.7 0.7], 'LineWidth', 1.5, 'DisplayName', 'Originale'); hold on;
    
    % Ordine 2 (Magenta)
    plot(output2, 'm', 'LineWidth', 1.2, 'DisplayName', 'IIR Ordine 2');
    
    % Ordine 4 (Rosso)
    plot(output4, 'r', 'LineWidth', 1.2, 'DisplayName', 'IIR Ordine 4');
    
    % Ordine 6 (Verde)
    plot(output6, 'g', 'LineWidth', 1.2, 'DisplayName', 'IIR Ordine 6');
    
    % Ordine 8 (Blu)
    plot(output8, 'b', 'LineWidth', 1.2, 'DisplayName', 'IIR Ordine 8');
    
    grid on;
    title(['Confronto Filtri (Ord 2,4,6,8) - Segnale #', num2str(k)]);
    xlabel('Campioni');
    ylabel('Ampiezza');
    legend('show', 'Location', 'best');
    hold off;
end