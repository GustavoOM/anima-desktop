# Anima Mechanic Ventilator API

### Introduction

This is an API REST that receives data from the ventilator controler and makes this data avaiable for the interface with the healthcare professional. 

This API was conceived to run in a Raspberry Pi 3 B+, although it should work well in any UNIX based system (setting the correct location of system's serial port in `read_serial.py`).

The ventilator controller tested with this API runs in an Arduino Duo and its code is avaiable in the repository opus-anima-arduino. 

This API and the controler communicate through a serial connection. The message received or transmitted is positional: it contains only comma separated values, with a ';' to mark the end of the message. The value expected in each position is defined by the schemas in `base.py`, if a specific element is not avaiable for sending, it is possible to send the message with that position empty. If the controler does not follow this schemas, the API is not guaranteed to work.

### Installation

For this API to run, it is necessary to have installed some python packages, luckly the file requirements.txt has all the necessary packages for the API to work, for installing everything from this file it is necessary to install python3 virtual environment:

- `sudo apt-get install python3-venv`

Then you need to create the virtual environment to install all the packages, better do it on root directory:

- `python3 -m venv animaenv`

The next step is to activate the virtual environment:

- `source animaenv/bin/activate`

The last step is to install the packages in requirements with the venv activated:

- `pip install -r requirements.txt`


### Initializing Database

When first runnning the code in a machine it is necessary to initialize the database. In order to do that, it is necessary to have sqlite3 installed on your environment, to do that in many Linux systems you need just to enter on a terminal:

- `sudo apt-get install sqlite3`

After that, simply run the initialize DB script **on api directory**:

- `sqlite3 persist.db < initialize_db.sql`


### DEBUG prints

There are some prints through out the code to help understand what is happening, they are activated by flags at the start of `base.py`, in a section called ENABLERS.

1. SHOW_API_LOG enables that flask print the REST methods htat are being called and allows errors to be printed normally.
2. SHOW_READ_SERIAL prints messages received on serial port.
3. SHOW_WRITE_SERIAL prints messages sent through serial port.
4. VENTILATOR_MODE enable normal workflow of API, when turned off disables HTTP request of `read_serial.py` and access to serial port. This option is useful for testing pieces of code in environments other than raspberry.
5. DEBUG_MODE show some prints of execution, specially alarms and indicaticators sent to screen and length of graphics data.


### Code Organization

- **application.py**:
	- **Imports**: `context.py`, `controller.py`, `base.py` and `write_serial.py`;
	- **Responsability**: manages all external connections, maps python functions to URLs endpoints;
	- It assumes that each requisition will follow the JSON formats defined in `json-formats.md`;
	- **Execute this file in order to run the API**;
	- It concentrates all the I/O communication to software external to the API.
- **context.py**: 
	- **Imports**: `strategies.py`, `base.py`, `logger.py` and `guesser.py`;
	- **Responsability**: manage ventilator strategy changes, call for DB methods and restore state if the API was shutdown abruptly;
- **controller.py**:
	- **Imports**: `guesser.py` and `base.py`;
	- **Responsability**: define all parameters received from controller, keeps a fixed size history of those parameters, regulates frequency of updating data on screen.
- **strategies.py**:
	- **Imports**: `validation.py`, `guesser.py`;
	- **Responsability**: define all the ventilator strategies, makes sure that the values received are in the required range;
	- Each mode defined in this file (that inherits from generic strategy class) is dynamically included in the avaible modes dictionary used by `context.py`.
- **guesser.py**:
	- **Imports**: `base.py`, `sensor.py`, `alarm.py`, `bound.py`, `graphics.py` and `metadata.py`.
	- **Responsability**: groups sensors, alarms, bounds and graphics in a way that can be easily acessed and dynamically parametrized.
- **sensor.py**:
	- **Imports**: `base.py`, `metadata.py` and `validation.py`;
	- **Responsability**: extract and calculate if necessary the measures to the operator using data from the controller.
- **alarm.py**:
	- **Imports**: `sensor.py`, `logger.py`, `base.py`, `metadata.py` and `validation.py`;
	- **Responsability**: extract and pass forward the alarms from the controller to the screen.
- **bound.py**:
	- **Imports**: `alarm.py`, `base.py`, `metadata.py` and `validation.py`;
	- **Responsability**: extract and pass forward the alarms which are bounded to indicators from the controller to the screen.
- **graphics.py**:
	- **Imports**: `sensor.py`, `base.py`, `metadata.py` and `validation.py`;
	- **Responsability**: defines and build the graphics data to show to the operator using data from the controller.
- **metadata.py**:
	- **Imports**: nothing;
	- **Responsability**: encapsulate columns names of configuration files and attribute name of objects in code, if mandatory attribute not mapped in config files column raises exception. 
- **validation.py**:
	- **Imports**: `base.py`;
	- **Responsability**: instatiate metaclasses that checks, on start, coherence of ids among subclasses and config files.
- **logger.py**
	- **Import**: `base.py`;
	- **Responsability**: centralizes all the classes that log information.
	- Centralizes access to DB.
- **write_serial.py**:
	- **Imports**: `base.py`;
	- **Responsability**: convert JSON to serial message format defined in `base.py`.
- **read_serial.py**:
	- **Import**: `base.py`;
	- **Responsability**: convert serial format message into JSON, read messages on serial port by event;
	- **Execute this file to run the proccess that continuously waits for messages on serial port**;
	- Ignore messages with wrong number of positional arguments.
- **base.py**:
	- **Imports**: nothing;
	- **Responsability**: keep fundamental configuration and static variables centralized, keep the code coherent.
	- Centralizes serial port configuration.
	- Centralizes URL definition.
	- Defines configuration files access and processing.
- **calibration.py**:
	- **Imports**: `write_serial.py` and `base.py`;
	- Command line process to help on calibrating the controller.
	- Apart from the rest of the code.
	- Executed on its own.
	- When exporting data with 'child' or 'adult' automatically overwrites configuration data used by api when starting ventilation.
	- Execute with `python3 calibration.py`


### Execution

There is two ways of executing the code:

FIRST is by running processes separately. Better to execute code in the following order inside `api` directory:

1. execute the API, the terminal should print the url that the API is running on:

	`python3 application.py`

2. open frontend/index.html in a brownser. 
3. execute the process that constantly reads data from serial in another terminal, the terminal should block and print a success message everytime it can deliver a message received in the serial port to the API. 

	`python3 read_serial.py`

SECOND is by running everything at once by executing stay_alive python script at root directory.

1. `python3 stay_alive.py`
	- This second approach tries to keep every process alive so if any one of them dies it is automatically restarted.
	- It does not cover the case in which stay_alive dies and another process dies.
	- In order to the call to chromium work there MUST NOT BE any chromium tab or window open, otherwise it will open infinite tabs.


### Configure Raspberry Pi 3 B+

For the serial port to work properly it is necessary to enable the primary serial port of the board and disable the linux console that the operating system connects to this serial port.
In order to do that through Raspbian GUI just click on Applications Menu -> Preferences -> Raspberry Pi Configuration, this will open a window, go to Interfaces tab and check Serial Port to **Enable** and Serial Console to **Disable**. 

### Connecting wires

```
 Arduino DUO       Raspberry 3 B+
       GND --------- GND
    19 RX1 --------- TX GPIO14
    18 TX1 --------- RX GPIO15
``` 