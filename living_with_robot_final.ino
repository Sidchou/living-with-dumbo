#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1

#include <RTCZero.h>

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 0;
/* Change these values to set the current initial date */
byte day = 29;
const byte month = 4;
const byte year = 2020;
byte tseconds = 0;
byte tminutes = 0;
byte thours = 0;

const int button1 = 2;
const int button2 = 1;
const int light = 3;
bool working = false;
bool state = false;
bool pstate = false;
float trans = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(light, OUTPUT);


  rtc.begin(); // initialize RTC

  // Set the time & date
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize display
  display.clearDisplay(); // Clear display before each start
  delay(100);

  analogWrite(light, 0);
}

void loop() {

  display.clearDisplay(); // Clear display before each start


  state = digitalRead(button1); // read button1
  // if button1 toggle on
  if (state && !pstate) {
    working = !working; // change state
  }
  pstate = state; // set button1 for toggle

  // if in working state
  if (working) {
    thours = rtc.getHours();
    tminutes = rtc.getMinutes();
    tseconds = rtc.getSeconds(); // set timer to last time paused

    displayReadings(0, 15, "working"); // display state

    // transition timer
    if (trans < 1) {
      trans += 0.01; //fade in
    }

    //    lightCycle(trans); // display light
  } else {
    // if in rest state
    rtc.setTime(thours, tminutes, tseconds); // pause timer

    displayReadings(0, 15, "rest");
    // transition timer
    if (trans > 0) {
    trans -= 0.01; // fade out
    }
    //    analogWrite(light, 0); // turn light off
  }

  lightCycle(trans); // display light


  // reset for next day
  if (digitalRead(button2)) {
    day++;
    rtc.setDate(day, month, year);
    rtc.setTime(hours, minutes, seconds);
    tseconds = 0;
    tminutes = 0;
    thours = 0;
    working = false;
  }


  //print time
  String t = "";
  t += print2digits(rtc.getHours());
  t += ":";
  t += print2digits(rtc.getMinutes());
  t += ":";
  t += print2digits(rtc.getSeconds());
  displayReadings(0, 35, t);

  //print date
  String d = "";
  d += print2digits(rtc.getDay());
  d += "/";
  d += print2digits(rtc.getMonth());
  d += "/";
  d += print2digits(rtc.getYear());
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.print(d);

  display.display();//display



}

void displayReadings(int a, int b, String message) {
  //  String message = "working";
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(a, b);
  display.print(message);
}

String print2digits(int number) {
  String n = "";
  n += number;
  if (number < 10) {
    n =  "0" + n ; // print a 0 before if the number is < than 10
  }
  return n;
}

void   lightCycle(float trans) {
  float dt = (float)millis();
  dt *= 6.283;  //rescale period form 2pi to 1
  dt /= 4000;  // rescale period from 1 to 4
  float t = (sin(dt) * 0.5 + 0.5); // 0 << >> 1
  t *= 200;
  t += 30;// 30<< >> 230

  t *= trans;//transition
  analogWrite(light, t);
}
