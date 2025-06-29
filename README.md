# PortPal Connector logic

Working on a simple Bluetooth LE to USB bridge to send commands from an iPhone app 

All testing is currently being done on an AnyCubic 3d printer by sending Gcode commands.


# Plan
The plan here is to have each component of this run as a seperate task using FreeRTOS.

## Components:
- Bluetooth manager: responsible for receiving commands from iPhone companion app
- Usb manager: responsible for relaying the commands to the connected device
