# PortPal Connector logic
## Keep in mind this is POC and will likely be completely chaged in a refactor after it has been proved out

Using an ESP32S3-devkitc-1 for developing

Working on a simple Bluetooth LE to USB bridge to send commands from an iPhone app 

All testing is currently being done on an AnyCubic 3d printer by sending Gcode commands.



# Plan
The plan here is to have each component of this run as a seperate task using FreeRTOS.

## Components:
- Bluetooth manager: responsible for receiving commands from iPhone companion app
- Usb manager: responsible for relaying the commands to the connected device
