%%Codice Matlab per il calcolo dei coefficienti di un FIR

fs = 100;        % frequenza di campionamento effettiva
fc = 5;         % frequenza di taglio desiderata
Na = 16;         % ordine del filtro (numero coefficienti)
Nb = 32;
Nc = 64;

% calcolo dei coefficienti
a = fir1(Na-1, fc/(fs/2), 'low', hamming(Na));
b = fir1(Nb-1, fc/(fs/2), 'low', hamming(Nb));
c = fir1(Nc-1, fc/(fs/2), 'low', hamming(Nc));

% esporta in formato C++
fprintf('float _coeffL[%d] = {\n', Na);
fprintf('\t%.8ff, ', a(1:end-1));
fprintf('%.8ff\n};\n', a(end));

fprintf('float _coeffM[%d] = {\n', Nb);
fprintf('\t%.8ff, ', b(1:end-1));
fprintf('%.8ff\n};\n', b(end));

fprintf('float _coeffH[%d] = {\n', Nc);
fprintf('\t%.8ff, ', c(1:end-1));
fprintf('%.8ff\n};\n', c(end));
