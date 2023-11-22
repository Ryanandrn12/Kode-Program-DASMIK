#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS3231  rtc(SDA, SCL);

void setup(){
  rtc.begin();
  lcd.init();
  lcd.backlight();
  
  rtc.setDate(21, 11, 2023);   //21 November 2023
  rtc.setTime(11, 56, 30);    //08:00:00
  rtc.setDOW(2);              //Selasa
}

void loop(){
  lcd.setCursor(0, 0);
  lcd.print(rtc.getDOWStr(1)); //hari
  lcd.setCursor(3, 0);
  lcd.print(",");

  lcd.setCursor(4, 0);
  lcd.print(rtc.getDateStr()); //tanggal

  lcd.setCursor(00, 1);
  lcd.print(rtc.getTimeStr()); //jam

  lcd.setCursor(12, 1);
  lcd.print(rtc.getTemp()); //suhu

  delay (1000);
}
