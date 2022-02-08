# MagicFocusEF Work-In-Progress
Enabling focusing using EF lenses on the BMPCC6K using build-in ultrasonic motors. No need for external focus motors. The camera control part uses the BlueMagic32 library.

![Alt M5Core2_Magic_FocusEF](images/m5core2_magicfocus.jpg?raw=true "M5Core2_Magic_FocusEF")
![Alt M5Core2_Magic_FocusEF_camera](images/m5core2_magicfocus_front.jpg?raw=true "M5Core2_Magic_FocusEF_camera")

# Required Hardware
* BMPCC6K
* Canon EF-mount AF lens
* ESP32 with Bluetooth, I recommend using the M5Core or M5Stick. The UI is written for the M5Stack Core2 ESP32 IoT Development Kit which costs aroun 50$. This kit comes with ESP32, touch screen, Bluetooth and a nice housing. 

# Disclaimer
 * The use of this software is at your own risk. The creator of this software cannot be held liable.
 * Using this software therefore is completely at your own risk.
 * Exessive use of the ultrasound motor of an EF lens could potentially lead to a faster wear.
 * There have been reports of automotive lidar sensors damaging the sensor of cameras. While the use of the TFminiS lidar is believed to create no harm to a camera sensor. This can also not be outruled and the utilization of such sensor is at your own risk.

# Working principle
A Lidar measures the distance between camera and object to be in focus. the distance measurement is then translated into shift increments of the ultrasound motor of the lens. Commands are transmitted via Bluetooth to the BMPCC6K which transmits the desired increment value to the EF AF lens.

# Compatible lenses
I've tested this library with the following Canon lenses:
- EF-S 17-55mm 2.8 IS
- EF 24-70mm 2.8 L
- EF 50mm 1.4
