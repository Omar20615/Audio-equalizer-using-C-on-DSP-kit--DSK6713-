clear all
clc
%% Read audio
[x, fs] = audioread('wavefile.wav');
x = x(:,1); % dropping the stereo sound
%%sound(x, fs);

audiowrite('sound.wav',x,fs)

%% Load coefficients
equalized=[]
load("LowPass.mat")
load("BandPass4kto8k.mat")
load("BandPass9kto13k.mat")
load("BandPass13kto17k.mat")
load("HighPass.mat")

%% Apply Equalization
b1=LowPass;
b2=BandPass4kto8k;
b3=BandPass9kto13k;
b4=BandPass13kto17k;
b5=HighPass;
c1=20;
c2=0;
c3=12;
c4=5;
c5=9;

y=10^(c2/20)*b1;

equalized=filter(10^(c1/20)*b1,1,x);
equalized=equalized+filter(10^(c2/20)*b2,1,x);
equalized=equalized+filter(10^(c3/20)*b3,1,x);
equalized=equalized+filter(10^(c4/20)*b4,1,x);
equalized=equalized+filter(10^(c5/20)*b5,1,x);

% 
soundsc(equalized,fs)
freqz(equalized)
% [h,w] =freqz(fft(app.equalized));
% plot(app.PulsePlotUIAxes,w/pi,20*log10(abs(h)));