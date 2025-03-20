# Trackless tram with smart stations

## Compiling and uploading to the ESP32
`arduino-cli compile --fqbn esp32:esp32:esp32 <sketchname>`
`arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 <sketchname>`

## Reading serial output
`stty -F /dev/ttyUSB0 <baudrate>`
`while true; do cat /dev/ttyUSB0; done`

## Calibrating the ultrasonic sensors
We used a ruler to place objects at known distances from the sensor, then compared the sensor output to the real distance.
This will tell us how accurate the sensors are, and if they are within acceptable margins of error.
