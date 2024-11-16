/*
  a_utility.ino: Colour matching algorithm
*/

int colour(){
  // Map the color measured within range
  for (int c = 0; c <= 2; c++) {
    colourArray[c] = (colourArray[c] - blackArray[c]) / greyDiff[c] * 255;
  }
  // Detect color
  int maxColor = maxx();
  if (colourArray[0]>245 && colourArray[1]>245 && colourArray[2]>245) return C_WHITE;
  if (maxColor == 2) return C_BLUE;
  if(maxColor == 1) return C_GREEN;
  if (colourArray[1] < 160 && colourArray[2] < 160) return C_RED;
  if (colourArray[1] < 160 || colourArray[2] < 160) return C_ORANGE;
  return C_PINK;
}