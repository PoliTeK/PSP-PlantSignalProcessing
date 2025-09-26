
clear
clc



fid = fopen('frequenze.txt','r');
data = table2array(readtable('frequenze.txt', 'Delimiter', ','));   % legge tutti i numeri come float
data = data';
fclose(fid);
%%

% Numero di campioni
N = length(data);

% Passo di campionamento (10 ms = 0.01 s)

Ts = 0.01;

% Asse dei tempi
t = (0:N-1) * Ts;

% Plot con scala temporale
plot(t, data(2,:), t, data(3,:));


xlabel('Tempo [s]');
ylabel('Valore');
title('Segnale campionato ogni 10 ms');
grid on;
