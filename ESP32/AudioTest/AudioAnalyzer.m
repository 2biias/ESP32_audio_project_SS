

%% Make a chirp

fs = 48000;
t = 0:1/fs:4;
chirp_data = chirp(t, 0, 2, 12000, 'linear', 90);
spectrogram(chirp_data, 128, 120, 129, fs , 'yaxis')

% Hanning window can be applied
%chirp_data = chirp_data.*hann(length(chirp_data))';

% Mean can be subtracted
%chirp_data = chirp_data-mean(chirp_data);

% Chirp can be zero-padded
%chirp_data = [zeros(1, 10000) chirp_data zeros(1, 10000)];

chirp_data = uencode(chirp_data,16, 1, 'signed');

chirp_data = [chirp_data; chirp_data]';

audiowrite('Input/InputChirp.wav', chirp_data, 48000);

%% Plot chirp

figure(100); clf
plot(t, chirp_data);

%% Listen to chirp

[toplay, fs2] = audioread('InputChirp.wav');
sound(toplay, fs2)

%% Reading LeftRight audiofile into int16

[data, fs] = audioread('Input/LeftRight.wav', 'native');

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

%% ************************** Audio analysis ******************************

%% FFT original

chirp_data_single = chirp_data(:,1);

N_samples = length(chirp_data_single);
freq_axis = (0:(N_samples/2)-1)*(fs/N_samples);

fft_original = abs(fft(chirp_data_single)/N_samples);

figure(500); clf
semilogx(freq_axis, 20*log10(fft_original(1:N_samples/2)))
hold on
grid on
xlim([1 24000])
xlabel('log(f) [Hz]')
ylabel('Gain [dB]')
hold off

%% FFT filtered

left_channel = Deserialized(:,1);
right_channel = Deserialized(:,2);
summed = left_channel + right_channel;
N_samples_left = length(left_channel);
N_samples_right = length(right_channel);
fft_left = abs(fft(left_channel)/N_samples_left);
fft_right = abs(fft(right_channel)/N_samples_right);
fft_summed = abs(fft(summed)/N_samples_right);

freq_axis = (0:(N_samples_left/2)-1)*(fs/N_samples_left);

figure(4); clf
semilogx(freq_axis, 20*log10(fft_left(1:N_samples_left/2)))
hold on
grid on
xlim([1 24000])
hold off

figure(5); clf
semilogx(freq_axis, 20*log10(fft_right(1:N_samples_left/2)))
hold on
grid on
xlim([1 24000])
hold off

figure(6); clf
semilogx(freq_axis, 20*log10(fft_summed(1:N_samples_left/2)))
hold on
grid on
xlim([1 24000])
hold off

%% Convert Read data to audio file

left_channel_st = [left_channel left_channel];
right_channel_st = [right_channel right_channel];
mix = [left_channel right_channel];
audiowrite('Output/OutputSound.wav', mix, 48000);

%% Plotting channel in time domain

t = [0:length(left_channel)-1].*(1/fs);

figure(211); clf
plot(t, right_channel)
