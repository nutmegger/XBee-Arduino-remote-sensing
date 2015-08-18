# XBee-Arduino-remote-sensing
For this project, XBee Modules are Series 2, configured in API mode, using frames.

General structure of frames:<br>
Byte 1:       Always 0x7E<br>
Byte 2:       Most significant byte (MSB), usually zero unless the number you are trying to describe is > 255<br>
Byte 3:       Least significant byte (LSB), along with the MSB, a representation of how many bytes you're going to send across in this frame<br>
Bytes 4 to n: Frame data<br>
Byte n+1:     checksum<br>
<br>
Generally, you have to build up a frame programatically and send it.<br>

