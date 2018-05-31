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
The [Zambretti Forecaster](https://en.wikipedia.org/wiki/Zambretti_Forecaster) is a forecasting tool which we can implement as an algorithm to use with our sensor readings. A [Zambretti algorithm](http://drkfs.net/zambretti.htm) will return an index `Z` indicating a description of short term weather changes. Originally the Zambretti Forecaster was a table joining the descriptions to the alphabet:

*A* Settled Fine
B Fine Weather
C Becoming Fine
D Fine Becoming Less Settled
E Fine, Possibly showers
F Fairly Fine, Improving
G Fairly Fine, Possibly showers, early
H Fairly Fine Showery Later
I Showery Early, Improving
J Changeable Mending
K Fairly Fine , Showers likely
L Rather Unsettled Clearing Later
M Unsettled, Probably Improving
N Showery Bright Intervals
O Showery Becoming more unsettled
P Changeable some rain
Q Unsettled, short fine Intervals
R Unsettled, Rain later
S Unsettled, rain at times
T Very Unsettled, Finer at times
U Rain at times, worse later.
V Rain at times, becoming very unsettled
W Rain at Frequent Intervals
X Very Unsettled, Rain
Y Stormy, possibly improving
Z Stormy, much rain
