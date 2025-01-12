#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "MPU6500_WE.h"
#include <deque>

#define PIN_WS2812B 0
#define NUM_PIXELS 12
#define DELAY_IN_MS 20
#define BREAK_DELAY_IN_MS 50
const int QUEUE_SIZE = 10; // Number of samples for the moving average

MPU6500_WE mpu;
Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);
std::deque<float> gValueQueue; // Queue to store the G-values for averaging

void check_if_breaking();
void kittAnimation();
void carAnimation();

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup started");

  ws2812b.begin();
  Serial.println("WS2812B initialized");

  // Initialize I2C with SDA and SCL pins
  Wire.begin(8, 9); // Set SDA to pin 8 and SCL to pin 9
  delay(500);

  // Check if the MPU9250 is connected
  if (!mpu.init())
  { // change to your own address
    Serial.println("MPU connection failed. Checking I2C devices...");
    // Scan for I2C devices
    for (int address = 1; address < 127; address++)
    {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0)
      {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
        {
          Serial.print("0");
        }
        Serial.print(address, HEX);
        Serial.println(" !");
      }
    }
    Serial.println("Please check your connection with `connection_check` example.");
    while (1)
    {
      delay(5000);
    }
  }
}

void loop()
{

  // Clear the strip at the beginning of each loop
  ws2812b.clear();
  ws2812b.show(); // Ensure to show the cleared state
  carAnimation(); // Move left with 3 lit LEDs

  // Move right with 3 lit LEDs
}

int animation[24][NUM_PIXELS] = {
    {0, 0, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 192, 192, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 64, 255, 255, 64, 0, 0, 0, 0},
    {0, 0, 0, 0, 128, 255, 255, 128, 0, 0, 0, 0},
    {0, 0, 0, 0, 192, 255, 255, 192, 0, 0, 0, 0},
    {0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0},

    {0, 0, 0, 64, 255, 255, 255, 255, 64, 0, 0, 0},
    {0, 0, 0, 128, 255, 255, 255, 255, 128, 0, 0, 0},
    {0, 0, 0, 192, 255, 255, 255, 255, 192, 0, 0, 0},
    {0, 0, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0},

    {0, 0, 64, 255, 255, 255, 255, 255, 255, 64, 0, 0},
    {0, 0, 128, 255, 255, 255, 255, 255, 255, 128, 0, 0},
    {0, 0, 192, 255, 255, 255, 255, 255, 255, 192, 0, 0},
    {0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0},

    {0, 64, 255, 255, 255, 255, 255, 255, 255, 255, 64, 0},
    {0, 128, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0},
    {0, 192, 255, 255, 255, 255, 255, 255, 255, 255, 192, 0},
    {0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0},

    {64, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 64},
    {128, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 128},
    {192, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 192},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}};

int animationIndex = 0;
void carAnimation()
{
  // Move forward
  for (int i = 0; i < 24; i++)
  {
    ws2812b.clear(); // Clear previous state
    for (int j = 0; j < NUM_PIXELS; j++)
    {
      int rColor = animation[i][j];
      ws2812b.setPixelColor(j, ws2812b.Color(rColor, 0, 0)); // set pixel to red
    }
    ws2812b.show(); // update to the WS2812B Led Strip
    check_if_breaking();
    delay(DELAY_IN_MS); // Delay to control speed
  }

  // Move backward
  for (int i = 23; i >= 0; i--)
  {
    ws2812b.clear(); // Clear previous state
    for (int j = 0; j < NUM_PIXELS; j++)
    {
      int rColor = animation[i][j];
      ws2812b.setPixelColor(j, ws2812b.Color(rColor, 0, 0)); // set pixel to red
    }
    ws2812b.show(); // update to the WS2812B Led Strip
    check_if_breaking();
    delay(DELAY_IN_MS); // Delay to control speed
  }
}

void kittAnimation()
{
  for (int start = 0; start <= NUM_PIXELS - 3; start++)
  {
    ws2812b.clear(); // Clear previous state
    for (int pixel = start; pixel < start + 3; pixel++)
    {
      ws2812b.setPixelColor(pixel, ws2812b.Color(255, 0, 0)); // set pixel to red
    }
    ws2812b.show();     // update to the WS2812B Led Strip
    delay(DELAY_IN_MS); // Delay to control speed
    check_if_breaking();
  }

  // Move left with 3 lit LEDs
  for (int start = NUM_PIXELS - 3; start >= 0; start--)
  {
    ws2812b.clear(); // Clear previous state
    for (int pixel = start; pixel < start + 3; pixel++)
    {
      ws2812b.setPixelColor(pixel, ws2812b.Color(255, 0, 0)); // set pixel to red
    }
    ws2812b.show();
    delay(DELAY_IN_MS);
    check_if_breaking();
  }
}

void blinkLights(int durationInSeconds)
{
  unsigned long endTime = millis() + (durationInSeconds * 1000);
  while (millis() < endTime)
  {
    ws2812b.fill(ws2812b.Color(255, 0, 0));
    ws2812b.show();
    delay(BREAK_DELAY_IN_MS);
    ws2812b.clear();
    ws2812b.show();
    delay(BREAK_DELAY_IN_MS);
  }
}

void check_if_breaking()
{
  xyzFloat gValue = mpu.getGValues();
  xyzFloat acc = mpu.getAccRawValues();

  // Calculate the resultant G-value (magnitude of acceleration vector)
  float resultantG = sqrt(gValue.x * gValue.x + gValue.y * gValue.y);

  // Add the resultant G-value to the queue
  gValueQueue.push_back(resultantG);

  // Remove the oldest value if the queue exceeds the size limit
  if (gValueQueue.size() > QUEUE_SIZE)
  {
    gValueQueue.pop_front();
  }

  if (gValueQueue.size() == QUEUE_SIZE)
  {
    float sum = 0;
    for (float val : gValueQueue)
    {
      sum += val;
    }
    float averageG = sum / QUEUE_SIZE;
    Serial.print("Average: ");
    Serial.println(averageG);

    if (averageG > 0.5)
    {
      Serial.println("Braking detected!");
      blinkLights(5);
      gValueQueue.clear();
    }
  }
}