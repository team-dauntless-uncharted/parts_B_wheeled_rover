#pragma once

class Speaker {
public:
    Speaker(int pin);
    void beep(int freq, int duration);
    void playStart();
    void playError();
    void playBeep();
    void playState(const int state);

private:
    void _play(const float* melody, int notes, int noteDuration);
    int _pin;
};