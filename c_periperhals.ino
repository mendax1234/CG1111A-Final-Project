/*
  Periperhals: buzzer sound & on-board arduino display colour
*/

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

/* Buzzer note definitions */
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// Define colors for FINISH
int ledColorsBuzzer[10][3] = {
  {0, 0, 255},    // BLUE
  {0, 255, 0},    // GREEN
  {255, 0, 0},    // RED
  {255, 100, 0},  // ORANGE
  {200, 0, 200},  // PINK
  {255, 255, 255},// WHITE
  {0, 255, 255},  // CYAN
  {255, 0, 255},  // PURPLE
  {255, 255, 0},  // YELLOW
  {128, 0, 128}   // VIOLET
};
int colorIndex = 0;  // Color index for cycling colors

// Interpolate and gradually change colors
void smoothColorTransition(int startColor[], int endColor[], uint32_t duration) {
  const int steps = 50;  // Number of steps for a smooth transition
  for (int i = 0; i <= steps; i++) {
    int r = startColor[0] + (endColor[0] - startColor[0]) * i / steps;
    int g = startColor[1] + (endColor[1] - startColor[1]) * i / steps;
    int b = startColor[2] + (endColor[2] - startColor[2]) * i / steps;
    RGBled.setColor(r, g, b);
    RGBled.show();
    // delay(duration / steps);
  }
}

void playNokiaMelody() {
  playNoteWithColor(NOTE_E5, 100); 
  playNoteWithColor(NOTE_D5, 100); 
  playNoteWithColor(NOTE_FS4, 200); 
  playNoteWithColor(NOTE_GS4, 200); 
  playNoteWithColor(NOTE_CS5, 100); 
  playNoteWithColor(NOTE_B4, 100); 
  playNoteWithColor(NOTE_D4, 200); 
  playNoteWithColor(NOTE_E4, 200); 
  playNoteWithColor(NOTE_B4, 100); 
  playNoteWithColor(NOTE_A4, 100); 
  playNoteWithColor(NOTE_CS4, 200); 
  playNoteWithColor(NOTE_E4, 200); 
  playNoteWithColor(NOTE_A4, 1000); 
}

void playMarioGameOver() {
  playNoteWithColor(NOTE_B4, 200); 
  playNoteWithColor(NOTE_F5, 300); 
  playNoteWithColor(NOTE_F5, 200); 
  playNoteWithColor(NOTE_F5, 200); 
  playNoteWithColor(NOTE_E5, 200); 
  playNoteWithColor(NOTE_D5, 200); 
  playNoteWithColor(NOTE_C5, 200); 
  playNoteWithColor(NOTE_G4, 100); 
  playNoteWithColor(NOTE_E4, 200); 
  playNoteWithColor(NOTE_C2, 500); 
  playNoteWithColor(REST, 2000); 
  playNoteWithColor(NOTE_C5, 200); 
  playNoteWithColor(REST, 200); 
  playNoteWithColor(NOTE_G4, 200); 
  playNoteWithColor(REST, 100); 
  playNoteWithColor(NOTE_E4, 200); 
  playNoteWithColor(REST, 100); 
  playNoteWithColor(NOTE_A4, 300); 
  playNoteWithColor(NOTE_B4, 300); 
  playNoteWithColor(NOTE_A4, 300); 
  playNoteWithColor(NOTE_GS4, 400); 
  playNoteWithColor(NOTE_AS4, 400); 
  playNoteWithColor(NOTE_G4, 400); 
  playNoteWithColor(NOTE_G4, 200); 
  playNoteWithColor(NOTE_D4, 200); 
  playNoteWithColor(NOTE_E4, 400); 
  buzzer.noTone();
}

void playNoteWithColor(uint16_t note, uint32_t duration) {
  int nextColorIndex = (colorIndex + 1) % 10;
  int *currentColor = ledColorsBuzzer[colorIndex];
  int *nextColor = ledColorsBuzzer[nextColorIndex];

  smoothColorTransition(currentColor, nextColor, duration);  // Smoothly transition LED colors
  buzzer.tone(note, duration);  // Play the note for its duration

  colorIndex = nextColorIndex;  // Update the color index
}

void celebrate() {
  playNokiaMelody();
  playMarioGameOver();
}
