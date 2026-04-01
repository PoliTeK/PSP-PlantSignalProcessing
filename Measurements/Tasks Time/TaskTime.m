% =========================================================================
% DSP MULTITASKING PROFILER
% =========================================================================
% FIRMWARE SETUP INSTRUCTIONS (C++):
% 1. Measure CPU cycles for your 4 main tasks using your hardware timer 
%    (e.g., System::GetTick() on Daisy Seed).
% 2. Accumulate the cycles over a set period (e.g., 50 audio callbacks) to
%    prevent flooding the serial port.
% 3. Calculate the average cycle count for each of the 4 tasks.
% 4. Send the 4 averages via serial as a comma-separated string ending 
%    with CR/LF. Example output: "52253,190,36245,20966402\r\n"
% 5. Reset your accumulators to zero and repeat.
%
% HOW TO USE THIS SCRIPT:
% 1. Plug in your board running the profiling firmware.
% 2. CRITICAL: Close any other Serial Monitors (VS Code, PuTTY, Arduino).
% 3. Update 'port_name' below to match your board's active COM port.
% 4. Update 'cpu_freq_mhz' to match your microcontroller's clock speed.
% 5. Run the script. Close the figure window to terminate the connection.
% =========================================================================

% --- CONFIGURATION ---
port_name = "COM3"; 
baud_rate = 115200;
cpu_freq_mhz = 480; % Set to 450 or 480 depending on your STM32 config

% Close existing serial connections to prevent access errors
if exist('s', 'var')
    clear s;
end
s = serialport(port_name, baud_rate);
configureTerminator(s, "CR/LF");

% --- GRAPHICS SETUP ---
fig = figure('Name', 'DSP Multitasking Profiler', 'NumberTitle', 'off', 'Position', [100, 100, 1000, 800]);

% Stack the 4 plots cleanly
t = tiledlayout(4, 1, 'TileSpacing', 'compact', 'Padding', 'compact');
title(t, 'Task Execution Time Analysis');

% Plot 1: Audio
ax1 = nexttile; hold(ax1, 'on'); grid(ax1, 'on'); ylabel(ax1, 'Audio (\mus)');
line_audio = animatedline(ax1, 'Color', 'r', 'LineWidth', 1.5);

% Plot 2: Encoder
ax2 = nexttile; hold(ax2, 'on'); grid(ax2, 'on'); ylabel(ax2, 'Encoder (\mus)');
line_enc = animatedline(ax2, 'Color', 'g', 'LineWidth', 1.5);

% Plot 3: DSP Logic
ax3 = nexttile; hold(ax3, 'on'); grid(ax3, 'on'); ylabel(ax3, 'Logic (\mus)');
line_logic = animatedline(ax3, 'Color', 'b', 'LineWidth', 1.5);

% Plot 4: Display
ax4 = nexttile; hold(ax4, 'on'); grid(ax4, 'on'); ylabel(ax4, 'Display (\mus)');
xlabel(ax4, 'Samples (~16.6 ms/sample)');
line_disp = animatedline(ax4, 'Color', 'k', 'LineWidth', 1.5);

% Control variables
window_size = 500; % Show the last 500 samples
sample_count = 0;

% Circular buffers for statistical calculation of the current window
buf_audio = nan(window_size, 1);
buf_enc   = nan(window_size, 1);
buf_logic = nan(window_size, 1);
buf_disp  = nan(window_size, 1);

flush(s);

% --- ACQUISITION LOOP ---
while ishandle(fig)
    if s.NumBytesAvailable > 0
        raw_str = readline(s);
        numeric_data = sscanf(raw_str, '%f,%f,%f,%f');
        
        if length(numeric_data) == 4
            % Convert CPU ticks to microseconds based on configured clock
            t_audio = numeric_data(1) / cpu_freq_mhz;
            t_enc   = numeric_data(2) / cpu_freq_mhz;
            t_logic = numeric_data(3) / cpu_freq_mhz;
            t_disp  = numeric_data(4) / cpu_freq_mhz;
            
            sample_count = sample_count + 1;
            
            % Update lines in their respective axes
            addpoints(line_audio, sample_count, t_audio);
            addpoints(line_enc,   sample_count, t_enc);
            addpoints(line_logic, sample_count, t_logic);
            addpoints(line_disp,  sample_count, t_disp);
            
            % Update circular buffers for live statistics
            idx = mod(sample_count - 1, window_size) + 1;
            buf_audio(idx) = t_audio;
            buf_enc(idx)   = t_enc;
            buf_logic(idx) = t_logic;
            buf_disp(idx)  = t_disp;
            
            % Synchronized X-axis scrolling for all plots
            if sample_count > window_size
                x_lims = [sample_count - window_size, sample_count];
            else
                x_lims = [0, window_size];
            end
            ax1.XLim = x_lims; ax2.XLim = x_lims; 
            ax3.XLim = x_lims; ax4.XLim = x_lims;
            
            % Update titles with calculated data every 10 samples
            if mod(sample_count, 10) == 0
                % Audio Data
                v_aud = buf_audio(~isnan(buf_audio));
                title(ax1, sprintf('AudioCallback | Typ: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_aud), min(v_aud), max(v_aud)));
                
                % Encoder Data
                v_enc = buf_enc(~isnan(buf_enc));
                title(ax2, sprintf('Encoder | Typ: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_enc), min(v_enc), max(v_enc)));
                
                % Logic Data
                v_log = buf_logic(~isnan(buf_logic));
                title(ax3, sprintf('DSP Logic | Typ: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_log), min(v_log), max(v_log)));
                
                % Display Data
                v_disp = buf_disp(~isnan(buf_disp));
                title(ax4, sprintf('Display Update | Typ: %.1f \\mus | Min: %.1f \\mus | Max: %.1f \\mus', mean(v_disp), min(v_disp), max(v_disp)));
                
                drawnow limitrate;
            end
        end
    end
end

% Release the serial port when the figure is closed
clear s;