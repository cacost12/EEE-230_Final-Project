// LCD, LM75, and I2C libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Instantiate LCD and temperature sensor objects
LiquidCrystal_I2C lcd(0x27,16,4);

// Counter Variable
volatile int count = 0;

// Debounce Counter
volatile int debounceCtr = 0;

// Button Pin
const int buttonPin = 2;

// Button State
volatile boolean state;


// Interrupt Service Routine 
void countISR(){
     // Debounce check
     ++debounceCtr;

     if (debounceCtr > 100){
         // Increment the counter
         ++count;
         debounceCtr = 0;
         state = digitalRead(buttonPin);

         // Wait until voltage goes low to return
         while(state == HIGH){
             state = digitalRead(buttonPin);
          }
      }
  }

void setup() {
  // put your setup code here, to run once:

  // Begin the serial monitor
  Serial.begin(9600);

   // Initialize I2C interface
  Wire.begin();

  // Initialize LCD Display 
  lcd.init();
  lcd.backlight();

  // Declare Button as input
  pinMode(buttonPin, INPUT);

  // Setup Interrupt
  attachInterrupt(digitalPinToInterrupt(buttonPin), countISR, HIGH);

  // Measure Initial State
  state = digitalRead(buttonPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Print the count every second
  Serial.print("Count: ");
  Serial.println(count);
  

  // Print Results to LCD Screen
  // Display results to the LCD screen
  lcd.setCursor(0,0);
  lcd.print("Counter Value: ");
  lcd.print(count);

  count = 0;
  delay(1000);
}
