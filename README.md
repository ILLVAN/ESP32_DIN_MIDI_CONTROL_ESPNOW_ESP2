# ESP32_DIN_MIDI_CONTROL_ESPNOW_ESP2
Using ESP32 boards to build a multinode MIDI controller with each node being responsible for a different task.

Communication between boards via ESPNOW.

ESP 1 - MIDI I/O: Arrange
ESP 2 - Display and Inputs
ESP 3 - MIDI I/O: TimeLine hidden features control: in sync looping, clock on/off etc.
ESP 4 - Web Server

WORK IN PROGRESS It's public for easily being able to talk about it with others, not because its done. 
ESP 2 is in a development state, not soldered yet. Right now it displays data from ESP 1 and ESP 3 and has a Poti which controls the loop level of ESP 3.
