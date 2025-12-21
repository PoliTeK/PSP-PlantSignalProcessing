%% Oscilloscopio e FFT per Daisy Seed (Direct Line-In)
% Richiede: Audio Toolbox
clear; clc; close all;

%% 1. SELEZIONE DISPOSITIVO DI INPUT
disp('------------------------------------------------');
disp('Ricerca dispositivi audio in corso...');
devInfo = audioDeviceReader;
devices = getAudioDevices(devInfo);

disp('Dispositivi di Input trovati:');
for i = 1:numel(devices)
    fprintf('[%d] %s\n', i, devices{i});
end
disp('------------------------------------------------');

% Chiede all'utente quale dispositivo usare
devIdx = input('Inserisci il NUMERO del dispositivo a cui hai collegato il Jack (es. Line-In / External Mic): ');

if isempty(devIdx) || devIdx < 1 || devIdx > numel(devices)
    error('Selezione non valida.');
end

selectedDevice = devices{devIdx};
fprintf('Hai selezionato: %s\n', selectedDevice);

%% 2. Configurazione Parametri
Fs = 48000;          % Daisy Seed lavora a 48kHz (importante matchare)
frameSize = 2048;    % Buffer size

try
    % Inizializza il lettore con il dispositivo scelto
    deviceReader = audioDeviceReader('Device', selectedDevice, ...
                                     'SampleRate', Fs, ...
                                     'SamplesPerFrame', frameSize);
    setup(deviceReader);
    disp('Acquisizione avviata. Chiudi la finestra del grafico per stoppare.');
catch ME
    warning('Impossibile usare 48kHz su questo dispositivo. Provo 44100Hz...');
    try
        Fs = 44100;
        deviceReader = audioDeviceReader('Device', selectedDevice, ...
                                         'SampleRate', Fs, ...
                                         'SamplesPerFrame', frameSize);
        setup(deviceReader);
    catch
        error(['Errore fatale: ' ME.message]);
    end
end

%% 3. Preparazione Grafica
fig = figure('Name', ['Analisi Input: ' selectedDevice], 'Color', 'w');

% -- Oscilloscopio --
subplot(2,1,1);
hTime = plot(zeros(frameSize, 1), 'Color', [0 0.6 0.8], 'LineWidth', 1.5);
grid on; axis tight;
ylim([-1.1 1.1]); % Daisy output range
title('Time Domain (Forma d''onda)');
xlabel('Campioni'); ylabel('Amplitude');

% -- FFT (Analizzatore di Spettro) --
subplot(2,1,2);
f_vec = Fs * (0:(frameSize/2)) / frameSize;
hFreq = plot(f_vec, zeros(length(f_vec), 1), 'Color', [0.8 0.2 0.2], 'LineWidth', 1.5);
grid on; 
xlim([0 5000]); % Zoom sulle frequenze utili (0-5kHz) per vedere bene la fondamentale
ylim([-120 0]);
title('Frequency Domain (FFT)');
xlabel('Frequenza (Hz)'); ylabel('dBFS');

%% 4. Loop Real-Time
win = blackmanharris(frameSize); % Finestra ottima per separare armoniche e rumore

while ishandle(fig)
    % Acquisizione
    audioData = deviceReader();
    if isempty(audioData), continue; end
    
    % Se stereo, prendiamo solo il primo canale (Left)
    if size(audioData, 2) > 1
        audioData = audioData(:, 1);
    end
    
    % Aggiorna Tempo
    set(hTime, 'YData', audioData);
    
    % Calcolo FFT
    signal_win = audioData .* win;
    Y = fft(signal_win);
    P2 = abs(Y / frameSize);
    P1 = P2(1:frameSize/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    
    % Conversione dB
    P1_dB = 20*log10(P1 + 1e-9); % 1e-9 evita log(0)
    
    % Aggiorna Frequenza
    set(hFreq, 'YData', P1_dB);
    
    drawnow limitrate;
end

release(deviceReader);
disp('Stop.');