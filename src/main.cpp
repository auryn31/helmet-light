#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "MPU6500_WE.h"
#include <deque>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

const char *ssid = "ESP32_Config";
const char *password = "12345678";

#define PIN_WS2812B 0
#define NUM_PIXELS 12
#define BREAK_DELAY_IN_MS 50


MPU6500_WE mpu;
Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);
std::deque<float> gValueQueue; // Queue to store the G-values for averaging
const int QUEUE_SIZE = 10; // Number of samples for the moving average
Preferences preferences;
AsyncWebServer server(80);
String selectedOption = "Kitt Car";
unsigned long lastActivityTime = 0; // Tracks the last activity time
const unsigned long timeoutDuration = 120000; // 2 minutes in milliseconds
bool isServerActive = false;
const int DEFAULT_SPEED = 20; // Default speed for the animation
int animationSpeed = DEFAULT_SPEED; // Variable to store the animation speed


void check_if_breaking();
void kittAnimation();
void carAnimation();
void loadOption();
void saveOption(String option, int speed);
void startServer();
void stopServer();
void rainbowAnimation();

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
  startServer();
}


void loop()
{

  // Clear the strip at the beginning of each loop
  ws2812b.clear();
  ws2812b.show(); // Ensure to show the cleared state

  if (selectedOption == "Kitt Car") {
    kittAnimation(); 
  } else if (selectedOption == "Audi Car") {
    carAnimation();
  } else if (selectedOption == "Race") {
    rainbowAnimation(); 
  }
  if (isServerActive && millis() - lastActivityTime > timeoutDuration) {
    stopServer();
  }
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
    delay(animationSpeed); // Delay to control speed
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
    delay(animationSpeed); // Delay to control speed
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
    delay(animationSpeed); // Delay to control speed
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
    delay(animationSpeed);
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
    if (averageG > 0.5)
    {
      Serial.println("Braking detected!");
      blinkLights(5);
      gValueQueue.clear();
    }
  }
}

// Function to save the selected option and speed in NVS
void saveOption(String option, int speed) {
  preferences.begin("my-app", false); // Open NVS
  preferences.putString("selectedOption", option); // Save the option
  preferences.putInt("animationSpeed", speed); // Save the speed
  preferences.end(); // Close NVS
  Serial.println("Option and speed saved to NVS: " + option + ", Speed: " + String(speed));
}

// Function to load the selected option and speed from NVS
void loadOption() {
  preferences.begin("my-app", true); // Open NVS in read-only mode
  selectedOption = preferences.getString("selectedOption", "Kitt Car"); // Default to "Kitt Car"
  animationSpeed = preferences.getInt("animationSpeed", DEFAULT_SPEED); // Load speed, default to DEFAULT_SPEED
  preferences.end(); // Close NVS
  Serial.println("Loaded option from NVS: " + selectedOption + ", Speed: " + String(animationSpeed));
}

void startServer(){
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Load the saved option from NVS
  loadOption();

  String head = 
              "<head>"
              "<meta charset='UTF-8'>"
              "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
              "<title>Select Your Option</title>"
              "<style>"
              "body {"
              "  font-family: Arial, sans-serif;"
              "  margin: 0;"
              "  padding: 0;"
              "  background-color: #f4f4f4;"
              "  display: flex;"
              "  justify-content: center;"
              "  align-items: center;"
              "  height: 100vh;"
              "  color: #333;"
              "}"
              "h1 {"
              "  font-size: 2em;"
              "  margin-bottom: 20px;"
              "  text-align: center;"
              "}"
              "form {"
              "  background-color: #fff;"
              "  padding: 20px;"
              "  border-radius: 8px;"
              "  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);"
              "  width: 90%;"
              "  max-width: 400px;"
              "}"
              "input[type='radio'] {"
              "  margin-right: 10px;"
              "}"
              "label {"
              "  font-size: 1.1em;"
              "  margin-bottom: 10px;"
              "  display: block;"
              "}"
              "input[type='submit'] {"
              "  background-color: #4CAF50;"
              "  color: white;"
              "  padding: 10px 20px;"
              "  border: none;"
              "  border-radius: 5px;"
              "  width: 100%;"
              "  font-size: 1.2em;"
              "  cursor: pointer;"
              "}"
              "input[type='submit']:hover {"
              "  background-color: #45a049;"
              "}"
              "@media (min-width: 600px) {"
              "  h1 {"
              "    font-size: 2.5em;"
              "  }"
              "  label {"
              "    font-size: 1.3em;"
              "  }"
              "}"
              "</style>"
              "</head>";

  // Web server route for displaying the radio buttons and speed slider
  server.on("/", HTTP_GET, [head](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html>"
              "<html lang='en'>" 
              + head +
              "<body>"
              "<div>"
              "<h1>Select Your Option</h1>"
              "<form action='/save' method='GET'>"
              "<input type='radio' id='kitt' name='option' value='Kitt Car' " + 
              String((selectedOption == "Kitt Car" ? "checked" : "")) + ">"
              "<label for='kitt'>Kitt Car</label><br>"
              "<input type='radio' id='audi' name='option' value='Audi Car' " + 
              String((selectedOption == "Audi Car" ? "checked" : "")) + ">"
              "<label for='audi'>Audi Car</label><br>"
              "<input type='radio' id='race' name='option' value='Race' " + 
              String((selectedOption == "Race" ? "checked" : "")) + ">"
              "<label for='race'>Race</label><br><br>"
              "<label for='speed'>Animation Speed (ms): " + String(animationSpeed) + "</label><br>"
              "<input type='range' id='speed' name='speed' min='10' max='60' value='" + String(animationSpeed) + "' oninput='this.nextElementSibling.value = this.value'>"
              "<output>" + String(animationSpeed) + "</output>"
              "<br><input type='submit' value='Save'>"
              "</form>"
              "</div>"
              "</body>"
              "</html>";
    request->send(200, "text/html", html);
  });

  // Web server route to save the selected option and speed
  server.on("/save", HTTP_GET, [head](AsyncWebServerRequest *request) {
    if (request->hasParam("option") && request->hasParam("speed")) {
      selectedOption = request->getParam("option")->value();
      animationSpeed = request->getParam("speed")->value().toInt(); // Get the speed from the slider
      saveOption(selectedOption, animationSpeed); 
      request->send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html lang='en'>"
      + head +
      "Option and speed saved! <a href='/'>Go back</a>"
      "</html>");
      lastActivityTime = millis();
    } else {
      request->send(400, "text/plain", "Option or speed not found!");
    }
  });

  // Start the web server
  server.begin();
  isServerActive = true;
  lastActivityTime = millis();
  Serial.println("Web server started.");
}

void stopServer() {
  server.end();
  WiFi.softAPdisconnect(true);
  isServerActive = false;
  Serial.println("Web server and Wi-Fi AP turned off to save power.");
}

void rainbowAnimation() {
  // Create a rainbow effect
for (int j = 0; j < 256; j++) { // 256 colors in the rainbow
    for (int i = 0; i < NUM_PIXELS; i++) {
      int pixelHue = (i * 256 / NUM_PIXELS + j) & 255; // Calculate the hue for each pixel
      // Set pixel color based on hue, with full saturation and value
      ws2812b.setPixelColor(i, ws2812b.ColorHSV(pixelHue * 256, 255, 255)); 
    }
    ws2812b.show(); // Update the WS2812B Led Strip
    delay(animationSpeed / 5); // Use the animation speed variable
  }
}