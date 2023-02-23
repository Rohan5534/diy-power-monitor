
### This version is a very simple version that just reads values from INA219 and spits it out onto SSD1306 128 x 64 display.

---

### Improvements to make in next version : 

* Add SD card support to log reading values
* Dynamically change the unit of displayed value according to value.
	* Ex - if value is 330.45 mA show it as is. But if the value is 1233.45 mA show it as 1.23 A.
* Add a tactile button which when pressed will cause the arduino to create a new logging file and start logging values there and when
  the button is pressed again, that file will close and will continue in normal logging file.
* Make whole thing battery powered (probably single 18650 battery with boost converter).
* Make everthing on a PCB board instead of breadboard.

 
