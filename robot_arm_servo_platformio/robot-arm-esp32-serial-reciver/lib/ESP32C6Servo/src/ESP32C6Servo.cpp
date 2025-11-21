#include "ESP32C6Servo.h"

#include "driver/ledc.h"

// Static channel usage table
bool ESP32C6Servo::_channelUsed[ESP32C6Servo::MAX_CHANNELS] = { false };
static bool timerConfigured[4] = { false, false, false, false };

ESP32C6Servo::ESP32C6Servo()
  : _pin(-1), _channel(-1), _minUs(500), _maxUs(2500) {}

int ESP32C6Servo::allocateChannel() {
    for (int ch = 0; ch < MAX_CHANNELS; ++ch) {
        if (!_channelUsed[ch]) {
            _channelUsed[ch] = true;
            return ch;
        }
    }
    return -1; // no channel available
}

void ESP32C6Servo::setupChannelIfNeeded(int channel) {
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    int timer = channel % 4; // simple mapping of channels to timers
    if (!timerConfigured[timer]) {
        ledc_timer_config_t tcfg = {};
        tcfg.speed_mode = LEDC_LOW_SPEED_MODE;
            tcfg.duty_resolution = LEDC_TIMER_16_BIT;
        tcfg.timer_num = (ledc_timer_t)timer;
        tcfg.freq_hz = FREQ;
        tcfg.clk_cfg = LEDC_AUTO_CLK;
        ledc_timer_config(&tcfg);
        timerConfigured[timer] = true;
    }
}

bool ESP32C6Servo::attach(int pin, int channel) {
    if (pin < 0) return false;
    _pin = pin;
    if (channel >= 0) {
        // use requested channel
        if (channel >= MAX_CHANNELS) return false;
        _channel = channel;
    } else {
        _channel = allocateChannel();
        if (_channel < 0) return false;
    }

    // Ensure timer for this channel is configured
    setupChannelIfNeeded(_channel);

    // Configure channel with gpio
    ledc_channel_config_t chcfg = {};
    chcfg.channel = (ledc_channel_t)_channel;
    chcfg.duty = 0;
    chcfg.gpio_num = (gpio_num_t)_pin;
    chcfg.speed_mode = LEDC_LOW_SPEED_MODE;
    chcfg.hpoint = 0;
    chcfg.timer_sel = (ledc_timer_t)(_channel % 4);
    if (ledc_channel_config(&chcfg) != ESP_OK) {
        // rollback
        _channelUsed[_channel] = false;
        _channel = -1;
        _pin = -1;
        return false;
    }

    // initialize with center pulse
    uint32_t periodUs = 1000000UL / FREQ; // e.g., 20000
    uint32_t duty = ((uint32_t)((_minUs + _maxUs) / 2) * (1UL << RESOLUTION)) / periodUs;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_channel);
    return true;
}

void ESP32C6Servo::detach() {
    if (_channel >= 0) {
        // stop PWM and detach pin
        ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_channel, 0);
        _channelUsed[_channel] = false;
        _channel = -1;
    }
    _pin = -1;
}

void ESP32C6Servo::writeMicroseconds(uint16_t us) {
    if (_channel < 0) return;
    uint32_t periodUs = 1000000UL / FREQ; // 20000
    // clamp
    if (us < _minUs) us = _minUs;
    if (us > _maxUs) us = _maxUs;
    uint32_t duty = ((uint32_t)us * (1UL << RESOLUTION)) / periodUs;
    if (duty > ((1UL << RESOLUTION) - 1)) duty = (1UL << RESOLUTION) - 1;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_channel);
}

void ESP32C6Servo::write(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    double micro = _minUs + (angle / 180.0) * (double)(_maxUs - _minUs);
    writeMicroseconds((uint16_t)round(micro));
}

void ESP32C6Servo::setPulseRange(uint16_t minUs, uint16_t maxUs) {
    if (minUs < 200) minUs = 200;
    if (maxUs > 3000) maxUs = 3000;
    if (minUs >= maxUs) return;
    _minUs = minUs;
    _maxUs = maxUs;
}
