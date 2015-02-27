/************************************************************************
*
* Names: Craig Bryan
*        Kat Drobnjakovic
*
* Course Code: SEG 4145
* Lab Number: 3
* File name: mr_roboto_lab_2
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

// MOTOR CONSTANTS
#define LEFT_MOTOR 2
#define RIGHT_MOTOR 8

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
#define TICKS_PER_DEGREE_LEFT 0.59
#define TICKS_PER_DEGREE_RIGHT 0.58
// right = 45
// left = 49
#define TICKS_PER_TILE 103

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
#define SONAR 22 //TODO set

// TEMPERATURE SENSOR CONSTANTS
#define TEMP_SENSOR 0x68 //address
int tempurature_reg = 0x01; //ambient temperature register

// OTHER CONSTANT
#define BOARD_LED 13

void setup() {
  //initialize board light pin
  pinMode(BOARD_LED, OUTPUT);

  //initialize wheel sensor
  pinMode(RIGHT_WHEEL_SENSOR, INPUT);
  pinMode(LEFT_WHEEL_SENSOR, INPUT);
  
  //Initialize lcd 
  LCD.begin(9600); //open serial port to LCD screen with baud at 9600
  
  //Initialize the temperature sensor communication
  Wire.begin();

  //Initialized serial port for debug
  Serial.begin(9600); 
}

void loop() {
  displayBlink();
  path();
  delay(1000);
 
}

/////////////////////////////////////////////////////////////////////////
///////////////////// LAB 2 SPECIFIC METHODS ////////////////////////////
/////////////////////////////////////////////////////////////////////////

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

/************************************************************************
*
* Name
* *************
* path
*
* Description
* *************
* The path for lab 2.
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
void path() {
  singlePrint("Path", 6);

  moveTilesForward(2); 
  turnDegreesRight(90);
  moveTilesForward(2);
  turnDegreesRight(90);
  moveTilesForward(3);
  turnDegreesRight(90);
  moveTilesForward(3);
  turnDegreesRight(90);
  moveTilesForward(2);
  turnDegreesLeft(45);
  moveTilesForward(1.41);
  turnDegreesLeft(135);
  moveTilesForward(4);
  turnDegreesLeft(90);
  moveTilesForward(2);
  turnDegreesLeft(90);
  moveTilesForward(2);
  fullStop();
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

void thing() {
  char* thing = "";
  String temp = String(readTemperatureSensor());


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
  turnTicksLeft(numDegrees * TICKS_PER_DEGREE);
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
  turnTicksRight(numDegrees * TICKS_PER_DEGREE);
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
int degressToTicks(int degrees) {
  return int(degrees * TICKS_PER_DEGREE_RIGHT);
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
  Wire.requestFrom(TEMP_SENSOR, 1)
  while(Wire.available() < 1);
  return Wire.read();
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


