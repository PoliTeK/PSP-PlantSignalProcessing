% --- CONFIGURAZIONE ---
port_name = "COM3"; % Aggiorna con la tua porta
baud_rate = 115200;

if exist('s', 'var')
    clear s;
end
s = serialport(port_name, baud_rate);
configureTerminator(s, "CR/LF");

% --- SETUP GRAFICO ---
fig = figure('Name', 'DSP Multitasking Profiler', 'NumberTitle', 'off', 'Position', [100, 100, 1000, 800]);

% Utilizzo di tiledlayout per incollonnare i 4 grafici in modo pulito
t = tiledlayout(4, 1, 'TileSpacing', 'compact', 'Padding', 'compact');
title(t, 'Analisi Tempi di Esecuzione Task');

% Grafico 1: Audio
ax1 = nexttile; hold(ax1, 'on'); grid(ax1, 'on'); ylabel(ax1, 'Audio (\mus)');
line_audio = animatedline(ax1, 'Color', 'r', 'LineWidth', 1.5);

% Grafico 2: Encoder
ax2 = nexttile; hold(ax2, 'on'); grid(ax2, 'on'); ylabel(ax2, 'Encoder (\mus)');
line_enc = animatedline(ax2, 'Color', 'g', 'LineWidth', 1.5);

% Grafico 3: Logica DSP
ax3 = nexttile; hold(ax3, 'on'); grid(ax3, 'on'); ylabel(ax3, 'Logica (\mus)');
line_logic = animatedline(ax3, 'Color', 'b', 'LineWidth', 1.5);

% Grafico 4: Display
ax4 = nexttile; hold(ax4, 'on'); grid(ax4, 'on'); ylabel(ax4, 'Display (\mus)');
xlabel(ax4, 'Campioni (~16.6 ms/campione)');
line_disp = animatedline(ax4, 'Color', 'k', 'LineWidth', 1.5);

% Variabili di controllo
window_size = 500; % Mostra gli ultimi 500 campioni
sample_count = 0;

% Buffer circolari per il calcolo statistico della finestra corrente
buf_audio = nan(window_size, 1);
buf_enc   = nan(window_size, 1);
buf_logic = nan(window_size, 1);
buf_disp  = nan(window_size, 1);

flush(s);

% --- LOOP DI ACQUISIZIONE ---
while ishandle(fig)
    if s.NumBytesAvailable > 0
        raw_str = readline(s);
        dati_numerici = sscanf(raw_str, '%f,%f,%f,%f');
        
        if length(dati_numerici) == 4
            % Conversione in microsecondi
            t_audio = dati_numerici(1) / 480;
            t_enc   = dati_numerici(2) / 480;
            t_logic = dati_numerici(3) / 480;
            t_disp  = dati_numerici(4) / 480;
            
            sample_count = sample_count + 1;
            
            % Aggiornamento delle linee nei rispettivi assi
            addpoints(line_audio, sample_count, t_audio);
            addpoints(line_enc,   sample_count, t_enc);
            addpoints(line_logic, sample_count, t_logic);
            addpoints(line_disp,  sample_count, t_disp);
            
            % Aggiornamento dei buffer per le statistiche
            idx = mod(sample_count - 1, window_size) + 1;
            buf_audio(idx) = t_audio;
            buf_enc(idx)   = t_enc;
            buf_logic(idx) = t_logic;
            buf_disp(idx)  = t_disp;
            
            % Scorrimento sincronizzato dell'asse X per tutti i grafici
            if sample_count > window_size
                x_lims = [sample_count - window_size, sample_count];
            else
                x_lims = [0, window_size];
            end
            ax1.XLim = x_lims; ax2.XLim = x_lims; 
            ax3.XLim = x_lims; ax4.XLim = x_lims;
            
            % Aggiornamento dei titoli con i dati calcolati ogni 10 campioni
            if mod(sample_count, 10) == 0
                % Dati Audio
                v_aud = buf_audio(~isnan(buf_audio));
                title(ax1, sprintf('AudioCallback | Tipico: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_aud), min(v_aud), max(v_aud)));
                
                % Dati Encoder
                v_enc = buf_enc(~isnan(buf_enc));
                title(ax2, sprintf('Encoder | Tipico: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_enc), min(v_enc), max(v_enc)));
                
                % Dati Logic
                v_log = buf_logic(~isnan(buf_logic));
                title(ax3, sprintf('DSP Logic | Tipico: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_log), min(v_log), max(v_log)));
                
                % Dati Display
                v_disp = buf_disp(~isnan(buf_disp));
                title(ax4, sprintf('Display Update | Tipico: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_disp), min(v_disp), max(v_disp)));
                
                drawnow limitrate;
            end
        end
    end
end

clear s;