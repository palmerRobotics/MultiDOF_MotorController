clear
close all
%s_raw.mat: data from current_sensor_noise_oscope.cvs, in matlab form (for easy laoding)
load('s_raw.mat')

% s_raw: unfiltered voltage read from drv8876 current sensor @ Fs_raw
% (I grabbed a TON of samples, way more than needed)

% s_raw = s_raw(1:5e4); % grab as much of the signal as you want here
T_raw = 5e-8; % Oscope's sampling period
Fs_raw = 1/T_raw; % sampling frequency
N_raw = size(s_raw,1); % number of samples
t_raw = (0:N_raw-1)*T_raw; % time vector corresponding to samples

Fs = 5000; % desired sampling frequency (current target frequency is 5kHz)
T = 1/Fs; % time vector corresponding to target frequency

F = Fs_raw / Fs;  

% s: unfiltered  voltage read from drv8876 current sensor @ Fs
s = s_raw(1:F:end); % take a sample from s_raw every F samples to sample s_raw at Fs
N = length(s);
t = (0:N-1)*T;

Fn = Fs/2; % nyquist frequency
order = 2; % desired order of iir filter
LPcutoff = 300; % desired cutoff frequency

WnLP = LPcutoff/Fn; % cutoff frequency normalized by nyquist frequency

%bLP = fir1(order,WnLP,'Low'); %good starting point, but poor frequency response
[b, a] = butter(order, WnLP, 'Low'); % implement IIR filter as butterworth filter
[z, p, k] = butter(order, WnLP, 'Low'); % get zeros, poles, and gain of filter - used to determine filter stability

%LPfilter = filter(bLP,1,s);
LPfilter_butter = filter(b,a,s); % filter the Fs sampled signal through the butterworth filter


figure(1);
subplot(2,1,1);
hold on
title('Raw vs. Filtered Signal');
ylabel('Volts (V)');
xlabel('Time (s)');
plot(t,s); % plot the noisy signal
plot(t,LPfilter_butter); % overlay the filtered signal on the unfiltered signal
legend('Raw Signal','Filtered Signal');
hold off

subplot(2,1,2);
hold on
title('Raw vs. Filtered FFT');
ylabel('Volts (V)');
xlabel('Frequency (Hz)');
plotFFT(Fs, s); % plot fft of Fs sampled signal
plotFFT(Fs, LPfilter_butter); % overlay fft of filtered signal
legend('Raw Signal','Filtered Signal');
hold off

% Dang these results look good

figure(2);
freqz(b,a); % bode plot of designed butterworth filter
title('Filter Frequency Reponse');


figure(3);
zplane(z,p); % check designed filter's stability. If poles are within unit circle, filter is stable
if isstable(b,a)
    txt = 'Filter is stable!';
else
    txt = 'Filter is unstable!';
end
text(-1.2, .9, txt);
title('Filter Zero-Pole Plot');
