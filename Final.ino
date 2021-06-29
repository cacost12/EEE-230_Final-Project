// Libraries 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Instantiate LCD object
LiquidCrystal_I2C lcd(0x27,16,4);

// LED Pins 
const int LED1 = 26; 
const int LED2 = 27;
const int LED3 = 28;
const int LED4 = 29;
const int LED5 = 30;
const int LED6 = 31;
const int LED7 = 32;
const int LED8 = 33; 
const int LEDs[] = {LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8};

// Switch Pins
const int SW1 = 3;
const int SW2 = 4;
const int SW3 = 5; 
const int SW4 = 6;

// Button Pins 
const int BT1 = 2;
const int BT2 = 7;
const int BT3 = 8;
const int BT4 = 9;
const int DigitalIO[] = {SW1,SW2,SW3,SW4,BT1,BT2,BT3,BT4};

// Switch Assignments
const int ARMsys = SW1;
const int enterSensorEN = SW2;
const int heatSensorEN = SW3;
const int motionSensorEN = SW4; 

// Button Assignments 
const int detectOpenDoor = BT1;
const int detectFire = BT2;
const int detectMotion = BT3;
const int alarmReset = BT4;

// LED Assignments 
const int ARMLED = LED1;
const int enterSensorLED = LED2;
const int heatSensorLED = LED3; 
const int motionSensorLED = LED4; 
const int alarmLED = LED5;
const int enterAlarmLED = LED6; 
const int heatAlarmLED = LED7; 
const int motionAlarmLED = LED8; 

// Boolean Constants 
const bool ON = HIGH;
const bool OFF = LOW;

// Switch States 
bool ARMstate;
bool enterSensorENstate;
bool heatSensorENstate; 
bool motionSensorENstate;

// Button States
volatile bool openDoorState;
volatile bool detectFireState;
volatile bool detectMotionState; 
bool alarmResetState;

// Debounce Counters
volatile int openDoorDCtr = 0;
volatile int fireDCtr = 0;
volatile int motionDetectDCtr = 0;

// Alarm in progress indicator
volatile bool alarmInProg = OFF;

// Test Functions
void testInputs(int inputPin);
void testLEDs();

// Polling Functions

// pollEnableInputs: check Switches for enable setting
//                   and set LEDs accordingly
void pollEnableInputs();

// checkReset: Poll the reset switch, reset alarms when 
//             press detected
void checkReset();

// LCD Display Function

// lcdDisplay: Display the system ARM and sensor 
//             activation status
void lcdDisplay();

// Interrupt Service Routines

// Open Door: Triggers when someone opens the door
void openDoor(){
  // Check if Entrance Sensor is enabled
  if(enterSensorENstate == ON && alarmInProg == OFF){
  // Debounce check
     ++openDoorDCtr;

     if (openDoorDCtr > 100){
         // Turn on LED
         openDoorDCtr = 0;
         openDoorState = ON;
         alarmInProg = ON;
         digitalWrite(enterAlarmLED, ON);
         digitalWrite(alarmLED, ON);
      }
    }
  }

// Start Fire: Triggers when a fire is detected
void startFire(){
   // Check if Heat Sensor is enabled
   if(heatSensorENstate == ON && alarmInProg == OFF){
       // Debounce check 
       ++fireDCtr;

       // Trigger after debounce is not detected
       if(fireDCtr > 100){
           // Turn on LED 
           fireDCtr = 0;
           detectFireState = ON;
           alarmInProg = ON;
           digitalWrite(heatAlarmLED, ON);
           digitalWrite(alarmLED, ON);
        }
    }
}

void motionDetected(){
     // Check if Motion Sensor is enabled
     if(motionSensorENstate == ON && alarmInProg == OFF){
         // Debounce Check 
         ++motionDetectDCtr;

         if(motionDetectDCtr > 100){
             // Turn on LED
             motionDetectDCtr = 0;
             detectMotionState = ON;
             alarmInProg = ON;
             digitalWrite(motionAlarmLED, ON);
             digitalWrite(alarmLED, ON);
          }
      }
  }

void setup() {
  // Set LEDs as outputs, turn off LEDs initially, 
  // and set swtichs/buttons as inputs
  for(int i=0; i<8; ++i){
       pinMode(LEDs[i], OUTPUT);
       digitalWrite(LEDs[i], LOW);
       pinMode(DigitalIO[i], INPUT);
    }

  // Setup Interrupts
  attachInterrupt(digitalPinToInterrupt(detectOpenDoor), openDoor, HIGH);
  attachInterrupt(digitalPinToInterrupt(detectFire), startFire, HIGH);
  attachInterrupt(digitalPinToInterrupt(detectMotion), motionDetected, HIGH);

  // Initialize I2C interface
  Wire.begin();

  // Initialize LCD Display 
  lcd.init();
  lcd.backlight();
  
  // Setup Serial Monitor 
  Serial.begin(9600);
}

void loop() {

  // Test Functions
  //testInputs(BT1);
  //testLEDs();
  
  // Poll Enable Inputs 
  pollEnableInputs();

  // Poll Reset Switch 
  checkReset();

  // Display System Status to LCD
  lcdDisplay();

}

void testInputs(int inputPin){
     // Infinite Loop that turns on LED1 
     // when test input is pulled high 
     bool state;
     while(1){
         state = digitalRead(inputPin);
         digitalWrite(LED1, state);
         delay(250);
      }
  };

void testLEDs(){
    // Infinite Loop
    while(1){
         // cycle through LEDs
         for(int i=0; i<8; ++i){
             digitalWrite(LEDs[i], ON);
             delay(250);
             digitalWrite(LEDs[i], OFF);
          }
      }
  }

void pollEnableInputs(){
     // ARM System
     ARMstate = digitalRead(ARMsys);
     digitalWrite(ARMLED, ARMstate);

     // Poll other enables if system is armed
     if(ARMstate == ON){
         // Entrance Sensor 
         enterSensorENstate = digitalRead(enterSensorEN);
         digitalWrite(enterSensorLED, enterSensorENstate);

         // Heat Sensor 
         heatSensorENstate = digitalRead(heatSensorEN);
         digitalWrite(heatSensorLED, heatSensorENstate);

         // Motion Sensor 
         motionSensorENstate = digitalRead(motionSensorEN);
         digitalWrite(motionSensorLED, motionSensorENstate);
      
      } else { // Disable the Sensors
         enterSensorENstate = OFF; 
         heatSensorENstate = OFF;
         motionSensorENstate = OFF;

         // Turn off the LEDs 
         for (int i=0; i<8; ++i){
             digitalWrite(LEDs[i], OFF);
          }
        }
  }

void checkReset(){
     // Check Reset button only if system is armed
     // and entrance sensor is enabled
     if(ARMstate == ON){
         alarmResetState = digitalRead(alarmReset);
         if (alarmResetState == ON){
             openDoorState = OFF;
             detectFireState = OFF;
             detectMotionState = OFF; 
             alarmResetState = OFF;
             alarmInProg = OFF;
             for(int i=4; i<8; ++i){
                 digitalWrite(LEDs[i], OFF);
              }
          }
      }
  }

void lcdDisplay(){
     // Print Security System State to the LCD screen 
     lcd.setCursor(0,0);
     if(ARMstate == ON){
        // System is armed
        lcd.print("System Armed");
        // Display Sensor Activation Status
        if(openDoorState == ON){
           lcd.setCursor(0,1);
           lcd.print("Door: Active");
           lcd.setCursor(0,2);
           lcd.print("Heat: OK    ");
           lcd.setCursor(0,3);
           lcd.print("Motion: OK    ");  
          } else if(detectFireState == ON){
           lcd.setCursor(0,1);
           lcd.print("Door: OK    ");
           lcd.setCursor(0,2);
           lcd.print("Heat: Active");
           lcd.setCursor(0,3);
           lcd.print("Motion: OK    "); 
             } else if(detectMotionState == ON){
              lcd.setCursor(0,1);
              lcd.print("Door: OK    ");
              lcd.setCursor(0,2);
              lcd.print("Heat: OK    ");
              lcd.setCursor(0,3);
              lcd.print("Motion: Active");
              }else{
                lcd.setCursor(0,1);
                lcd.print("Door: OK    ");
                lcd.setCursor(0,2);
                lcd.print("Heat: OK    ");
                lcd.setCursor(0,3);
                lcd.print("Motion: OK    ");
                }
         // Correct for Enable inputs 
         if(enterSensorEN == OFF){
             lcd.setCursor(0,1);
             lcd.print("Door: OFF   ");
          }
         if (heatSensorEN == OFF){
            lcd.setCursor(0,2);
            lcd.print("Heat: OFF   ");
          }
         if (motionSensorEN == OFF){
            lcd.setCursor(0,3);
            lcd.print("Motion: OFF   ");
          }
      }else{
        // System is not armed 
        lcd.print("Ready to Arm");
        for(int i= 1; i<4; ++i){
             lcd.setCursor(0,i);
             lcd.print("            ");
          }
        }
  }
