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


## Work organization

...


## How it works

### State diagram

![alt text](https://i.ibb.co/60hX7Tm1/diagram.png)

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
|   ├── components/           # Custom project components
|   |   ├── weight/           
|   |   └── wifi/
│   ├── main/                 # Main application files
│   └── diagram.json          # Wokwi diagram components
└── web-service/
    ├── api
    │   ├── app.js            # Main API code
    │   └── package.json      # Express.js dependencies
    └── frontend/
        ├── app/
        ├── components/
        ├── hooks/
        ├── lib/
        ├── public/
        └── package.json      # React and Next.js dependencies
```
The project follows a service oriented architecture: it uses the component manager provided by ESP-IDF to interact with external driver libraries, which are included in  [idf_component.yml](esp/main/idf_component.yml), and exploits the build system to integrate the developed [components](esp/components/) via CMake. These approach guarantees scalability and modularity for future development.
### API model:

- **`/status`**:
returns a JSON containing the status of the circuit (microcontroller and sensors)

- **`/spots`**:
returns a JSON containing the info on the parking lot and the parked cars

- **`/events`**:

  - **`/entry`**:
  notifies an entry event (barrier triggered / plate detected)

  - **`/exit`**:
  notifies an exit event

- **`/barrier`**:

  - **`/info`**:
  returns a JSON containing the info on the parking lot barrier

  - **`/on`**:
  opens the barrier on command, overriding any other signal

  - **`/off`**:
  closes the barrier on command, overriding any other signal

  - **`/default`**:
  leaves the barrier on control of the system


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
...

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
