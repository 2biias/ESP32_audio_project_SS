

%% Make a chirp

fs = 48000;
t = 0:1/fs:2;
chirp_data = chirp(t, 0, 1, 10000);
spectrogram(chirp_data, 128, 120, 129, fs , 'yaxis')

chirp_data = uencode(chirp_data,16, 1, 'signed');

chirp_data = [chirp_data; chirp_data]';

audiowrite('Input/InputChirp.wav', chirp_data, 48000);

%% Test on chirp
fc = 300;
fs = 48000;
fc/(fs/2)
[b1,a1] = butter(2,fc/(fs/2),'high')
figure(1); clf
freqz(b1,a1)

filter_response_HP = freqz(b1, a1, fs/2, fs);

figure(2); clf
[b2, a2] = butter(2, fc/(fs/2), 'low')
freqz(b2,a2)

filter_response_LP = freqz(b2, a2, fs/2, fs);

parallel = filter_response_HP + filter_response_LP;

f_axis = logspace(log10(1), log10(fs/2), fs/2);

figure(500); clf
semilogx(f_axis, 20*log10(abs(filter_response_HP)),'linewidth',2)
hold on
semilogx(f_axis, 20*log10(abs(filter_response_LP)),'linewidth',2)
xlabel('log(f) [Hz]')
ylabel('Gain [dB]')
fc1 = xline(300, 'g', 'LineWidth',1.5);
legend([fc1], ["300Hz"]);
xlim([0, 1000]);
ylim([-10, 3])
hold off

figure(3); clf
hold on
subplot(2, 1, 1)
semilogx(f_axis, 20*log10(abs(filter_response_HP)),'linewidth',2)
xlabel('log(f) [Hz]')
ylabel('Gain [dB]')
fc1 = xline(300, 'g', 'LineWidth',1.5);
legend([fc1], ["300Hz"]);
xlim([0, 1000]);
ylim([-10, 3])
subplot(2, 1, 2)
semilogx(f_axis, 20*log10(abs(filter_response_LP)),'linewidth',2)
xlabel('log(f) [Hz]')
ylabel('Gain [dB]')
fc1 = xline(300, 'g', 'LineWidth',1.5);
legend([fc1], ["300Hz"]);
xlim([0, 1000]);
ylim([-10, 3])
hold off


figure(4); clf
hold on
subplot(2, 1, 1)
semilogx(f_axis, 20*log10(abs(parallel)),'linewidth',2)
xlabel('log(f) [Hz]')
ylabel('Gain [dB]')
fc1 = xline(300, 'g', 'LineWidth',1.5);
legend([fc1], ["300Hz"]);
xlim([0, fs/2]);
ylim([-20, 17])
subplot(2, 1, 2)
semilogx(f_axis, rad2deg(unwrap(angle(parallel))), 'LineWidth',1.5)
xlabel('log(f) [Hz]')
ylabel('Fase [grader]')
xlim([0, fs/2]);
fc1 = xline(300, 'g', 'LineWidth',1.5);
legend([fc1],["300Hz"]);
hold off

y1 = svf(x, 500, 0.707, 'lp') + svf(x, 500, 0.707, 'hp');

y2 = svf(svf(x, 500, 0.707, 'lp'), 500, 0.707, 'lp') + svf(svf(x, 500, 0.707, 'hp'), 500, 0.707, 'hp');

%% Listen to chirp

[toplay, fs2] = audioread('InputChirp.wav');
sound(toplay, fs2)

%% Reading LeftRight audiofile into int16

[data, fs] = audioread('LeftRight.wav', 'native');

%% Listen to original sound
[toplay, fs2] = audioread('LeftRight.wav');
sound(toplay, fs2)

%% Power of inputsignal

Power_original = CalcPower(chirp_data);

%% Write read audiofile to format

% Convert to single coloumn
% Outputs "Input/InputFile.data" used by AudioProcess.exe
Serialized = SerializeToFile(chirp_data);

%% Read data from DSP program outputfile
% Reads output file from AudioProcess.exe from "Output/Outputfile.data"

[raw, Deserialized] = DeserializeFromFile(length(Serialized));

%% Power of processed data

Power_processed = CalcPower(Deserialized);

%% Audio analysis

left_channel = Deserialized(:,1);
right_channel = Deserialized(:,2);
N_samples_left = length(left_channel);
N_samples_right = length(right_channel);
fft_left = abs(fft(left_channel));
fft_right = abs(fft(right_channel));

freq_axis = (0:(N_samples_left/2)-1)*(fs/N_samples_left);

figure(1); clf
semilogx(freq_axis, 20*log(fft_left(1:N_samples_left/2)))
hold on
grid on
xlim([1 24000])
hold off

figure(2); clf
semilogx(freq_axis, 20*log(fft_right(1:N_samples_left/2)))
hold on
grid on
xlim([1 24000])
hold off
%% Convert Read data to audio file

audiowrite('Output/OutputSound.wav', left_channel, 48000);

%% Is equal
isequal(data, deserialized)
