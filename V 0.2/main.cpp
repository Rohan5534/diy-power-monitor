#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_RESET -1
SSD1306AsciiWire display;

#define CHIPSELECT 10
#define ENABLE_DEDICATED_SPI 1
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#define SPI_DRIVER_SELECT 0

SdFat32 sd;
File32 file;

Adafruit_INA219 ina219;

float voltage = 0, current = 0, power = 0;
char vbuf[16] = {0}, cbuf[16] = {0}, pbuf[16] = {0};

char buf[64] = {0};

volatile bool flag = false;
volatile int count = 0;

void setup()
{
  Wire.begin();
  Wire.setClock(400000L);
  display.begin(&Adafruit128x64, DISPLAY_ADDRESS, DISPLAY_RESET);
  display.set1X();
  display.setFont(X11fixed7x14);
  display.clear();

  sd.begin(CHIPSELECT);

  ina219.begin();

  ADCSRA = 0;
  ACSR = 0x80;
  cli();
  // Initial setup
  TCCR1A = 0, TCCR1B = 0, TCNT1 = 0;
  // 10Hz frequency
  OCR1A = 24999;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // 64 prescaler
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();

  file.open("readings.csv", O_WRITE | O_CREAT | O_TRUNC);
  file.print("Time,Voltage,Current,Power\n");
  file.sync();
  file.seekEnd();
}
void loop()
{

  if (flag)
  {
    ina219.powerSave(false);
    voltage = ina219.getBusVoltage_V() + ((ina219.getShuntVoltage_mV()) / 1000.0);
    current = ina219.getCurrent_mA();
    power = voltage * current;
    ina219.powerSave(true);

    dtostrf(voltage, 10, 3, vbuf);
    vbuf[11] = '\0';
    display.setCursor(4, 1);
    display.print(vbuf);
    display.println(" V");

    dtostrf(current, 10, 3, cbuf);
    cbuf[11] = '\0';
    display.setCursor(4, 3);
    display.print(cbuf);
    display.println(" mA");

    dtostrf(power, 10, 3, pbuf);
    pbuf[11] = '\0';
    display.setCursor(4, 5);
    display.print(pbuf);
    display.println(" mW");

    display.setCursor(100, 6.5);

    flag = false;

    if (count > 9)
    {
      sprintf(buf, "%lu,%s,%s,%s\n", millis() / 1000, vbuf, cbuf, pbuf);
      if (!file.write(buf))
      {
        file.write(buf);
      }
      if (!file.sync())
      {
        file.sync();
      }
      display.println(millis() / 1000);
      count = 0;
    }
  }
}

ISR(TIMER1_COMPA_vect)
{
  flag = true;
  count++;
}
