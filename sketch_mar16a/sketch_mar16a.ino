#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h"
#include <TinkerKit.h>
#include <SPI.h>
#include <TFT.h>
#define dht_dpin A2

TKAccelerometer accelerometer(I0, I1);  // creating the object 'accelerometer' that belongs to the 'TKAccelerometer' class 
                                  // and giving the values to the desired input pins
boolean hasMoved;
byte bGlobalErr;
byte dht_dat[5];
byte past[10]; //last 10 temps read, contains 0s
int pastCount = 0;
int xAxisValue = 0;           // a variable to store theaccelerometer's x value
int yAxisValue = 10;           // a variable to store theaccelerometer's y value
int prevXPos = xAxisValue;
int prevYPos = yAxisValue;
int loops = 0;

// Screen
// pin definition for the Leonardo
#define cs   7
#define dc   0
#define rst  1

TFT TFTscreen = TFT(cs, dc, rst);

void setup(){
  InitDHT();
  setUpScreen();
  Serial.begin(9600);
  hasMoved = false;
  delay(300);
  loops = 0;
}

void loop(){
  ReadDHT();
  if (hasMoved && loops > 1) {
    switch (bGlobalErr){
      case 0: {
        Serial.print("Current humidity = ");
        Serial.print(dht_dat[0], DEC);
        Serial.print(".");
        Serial.print(dht_dat[1], DEC);
        Serial.print("% ");
        Serial.print("temperature = ");
        Serial.print(dht_dat[2], DEC);
        Serial.print(".");
        Serial.print(dht_dat[3], DEC);
        Serial.println("C ");
        int val = danger((int) dht_dat[2], (int) dht_dat[0]);
        if (val >= 2) { //danger zone
          drawFace(1);
          Serial.println("danger");
          takeAction(val);
        }
        else if (val == 1) {//val is 1, need to take action if temp is rising
          addPast((byte) val);
          int rising = 1; // 1 if rising, 0 if not rising   
               
          if (past[pastCount - 1] != 1) { //at least 2 1 in a row
            rising = 0;
          }        
          else { //check if rising sequence
            for (int i = 1; i < pastCount; i++) { //sustained 1 at least 3 cycles
              if (past[i] < past[i - 1] && pastCount > 0) { //descending sequence in cache
                 rising = 0;
              }
            }
          }
          if (rising) {
            Serial.println("warning");
            takeAction(val);
          }
        }      
        else { //val is 0, safe for now
          drawFace(0);
          Serial.println("okay");
        }
        break;
      }
      case 1:
        Serial.println("Error 1: DHT start condition 1 not met.");
        break;
      case 2:
        Serial.println("Error 2: DHT start condition 2 not met.");
        break;
      case 3:
        Serial.println("Error 3: DHT checksum error.");
        break;
      default:
        Serial.println("Error: Unrecognized code encountered.");
        break;
    }
  }  
  else {
    Serial.println("false");
    hasMoved = move();
    if(hasMoved) {
      Serial.println("MOVED!");
    }
  }
  delay(5000); //update every 5 seconds
  loops++;
}

void addPast(byte val) { //past is a cache holding last 5 values
  if (pastCount < 10) {
    past[pastCount] = val;
    pastCount++;
  }
  else { //array is filled, delete oldest, then add newest to end
    int x = 0;
    for (x = 1; x < 10; x++) {
      past[x - 1] = past[x];
    }
    past[x] = val;
    pastCount = 10;
  }
}

boolean move() {
  boolean moved = false;
  // read the both joystick axis values:
  xAxisValue = accelerometer.readX();  
  yAxisValue = accelerometer.readY(); 
  // print the results to the serial monitor:
  Serial.print("Accelerometer X = " );                       
  Serial.print(xAxisValue);   
  Serial.print("\t Accelerometer Y = " );                       
  Serial.println(yAxisValue);

  if (abs(prevXPos - xAxisValue) > 5 || abs(prevYPos - yAxisValue) > 5) {
    moved = true;
  }

  prevXPos = xAxisValue;
  prevYPos = yAxisValue;
  
  return moved;
}

/*This method returns an int corresponding to danger value of given
  temperature and humidity, 0 to 3, increasing danger.
  */
int danger(int temp, int hum) {
  if (temp > 40 || hum > 100) {
    return 3;
  }
  
  byte arr[20][17] = {  //initialize 2D array
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2},
    {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3},
    {0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,3},
    {0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3},
    {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,3},
    {0,0,1,1,1,1,2,2,2,2,3,3,3,3,3,3,3},
    {0,1,1,1,1,2,2,2,2,3,3,3,3,3,3,3,3},
    {1,1,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3},
    {1,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3},
    {1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3},
    {1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3},
    {2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}
  };
  int indexT = temp - 21;
  int indexH = ( hum - 20 ) / 5;
  return arr[indexT][indexH]; //return danger value
}

void setUpScreen() {
  // initialize the display
  TFTscreen.begin();
  // black background
  TFTscreen.background(0, 0, 0 );

  drawFace(0);
}

void drawFace(int a) {
  TFTscreen.fill(255, 255, 255);
  drawEyes();
  drawMouth(a);
  
}

void drawEyes() {
  int px = 4;
  int width = TFTscreen.width();
  int height = TFTscreen.height();
  //TFTscreen.stroke(255,255,255);
//  TFTscreen.fill(255, 255, 255);
  TFTscreen.rect((width*2)/8-px, ((height*4)/8)-px, 2*px, 2*px);
  TFTscreen.rect((width*6)/8-px, (height*4)/8-px, 2*px, 2*px);
}

void drawMouth(int a) {
  TFTscreen.fill(0, 0, 0);
  int px = 4;
  int width = TFTscreen.width();
  int height = TFTscreen.height();
  TFTscreen.fill(255, 255, 255);
  switch ( a ) {
  case 0:
    TFTscreen.rect((width*4)/8-3*px, (height*4)/8+px, 1*px, 1*px);
    TFTscreen.rect((width*4)/8-2*px, (height*4)/8+2*px, 4*px, 1*px);
    TFTscreen.rect((width*4)/8+2*px, (height*4)/8+px, 1*px, 1*px);
  case 1:
    TFTscreen.rect((width*4)/8-3*px, (height*4)/8+2*px, 1*px, 1*px);
    TFTscreen.rect((width*4)/8-2*px, (height*4)/8+px, 4*px, 1*px);
    TFTscreen.rect((width*4)/8+2*px, (height*4)/8+2*px, 1*px, 1*px);
  case 2:
    TFTscreen.rect((width*4)/8-3*px, (height*4)/8+px, 1*px, 1*px);
    TFTscreen.rect((width*4)/8-2*px, (height*4)/8+px, 4*px, 1*px);
    TFTscreen.rect((width*4)/8+2*px, (height*4)/8+px, 1*px, 1*px);
  default:
    break;
  }
}

