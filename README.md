# ESP32 LED Animation Project

## Video

YouTube: https://youtu.be/EtDtujpCDo8

## Description
This project utilizes an ESP32 microcontroller to control a WS2812B LED strip with various animations. It also integrates an MPU6500 sensor to detect motion and allows users to select different animation modes via a web interface.

## Features
- Control WS2812B LED strip with multiple animations (Kitt Car, Audi Car, Race).
- Motion detection using MPU6500 sensor.
- Web server for selecting animation options and speed.
- Adjustable animation speed via a web interface.

## Table of Contents
- [ESP32 LED Animation Project](#esp32-led-animation-project)
  - [Video](#video)
  - [Description](#description)
  - [Features](#features)
  - [Table of Contents](#table-of-contents)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Configuration](#configuration)
  - [Contributing](#contributing)
  - [License](#license)

## Hardware Requirements
- ESP32 microcontroller (e.g., Lolin C3 Mini)
- WS2812B LED strip
- MPU6500 sensor
- Jumper wires and breadboard (if needed)

## Software Requirements
- [PlatformIO](https://platformio.org/) for building and uploading the code.
- Arduino framework for ESP32.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/yourproject.git
   cd yourproject
   ```

2. Open the project in PlatformIO.

3. Install the required libraries specified in `platformio.ini`:
   ```ini
   lib_deps = 
       fastled/FastLED@^3.9.10
       adafruit/Adafruit NeoPixel@^1.12.3
       hideakitai/MPU9250@^0.4.8
   ```

## Usage
1. Connect the hardware as per the wiring diagram.
2. Upload the code to the ESP32 using PlatformIO.
3. Open the Serial Monitor to view debug messages.
4. Connect to the ESP32's Wi-Fi network (SSID: `ESP32_Config`, Password: `12345678`).
5. Open a web browser and navigate to the ESP32's IP address to access the animation selection interface.

## Configuration
The project configuration is defined in the `platformio.ini` file. You can modify the following parameters:
- `monitor_speed`: Set the baud rate for the Serial Monitor.
- `upload_speed`: Set the upload speed for the ESP32.

## Contributing
If you would like to contribute to this project, please fork the repository and submit a pull request. 

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a pull request

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
