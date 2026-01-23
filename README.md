# Tiny Parking System - Embedded Software for the Internet of Things

This repository contains all of the code (and more) about our project for the Embedded Software for the Internet of Things course, held by prof. Kasim Sinan Yildirim.

Our project, Tiny Parking System, wants to simulate a realistic parking system scaled to small sized vehicles (like toy cars or very small RC vehicles) by creating an automated environment for parking management, which can be managed and controlled autonomously (and remotely, too) thanks to the complex interconnected environment made up by the various sensors, modules and web services.


### Table of Contents

- [About the project](#about-the-project)
- [Work organization](#work-organization)
- [How it works](#how-it-works)
- [Project documentation](#work-organization)
  - [Libraries documentation](#libraries-documentation)
  - [Error codes meaning](#error-codes-meaning)
  - [Serial communication](#serial-communication)
  - [Debugging](#debugging)
  - [Web Server](#web-server)
- [Requirements](#requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software requirements](#software-requirements)
- [Known issues and possible improvements](#known-issues-and-possible-improvements)
- [Conclusions](#conclusions)
- [Additional resources](#additional-resources)


## About the project

The project has been developed by the following students:
| Name | Contact |
| -- | -- |
| Mirco Stelzer | mirco.stelzer@studenti.unitn.it |
| Leonardo Conforti | leonardo.conforti@studenti.unitn.it |
| Daniel Casagranda | daniel.casagranda@studenti.unitn.it |
| Amar Sidkir | amar.sidkir@studenti.unitn.it |

Our work is based on everything we have learned in the last 3 years, but most importantly most of the guidelines were took from the teacher lectures. Some examples include:
- Code structure
- Testing and debugging
- Interoperability
- Hardware dependent/independent functions
- And more...

## How it works

### State diagram

![alt text](https://i.ibb.co/60hX7Tm1/diagram.png)

A finite state machine handles the behaviour of the whole system as a separate, non-blocking task. The FSM manages state transitions based on sensor events (such as vehicle detection, license plate recognition, and exit signals) and controls the barrier, LED indicators, and display updates accordingly. The system optimizes power consumption by entering a low-power sleep mode when idle and only activating the computer vision processing when a vehicle is detected at the entrance.

### General features

- A dispositivo LED RGB (green/red/blue) will indicate in real-time the availability of parking spaces: the LED will be green when free spots are available, red when the parking lot is full, and blue if the object entering is not a car.
- If the parking lot is full, the system will prevent the opening of the entrance barrier, thereby blocking vehicle access.
- An LCD display will continuously show the parking lot’s opening and closing hours, beyond which access will be allowed only for exits.
- A digital display will show the updated number of available free spots.

### Vehicle Entry and Exit Management

- Sensors will be installed at both the entrance and exit points to detect vehicles passing through.
- Upon detection of a vehicle entering or exiting, the barrier will automatically lift to allow passage.
- Simultaneously, the counter of free parking spots will be incremented or decremented according to the flow of vehicles.
- Updated information regarding occupied and free spots will be continuously reported on an online control dashboard.
- (Maybe?) Find if its possible to have a sort of “bluetooth/radio” key which, if pressed, opens the gate. This can also activate the barrier if in a close range, which indicates the owner of the key inside the vehicle is nearby

### Sensors and Object Type Discrimination

- To discriminate between vehicles (cars, trucks) and other presences (e.g., pedestrians) at the entrance, a weight sensor will be used (ha lo stesso costo del sensore ultrasuoni di circa 2 su AliExpress).
- The entrance barrier will lift only in the presence of a vehicle identified as a car, preventing unintended openings for trucks or pedestrians.
- An ultrasonic sensor will be used to detect cars exiting and will safely raise the corresponding barrier.
- Naturally, all vehicles inside the lot can exit freely.

### Access Constraints for Heavy Vehicles

- The system will prevent trucks from entering if they were not authorized to enter through the entrance barrier, thus ensuring consistent and secure access control.
- If vehicles other than cars are authorized to enter, this implies they have a dedicated remote control for access that allows them to raise the entrance barrier.

### Computer Vision

- A camera sensor captures the front of the car, and thanks to a computer vision model, extracts the license plate to keep track of who enters the parking lot. This is a new information to store in our dataset.
    - Possible variation: We have a dataset of the cars (license plates) allowed to access the parking. When a car try to enter, if its license plate is not in the database, the barrier will not lift.

### Online Dashboard

- Simulation of time (possibly to communicate the opening/closing of the parking lot) by a dedicated wheel/input from the dashboard
- A view of free and occupied parking lots:
    - This requires to know which lot is the car parking onto, maybe via small detection sensors installed under each parking lot


## Project documentation

### Project structure:

```
Tiny-Parking-System
├── esp/
|   ├── components/            # Custom project components
|   |   ├── cv/
|   |   ├── https/
|   |   ├── init/
|   |   ├── oled/
|   |   ├── servo_motor/
|   |   ├── ultrasonic_sensor/
|   |   ├── weight/
|   |   └── wifi/
│   ├── main/                  # Main application files
|   |   ├── fsm.c              
|   |   ├── idf_component.yml  # Includes third party libs
|   |   ├── Kconfig.projbuild  # Custom configuration
|   |   └── main.c             
│   └── diagram.json           # Wokwi diagram components
└── web-service/
    ├── api
    │   ├── app.js             # Main API code
    │   └── package.json       # Express.js dependencies
    └── frontend/
        ├── app/
        ├── components/
        ├── hooks/
        ├── lib/
        ├── public/
        └── package.json       # React and Next.js dependencies
```
### Project Architecture

The project follows a service-oriented architecture using the ESP-IDF component manager to integrate external driver libraries listed in [`idf_component.yml`](esp/main/idf_component.yml). The build system leverages CMake to incorporate custom [`components`](esp/components/), ensuring scalability and modularity for future development.

#### Dependencies

The project utilizes the following third-party libraries:

| Library | Purpose |
| -- | -- |
| `espressif/esp32-camera` | Camera interface for computer vision |
| `esp-idf-lib/ultrasonic` | Ultrasonic sensor for vehicle detection |
| `esp-idf-lib/hx711` | Weight sensor for vehicle classification |
| `espressif/cjson` | JSON parsing for API communication |
| `espressif/servo` | Servo motor for the parking barrier |
| `nopnop2002/ssd1306` | OLED for information display | 

### API model:

To communicate the status and events occuring in the system, we developed a very basic API model to send events and update existing logs/info on the main webapp:

- **`/status`**:

  - **`GET`**:
  returns the status of the ESP microcontroller, the parking info and its sensors

  - **`PUT`**:
  updates the status of the ESP microcontroller, the parking info and its sensors

- **`/entry`**:
records a vehicle entering the parking system and returns an entryID

- **`/entry/{entryID}`**:
records the result of a specific parking entry request (allowed/denied)

- **`/exit`**:
records a vehicle exiting the parking system


## Configuration

### Prerequisites
Before getting started, ensure that you have the following installed:
- **Node.js** (version 12 or later)
- **npm** (comes with Node.js)

### 1. Clone the repository

  ``` 
  git clone https://github.com/AmarS03/Tiny-Parking-System.git
  ```

### 2. Setting up the circuit
#### Pinout
| Component | GPIO Pin |
| -- | -- |
| Camera PWDN | GPIO 38 |
| Camera VSYNC | GPIO 6 |
| Camera HREF | GPIO 7 |
| Camera PCLK | GPIO 13 |
| Camera XCLK | GPIO 15 |
| Camera SDA | GPIO 4 |
| Camera SCL | GPIO 5 |
| Camera D0-D7 | GPIO 11, 9, 8, 10, 12, 18, 17, 16 |
| Ultrasonic TRIG | GPIO 3 |
| Ultrasonic ECHO | GPIO 20 |
| Weight Sensor DOUT | GPIO 21 |
| Weight Sensor CLK | GPIO 14 |
| Servo Motor PWM | GPIO 1 |
| OLED SDA | GPIO 42 |
| OLED SCL | GPIO 41 |

...

...

#### Weight Calibration
 - The project provides a code stub for the weight sensor calibration. It requires the user to place an object of known weight on the platform when prompted and wait for the process to finish.
 - The offset is stored in an NVS partition and it's loaded each time the application is run.
 - In order to run the calibration code, go to Project Configuration in [menuconfig]() and enable the Weight Calibration flag.

### 3. Configure the ESP-IDF framework
- Download the ESP-IDF framework A complete guide can be found [here](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/get-started/index.html).
- Run the following commands:
  ```bash
    idf.py set-target esp32s3
    idf.py fullclean
    idf.py menuconfig 
  ```
- Once landed in the configuration menu, enable the following options:
  - Enable ESP PSRAM (through Component config)
    - Enable Octal Flash and set clock speed to 80 MHz
  - Set the correct flash size (usually 8 MB) and make sure that the SPI speed matches the one of the PSRAM (through Serial flasher config)
  - Set the WiFi SSID and password in Project Configuration (these will be locally stored in the configuration file)
  - Set the Partition Table to large single factory app; this will allow you to flash the executable on the device

### 4. Setting up the Web Service
- Add the environment keys:
  - ...
- Inside **/web-service/api/** run:
  - `npm install`
  - `node app.js`
- Inside **/web-service/frontend/** run:
  - `npm install`
  - `npm run build`
  - `npm run dev`

## Run the project
- Once everything is setup properly simply run the following command:
    ```bash
    idf.py build flash monitor 
    ```

## Wokwi Simulation
- The project also offers the possibility to test the whole system without having any additional hardware, thanks to the integration with [Wokwi](https://wokwi.com/). This allows the user to prototype and test fast and remotely.
- In order to run the simulation successfully:
  - Go back to the configuration menu
  - Make sure to disable PSRAM, otherwise Wokwi will abort the process
  - Go to Project Configuration and enable the mock camera option. This will automatically set the SSID and password to run the example in Wokwi, which uses a pre-fetched mock image to test the CV algorithm.
  - Go to https://www.circuitdigest.cloud, login or create an account and generate your API key. This will be used to query the cloud server for the NPR algorithm.
  - Come back to the Project Configuration menu and paste your API key.
- After everything's set, open the [diagram.json](esp/diagram.json) file and start the simulation. Play with the sensors to see the different interactions.

## Known issues and possible improvements

1. Two or more vehicles cannot enter and exit the parking lot simultaneously. This does not obviously reflect real life parking lots, where we have two separate lanes. The reason is simply the budget limitation to buy double the amount of equipment (barriers, motors, weight sensors, ultrasound sensors...), just to resolve this small issue. So we simply imposed that one vehicle can only enter or exit at a time.

1. Vehicle exiting from the parking lot should also be recognized with a secondary camera. Due to the budget limitation, we opted for a manual removal instead by simulating the process and randomly removing one of the parked vehicle.


## Conclusions

...


## Additional resources

#### Presentation:

#### Video:
