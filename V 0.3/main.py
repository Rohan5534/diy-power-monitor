# Import necessary libraries
import os
import time
from machine import I2C, SPI, Pin, Timer, RTC

import ina219
import sdcard
from ssd1306 import SSD1306_I2C

# I2C bus initialization with 400Khz frequency
i2c=I2C(0,sda=Pin(0), scl=Pin(1), freq=400000)

# Turn on the onboard LED when program runs
led = Pin(25, Pin.OUT)
led.value(1)

WIDTH  = 128                                   # oled display width
HEIGHT = 64                                    # oled display height

display = SSD1306_I2C(WIDTH, HEIGHT, i2c)      # Init oled display


# Flag that gets set after each second 
flag = False

# Timer callback handler
def f(t):
    global flag
    flag = True
    
# SPI bus initialization for SD card with 80Mhz frequency
spi = SPI(0, baudrate=80_000_000, polarity=0, phase=0, bits=8, sck=Pin(2), mosi=Pin(3), miso=Pin(4))
# Chip Select pin
CS = Pin(5, Pin.OUT)

#SD card initialization and mounting
sd = sdcard.SDCard(spi,CS)
vfs = os.VfsFat(sd)
os.mount(vfs, "/sd")


# INA219 sensor initialization
sensor = ina219.INA219(i2c)
voltage = 0
current = 0

# Writing the header line into values.csv file in SD card root directory
# If file is already present, it will be overwritten
with open("/sd/values.csv", "w") as file:
    file.write("Time,Voltage,Curret,Power\n")
    
# Initialize on board RTC with 00:00:00 HH:MM:SS and a generic date, month, year
rtc = RTC()
rtc.datetime((2023,01,01,1,00,00,00,00))

# Initialize hardware timer to interrupt at each second. f is callback function
timer =Timer(period = 1000, mode = Timer.PERIODIC, callback = f)

while True:
    # Read sensor data
    voltage = round(sensor.bus_voltage + sensor.shunt_voltage,3)
    current = round(sensor.current,1)
    power = round(voltage*current,3)
    
    # If flag is set , it means that 1 second has passed and we should write sensor readings to SD card
    if flag:
        
        # Total number of seconds since RTC initialization is give by formula :
        # Seconds = (rtc.datetime()[2]-1)*24*3600+rtc.datetime()[4]*3600+rtc.datetime()[5]*60+rtc.datetime()[6]
        # This formula considers upto 30 days since RTC startup.
        
        # This isnt really necessary (We could make a count=0 variable and increment it inside timer callback)
        # But I just wanted to use the on board RTC.
        with open("/sd/values.csv", "a") as file:
            file.write(f"{(rtc.datetime()[2]-1)*24*3600+rtc.datetime()[4]*3600+rtc.datetime()[5]*60+rtc.datetime()[6]},{voltage},{current},{power}\n")
        flag = False
    
    display.fill(0)
    display.text(f"{voltage : .3f} V",5,5)
    display.text(f"{current : .1f} mA",5,18)
    display.text(f"{power : .3f} mW",5,31)
    display.show()
    time.sleep(0.1)
