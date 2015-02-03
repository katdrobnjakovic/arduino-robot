/**************************************************
****************************
*
* Names: Craig Bryan
*        Kat Drobnjakovic
*
* Course Code: SEG 4145
* Lab Number: 1
* File name: mr_roboto_lab_1
* Date: January 22, 2015
*
*
* Description
* *************
* A simple set of robot controls.
*
**************************************************
****************************
*/ 

#include <SoftwareSerial.h>

// MOTOR CONSTANTS
#define LEFT_MOTOR 2
#define RIGHT_MOTOR 4

// DIRECTION CONSTANTS
#define LEFT_BACKWARD 10
#define LEFT_FORWARD 194
#define RIGHT_BACKWARD 191.5
#define RIGHT_FORWARD 10
#define STOP 0

// TURN CONSTANTS
#define RIGHT_MS_PER_DEG 11.80
#define LEFT_MS_PER_DEG 10.5

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

// OTHER CONSTANT
#define BOARD_LED 13

void setup() {
  //initialize motor pins
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  
  //initialize board light pin
  pinMode(BOARD_LED, OUTPUT);
  
  //Initialize lcd 
  LCD.begin(9600); //open serial port to LCD screen with baud at 9600
  
  //Initialized serial port for debug
  Serial.begin(9600); 
}

void loop() {
  displayBlink();
  path1();
  path2();
  
}

/////////////////////////////////////////////////////////////////////////
///////////////////// LAB 1 SPECIFIC METHODS ////////////////////////////
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
* path1
*
* Description
* *************
* The first path for lab 1.
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
void path1() {
   singlePrint("Path 1", 6);
   
   //Forward traversal
   delay(3000);
   moveForward(5000);
   turnLeft(90);
   moveForward(5000);
   turnRight(90);
   moveForward(5000);
   turnRight(90);
   moveForward(5000);
   turnLeft(90);
   moveForward(5000);
   fullStop();
   delay(5000);
   
   //Backward traversal
   /**moveBackward(5000);
   turnRight(90);
   moveBackward(5000);
   turnLeft(90);
   moveBackward(5000);
   turnLeft(90);
   moveBackward(5000);
   turnRight(90);
   moveBackward(5000);
   fullStop();
   delay(5000);*/
}

/************************************************************************
*
* Name
* *************
* path2
*
* Description
* *************
* The second path for lab 1.
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
void path2() {
   singlePrint("Path 2", 6);
   
   //Forward traversal
   delay(3000);
   moveForward(5000);
   turnRight(90);
   moveForward(5000);
   turnLeft(135);
   moveForward(5000);
   turnRight(135);
   moveForward(5000);
   turnLeft(90);
   moveForward(5000);
   fullStop();
   delay(5000);
   
   //Backward traversal
   /*moveBackward(5000);
   turnRight(90);
   moveBackward(5000);
   turnLeft(135);
   moveBackward(5000);
   turnRight(135);
   moveBackward(5000);
   turnLeft(90);
   moveBackward(5000);
   fullStop();*/
}

/////////////////////////////////////////////////////////////////////////
///////////////////// BASIC MOVEMENT METHODS ////////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* forward
*
* Description
* *************
* This causes the robot to start moving forward.
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
void forward() {
  analogWrite(LEFT_MOTOR, LEFT_FORWARD);
  analogWrite(RIGHT_MOTOR, RIGHT_FORWARD);
}

/************************************************************************
*
* Name
* *************
* backward
*
* Description
* *************
* This causes the robot to start moving backward.
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
void backward() {
  analogWrite(LEFT_MOTOR, LEFT_BACKWARD);
  analogWrite(RIGHT_MOTOR, RIGHT_BACKWARD);
}

/************************************************************************
*
* Name
* *************
* left
*
* Description
* *************
* This causes the robot to start turning left.
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
void left() {
  analogWrite(LEFT_MOTOR, LEFT_BACKWARD);
  analogWrite(RIGHT_MOTOR, RIGHT_FORWARD);
}

/************************************************************************
*
* Name
* *************
* right
*
* Description
* *************
* This causes the robot to start turning right.
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
void right() {
  analogWrite(LEFT_MOTOR, LEFT_FORWARD);
  analogWrite(RIGHT_MOTOR, RIGHT_BACKWARD);
}

/************************************************************************
*
* Name
* *************
* stop
*
* Description
* *************
* This causes the robot to stop completely.
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
  analogWrite(LEFT_MOTOR, STOP);
  analogWrite(RIGHT_MOTOR, STOP); 
  singlePrint("Stopped", 7);
}

/////////////////////////////////////////////////////////////////////////
///////////////// MORE COMPLEX MOVEMENT METHODS /////////////////////////
/////////////////////////////////////////////////////////////////////////

/************************************************************************
*
* Name
* *************
* moveForward
*
* Description
* *************
* This causes the robot to move forward for the specified time.
*
* Parameters
* *************
* int duration - the amount of time the robot should move forward.

* Returns
* *************
* void
*
*
*************************************************************************/ 
void moveForward(int duration) {
  doublePrint("Moving", 6, "forward", 7);

  int t = 0;

  while(t < duration) {
    forward();
    delay(1);
    t++;
  }

  fullStop();
}

/************************************************************************
*
* Name
* *************
* moveBackwards
*
* Description
* *************
* This causes the robot to move backward for the specified time.
*
* Parameters
* *************
* int duration - the amount of time the robot should move backward.

* Returns
* *************
* void
*
*
*************************************************************************/ 
void moveBackward(int duration) {
  doublePrint("Moving", 6, "backward", 8);
  int t = 0;

  while(t < duration) {
    backward();
    delay(1);
    t++;
  }

  fullStop();
}

/************************************************************************
*
* Name
* *************
* turnLeft
*
* Description
* *************
* This causes the robot to turn by the provided number of degrees to the 
* left.
*
* Parameters
* *************
* int deg - the number of degrees to the left the robot should turn.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void turnLeft(int deg) {
  doublePrint("Turning", 7, "left", 4);
  int t = 0;

  while(t < deg*LEFT_MS_PER_DEG) {
    left();
    delay(1);
    t++;
  }

  fullStop();
}

/************************************************************************
*
* Name
* *************
* turnRight
*
* Description
* *************
* This causes the robot to turn by the provided number of degrees to the 
* right.
*
* Parameters
* *************
* int deg - the number of degrees to the right the robot should turn.
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
void turnRight(int deg) {
  doublePrint("Turning", 7, "right", 5);
  int t = 0;

  while(t < deg*RIGHT_MS_PER_DEG) {
    right();
    delay(1);
    t++;
  }

  fullStop();
}

/////////////////////////////////////////////////////////////////////////
/////////////////// LCD SCREEN RELATED METHOD ///////////////////////////
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
  delay(10);
  digitalWrite(BOARD_LED, LOW);
} 

/************************************************************************
*
* Name
**************
* SerialEvent
*
* Description
* *************
* This is a built-in Arduino method that fires when a serial message is 
* detected over the default serial port. We used it to debug the movement
* methods.
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
void SerialEvent() {
  while(Serial.available()) {
     char inChar = (char)Serial.read();
     
     if (inChar == 'f') {
       forward(); 
       
     } else if (inChar == 'b') {
       backward();
       
     } else if (inChar == 'l') {
       left();
       
     } else if (inChar == 'r') {
       right();
       
     } else {
      Serial.print("Invalid command: \"");
      Serial.print(inChar); 
      Serial.print("\"\n");
     }
  }
}
