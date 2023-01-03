<a id="library"></a>

# gbj\_appheartbeat
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of a `heartbeat pulses generator` for resetting (tapping) an external watchdog timers. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* The library is reusable for various projects without need to code a heartbeat management.
* Update in library is valid for all involved projects.


## Fundamental functionality
* The library controls polarity (Active High, Active Low) of heartbeat pulses for various hardware design of watchdog timers.
* It controls accessibility of a heartbeat's GPIO, i.e., can totally avoid manipulation with the corresponding pin. It is neccessary for microcontroller, where serial TX pin should be employed as heartbeat pin, so that using the heartbeat generator and serial monitor at once is not possible.
* It controls accessibility (blocked, free) of the heartbeat.
* It controls ability (enabled, disabled) of the heartbeat.
* The library utilizes internal timer for generatig heartbeat pulses.
* A heartbeat pulse is generated in blocking mode of the microcontroller.


<a id="internals"></a>

## Internal parameters
Internal parameters are hard-coded in the library usually as enumerations and have no setters and getters associated.

* **Time period for heartbeat** (`15 s`): The parameter for time between repeated heartbeat pulses generated by the library.
* **Time length for pulses** (`10 ms`): The parameter for time length of a heartbeat pulse generated by the library.


<a id="dependency"></a>

## Dependency
* **gbj\_timer**: Library for executing internal timer within an instance object loaded from the file `gbj_timer.h`.

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif ESP8266 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Espressif ESP32 platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="constants"></a>

## Constants
* **VERSION**: Name and semantic version of the library.


<a id="interface"></a>

## Interface
* [gbj_appheartbeat()](#gbj_appheartbeat)
* [begin()](#begin)
* [run()](#run)
* [block()](#ignore)
* [free()](#ignore)
* [enable()](#allow)
* [disable()](#allow)

### Getters
* [isBlocked()](#access)
* [isFree()](#access)
* [isEnabled()](#ability)
* [isDisabled()](#ability)
* [getPeriod()](#getPeriod)
* [getPulse()](#getPulse)

### Setters
* [setPeriod()](#setPeriod)
* [setPulse()](#setPulse)


<a id="gbj_appheartbeat"></a>

## gbj_appheartbeat()

#### Description
Constructor creates the class instance object and sets operational parameters, especially values of heartbeat pin for its polarity.
  * The heartbeat is ACTIVE HIGH by default, i.e., the short pulse is at logical 1 and in idle state is on logical 0.

#### Syntax
    gbj_appheartbeat(byte pinHeartbeat, bool reverse, bool block)

#### Parameters
* **pinHeartbeat**: Number of a GPIO pin of the microcontroller providing a heartbeat pulses and aimed for connecting with external watchdog timers.
  * *Valid values*: available output GPIO pins
  * *Default value*: none


* **reverse**: Flag determining reverse polarity of the heartbeat pulses, which is ACTIVE LOW, i.e., the short pulse is at logical 0 and in idle state is on logical 1.
  * *Valid values*: true or false
  * *Default value*: false


* **block**: Flag whether the heartbeat pin is not controlled alltogether. It is suitable where heartbeat pin is connected to some serial pin, so that the using heartbeat and serial monitor at once is not possible.
  * *Valid values*: true or false
  * *Default value*: false

#### Returns
Object performing heartbeat generator management.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The initialization method of the instance object, which should be called in the setup section of a sketch.
* The method activates a heartbeat pin and sets ability of it.
* The ability can be change later with corresponding method.
* The method generates an initial heartbeat pulse as well, if the heartbeats are enabled.

#### Syntax
    void begin(bool enabled)

#### Parameters
* **enabled**: Flag defining the ability of heartbeat. If disabled, the heartbeat pulses are not generated.
  * *Valid values*: true or false
  * *Default value*: true

#### Returns
None

#### See also
[enable(), disable()](#allow)

[Back to interface](#interface)


<a id="run"></a>

## run()

#### Description
The method generates heartbeat pulses and should be called frequently, usually in the loop function of a sketch.

#### Syntax
    void run()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="ignore"></a>

## block(), free()

#### Description
The methods manipulate accessibility of the heartbeat pin, either blocks or frees it.
- After blocking the heartbeat pin during the firmware run, its mode should be set exactly in a sketch accordingly to the new usage of it.
- At freeing the heartbeat pin during the firmware run, the method sets it to output mode.

#### Syntax
    void block()
    void free()

#### Parameters
None

#### Returns
None

#### See also
[isBlocked(), isFree()](#access)

[Back to interface](#interface)


<a id="allow"></a>

## enable(), disable()

#### Description
The methods manipulate ability of the heartbeat pin, either enables or disable it.
- At disabling the heartbeat the method sets the pin to the idle state.

#### Syntax
    void enable()
    void disable()

#### Parameters
None

#### Returns
None

#### See also
[isEnabled(), isDisabled()](#ability)

[begin()](#begin)

[Back to interface](#interface)


<a id="access"></a>

## isBlocked(), isFree()

#### Description
The particular getter returns flag determining whether corresponding accessibility of the heartbeat pin is valid.

#### Syntax
    bool isBlocked()
    bool isFree()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding heartbeat pin's accessibility.

#### See also
[block(), free()](#ignore)

[Back to interface](#interface)


<a id="ability"></a>

## isEnabled(), isDisabled()

#### Description
The particular getter returns flag determining whether corresponding ability of the heartbeat pin is valid.

#### Syntax
    bool isEnabled()
    bool isDisabled()

#### Parameters
None

#### Returns
Boolean flag about validity of corresponding heartbeat pin's ability.

#### See also
[enable(), disable()](#allow)

[Back to interface](#interface)


<a id="getPeriod"></a>

## getPeriod()

#### Description
The method returns current period of repeating the heartbeat pulses.

#### Syntax
    unsigned long getPeriod()

#### Parameters
None

#### Returns
Current heartbeat period in milliseconds.

#### See also
[setPeriod()](#setPeriod)

[Back to interface](#interface)


<a id="getPulse"></a>

## getPulse()

#### Description
The method returns current length of a heartbeat pulse.

#### Syntax
    byte getPulse()

#### Parameters
None

#### Returns
Current heartbeat pulse length in milliseconds.

#### See also
[setPulse()](#setPulse)

[Back to interface](#interface)


<a id="setPeriod"></a>

## setPeriod()

#### Description
The overloaded method sets a new repeating period of the heartbeat pulses input either in milliseconds or in seconds.
* If the input time period is of type integer, it is considered to be in milliseconds.
* If the input time period is of type string, it is considered to be in seconds.
* The method with textual input argument is useful with conjunction with a project data hub, which data has always string data type.
* If input period is zero or not numerical (leading to zero), the library sets the [internal default Time period for heartbeat](#internals).

#### Syntax
    void setPeriod(unsigned long period)
    void setPeriod(String periodSec)

#### Parameters
* **period**: Numerical time period for generating heartbeat pulses in milliseconds.
  * *Valid values*: 0 ~ 2^32 - 1
  * *Default value*: 0


* **periodSec**: Textual time period for generating heartbeat pulses in seconds.
  * *Valid values*: String
  * *Default value*: none

#### Returns
None

#### See also
[getPeriod()](#getPeriod)

[Back to interface](#interface)


<a id="setPulse"></a>

## setPulse()

#### Description
The overloaded method sets a new length of heartbeat pulses input in milliseconds.
* The method with textual input argument is useful with conjunction with a project data hub, which data has always string data type.
* If input period is zero or not numerical (leading to zero), the library sets the [internal default Time length for pulses](#internals).

#### Syntax
    void setPulse(byte period)
    void setPulse(String period)

#### Parameters
* **period**: Numerical or textual time length of heartbeat pulses in milliseconds.
  * *Valid values*: 0 ~ 255
  * *Default value*: 0 in case of numerical input

#### Returns
None

#### See also
[getPulse()](#getPulse)

[Back to interface](#interface)
