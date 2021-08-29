// Mandelbrot set generator
// Based on the C# version at:http://csharphelper.com/blog/2014/07/draw-a-mandelbrot-set-fractal-in-c/
#include <SPI.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef struct rgb
{
  int r;
  int g;
  int b;
} rgb;

int width = 800;
int height = 480;

double yMin = -1.5;
double yMax = 1.5;
double xMin = -2.5;
double xMax = 1.5;
int MaxIterations = 60;
const int colourCount = 60;
uint16_t colours[colourCount];
// Work until the magnitude squared > 4.
const int MAX_MAG_SQUARED = 40;

void setup()
{
  Serial.begin(9600);
  if (!Serial) delay(5000);

  uint16_t ID = tft.readID();
  if (ID == 0xD3D3)
  {
    ID = 0x1963;
  }
  tft.begin(ID);
  pinMode(8, OUTPUT);  //backlight
  digitalWrite(8, HIGH);//on
  tft.fillScreen(WHITE);
  createColourArray();
  Serial.println("Calculate Mandelbrot set");
  Serial.print("("); Serial.print(xMin);Serial.print(", ");Serial.print(yMin);Serial.print(") -> (");
  Serial.print(xMax);Serial.print(", ");Serial.print(yMax);Serial.println(")");
  int startTime = millis();
  calculateMandelbrot();
  int endTime = millis();

  Serial.print("Calculation complete in: ");Serial.print(endTime - startTime);Serial.println("ms");
}

void loop()
{
  // put your main code here, to run repeatedly:

}

void createColourArray()
{

  // r > y > g > c > b > m > r
  // 255, 0, 0
  // 255, 255, 0
  // 0, 255, 0
  // 0, 255, 255.
  // 0, 0, 255
  // 255, 0, 255
  int steps = 10;
  int offset = 0;
  rgb stages[7];
  stages[0].r = 255; stages[0].g = 0; stages[0].b = 0;
  stages[1].r = 255; stages[1].g = 255; stages[1].b = 0;
  stages[2].r = 0; stages[2].g = 255; stages[2].b = 0;
  stages[3].r = 0; stages[3].g = 255; stages[3].b = 255;
  stages[4].r = 0; stages[4].g = 0; stages[4].b = 255;
  stages[5].r = 255; stages[5].g = 0; stages[5].b = 255;
  stages[6].r = 255; stages[6].g = 0; stages[6].b = 0;

  for(int r = 0; r < 6; r++)
  {
    rgb startCol = stages[r];
    rgb endCol = stages[r+1];

    double rDelta = (double)(endCol.r - startCol.r) / (double)steps;
    double gDelta = (double)(endCol.g - startCol.g) / (double)steps;
    double bDelta = (double)(endCol.b - startCol.b) / (double)steps;
    
    double newRed = startCol.r;
    double newGreen = startCol.g;
    double newBlue = startCol.b;
    for (int i = 0; i < steps; i++)
    {
      colours[offset] = tft.color565((int)newRed, (int)newGreen, (int)newBlue);
//      Serial.print("colour[");Serial.print(offset);Serial.print("] = (");Serial.print(newRed);Serial.print(", ");
//        Serial.print(newGreen);Serial.print(", ");Serial.print(newBlue);Serial.println(")");
      newRed += rDelta;
      newGreen += gDelta;
      newBlue += bDelta;
      offset++;
    }
  }

}

void calculateMandelbrot()
{
  double dReaC = (xMax - xMin) / (width - 1);
  double dImaC = (yMax - yMin) / (height - 1);
  double Zr =0;
double Zim = 0;
double Z2r =0;
double Z2im = 0;
  // Calculate the values.
  
  double ReaC = xMin;
  for (int X = 0; X < width; X++)
  {
    double ImaC = yMin;
    for (int Y = 0; Y < height; Y++)
    {
      double ReaZ = Zr;
      double ImaZ = Zim;
      double ReaZ2 = Z2r;
      double ImaZ2 = Z2im;
      int clr = 1;
      while ((clr < MaxIterations) && (ReaZ2 + ImaZ2 < MAX_MAG_SQUARED))
      {
        // Calculate Z(clr).
        ReaZ2 = ReaZ * ReaZ;
        ImaZ2 = ImaZ * ImaZ;
        ImaZ = 2 * ImaZ * ReaZ + ImaC;
        ReaZ = ReaZ2 - ImaZ2 + ReaC;
        clr++;
      }

      uint16_t colour =  colours[clr % colourCount];
      // Set the pixel's value.
      tft.drawPixel(X, Y,colour);
      ImaC += dImaC;
    }
    ReaC += dReaC;
  }
}
