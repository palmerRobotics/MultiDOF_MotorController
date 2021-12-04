function plotFFT(Fs, s)
%PLOTFFT Plots the one-sided FFT of a signal
%   Detailed explanation goes here
% f: frequency content of signal (up to nyquist frequency)
% P1: magnitude (in V) of signal at that frequency component
Y = fft(s);
N = length(s);

P2 = abs(Y/N);
P1 = P2(1:N/2 + 1);
P1(2:end - 1) = 2*P1(2:end - 1);

f = Fs * (0:(N/2))/N;
plot(f,P1);
end

