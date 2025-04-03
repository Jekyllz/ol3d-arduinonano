#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <math.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Cube vertices
int16_t cube3D[8][3] = {
  {-30, -30, -30}, {30, -30, -30}, {30, 30, -30}, {-30, 30, -30},
  {-30, -30,  30}, {30, -30,  30}, {30, 30,  30}, {-30, 30,  30}
};



int16_t cube2D[8][2];
int16_t prevCube2D[8][2]; // For flicker reduction
float angleX = 0, angleY = 0, angleZ = 0;

// ---- Rotation functions moved HERE (before loop()) ----
void rotateX(float &x, float &y, float &z, float angle) {
  float yy = y * cos(angle) - z * sin(angle);
  float zz = y * sin(angle) + z * cos(angle);
  y = yy; z = zz;
}

void rotateY(float &x, float &y, float &z, float angle) {
  float xx = x * cos(angle) + z * sin(angle);
  float zz = -x * sin(angle) + z * cos(angle);
  x = xx; z = zz;
}

void rotateZ(float &x, float &y, float &z, float angle) {
  float xx = x * cos(angle) - y * sin(angle);
  float yy = x * sin(angle) + y * cos(angle);
  x = xx; y = yy;
}

void setup() {
  Serial.begin(115200);
  tft.init(135, 240);           // Initialize ST7789
  SPI.beginTransaction(SPISettings(9500000, MSBFIRST, SPI_MODE3)); // 8MHz SPI
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  // Erase old lines (black)
  for (int i = 0; i < 4; i++) {
    tft.drawLine(prevCube2D[i][0], prevCube2D[i][1], prevCube2D[(i+1)%4][0], prevCube2D[(i+1)%4][1], ST77XX_BLACK);
    tft.drawLine(prevCube2D[i+4][0], prevCube2D[i+4][1], prevCube2D[4+(i+1)%4][0], prevCube2D[4+(i+1)%4][1], ST77XX_BLACK);
    tft.drawLine(prevCube2D[i][0], prevCube2D[i][1], prevCube2D[i+4][0], prevCube2D[i+4][1], ST77XX_BLACK);
  }

  // Update angles
  angleX += 0.02;
  angleY += 0.03;
  angleZ += 0.2;

  // Project 3D to 2D
  for (int i = 0; i < 8; i++) {
    float x = cube3D[i][0], y = cube3D[i][1], z = cube3D[i][2];
    rotateX(x, y, z, angleX);
    rotateY(x, y, z, angleY);
    rotateZ(x, y, z, angleZ);
    cube2D[i][0] = (int16_t)(x + tft.width() / 2);
    cube2D[i][1] = (int16_t)(y + tft.height() / 2);
  }

  // Draw new lines (white)
  for (int i = 0; i < 4; i++) {
    tft.drawLine(cube2D[i][0], cube2D[i][1], cube2D[(i+1)%4][0], cube2D[(i+1)%4][1], ST77XX_GREEN);
    tft.drawLine(cube2D[i+4][0], cube2D[i+4][1], cube2D[4+(i+1)%4][0], cube2D[4+(i+1)%4][1], ST77XX_GREEN);
    tft.drawLine(cube2D[i][0], cube2D[i][1], cube2D[i+4][0], cube2D[i+4][1], ST77XX_GREEN);
  }

  memcpy(prevCube2D, cube2D, sizeof(cube2D));
  delay(5);
}
