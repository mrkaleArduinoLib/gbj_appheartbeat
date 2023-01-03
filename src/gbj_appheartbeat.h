/*
  NAME:
  gbj_appheartbeat

  DESCRIPTION:
  Library generates heartbeat pulses for watchdog timers.
  - Pulses can be active high or active low.


  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
 */
#ifndef GBJ_APPHEARTBEAT_H
#define GBJ_APPHEARTBEAT_H

#if defined(ESP8266)
  #include <Arduino.h>
#elif defined(ESP32)
  #include <Arduino.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif
#include "gbj_serial_debug.h"
#include "gbj_timer.h"

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appheartbeat"

class gbj_appheartbeat
{
public:
  const char *VERSION = "GBJ_APPHEARTBEAT 1.0.0";

  /*
    Constructor

    DESCRIPTION:
    Constructor creates the class instance object and sets operational
    parameters.

    PARAMETERS:
    pinHeartbeat - Number of a GPIO pin of the microcontroller providing a
    heartbeat pulses and aimed for connecting with external watchdog timers.
    The heartbeat is ACTIVE HIGH by default, i.e., the short pulse is at
    logical 1 and in idle state is on logical 0.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255
    reverse - Flag whether the heartbeat works in reverse mode, which is
    ACTIVE LOW, i.e., the short pulse is at logical 0 and in idle state is
    on logical 1.
      - Data type: boolean
      - Default value: false
      - Limited range: true, false
    block - Flag whether the GPIO pin for heartbeat is not controlled
    alltogether. It is suitable where heartbeat pin is connected to some serial
    pin, so that the using heartbeat and serial monitor at once is not possible.
      - Data type: boolean
      - Default value: false
      - Limited range: true, false

    RETURN: object
  */
  inline gbj_appheartbeat(byte pinHeartbeat,
                          bool reverse = false,
                          bool block = false)
  {
    pin_ = pinHeartbeat;
    blocked_ = block;
    if (reverse)
    {
      ACTIVE = LOW;
      IDLE = HIGH;
    }
    else
    {
      ACTIVE = HIGH;
      IDLE = LOW;
    }
    timer_ = new gbj_timer(Timing::PERIOD_HEARTBEAT);
  }

  /*
    Initialization

    DESCRIPTION:
    The method should be called in the SETUP section of a sketch and sets up
    the heartbeat.
    - The method generates an initial heartbeat pulse as well, if the heartbeats
      are enabled.

    PARAMETERS: None

    RETURN: None
  */
  inline void begin(bool enabled = true)
  {
    enabled ? enable() : disable();
    if (isFree())
    {
      pinMode(pin_, OUTPUT);
    }
    // Initial heartbeat pulse
    beat();
  }

  /*
    Blocking the heartbeat pin

    DESCRIPTION:
    The method disable to use the heartbeat pin for generating pulses and
    library it fully ignores.

    PARAMETERS: None

    RETURN: None
  */
  inline void block() { blocked_ = true; }

  /*
    Unblocking the heartbeat pin

    DESCRIPTION:
    The method enables to use the heartbeat pin for generating pulses.
    - The method sets the pin to output mode for sure, if it has been changed
      during time it was blocked.

    PARAMETERS: None

    RETURN: None
  */
  inline void free()
  {
    blocked_ = false;
    pinMode(pin_, OUTPUT);
  }

  /*
    Enabling heartbeat pulses

    DESCRIPTION:
    The method enables the generating heartbeat pulses and generates a pulse.
    - The method can be used repeatedly for generating a heartbeat pulse
      forcibly outside of a timer period.

    PARAMETERS: None

    RETURN: None
  */
  inline void enable()
  {
    enabled_ = true;
    beat();
  }
  inline void disable()
  {
    enabled_ = false;
    if (isFree())
    {
      digitalWrite(pin_, IDLE);
    }
  }

  /*
    Processing

    DESCRIPTION:
    The method should be called in an application sketch loop.
    It processes main functionality and is controlled by the internal timer.

    PARAMETERS: None

    RETURN: none
  */
  inline void run()
  {
    if (timer_->run())
    {
      SERIAL_TITLE("Heartbeat")
      beat();
    }
  }

  // Setters

  /*
    Set timer period for heartbeat

    DESCRIPTION:
    The method is overloaded and sets timer period for generating heartbeat
    pulses.
    - If the input time period is of type integer, it is considered to be in
      milliseconds.
    - If the input time period is of type string, it is considered to be in
      seconds.
    - If no input time period is provided, the setter sets the default period
      hardcoded in the library.

    PARAMETERS:
    period - Numerical time period for generating heartbeat pulses
    in milliseconds.
      - Data type: non-negative long integer
      - Default value: 0
      - Limited range: 0 ~ 2^32-1
    periodSec - Textual time period for generating heartbeat pulses
    in seconds.
      - Data type: string
      - Default value: none
      - Limited range: none

    RETURN: none
  */
  inline void setPeriod(unsigned long period = 0)
  {
    timer_->setPeriod(period == 0 ? Timing::PERIOD_HEARTBEAT : period);
  }
  inline void setPeriod(String periodSec)
  {
    setPeriod(1000 * (unsigned long)periodSec.toInt());
  }

  /*
  Set heartbeat pulse length

  DESCRIPTION:
  The method is overloaded and sets length of generated heartbeat pulses.
  - The input time period can be of type integer or string and in either case
    it is considered to be in milliseconds.
  - If no input time period is provided, the setter sets the default length
    hardcoded in the library.

  PARAMETERS:
  period - Time length of generated heartbeat pulses in milliseconds.
    - Data type: byte or string
    - Default value: 0 in case of byte
    - Limited range: 0 ~ 255

  RETURN: none
*/
  inline void setPulse(byte period = 0)
  {
    lenPulse_ = period ? period : Timing::PERIOD_PULSE;
  }
  inline void setPulse(String period) { setPulse((byte)period.toInt()); }

  // Getters
  inline bool isBlocked() { return blocked_; }
  inline bool isFree() { return !isBlocked(); }
  inline bool isEnabled() { return enabled_; }
  inline bool isDisabled() { return !isEnabled(); }
  inline unsigned long getPeriod() { return timer_->getPeriod(); }
  inline byte getPulse() { return lenPulse_; }

private:
  enum Timing : unsigned long
  {
    PERIOD_HEARTBEAT = 15 * 1000,
    PERIOD_PULSE = 10,
  };
  gbj_timer *timer_;
  bool blocked_, enabled_, halted_;
  byte pin_, lenPulse_;
  byte ACTIVE, IDLE;

  /*
    Heartbeat

    DESCRIPTION:
    The method generate a heartbeat pulse in the blocking mode. The pulse is
    generated when the heartbeat is enabled and not blocked.

    PARAMETERS: None

    RETURN: none
  */
  inline void beat()
  {
    if (isFree() && isEnabled())
    {
      digitalWrite(pin_, ACTIVE);
      delay(lenPulse_);
      digitalWrite(pin_, IDLE);
    }
  }
};

#endif