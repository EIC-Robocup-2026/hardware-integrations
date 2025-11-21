/*
  ESP32C6Servo.h - Lightweight servo library using LEDC for ESP32-C6
  Provides attach(pin), writeMicroseconds(us), write(angle), detach(), setPulseRange()
*/
#ifndef ESP32C6SERVO_H
#define ESP32C6SERVO_H

#include <Arduino.h>

class ESP32C6Servo {
public:
    ESP32C6Servo();
    bool attach(int pin, int channel = -1); // attach to GPIO pin, optional channel
    void detach();
    void writeMicroseconds(uint16_t us);
    void write(int angle); // 0-180
    void setPulseRange(uint16_t minUs, uint16_t maxUs);

    int getPin() const { return _pin; }
    int getChannel() const { return _channel; }

private:
    int _pin;
    int _channel;
    uint16_t _minUs;
    uint16_t _maxUs;

    static const uint32_t FREQ = 50; // 50 Hz for standard servos
    static const uint8_t RESOLUTION = 16; // bits of resolution
    static const uint8_t MAX_CHANNELS = 16; // LEDC channels available
    static bool _channelUsed[MAX_CHANNELS];
    static void setupChannelIfNeeded(int channel);
    static int allocateChannel();
};

#endif // ESP32C6SERVO_H
