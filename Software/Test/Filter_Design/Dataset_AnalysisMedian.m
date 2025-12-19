%% Data Acquirement
close all
clear
clc

n = 2496;  % # of samples x measurements
m = 3;     % # of measurements

% --- CARICAMENTO DATI (Invariato) ---
Max1s_path = ["Datasets/Dataset2/Max1_1s.txt" "Datasets/Dataset2/Max2_1s.txt" "Datasets/Dataset2/Max3_1s.txt"];
Max5s_path = ["Datasets/Dataset2/Max1_5s.txt" "Datasets/Dataset2/Max2_5s.txt" "Datasets/Dataset2/Max3_5s.txt"];
Min_path   = ["Datasets/Dataset2/Min1_5s.txt" "Datasets/Dataset2/Min2_5s.txt" "Datasets/Dataset2/Min3_5s.txt"];
DSUp_path  = ["Datasets/Dataset2/DiscreteSweepUp1.txt" "Datasets/Dataset2/DiscreteSweepUp2.txt" "Datasets/Dataset2/DiscreteSweepUp3.txt"];
DSDown_path= ["Datasets/Dataset2/DiscreteSweepDown1.txt" "Datasets/Dataset2/DiscreteSweepDown2.txt" "Datasets/Dataset2/DiscreteSweepDown3.txt"];
SSDown_path= ["Datasets/Dataset2/SemidisSweepDown1.txt" "Datasets/Dataset2/SemidisSweepDown2.txt" "Datasets/Dataset2/SemidisSweepDown3.txt"];
SSUp_path  = ["Datasets/Dataset2/SemidisSweepUp1.txt" "Datasets/Dataset2/SemidisSweepUp2.txt" "Datasets/Dataset2/SemidisSweepUp3.txt"];

Max1s  = zeros(n,m); Max5s  = zeros(n,m); Min    = zeros(n,m);
DSUp   = zeros(n,m); DSDown = zeros(n,m); SSUp   = zeros(n,m); SSDown = zeros(n,m);
temp   = zeros(n,m);

% Matrix creation with padding
for i = 1 : 3
    temp1 = readmatrix(Max1s_path(i)); temp(1:length(temp1), 1:3) = temp1; Max1s(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(Max5s_path(i)); temp(1:length(temp1), 1:3) = temp1; Max5s(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(Min_path(i)); temp(1:length(temp1), 1:3) = temp1; Min(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(DSUp_path(i)); temp(1:length(temp1), 1:3) = temp1; DSUp(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(DSDown_path(i)); temp(1:length(temp1), 1:3) = temp1; DSDown(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(SSUp_path(i)); temp(1:length(temp1), 1:3) = temp1; SSUp(:,i) = temp(:,1); temp = zeros(n,m);
    temp1 = readmatrix(SSDown_path(i)); temp(1:length(temp1), 1:3) = temp1; SSDown(:,i) = temp(:,1); temp = zeros(n,m);
end    

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

Ts = 0.01;                  
fs = 1/Ts;
med_win = 5; % Finestra del filtro mediano

%% 1. Design Filtro BUTTERWORTH (Ordine 2, 4 Hz)
fc_but = 4;
Wn_but = fc_but / (fs/2);
[b_but, a_but] = butter(2, Wn_but, 'low');

%% 2. Design Filtro BESSEL (Ordine 2, 5 Hz)
fc_bes = 5;
Wo_bes = 2 * pi * fc_bes; % Freq angolare analogica
[b_bes_s, a_bes_s] = besself(2, Wo_bes); % Analogico
[b_bes, a_bes] = bilinear(b_bes_s, a_bes_s, fs); % Digitale

%% Elaborazione e Confronto (4 Strategie)
DUT = [Max1s, Max5s, Min, DSUp, DSDown, SSUp, SSDown];
num_signals = size(DUT, 2);

for k = 1:num_signals
    input = DUT(:,k);
    
    % --- Strategia A: Solo Butterworth 4Hz ---
    y_only_but = filter(b_but, a_but, input);
    
    % --- Strategia B: Solo Bessel 5Hz ---
    y_only_bes = filter(b_bes, a_bes, input);
    
    % --- Calcolo intermedio: Filtro Mediano ---
    % Nota: medfilt1 applica il filtro mediano monodimensionale
    y_median = medfilt1(input, med_win); 
    
    % --- Strategia C: Mediano + Butterworth 4Hz ---
    y_med_but = filter(b_but, a_but, y_median);
    
    % --- Strategia D: Mediano + Bessel 5Hz ---
    y_med_bes = filter(b_bes, a_bes, y_median);
    
    % --- PLOTTING ---
    f = figure(100 + k);
    f.Position = [100 100 1200 600]; % Allarga la finestra
    
    % Plot Input (Grigio e sottile per riferimento)
    plot(input, 'Color', [0.85 0.85 0.85], 'LineWidth', 1.5, 'DisplayName', 'Raw Input'); hold on;
    
    % Plot Strategia A (Solo Butter) - Tratteggiato Blu
    plot(y_only_but, '--b', 'LineWidth', 1.2, 'DisplayName', 'Solo Butterworth (4Hz)');
    
    % Plot Strategia B (Solo Bessel) - Tratteggiato Verde
    plot(y_only_bes, '--g', 'LineWidth', 1.2, 'DisplayName', 'Solo Bessel (5Hz)');
    
    % Plot Strategia C (Mediano + Butter) - Solido Magenta
    plot(y_med_but, 'm', 'LineWidth', 1.5, 'DisplayName', 'Mediano + Butterworth');
    
    % Plot Strategia D (Mediano + Bessel) - Solido Rosso (Spesso)
    plot(y_med_bes, 'r', 'LineWidth', 2.0, 'DisplayName', 'Mediano + Bessel');
    
    grid on;
    title(['Confronto 4 Strategie - Segnale #', num2str(k)], 'FontSize', 14);
    xlabel('Campioni');
    ylabel('Ampiezza');
    legend('show', 'Location', 'best', 'FontSize', 10);
    
    % Zoom automatico su una zona interessante se necessario, 
    % oppure lascia libero
    xlim([1, n]);
    
    hold off;
end