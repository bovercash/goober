#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SSD1306_NO_SPLASH
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// NANO 19(SDA) 18 (SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Encoder
#define encoderP1  8
#define encoderP2  9
#define encoderBtn  10
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastEncoderValue = 0;

// Menu State
#define MENU_STATE_SELECT 0
#define MENU_STATE_ENCODER 1
byte menuState = 0;
int dl = 0;

// Encoder Test State
float selectedAngle = 0;
byte btnPressed = 0;
float rad = 0.0;


void setup() {
   Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever

    //TODO: Play beep to indicate problem
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  displayMenu();

  //enable encoder
  pinMode(encoderP1, INPUT);
  pinMode(encoderP2, INPUT);
  
  digitalWrite(encoderP1, HIGH); //turn pullup resistor on
  digitalWrite(encoderP2, HIGH); //turn pullup resistor on
  
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  //attachInterrupt(digitalPinToInterrupt(encoderP1), updateEncoder, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(encoderP2), updateEncoder, CHANGE);

  menuState = MENU_STATE_ENCODER;
}

void loop() {
  dl++;
  updateEncoder();

  if(dl > 10000){
    dl = 0;
    if(menuState == MENU_STATE_ENCODER) {
      processEncoderDemo();
    }
  }
}

void processEncoderDemo(void) {
  if(lastEncoderValue == 0){
    lastEncoderValue = encoderValue;
  }

  if(encoderValue > lastEncoderValue){
    selectedAngle = selectedAngle - 30;
    if(selectedAngle < 0){
      selectedAngle = 359;
    }
    lastEncoderValue = encoderValue;
    Serial.println(selectedAngle);
  } else if(encoderValue < lastEncoderValue){
    selectedAngle = selectedAngle + 30;
    if(selectedAngle > 359) {
      selectedAngle = 0;
    }
    lastEncoderValue = encoderValue;
    Serial.println(selectedAngle);
  }

  
  
  rad = toRads(selectedAngle);

  unsigned px = SCREEN_WIDTH/2 + cos(rad)*20;
  unsigned int py = SCREEN_HEIGHT/2 + 10 + sin(rad)*20;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); 
  display.println(F("Encoder"));
  
  display.drawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 10, 20, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 10, px, py, SSD1306_WHITE);
  display.drawCircle(px,py,4,SSD1306_WHITE);
  display.setCursor(5, 55);
  display.setTextSize(1);
  display.print(selectedAngle);
  display.display();
}

float toRads(float deg) {
  return deg / 360 * 2 * 3.14159267;
}

void updateEncoder(){
  int MSB = digitalRead(encoderP1); //MSB = most significant bit
  int LSB = digitalRead(encoderP2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB;
  
  //converting the 2 pin value to single number 
  int sum = (lastEncoded << 2) | encoded;
   
  //adding it to the previous encoded value 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011){
    encoderValue ++;
  }

  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
    encoderValue --;
  }

  lastEncoded = encoded; //store this value for next time 
} 

void displayMenu(void) {
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);

  // HEADER
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println(F("Goober V0.1"));
  
  // Menu 
  display.setCursor(0, 20);
  display.setTextSize(1);

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
  display.println(F("Rotary Test"));

  display.setTextColor(SSD1306_WHITE);
  display.println(F("Vibration Test"));
  display.println(F("Sound Test"));

  display.display();
}


