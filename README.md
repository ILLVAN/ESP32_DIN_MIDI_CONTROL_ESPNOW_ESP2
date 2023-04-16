# ESP32_DIN_MIDI_CONTROL_ESPNOW_ESP2
Using ESP32 boards to build a multinode MIDI controller with each node being responsible for a different task.

Communication between boards via ESPNOW.

ESP 1 - MIDI I/O: Arrange <br>
ESP 2 - Display and Inputs <br>
ESP 3 - MIDI I/O: TimeLine hidden features control: in sync looping, clock on/off etc. <br>
ESP 4 - Web Server <br>

WORK IN PROGRESS It's public for easily being able to talk about it with others, not because its done. <br>
ESP 2 is in a development state, not soldered yet. Right now it displays data from ESP 1 and ESP 3 and has a poti which controls the loop level of ESP 3.
