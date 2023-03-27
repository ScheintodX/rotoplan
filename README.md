Rotoplan
===============================================================================

This is a stm32f03x firmware for running an BLDC motor using varying readily
available hardware.

In contrast to factory firmware this one uses Sinus / PWM for a much smoother
and more silent running and tighter feedback to keep the speed much more
constant.

This is currently work in progress



Supported Hardware
===============================================================================

WS55-220
===============================================================================
This one runs reasonably well and produces a fair amount of torque.
It supports HALL sensors. But when running of them it only does block 
commutation and is pretty loud. 
If hall sensor are not connected it uses a more silent way of pwm 
production.
It has a stm32f030k6t6 mcu in an adequate LQFP 32 pin package.


Characteristics
-------------------------------------------------------------------------------
* 20-50V DC
* Hall sensor support -> block commutation
* Back EMF support
* Speed signal output
* Speed input via voltage divider
* 10V supply voltage for hall and peripherals
* can be programmed and original firmware can be read
* UART: None. Pins PB6(TX1) and PB7(RX1) are free but not nc.
* No crystal (but internal 8MHz osc)
* MCU: STM32F030K6T6 
* Driver: probably LN4308 + 6 Mosfets(?) which are glued to the heatsink
  using HI/LO inputs


Pinout
-------------------------------------------------------------------------------
+/-: 20..50V DC. Voltage influences speed
UVW: Motor output. PWM 15kHz
HUHVHW: Hall input. 
EN: Enable motor. Shows blinking error when not enabled.
F/R: Forward reverse. 


ISP-Header
-------------------------------------------------------------------------------
As printed on silkscreen:

* GND
* TCK - PA14 - SWCLK
* TWS - PA13 - SWDIO
* 5V

Which is standard stm32 


Documentation:
-------------------------------------------------------------------------------

* STM32F030xx https://www.st.com/resource/en/datasheet/stm32f030f4.pdf
* WS55-220    https://myosuploads3.banggood.com/products/20190508/20190508220426SKU600181WS55-1801.pdf 
              (but note that the connection diagram is not acurate)
* LN4308      http://www.natlinear.com/uploadfiles/2014/ln/LN4308_E.pdf



ZM-7205A
===============================================================================
This is the only one which uses 230V AC converted to 3XXV DC to power
the motor. The hardware driver is identical 
This one is hardware-wise ideal for running and washing machine motor
but the software is of poor quality.
It supports HALL input but only uses it to block commutate.
It is to some extand configurable with an programming device but that
doesn't work very well and the parameters are somewhat limited or don't
work.
In contrast to some advertisement it does *not* provide sinusoidal 
commutation with stock firmware.
It uses an CKS32F030xx which is a stm32f030 ripoff.

Characteristics
-------------------------------------------------------------------------------
* 230V AC
* Hall sensor support -> block commutation
* No or bad back emf support
* Driver: FNB41060
  using HI/LO inputs


Documentation
-------------------------------------------------------------------------------
FNB41060   https://www.farnell.com/datasheets/1793299.pdf
