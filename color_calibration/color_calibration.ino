#include "MeMCore.h"

/* Define constants */
#define RGBWait 100
#define LDRWait 30
#define LED 13

/* Initialisation of color-sensing constants */
const char R = 0, G = 1, B = 2;

/* Define variables necessary for mBot */
float colourArray[] = { 0, 0, 0 };
int status = 0;
enum Color {  
  C_BLUE, C_GREEN, C_RED, C_ORANGE, C_PINK, C_WHITE
};

/* Setup ports */
MeRGBLed RGBled(0, 30);
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);

void setup() {
  pinMode(A7, INPUT);//button
  Serial.begin(9600);
  RGBled.setpin(LED);
}

int getAvgReadingLDR(int times){      
  int reading;
  int total =0;
  for (int i = 0; i < times; i++){
    reading = ir_adapter.aRead2();
    total = reading + total;
    delay(LDRWait);
  }
  return total / times;
}

/* Color Sensor - Read */
void readColor(){
  // Turn ON RED LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[0] = getAvgReadingLDR(3);
  
  // Turn ON GREEN LED
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[1] = getAvgReadingLDR(3);

  // Turn ON BLUE LED
  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  colourArray[2] = getAvgReadingLDR(3);

  delay(RGBWait);
}

int maxx(){
  // Find the three colors with the highest proportion
  int max = 0;
  for (int c = 1; c <= 2; c++){
    if (colourArray[c] > colourArray[max]){
      max = c;
    }
  }
  return max;
}

/* Detect Colour */
int colour(){
  readColor();
  delay(RGBWait);
  // Map the color measured within range
  for (int c = 0; c <= 2; c++) {
    /* Test the other colours */
    //colourArray[c] = (colourArray[c] - blackArray[c]) / greyDiff[c] * 255;
    /* Get the white and black array value */
    Serial.print(colourArray[c]);
    Serial.print(" ");
  }
  Serial.println();
  // Detect color
  int maxColor = maxx();
  if (colourArray[0]>245 && colourArray[1]>245 && colourArray[2]>245) return C_WHITE;
  if (maxColor == 2) return C_BLUE;
  if(maxColor == 1) return C_GREEN;
  if (colourArray[1] < 160 && colourArray[2] < 160) return C_RED;
  if (colourArray[1] < 160 || colourArray[2] < 160) return C_ORANGE;
  return C_PINK;
}

/* mBot LED */
void display_color(int c) {
  int ledColors[6][3] = {
    {0, 0, 255},    // BLUE
    {0, 255, 0},    // GREEN
    {255, 0, 0},    // RED
    {255, 100, 0},  // ORANGE
    {200, 0, 200},  // PINK
    {255, 255, 255} // WHITE
  };

  RGBled.setColor(ledColors[c][R], ledColors[c][G], ledColors[c][B]);
  RGBled.show();  // Show colours on-board arduino
}

void loop() {
  if (analogRead(A7) < 100) {
    status = 1 - status;
    delay(500);
  }
  if (status == 1){
    int predicted_color = colour();
    display_color(predicted_color);
    delay(1000);
  }
}
