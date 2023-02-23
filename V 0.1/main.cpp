#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <Adafruit_INA219.h>

#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_RESET -1
SSD1306AsciiWire display;

Adafruit_INA219 ina219;
volatile bool flag = false;

float voltage = 0, current = 0, power = 0;
char vbuf[16] = {0}, cbuf[16] = {0}, pbuf[16] = {0};

void setup()
{
  Wire.begin();
  Wire.setClock(400000L);
  display.begin(&Adafruit128x64, DISPLAY_ADDRESS, DISPLAY_RESET);
  display.set1X();
  display.setFont(X11fixed7x14);
  display.clear();

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

    flag = false;
  }
}

ISR(TIMER1_COMPA_vect)
{
  flag = true;
}
