### This version uses a raspberry pi pico as the brains of the operation. It uses micropython to read from sensor , write that to display and to SD card.

---

### Features : 

All the previous version features plus - 

* The pi pico board has on board buck boost convert, so we can power the whole system with just a 3.7 Lithium battery (with a TP4056 protection module ofc)


### Improvements :

* Read the battery voltage on `vsys` pin and display it.
