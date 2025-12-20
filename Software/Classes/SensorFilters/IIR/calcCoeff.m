%% Codice Matlab per il calcolo dei coefficienti di un IIR (Bessel & Butterworth)
clc
clear

fs = 100;                                               % frequenza di campionamento effettiva
fc_bes = 5;                                             % frequenza di taglio desiderata Bessel
fc_but = 4;                                             % frequenza di taglio desiderata Butterworth
N = 2;                                                  % ordine del filtro

% --- CALCOLO BUTTERWORTH ---
Wn_but = fc_but / (fs/2);                               % Frequenza normalizzata
[b_but, a_but] = butter(N, Wn_but, 'low');              % Calcolo Digitale

% --- CALCOLO BESSEL ---
Wo_bes = 2 * pi * fc_bes;                               % Freq angolare analogica
[b_bes_s, a_bes_s] = besself(N, Wo_bes);                % Calcolo Analogico
[b_bes, a_bes] = bilinear(b_bes_s, a_bes_s, fs);        % Conversione Digitale

% --- ESPORTAZIONE IN FORMATO C++ ---

fprintf('/* ===========================================================\n');
fprintf(' * FILTER COEFFICIENTS EXPORT\n');
fprintf(' * Fs = %d Hz\n', fs);
fprintf(' * =========================================================== */\n\n');

% 1. Stampa Butterworth
fprintf('// Butterworth Low-Pass (Order: %d, Fc: %d Hz)\n', N, fc_but);
fprintf('// b = numerator, a = denominator\n');

fprintf('float butter_b[%d] = {', length(b_but));
fprintf('%.8ff, ', b_but(1:end-1));
fprintf('%.8ff};\n', b_but(end));

fprintf('float butter_a[%d] = {', length(a_but));
fprintf('%.8ff, ', a_but(1:end-1));
fprintf('%.8ff};\n', a_but(end));

fprintf('\n');

% 2. Stampa Bessel
fprintf('// Bessel Low-Pass (Order: %d, Fc: %d Hz)\n', N, fc_bes);
fprintf('// b = numerator, a = denominator\n');

fprintf('float bessel_b[%d] = {', length(b_bes));
fprintf('%.8ff, ', b_bes(1:end-1));
fprintf('%.8ff};\n', b_bes(end));

fprintf('float bessel_a[%d] = {', length(a_bes));
fprintf('%.8ff, ', a_bes(1:end-1));
fprintf('%.8ff};\n', a_bes(end));

fprintf('\n// Ricorda: nell''implementazione IIR Direct Form II,\n');
fprintf('// spesso a[0] è 1.0 e i coefficienti a[1]...a[n] vanno sottratti.\n');