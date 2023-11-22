#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <DS3231.h>

String nama1 = "Ryan Andriyani";
String nama2 = "Mahesa Putra";
String nama3 = "Ilham Fauzi";
String nama4 = "Hanif Hilmi";

DS3231 rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte smile[8] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000
};
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Time t;
int Hor;
int Min;
int Sec;

String day1, day2, day3, day4;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  rtc.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, smile);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  Serial.println("LABEL,Day,Date,Name,Time,Info");
  while (!Serial)
    ;  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
}
void loop()  // run over and over again
{
  lcd.setCursor(0, 0);
  lcd.print(rtc.getDOWStr(1));  //hari
  lcd.setCursor(3, 0);
  lcd.print(",");

  lcd.setCursor(4, 0);
  lcd.print(rtc.getDateStr());  //tanggal

  lcd.setCursor(00, 1);
  lcd.print(rtc.getTimeStr());  //jam

  lcd.setCursor(12, 1);
  lcd.print(rtc.getTemp());  //suhu
  getFingerprintID();
  delay(1000);

  t = rtc.getTime();
  Hor = t.hour;
  Min = t.min;
  Sec = t.sec;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK && finger.fingerID == 1 && Hor < 8 && day1 != rtc.getDOWStr(2)) {
    day1 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(1, 1);
    lcd.print(nama1);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama1);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("On Time");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 1 && day1 != rtc.getDOWStr(2)) {
    day1 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(1, 1);
    lcd.print(nama1);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama1);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("Late");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Already");
    lcd.setCursor(0, 1);
    lcd.print("Absent Today");
    delay(2000);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 2 && Hor < 8 && day2 != rtc.getDOWStr(2)) {
    day2 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama2);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama2);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("On Time");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 2 && day2 != rtc.getDOWStr(2)) {
    day2 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama2);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama2);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("Late");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Already");
    lcd.setCursor(0, 1);
    lcd.print("Absent Today");
    delay(2000);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 3 && Hor < 8 && day3 != rtc.getDOWStr(2)) {
    day3 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama3);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama3);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("On Time");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 3 && day3 != rtc.getDOWStr(2)) {
    day3 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama3);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama3);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("Late");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Already");
    lcd.setCursor(0, 1);
    lcd.print("Absent Today");
    delay(2000);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 4 && Hor < 8 && day4 != rtc.getDOWStr(2)) {
    day4 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama4);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama4);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("On Time");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 4 && day4 != rtc.getDOWStr(2)) {
    day4 = rtc.getDOWStr(2);
    lcd.clear();
    digitalWrite(8, HIGH);
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
    lcd.write(0);
    lcd.setCursor(3, 1);
    lcd.print(nama4);
    Serial.print("DATA,");
    Serial.print(rtc.getDOWStr(2));
    Serial.print(",");
    Serial.print(rtc.getDateStr());
    Serial.print(",");
    Serial.print(nama4);
    Serial.print(",");
    Serial.print(rtc.getTimeStr());
    Serial.print(",");
    Serial.println("Late");
    delay(2000);
    digitalWrite(8, LOW);
    lcd.clear();
  } else if (p == FINGERPRINT_OK && finger.fingerID == 4) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Already");
    lcd.setCursor(0, 1);
    lcd.print("Absent Today");
    delay(2000);
    lcd.clear();
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    digitalWrite(9, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("Not  Match");
    delay(2000);
    digitalWrite(9, LOW);
    lcd.clear();
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
