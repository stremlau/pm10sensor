#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <math.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9
#include <SDS011.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

float pm10, pm25;
int lastPm10 = -1;
int lastPm25 = -1;
int error;
int i = 0;

SDS011 my_sds;

void setup() {
	my_sds.begin(3,2);
	Serial.begin(9600);

  // paint display black
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(2);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);

  // add basic UI labels
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(5, 5);
  tft.print("PM");
  tft.setCursor(5, 13);
  tft.print("10");

  tft.setCursor(85, 5);
  tft.print("PM");
  tft.setCursor(85, 13);
  tft.print("2.5");
}

void loop() {
	error = my_sds.read(&pm25,&pm10);
 
	if (!error) { // when there are new values (every second)
		Serial.println("P2.5: "+String(pm25));
		Serial.println("P10:  "+String(pm10));

   if ((int) pm10 != lastPm10) { //PM10 value changed
     lastPm10 = (int) pm10;

     // clear screen
     tft.fillRect(24, 5, 56, 14, ST7735_BLACK); 

     // write pm10 value
     tft.setCursor(24, 5);
     tft.setTextSize(2);
     tft.setTextColor(ST7735_WHITE);
     tft.print(String(lastPm10));
   }

   if ((int) pm25 != lastPm25) {
     lastPm25 = (int) pm25;

     // clear screen
     tft.fillRect(110, 5, 50, 14, ST7735_BLACK); 
     
     // write pm10 value
     tft.setCursor(110, 5);
     tft.setTextSize(2);
     tft.setTextColor(ST7735_WHITE);
     tft.print(String(lastPm25));
   }

   int dispPm10 = lastPm10;
   uint16_t color = ST7735_BLUE;
   
   if (dispPm10 > 100) {
    dispPm10 = 100;
    color = ST7735_RED;
   }
   else if (dispPm10 >= 51) {
    // dark orange
    color = 0b1111110001100000;
   }
   else if (dispPm10 >= 36) {
    color = ST7735_YELLOW;
   }
   else if (dispPm10 >= 21) {
    color = ST7735_CYAN;
   }
   else if (dispPm10 >= 11) {
    // light blue
    color = 0b0000110011111111;
   }
   else if (dispPm10 >= 0) {
    color = ST7735_BLUE;
   }

   // clear the next three columns
   tft.drawFastVLine(i, 28, 100, ST7735_BLACK);
   tft.drawFastVLine(i+1, 28, 100, ST7735_BLACK);
   tft.drawFastVLine(i+2, 28, 100, ST7735_BLACK);

   // draw new column
   tft.drawFastVLine(i, 128 - dispPm10, dispPm10, color);
   //tft.drawPixel(i, 128 - lastP25, ST7735_BLACK);

   // redraw threshold line on top
   tft.drawFastHLine(0, 128 - 50, 160, ST7735_YELLOW);

   // increment column number
   i++;
   if (i == 160) i = 0;
	}
	delay(100);
}
