/************************************************************************
*
* Names: Craig Bryan
*        Kat Drobnjakovic
*
* Course Code: SEG 4145
* Lab Number: 4
* File name: ms_roboto_lab_4
* Date: March 12, 2015
*
*
* Description
* *************
* A simple set of robot movement controls using the Servo library and 
* wheel optical sensor feedback. Also includes collision avoidance and
* temperature sensor reading.
*
*************************************************************************/ 

#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <Wirefree.h>
#include <WifiClient.h> 

//DEBUG MODE FLAG
#define SERIAL_DEBUG false // If true, only parse commands
#define SERIAL_COMMAND false // If true, does not wireless stuff
#define WIRELESS_DEBUG true

// MOTOR CONSTANTS
#define LEFT_MOTOR 45
#define RIGHT_MOTOR 8

//HEAD CONSTANTS
#define HEAD_MOTOR 7
Servo headServo;
boolean isHeadAttached;
#define MAX_LEFT_HEAD_TURN 55
#define MAX_RIGHT_HEAD_TURN 105
#define DEGREES_PER_ITERATION 10
#define FULL_LEFT_DEGREE 0 //this is what we consider zero, need to calibrate

// WHEEL SENSOR CONSTANTS
#define RIGHT_WHEEL_SENSOR 49
#define LEFT_WHEEL_SENSOR 48

// DIRECTION CONSTANTS
#define LEFT_BACKWARD 0
#define LEFT_FORWARD 180
#define RIGHT_BACKWARD 180
#define RIGHT_FORWARD 0
#define LEFT_STOP 80
#define RIGHT_STOP 81

// SERVO VARIABLES
Servo rightServo;
Servo leftServo;
boolean isRightAttached;
boolean isLeftAttached;
#define THRESHOLD 2

// TURN AND DISTANCE CONSTANTS
#define TICKS_PER_DEGREE_LEFT 0.57
#define TICKS_PER_DEGREE_RIGHT 0.60
#define TICKS_PER_TILE 103
#define TICKS_PER_CM 20 //TODO calibrate

// LCD CONSTANTS
int lcd_pin_number = 19;
SoftwareSerial LCD(0, lcd_pin_number);

// LCD COMMAND HEXS
#define CLEAR 0x01
#define DISPLAY_OFF 0x08
#define DISPLAY_ON 0x0C
#define LEFT_SCROLL 0x18
#define RIGHT_SCROLL 0x1C
#define BACKLIGHT_OFF 0x80
#define BACKLIGHT_ON 0x90
#define CMD_FLAG 0xFE

// SONAR SENSOR CONSTANTS
#define SONAR 22
long distance = 0; 

// TEMPERATURE SENSOR CONSTANTS
#define TEMP_SENSOR 0x68 //address
int tempurature_reg = 0x01;

// WIRELESS CONSTANTS
WIFI_PROFILE wireless_prof = {
                /* SSID */ "Robolab",
 /* WPA/WPA2 passphrase */ "w1r3l3ss!",
    /* Robot IP address */ "10.136.160.16",
         /* subnet mask */ "255.255.255.0",
          /* Gateway IP */ "10.136.160.1" };

//TODO PUT IN COMPUTER IP
String remote_server = "137.122.45.88"; // peer device IP address

String remote_port = "32114"; // arbitrary port

//Creates a client that can connect to a specified internetIP address and port number
WifiClient client(remote_server, remote_port, PROTO_UDP);

// OTHER CONSTANT
#define BOARD_LED 13

// Command struct
struct Command {
  char cmd;
  int integerParam;
  bool allGood;
  char result[100];
};

void setup() {
  //initialize board light pin
  pinMode(BOARD_LED, OUTPUT);

  //initialize wheel sensor
  pinMode(RIGHT_WHEEL_SENSOR, INPUT);
  pinMode(LEFT_WHEEL_SENSOR, INPUT);
  
  //Start the motors disattached
  isHeadAttached = false;
  isLeftAttached = false;
  isRightAttached = false;

  //Center the head
  turnHeadTo(90); // TODO calibrate to center

  //Initialize lcd 
  LCD.begin(9600); //open serial port to LCD screen with baud at 9600
  
  //Initialize the temperature sensor communication
  Wire.begin();

  //Initialized serial port for debug
  Serial.begin(9600); 

  if(!SERIAL_DEBUG && !SERIAL_COMMAND) {
    // connect to AP
    Wireless.begin(&wireless_prof);
  
    if(client.connect()) {
      Serial.println("Connected to Ms. Roboto"); //Send this to computer
      client.println("Confirm connection");
    } else {
      Serial.println("FAILED");
    }
  } else {
    Serial.println("Done setup");
  }
}

void loop() {
  char message[10]; //Arbitrarily long array
  int index = 0;
  boolean inputToParse = false;
  
  if(SERIAL_DEBUG || SERIAL_COMMAND) {
    while(Serial.available()) {
        char in = (char)Serial.read();
  
        message[index++] = in;
        inputToParse = true;
        delay(1);
    }
  
    if(inputToParse) {
      message[index] = '\0';
      Serial.println(message);
      struct Command command = parseCommandString(message);
      command = runCommand(command);
      Serial.println(buildResponse(command));
      inputToParse = false;
    }
  } else {
    while(client.available()) {
      message[0] = '\0'; 
      char in = (char)client.read();

      message[index++] = in;
      inputToParse = true;
      delay(1);
    }
    
    if(inputToParse) {
      message[index] = '\0';
  
      struct Command command = parseCommandString(message);
      command = runCommand(command);
      sendResponse(command);
      inputToParse = false;
      client.flush();
    }
  }

  delay(1);
}

/////////////////////////////////////////////////////////////////////////
///////////////////// LAB 4 SPECIFIC METHODS ////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* parseCommandString
*
* Description
* *************
* Takes the incoming command string and parses into a Command struct.
*
* Parameters
* *************
* message - the incoming command string
*
* Returns
* *************
* A new command struct, with the command character and integer param set.
* If the parameter is too long, it will set the allGood flag to false to
* tell the later functions that something went wrong.
*
*
*************************************************************************/ 
struct Command parseCommandString(char* message) {
  struct Command command;
  command.cmd = message[0];
  command.allGood = true;

  int index = 0;
  char param[8];

  while(true) {
    // Reached the end of the parameter, set it and break the loop
    if(message[index + 2] == '\0') {
      command.integerParam = atoi(param);
      break;
    } 

    // Read to the end of the input array, not good. Set a flag in the
    // command and break the loop
    if(index > 9) {
      command.allGood = false;
      break;
    }

    // Set that digit of the parameter
    param[index] = message[index + 2];
    index++;
    
    
  }
  if (WIRELESS_DEBUG) {
    command.allGood = false; 
  }
  return command;
}

/************************************************************************
*
* Name
* *************
* runCommand
*
* Description
* *************
* Runs the command in the passed in Command struct. Uses the command character
* to identify the type of command and uses the
*
* Parameters
* *************
* None
*
* Returns
* *************
* The command struct with the result field filled with the appropriate info.
* Also sets the command character to either 'd' (for done) or 'e' (for error).
*
*
*************************************************************************/ 
struct Command runCommand(struct Command command) {
  char message[1024];

  if(!command.allGood) {
    if (!WIRELESS_DEBUG) {
      strcpy(message, "Command parameter overran the input array");
    }
    else {
      strcpy(message, "Is in WIRELESS_DEBUG mode");
    }
    return setError(command, message);
  }

  if(command.cmd == 'f') {
    moveCMForward(command.integerParam);
    message[0] = '\0';
    command.cmd = 'd';
  } else if(command.cmd == 'b') {
    moveCMBackwards(command.integerParam);
    message[0] = '\0';
    command.cmd = 'd';
  } else if(command.cmd == 'r') {
    turnDegreesRight(command.integerParam);
    message[0] = '\0';
    command.cmd = 'd';
  } else if(command.cmd == 'l') {
    turnDegreesLeft(command.integerParam);
    message[0] = '\0';
    command.cmd = 'd';
  } else if(command.cmd == 'p') {
    strcpy(message, readDistance());
    command.cmd = 'w';
  } else if(command.cmd == 't') {
    strcpy(message, readTemperature());
    command.cmd = 'w';
  } else {
    command.allGood = false;
    strcpy(message, "Unrecognized command character");
    return setError(command, message);
  }

  
  strcpy(command.result, message);
  return command;
}

/************************************************************************
*
* Name
* *************
* setError
*
* Description
* *************
* Sets the Command struct to an error state, with and error message
*
* Parameters
* *************
* command - the Command struct being processed currently
* message - the error message to be saved in the Command struct
*
* Returns
* *************
* The same Command struct with the command character set to 'e', and the 
* result string set to the passed in message.
*
*
*************************************************************************/ 
struct Command setError(struct Command command, char* message) {
  command.cmd = 'e';
  strcpy(command.result, message);
  return command;
}

/************************************************************************
*
* Name
* *************
* sendResponse
*
* Description
* *************
* Sends a response to the connected peer using the command struct passed in. 
*
* Parameters
* *************
* command - the Command struct that has been processed.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void sendResponse(struct Command command) {
  char* response = buildResponse(command);
  client.println(response);
}

/************************************************************************
*
* Name
* *************
* buildResponse
*
* Description
* *************
* Builds a response string, which is the Command struct's command character,
* followed by a space, followed by the Command struct's result string.
*
* Parameters
* *************
* command - the Command struct that has been processed.
*
* Returns
* *************
* char* - the response string built from the passed in Command struct.
*
*
*************************************************************************/ 
char* buildResponse(struct Command command) {
  char response[102];

  response[0] = command.cmd;
  response[1] = ' ';
  
  int index = 0;
  while(true) {
    if(command.result[index] == '\0') {
      response[index + 2] = '\0';
      break;
    }

    response[index + 2] = command.result[index];
    index++;
  }

  return response;
}

/************************************************************************
*
* Name
* *************
* displayBlink
*
* Description
* *************
* This does the first part of the lab, where it displays student numbers
* and blinks the LED for 5 seconds.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void displayBlink() {
  delay(1000);
  doublePrint("6002764", 7, "6965144", 7);
  digitalWrite(BOARD_LED, LOW);
  
  for(int i = 0; i < 5; i++) {
    delay(990);
    flickerBeat();
  }
}

/////////////////////////////////////////////////////////////////////////
///////////////////// LAB 4 COMMANDS FUNCTIONS //////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* moveCMForward
*
* Description
* *************
* Moves the robot forward by the passed in number of centimeters
*
* Parameters
* *************
* cm - the number of centimeters to move forward
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void moveCMForward(int cm) {
  if(!SERIAL_DEBUG) {
    moveTicksForward(cm * TICKS_PER_CM);
  } else {
    Serial.print("Moving forward ");
    Serial.println(cm);
  }
}

/************************************************************************
*
* Name
* *************
* moveCMBackward
*
* Description
* *************
* Moves the robot backwards by the passed in number of centimeters
*
* Parameters
* *************
* cm - the number of centimeters to move backwards
*
* Returns
* *************
* void
*
*
*************************************************************************/
void moveCMBackwards(int cm) {
  if(!SERIAL_DEBUG) {
    moveTicksBackward(cm * TICKS_PER_CM);
  } else {
    Serial.print("Moving backward ");
    Serial.println(cm);
  }
}

/************************************************************************
*
* Name
* *************
* readTemperature
*
* Description
* *************
* Reads the temperature from the temperature sensor, and returns it as a 
* c-String.
*
* Parameters
* *************
* None
*
* Returns
* *************
* char* - the temperature as a c-string.
*
*
*************************************************************************/
char* readTemperature() {
  if(!SERIAL_DEBUG) {
    char tempString[10];
    byte temp = readTemperatureSensor();
    sprintf(tempString, "%d", temp);
    return tempString;
  } else {
    Serial.println("Reading the temperature");
    return "10 degrees C"; 
  }
}

/************************************************************************
*
* Name
* *************
* readDistance
*
* Description
* *************
* Reads the distance using the sonar sensor and returns the result in 
* centmeters as a c-String.
*
* Parameters
* *************
* None
*
* Returns
* *************
* char* - the distance to the nearest straight-ahead object in centimeters,
* as a c-String. 
*
*
*************************************************************************/
char* readDistance() {
  if(!SERIAL_DEBUG) {
    char distString[10];
    long distance = readSonarSensor() / (29 * 2);
    sprintf(distString, "%d", distance);
    return distString;
  } else {
    Serial.println("Reading the distance");
    return "1000 cm"; 
  }
}

/////////////////////////////////////////////////////////////////////////
///////////////////// BASIC MOVEMENT METHODS ////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* leftForward
*
* Description
* *************
* This causes the left wheel to rotate forward.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void leftForward() {
  attachLeftServo();
  leftServo.write(LEFT_FORWARD);
}

/************************************************************************
*
* Name
* *************
* rightBackward
*
* Description
* *************
* This causes the right wheel to rotate backward.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void rightBackward() {
  attachRightServo();
  rightServo.write(RIGHT_BACKWARD);
}

/************************************************************************
*
* Name
* *************
* leftBackward
*
* Description
* *************
* This causes the left wheel to rotate backward.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void leftBackward() {
  attachLeftServo();
  leftServo.write(LEFT_BACKWARD);
}

/************************************************************************
*
* Name
* *************
* rightForward
*
* Description
* *************
* This causes the right wheel to rotate forward.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void rightForward() {
  attachRightServo();
  rightServo.write(RIGHT_FORWARD);
}

/************************************************************************
*
* Name
* *************
* fullStop
*
* Description
* *************
* This causes the robot to stop more permenantly than either of the
* individual wheel stop functions. It accomplishes this by detaching the 
* servos from the driver.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void fullStop() {
  rightStop();
  delay(5);
  isLeftAttached = false;
  isRightAttached = false;
  leftServo.detach();
  rightServo.detach(); 
}

/************************************************************************
*
* Name
* *************
* leftStop
*
* Description
* *************
* This causes the left wheel to stop rotating. This is to be used as a
* temporary stop. See fullStop for a more complete stop. This is done without 
* detaching the servos. Unfortunately, for our particular robot, there is
* no motor constant that allows the left side to stop completely.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void leftStop() {
  attachLeftServo();
  leftServo.write(LEFT_STOP);
}

/************************************************************************
*
* Name
* *************
* rightStop
*
* Description
* *************
* This causes the left wheel to stop rotating. This is to be used as a
* temporary stop. See fullStop for a more complete stop.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void rightStop() {
  attachRightServo();
  rightServo.write(RIGHT_STOP);
}

/////////////////////////////////////////////////////////////////////////
////////////////// TICK-BASED COMPLEX MOVEMENT METHODS //////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* moveTicksForward
*
* Description
* *************
* This causes the robot to move forward for the specified number of wheel
* sensor ticks. It self-corrects any discrepencies between the left and
* right wheels. To smooth out the movement, a threshold difference between 
* the wheels is used. No correction takes place until the two wheels are
* out of sync by at least the threshold.
*
* Parameters
* *************
* int numTicks - the number of sensor segments the robot should move 
* forward.
*
* Returns
* *************
* void
*
*
*************************************************************************/
void moveTicksForward(int numTicks) {
  doublePrint("Moving", 6, "forward", 7);

  boolean leftReading = readLeftSensor();
  boolean rightReading = readLeftSensor();

  int numLeftTicks = -1;
  int numRightTicks = 0;

  leftForward();
  rightForward();
    
  while(numLeftTicks < numTicks || numRightTicks < numTicks) {

    if(leftReading != readLeftSensor()) {
      ++numLeftTicks;
      leftReading = readLeftSensor();
    }

    if(rightReading != readRightSensor()) {
      ++numRightTicks;
      rightReading = readRightSensor();
    } 

    // right and left discrepency correction
    if((numRightTicks - numLeftTicks) > THRESHOLD) {
      rightStop();
      leftForward();
    } else if((numLeftTicks - numRightTicks) > THRESHOLD) {
      leftStop();
      rightForward();
    } else {
      leftForward();
      rightForward();
    }
  }

  stopRobot();
}

/************************************************************************
*
* Name
* *************
* moveTicksBackward
*
* Description
* *************
* This causes the robot to move backward for the specified number of wheel
* sensor ticks. It self-corrects any discrepencies between the left and
* right wheels. To smooth out the movement, a threshold difference between 
* the wheels is used. No correction takes place until the two wheels are
* out of sync by at least the threshold.
*
* Parameters
* *************
* int numTicks - the number of sensor segments the robot should move 
* backward.
*
* Returns
* *************
* void
*
*
*************************************************************************/
void moveTicksBackward(int numTicks) {
  doublePrint("Moving", 6, "backward", 8);
  
  boolean leftReading = readLeftSensor();
  boolean rightReading = readLeftSensor();

  int numLeftTicks = -1;
  int numRightTicks = 0;

  leftBackward();
  rightBackward();
    
  while(numRightTicks < numTicks || numRightTicks < numTicks) {

    if(rightReading != readRightSensor()) {
      numRightTicks++;
      rightReading = readRightSensor();
    } 
    
    if(leftReading != readLeftSensor()) {
      numLeftTicks++;
      leftReading = readLeftSensor();
    }

    // right and left discrepency correction
    if((numLeftTicks - numRightTicks) > THRESHOLD) {
      leftStop();
      rightBackward();
    } else if((numRightTicks - numLeftTicks) > THRESHOLD) {
      rightStop();
      leftBackward();
    } else {
      leftBackward();
      rightBackward();
    }
  }

  stopRobot();
}

/************************************************************************
*
* Name
* *************
* turnTicksLeft
*
* Description
* *************
* This causes the robot to turn left for the provided number of ticks. 
* It self-corrects any discrepencies between the left and
* right wheels. To smooth out the movement, a threshold difference between 
* the wheels is used. No correction takes place until the two wheels are
* out of sync by at least the threshold.
*
* Parameters
* *************
* int numTicks - the number of sensor segments the robot should move 
* forward.
*
* Returns
* *************
* void
*
*
*************************************************************************/
void turnTicksLeft(int numTicks) {
  doublePrint("Turning", 7, "left", 4);
  
  boolean leftReading = readLeftSensor();
  boolean rightReading = readLeftSensor();

  int numLeftTicks = -1;
  int numRightTicks = 0;

  rightForward();
  leftBackward();
    
  while(numRightTicks < numTicks || numRightTicks < numTicks) {

    if(rightReading != readRightSensor()) {
      numRightTicks++;
      rightReading = readRightSensor();
    } 
    
    if(leftReading != readLeftSensor()) {
      numLeftTicks++;
      leftReading = readLeftSensor();
    }

    // right and left discrepency correction
    if((numLeftTicks - numRightTicks) > THRESHOLD) {
      leftStop();
      rightForward();
    } else if((numRightTicks - numLeftTicks) > THRESHOLD) {
      rightStop();
      leftBackward();
    } else {
      rightForward();
      leftBackward();
    }
  }

  stopRobot();
}

/************************************************************************
*
* Name
* *************
* turnTicksRight
*
* Description
* *************
* This causes the robot to turn right for the provided number of ticks. 
* It self-corrects any discrepencies between the left and
* right wheels. To smooth out the movement, a threshold difference between 
* the wheels is used. No correction takes place until the two wheels are
* out of sync by at least the threshold.
*
* Parameters
* *************
* int numTicks - the number of sensor segments the robot should move 
* forward.
*
* Returns
* *************
* void
*
*
*************************************************************************/
void turnTicksRight(int numTicks) {
  doublePrint("Turning", 7, "right", 5);

  boolean leftReading = readLeftSensor();
  boolean rightReading = readLeftSensor();

  int numLeftTicks = -1;
  int numRightTicks = 0;

  leftForward();
  rightBackward();
    
  while(numLeftTicks < numTicks || numRightTicks < numTicks) {

    if(leftReading != readLeftSensor()) {
      numLeftTicks++;
      leftReading = readLeftSensor();
    }

    if(rightReading != readRightSensor()) {
      numRightTicks++;
      rightReading = readRightSensor();
    } 

    // right and left discrepency correction
    if((numRightTicks - numLeftTicks) > THRESHOLD) {
      rightStop();
      leftForward();
    } else if((numLeftTicks - numRightTicks) > THRESHOLD) {
      leftStop();
      rightBackward();
    } else {
      leftForward();
      rightBackward();
    }
  }

  stopRobot();
}

/************************************************************************
*
* Name
* *************
* stopRobot
*
* Description
* *************
* This causes the robot to stop completely, and prints a 'stopped' message.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void stopRobot() {
  fullStop();
  singlePrint("Stopped", 7);
}

/////////////////////////////////////////////////////////////////////////
//////////// TILE AND DEGREE-BASED COMPLEX MOVEMENT METHODS /////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* moveTilesForward
*
* Description
* *************
* This uses the moveTicksForward to move the robot forward a number of tiles
*
* Parameters
* *************
* float numTiles - the number of tiles to move forward
*
* Returns
* *************
* void
*
*
*************************************************************************/
void moveTilesForward(float numTiles) {
  moveTicksForward(numTiles * TICKS_PER_TILE);
}

/************************************************************************
*
* Name
* *************
* moveTilesBackward
*
* Description
* *************
* This uses the moveTicksBackward to move the robot backward a number of tiles
*
* Parameters
* *************
* float numTiles - the number of tiles to move backward
*
* Returns
* *************
* void
*
*
*************************************************************************/
void moveTilesBackward(float numTiles) {
  moveTicksBackward(numTiles * TICKS_PER_TILE);
}

/************************************************************************
*
* Name
* *************
* turnDegreesLeft
*
* Description
* *************
* This uses the turnTicksLeft to turn a certain number of degrees
*
* Parameters
* *************
* int numDegrees - the number of degrees to turn to the left
*
* Returns
* *************
* void
*
*
*************************************************************************/
void turnDegreesLeft(int numDegrees) {
  if(!SERIAL_DEBUG) {
    turnTicksLeft(numDegrees * TICKS_PER_DEGREE_LEFT);
  } else {
    Serial.print("Turning left ");
    Serial.println(numDegrees);
  }
}

/************************************************************************
*
* Name
* *************
* turnDegreesRight
*
* Description
* *************
* This uses the turnTicksRight to turn a certain number of degrees
*
* Parameters
* *************
* int numDegrees - the number of degrees to turn to the right
*
* Returns
* *************
* void
*
*
*************************************************************************/
void turnDegreesRight(int numDegrees) {
  if(!SERIAL_DEBUG) {
    turnTicksRight(numDegrees * TICKS_PER_DEGREE_RIGHT);
  } else {
    Serial.print("Turning right ");
    Serial.println(numDegrees);
  }
}

/////////////////////////////////////////////////////////////////////////
///////////////// TILES TO TICKS AND DEGREES METHODS ////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* tilesToTicks
*
* Description
* *************
* A convenience method to translate tile distance inputs to ticks.
*
* Parameters
* *************
* float tiles - the number of tiles to convert to ticks.
*
* Returns
* *************
* int ticks - the number of ticks corresponding to the input number of 
*             tiles.
*
*
*************************************************************************/ 
int tilesToTicks(float tiles) {
  return int(tiles * TICKS_PER_TILE);
}

/************************************************************************
*
* Name
* *************
* degreesToTicks
*
* Description
* *************
* A convenience method to translate rotational degreess to ticks.
*
* Parameters
* *************
* int degrees - the number of degrees to convert to ticks.
*
* Returns
* *************
* int ticks - the number of ticks corresponding to the input number of 
*             degrees.
*
*
*************************************************************************/ 
int degreesToTicks(int degrees) {
  return int(degrees * TICKS_PER_DEGREE_RIGHT);
}

/////////////////////////////////////////////////////////////////////////
/////////////////////. HEAD MOVEMENT METHODs ////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* turnHeadTo
*
* Description
* *************
* This causes the robot head to turn to the specified degree. 
*
* Parameters
* *************
* int degree - the degree to turn the robot head to. 0 degrees is a full
*              left head turn 180 is a full right head turn. 90 degrees is
*              is straight on.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void turnHeadTo(int degree) {
  if(degree < MAX_LEFT_HEAD_TURN) degree = MAX_LEFT_HEAD_TURN;
  else if(degree > MAX_RIGHT_HEAD_TURN) degree = MAX_RIGHT_HEAD_TURN;

  if(!isHeadAttached) headServo.attach(HEAD_MOTOR);

  headServo.write(FULL_LEFT_DEGREE + degree);
}

/////////////////////////////////////////////////////////////////////////
/////////////////// LCD SCREEN RELATED METHODs //////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* lcdPrint
*
* Description
* *************
* This is a wrapper method for printing messages to the LCD screen.
*
* Parameters
* *************
* char* msg - a C-string that is the message to be displayed.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void lcdPrint(char* msg) {
  LCD.print(msg);
}

/************************************************************************
*
* Name
* *************
* sendCommand
*
* Description
* *************
* This is a wrapper method for sending commands to the LCD screen. 
* The LCD command hex numbers are defined ath the top.
*
* Parameters
* *************
* byte cmd - the defined command for the LCD screen.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void sendCommand(byte cmd) {
  LCD.write(CMD_FLAG);
  LCD.write(cmd);
  delay(10);
}

/************************************************************************
*
* Name
* *************
* placeCursor
*
* Description
* *************
* This is a convenience method that moves the cursor to the requested row
* and column.
*
* Parameters
* *************
* int row - the row the cursor should move to.
* int col - the column the cursor should move to.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void placeCursor(int row, int col) {
  LCD.write(CMD_FLAG);
  LCD.write(col + row*64 + 128);
  delay(10);
}

/************************************************************************
*
* Name
* *************
* doublePrint
*
* Description
* *************
* This prints two strings, one on the top row, one on the bottom row of the
* LCD screen.
*
* Parameters
* *************
* char* wordOne - the C-style string to print on the top row.
* int lengthOne - the length of wordOne.
* char* wordTwo - the C-style string to print on the bottom row.
* int lengthTwo - the length of wordTwo.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void doublePrint(char* wordOne, int lengthOne, char* wordTwo, int lengthTwo) {
  sendCommand(CLEAR);
  
  int spacerOne = (15-lengthOne)/2;
  int spacerTwo = (15-lengthTwo)/2;
  
  placeCursor(0, spacerOne);
  lcdPrint(wordOne);
  placeCursor(1, spacerTwo);
  lcdPrint(wordTwo);
}

/************************************************************************
*
* Name
* *************
* singlePrint
*
* Description
* *************
* This prints one string, centered on the top row of the LCD screen.
* It just calls doublePrint with empty arguments for the second word.
*
* Parameters
* *************
* char* wordOne - the C-style string to print on the top row.
* int lengthOne - the length of wordOne.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void singlePrint(char* wordOne, int lengthOne) {
  doublePrint(wordOne, lengthOne, "\0", 15);
}

/////////////////////////////////////////////////////////////////////////
///////////////////////// SENSOR METHODS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* readLeftSensor
*
* Description
* *************
* Reads the optical sensor input for the left wheel.
*
* Parameters
* *************
* None
*
* Returns
* *************
* boolean - true if the sensor is detecting a shiny segment of the sensor,
*           false otherwise. 
*
*
*************************************************************************/ 
boolean readLeftSensor() {
  return boolean(digitalRead(LEFT_WHEEL_SENSOR));
}

/************************************************************************
*
* Name
* *************
* readRightSensor
*
* Description
* *************
* Reads the optical sensor input for the right wheel.
*
* Parameters
* *************
* None
*
* Returns
* *************
* boolean - true if the sensor is detecting a shiny segment of the sensor,
*           false otherwise. 
*
*
*************************************************************************/ 
boolean readRightSensor() {
  return boolean(digitalRead(RIGHT_WHEEL_SENSOR));
}

/************************************************************************
*
* Name
* *************
* readSonarSensor
*
* Description
* *************
* Gets a reading from the sonar range sensor.
*
* Parameters
* *************
* None
*
* Returns
* *************
* long - the raw sonar reading
*
*
*************************************************************************/ 
long readSonarSensor() {
  pinMode(SONAR, OUTPUT);
  digitalWrite(SONAR, LOW);
  delayMicroseconds(2);
  digitalWrite(SONAR, HIGH);
  delayMicroseconds(5);
  digitalWrite(SONAR, LOW);
  pinMode(SONAR, INPUT);
  return pulseIn(SONAR, HIGH);
}

/************************************************************************
*
* Name
* *************
* readTemperatureSensor
*
* Description
* *************
* Reads the temperature sensor.
*
* Parameters
* *************
* None
*
* Returns
* *************
* byte - the temperature reading
*
*
*************************************************************************/ 
byte readTemperatureSensor() {
  Wire.beginTransmission(TEMP_SENSOR);
  Wire.write(tempurature_reg);
  Wire.endTransmission();
  Wire.requestFrom(TEMP_SENSOR, 1);
  while(Wire.available() < 1);
  return Wire.read();
}

void printTemp() {
  char temperatureString[11];
  String temp = String(readTemperatureSensor()) + " degrees";
  temp.toCharArray(temperatureString, 11);
  
  //printf(temperatureString, readTemperatureSensor());
  doublePrint("Temperature", 11, temperatureString, 11); 
}

/////////////////////////////////////////////////////////////////////////
///////////////////////// OTHER METHODS /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* flickerBeat
*
* Description
* *************
* This causes the Arduino LED to flash on and off.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void flickerBeat(){
  digitalWrite(BOARD_LED, HIGH);
  delay(20);
  digitalWrite(BOARD_LED, LOW);
} 

/************************************************************************
*
* Name
* *************
* attachLeftServo
*
* Description
* *************
* Initializes the left motor for movement commands via the Servo library.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void attachLeftServo() {
  if (!isLeftAttached) {
    isLeftAttached = true;
    leftServo.attach(LEFT_MOTOR);
  }
}

/************************************************************************
*
* Name
* *************
* attachRightServo
*
* Description
* *************
* Initializes the right motor for movement commands via the Servo library.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void attachRightServo() {
  if (!isRightAttached) {
    isRightAttached = true;
    rightServo.attach(RIGHT_MOTOR);
  }
}
