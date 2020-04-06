#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS.h>
 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON 2 // pin 2 for the event button
 
#define OLED_RESET -1
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
TinyGPS gps;
SoftwareSerial gpsSerial(3, 4);
 
char buf[22];

float max_speed = 0;
 
void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // LCD init failed
    Serial.println(F("Display failed to initialize"));
    for (;;);
  }
  gpsSerial.begin(9600);

  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), button_pressed, FALLING);

  display.clearDisplay();
  display.setCursor(53, 30);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(F("READY"));
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(25, 30);
  display.print(F("NO GPS SIGNAL"));
  display.display();
  Serial.println(F("Ready"));
}
 
void display_print(unsigned int x, unsigned int y, const char* text) {
  display.setCursor(x,y);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
 
  display.print(text);
  display.display();
}

void button_pressed() {
  max_speed = 0;
}
 
void loop() {
  while(gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      float lat;
      float lon;
      float float_kph;
      char strbuf[10];
      //float alt;
      int year;
      byte month, day, hour, minute, second, hundredth;
      
      gps.f_get_position(&lat, &lon);
      //gps.get_datetime(&date, &time);
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth);

      //alt = gps.f_altitude();
      float_kph = gps.f_speed_kmph();
      if (float_kph > max_speed)
        max_speed = float_kph;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      dtostrf(lat, 8, 6, strbuf);
      sprintf(buf, "lat: %s", strbuf);
      display.print(buf);
      
      display.setCursor(0, 10);
      dtostrf(lon, 8, 6, strbuf);
      sprintf(buf, "lon: %s", strbuf);
      display.print(buf);
      
      display.setCursor(0, 20);
      dtostrf(float_kph, 6, 2, strbuf);
      sprintf(buf, "speed: %s kph", strbuf);
      display.print(buf);

      display.setCursor(0, 30);
      dtostrf(max_speed, 6, 2, strbuf);
      sprintf(buf, "max spd: %s kph", strbuf);
      display.print(buf);
      /*
      display.setCursor(0, 40);
      dtostrf(alt, 4, 2, strbuf);
      sprintf(buf, "alt: %s m", strbuf);
      display.print(buf);
      */
      display.setCursor(0, 40);
      sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
      display.print(buf);

      display.display();
    }
  }
}
