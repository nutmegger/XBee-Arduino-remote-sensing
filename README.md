# XBee-Arduino-remote-sensing
Description of project:

Read temperature and humidity settings from remote sensors.  Transmit the readings to a central base.  Central base will POST the readings to a PHP form on web server where they are stored in a database table.  Web page fetches the readings and displays them in a charts using Angular and D3.js
<br>
Remote sensors: DHT22 sensor, Arduino UNO and XBee Series 2 module.  
<br>
<br>
Central base: XBee Series 2 module, sparkfun XBee shield, Arduino Ethernet shield, Arduino UNO
<br>
<br>
PHP form pushes the form data into a mySQL table
<br>
<br>
Another PHP script SELECTs the data via parameterized query into JSON
<br>
<br>
Web page readds the JSON data to make a pretty and robust chart with D3.js

