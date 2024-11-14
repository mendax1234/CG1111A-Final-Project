/*
  a_utility.ino: Colour matching algorithm
*/

int colour(){
  // 0: Blue, 1: Green, 2: Red, 3: Orange, 4: Pink
  // delay(RGBWait);
  // Map the color measured within range
  
  for (int c = 0; c <= 2; c++) {
    colourArray[c] = (colourArray[c] - blackArray[c]) / greyDiff[c] * 255;
    // Serial.print(colourArray[c]);
    // Serial.print(" ");
  }
  // Serial.println();
  // for(int i=0;i<3;i+=1){ 
  //   Serial.print(colourArray[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
  // Detect color
  int maxColor = maxx();
  if (colourArray[0]>245 && colourArray[1]>245 && colourArray[2]>245) return C_WHITE;
  if (maxColor == 2) return C_BLUE;
  if(maxColor == 1) return C_GREEN;
  if (colourArray[1] < 160 && colourArray[2] < 160) return C_RED;
  if (colourArray[1] < 160 || colourArray[2] < 160) return C_ORANGE;
  return C_PINK;
}