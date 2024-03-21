# chickenHouse
Chicken house automation with door and light management

The chicken house light can be switched on/off using the push switch. (It switches itself off again after a short time).  
The length of day is determined based on the longitude and latitude, as well as the current time. If the (light) day is shorter than a certain period of time, the light is switched on until sunrise. This ensures a minimum length of day. (Increasing the quantity of eggs)  
The door can be opened and closed using a push button. The door is also controlled by the sunrise and sunset times.  
The earliest time at which the door should open in the morning can also be set. (prevents problems with the neighbours)

I have been using it for several years. After some initial bugs, no more problems.

Required components:
 - 1 x Arduino Nano
 - 2 x DS18B20 (outside, inside - just for logging purposes)
 - 1 x relay card with 2 relays (switch light)
 - 1 x 360°-Servo (The servo winds up the cord via a reel.)
 - 2 x microswitches as limit switches for the door
 - 1 x RTC DS1307
 - 1 x SD card module (logging)
 - 2 x RS 485 modules (for communication with a master - optional)
 - 16x4 LCD
 - I2C LCD Adapter
