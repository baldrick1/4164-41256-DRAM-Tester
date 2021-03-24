/*  
**  4164 - 41256 DRAM TESTER
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
  
**  
**  Derived from work created by:
**  _
** |_|___ ___
** | |_ -|_ -|
** |_|___|___|
**  iss(c)2020
**
**  Based on a project hosted at: https://forum.defence-force.org/viewtopic.php?f=9&t=1699
**
*/

#include <SoftwareSerial.h>

#define DI          15  // PC1  4164/41256 pin 2
#define DO           8  // PB0  4164/41256 pin 14
#define CAS          9  // PB1  4164/41256 pin 15
#define RAS         17  // PC3  4164/41256 pin 4
#define WE          16  // PC2  4164/41256 pin 3

#define XA0         18  // PC4  4164/41256 pin 5
#define XA1          2  // PD2  4164/41256 pin 7
#define XA2         19  // PC5  4164/41256 pin 6
#define XA3          6  // PD6  4164/41256 pin 12
#define XA4          5  // PD5  4164/41256 pin 11
#define XA5          4  // PD4  4164/41256 pin 10
#define XA6          7  // PD7  4164/41256 pin 13
#define XA7          3  // PD3  4164/41256 pin 10
#define XA8         14  // PC0  4164 N/C / 41256 pin 1

#define M_TYPE      10  // PB2
#define R_LED       11  // PB3
#define G_LED       12  // PB4

#define RXD          0  // PD0
#define TXD          1  // PD1

#define BUS_SIZE     9

/* ================================================================== */
volatile int bus_size;

//SoftwareSerial USB(RXD, TXD);

const unsigned int a_bus[BUS_SIZE] = {
  XA0, XA1, XA2, XA3, XA4, XA5, XA6, XA7, XA8
};

void setBus(unsigned int a) {
  int i;
  for (i = 0; i < BUS_SIZE; i++) {
    digitalWrite(a_bus[i], a & 1);
    a /= 2;
  }
}

void writeAddress(unsigned int r, unsigned int c, int v) {
  /* row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* rw */
  digitalWrite(WE, LOW);

  /* val */
  digitalWrite(DI, (v & 1) ? HIGH : LOW);

  /* col */
  setBus(c);
  digitalWrite(CAS, LOW);

  digitalWrite(WE, HIGH);
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
}

int readAddress(unsigned int r, unsigned int c) {
  int ret = 0;

  /* row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* col */
  setBus(c);
  digitalWrite(CAS, LOW);

  /* get current value */
  ret = digitalRead(DO);

  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);

  return ret;
}

void error(int r, int c)
{
  unsigned long a = ((unsigned long)c << bus_size) + r;
  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, HIGH);
  interrupts();
  Serial.println();
  Serial.print("CHIP FAILED TEST $");
  Serial.println(a, HEX);
  Serial.flush();
  while (1)
    ;
}

void ok(void)
{
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, LOW);
  interrupts();
  Serial.println();
  Serial.println("CHIP TESTED OK!");
  Serial.flush();
  while (1)
    ;
}

void blink(void)
{
  digitalWrite(G_LED, LOW);
  digitalWrite(R_LED, LOW);
  delay(1000);
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);
}

void green(int v) {
  digitalWrite(G_LED, v);
}

void fill(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1 << bus_size); c++) {
    green(g ? HIGH : LOW);
    for (r = 0; r < (1 << bus_size); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
    }
    g ^= 1;
  }
  blink();
}

void fillx(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1 << bus_size); c++) {
    green(g ? HIGH : LOW);
    for (r = 0; r < (1 << bus_size); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
      v ^= 1;
    }
    g ^= 1;
  }
  blink();
}

void setup() {
  int i;

  Serial.begin(9600);
  while (!Serial)
    ; /* wait */

  Serial.println();
  Serial.print("4164 - 41256 DRAM TESTER");

  Serial.println();
  Serial.print("Licensed under GPL v3.0");
  Serial.println();
  
  for (i = 0; i < BUS_SIZE; i++)
    pinMode(a_bus[i], OUTPUT);

  pinMode(CAS, OUTPUT);
  pinMode(RAS, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(DI, OUTPUT);

  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);

  pinMode(M_TYPE, INPUT);
  pinMode(DO, INPUT);

  digitalWrite(WE, HIGH);
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);

  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);

  if (digitalRead(M_TYPE)) {
    /* jumper not set - 41256 */
    bus_size = BUS_SIZE;
    Serial.print("CHIP IS 256K x 1");
  } else {
    /* jumper set - 4164 */
    bus_size = BUS_SIZE - 1;
    Serial.print("CHIP IS 64K x 1");
  }
  Serial.flush();

  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, LOW);

  noInterrupts();
  for (i = 0; i < (1 << BUS_SIZE); i++) {
    digitalWrite(RAS, LOW);
    digitalWrite(RAS, HIGH);
  }
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);
}

void loop() {
  interrupts(); Serial.print("."); Serial.flush(); noInterrupts(); fillx(0);
  interrupts(); Serial.print("."); Serial.flush(); noInterrupts(); fillx(1);
  interrupts(); Serial.print("."); Serial.flush(); noInterrupts(); fill(0);
  interrupts(); Serial.print("."); Serial.flush(); noInterrupts(); fill(1);
  ok();
}
