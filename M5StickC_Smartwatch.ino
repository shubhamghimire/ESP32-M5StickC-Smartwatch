#include <M5StickC.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"


const char* ssid = "put your wifi ssid here";
const char* password = "put your wifi password here";
const char* ntpServer = "ntp.jst.mfeed.ad.jp";

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

int battery = 0;
float b = 0;
int state = 1;


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Axp.ScreenBreath(10);  //Screen brightness 7-15

  setTime();


}

void loop() {
  // put your main code here, to run repeatedly:
  loadDisplay();
  batteryPercent();
  delay(1000);

}

void setTime(){

  // Connecting to Wi-Fi

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  delay(500);

  while (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected! Setting Time:");

    // Set NTP time to local

    configTime(9 * 3600, 3600, ntpServer);  // 9 is for Japan's timezone(UTC +9).


    // Get local time
    struct tm timeInfo;
    if(getLocalTime(&timeInfo)){

      // Set RTC Time
      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours = timeInfo.tm_hour;
      TimeStruct.Minutes = timeInfo.tm_min;
      TimeStruct.Seconds = timeInfo.tm_sec;
      M5.Rtc.SetTime(&TimeStruct);

      RTC_DateTypeDef DateStruct;
      DateStruct.WeekDay = timeInfo.tm_wday;
      DateStruct.Month = timeInfo.tm_mon + 1;
      DateStruct.Date = timeInfo.tm_mday;
      DateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetData(&DateStruct);
      
    }

    // Disconnect Wi-Fi if it did connect

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}

void loadDisplay(){

    static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);

    if(RTC_TimeStruct.Hours > 12) {      // Change to 12 hour format
      RTC_TimeStruct.Hours -= 12;
    }

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(13, 9);
    M5.Lcd.printf("%s %04d-%02d-%02d", wd[RTC_DateStruct.WeekDay], RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    
    M5.Lcd.drawLine(0 , 23, 159, 23, 0x07FF);
    
    M5.Lcd.setCursor(12, 35);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.printf("%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes);
    M5.Lcd.setCursor(110, 45);
    M5.Lcd.setTextColor(TFT_YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("%02d", RTC_TimeStruct.Seconds);
    
}

void batteryPercent(){

  M5.Lcd.setCursor(115, 9);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(GREEN);
  b = M5.Axp.GetVbatData() * 1.1 / 1000;
  battery = ((b - 3.0) / 1.2) * 100;

  if (battery>100){
    battery = 100;
  }
  else if (battery < 100 && battery > 9){
    M5.Lcd.print(" ");
  }
  else if(battery < 9){
    M5.Lcd.print("  ");
  }
  if (battery < 10){
    M5.Axp.DeepSleep();
  }
  M5.Lcd.print(battery);
  M5.Lcd.print("%");
}
