# EmbeddedManiacMansion
Maniac Mansion short episode embedded in STM32F4 ARM cortex M4

![STM32F4_Discovery_Setup](../media/IMG_1120.JPG?raw=true)

##Hardware
Implemented in STM32F4-DISC1 ST Discovery Board, code can be easily taylored for other ARM microcontrollers, not only cortex M4. This version uses also the following equipment:

* STM32F4DIS-BB Base Board includes SD CARD slot needed for images and DB-9 serial port connected to UART6.
* STM32F4DIS-LCD the LCD and Touchscreen module for the STM32F4DIS-BB.
* 2-axis potentiometer Joystick, taylored in joystick.h, X-Axis is connected to GPIO_B-0 and Y-Axis is connected to GPIO_B-1.
* 4 control pushbuttons. Buttons A, B, Start and Select are taylored in pushbuttons.h and connected to GPIO_A-8, GPIO_D-2, GPIO_A-15 and GPIO_D-11.

##Software

##Libraries
