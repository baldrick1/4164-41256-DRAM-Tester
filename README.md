Arduino based 4164 / 41256 DRAM tester

This project hosts the source material to build a 4164 / 41256 DRAM chip tester based on an Arduino Nano.

Instructions: Insert chip to be tested into the ZIF socket, select 4164 or 41256 via toggle switch, power up the Arduino (either through the USB port or optional power port for standalone operation) and press the "START" button.

The green LED will blink thoughout the test.  A 4164 takes approximately 80 seconds to test, a 41256 will take longer.

If you connect the USB cable to your host computer and use the Arduino Serial Monitor, the tester will return a good / bad result and where the bad result occurred.
