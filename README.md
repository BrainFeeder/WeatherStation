# WeatherStation
Code for an Arduino UNO driven weather station, using the Sparkfun Weather Shield

## BOM
- Arduino UNO R3 model B (or similar UNO compatible board)
- Sparkfun Weather Shield for Arduino Uno R3

## Arduino libraries
- Wire.h
- SparkFunMPL3115A2.h
- SparkFun_Si7021_Breakout_Library.h
- ArduinoJson.h

## Introduction
As an Arduino starter I wanted to built my first complete, and usable, project. A weather station seemed to be the next logical thing to do. As I was browsing for ideas I stumbled upon many instructables from others, but most of them kept it basic as in 'reading out data from the sensors'. Thus, not very exiting.

## Zambretti
The [Zambretti Forecaster](https://en.wikipedia.org/wiki/Zambretti_Forecaster) is a forecasting tool which we can implement as an algorithm to use with our sensor readings. A [Zambretti algorithm](http://drkfs.net/zambretti.htm) will return an index `Z` indicating a description of short term weather changes. Originally the Zambretti Forecaster was a table joining the descriptions to the alphabet. 

The algorithm idea I used as a base was borrowed from [fandonov/weatherstation](https://github.com/fandonov/weatherstation), another Arduino weather station project (with other sensors and *bling*).

## TODO
- Add WiFi shield
- Run HTTP server to serve JSON (or) Send HTTP request
- Port to Arduino MEGA for extra connectors
- Add solar charging battery power supply
- Add LCD display
- ...
