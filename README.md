# **Smarduino**
*Get it? Smardens and Arduino....Smarduino??*

## **Developer Guidelines**
### **Overview**
To compile and test our Smart Garden solution you will need the software below. With the required software installed you will also need to following settings inside the Arduino IDE. Our solution is split into two parts. The first is the Arduino Due Main code which has several different functions: store all sensor data on an SD card from the ESP webserver connected to its serial port, deciding if it's time to water based on soil mositure levels, and powering all the devices connected to it(SD card, ESP webserver, water solenoid valve). The second are the AP_Start and AP_Connect sketches which handle sending and receiving the sensor data from the garden sensors at specific intervals. The AP_Connect is altered for each specific sensor in the garden. 

There are wiring diagrams located in the Diagrams folder which detail the embedded system layout. 

### **Required Software**
* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* [Postman](https://www.getpostman.com/)

### **Development
#### Arduino IDE Settings**
* Open the Arduino IDE and go to File -> Preferences
* **Make sure the "Sketchbook Location:" is pointed to where this repository is**
* Enter [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json) into "Additional Boards Manager URLs:"
* In Tools -> Board select "Boards Managers..."
  * Find "ESP8266" and install
  * Find "Arduino SAM Boards (32-bits ARM Cortex-M3)" and install
* After installation, your IDE is ready for development on the ESP and Arduino Due
* Pick your desired development board in Tools -> Board

#### **Arduino IDE Libraries**
* All required libraries are located in the "libraries" folder
  * If you receive "Missing Library" errors please submit issue tickets so we can resolve them

### **Board Settings**

*Settings are located in the Tools dropdown*

#### **ESP8266 Settings**
*We are using the [Adafruit HUZZAH ESP8266](https://www.adafruit.com/product/2471)*

```
Board: "Adafruit Feather HUZZAH ESP8266"
Flash Size: "4M (1M SPIFFS)"
Debug Port: "Disabled"
Debug Level: "None"
lwlP Variant: "v2 Lower Memory"
VTables: "Flash"
CPU Frequency: "160 MHz"
Upload Speed: "115200"
Erase Flash: "All Flash Contents"
```
*Port will vary depending on which USB port the device is plugged into on your machine*

*[Postman](https://www.getpostman.com/) was used to make HTTP POST requests, it may be useful when developing the client and server*
  
#### **Arduino Due Settings**
*We are developing on the [Arduino Due](https://store.arduino.cc/usa/arduino-due)*
```
Board: "Arduino Due (Programming Port)"
```
*Port will vary depending on which USB port the device is plugged into on your machine*


## Mini-Cheatsheet for Git Instructions
#### Helpful Definitions
* **Master Branch**: branch with latest working code (main program)
* **Remote Branch**: your current local branch
* **Upstream Branch**: branch that can be tracked by GitHub
* **Origin**: perform operation on the source of your target

### Changing to a *New* Branch
*making a new branch with all your current work*
```
git checkout -b {newbranchname}
```

### Changing to a Branch
*this is your private workspace*
```
git checkout {branchname}
```

### Pushing to a Branch
*whenever you push, you should ALWAYS **ADD**, **COMMIT**, then **PULL** first*
```
git add .                    // period means "everything", you can optionally chose to add only specific files
git commit -m "message"      // **REQUIRED** every commit requires a message
git pull origin master       // pulls code FROM master TO whichever branch you're on (if you are already on master branch, keyword origin is optional)
        // **ALWAYS** pull before you push (make sure to add and commit first; to avoid overriding data)
git push                     // push to your upstream branch
```
**Order of Operations:   	 Add -> Commit -> Pull -> Push**

### Pushing to Your Branch for the First Time
*must set upstream so that github can track it* 
```
git push -u origin {branchname}    // -u argument is only necessary in your first push on a new branch
```

### View Status of Modified Files
```
git status
```

### View Specific Changes Within Files
```
git diff                   // overview of things changed
git diff {filename}        // see specific changes in file 
git diff {branchname}      // view differences between branches
:wq                        // exits vim mode

```

### View List of Commits
```
git log      // view commit log
git checkout {first 6 characters of chosen checkpoint hash} -b {new branch name}  // checkout log to new branch
```
----
