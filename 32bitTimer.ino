// LCD, LM75, and I2C libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Instantiate LCD and temperature sensor objects
LiquidCrystal_I2C lcd(0x27,16,4);

// Processor Frequency 
#define TICKS_PER_SECOND 80000000

// Turn on Register T2 and T3
#define T2_ON 0x8008
#define T2_PS_1_1 0
#define T2_SOURCE_INT 0

// Counting Variables 
uint32_t lastCnt = 0;
volatile uint32_t extPeriod = 0;
volatile uint32_t extPeriodCntr = 0;
volatile uint32_t counter = 0;
volatile float realtime = 0.0;

/* Define the Interrupt Service Routine (ISR) */
void __attribute__((interrupt)) myISR() {
  extPeriodCntr++;
  
  if ( extPeriodCntr > extPeriod ){
    counter++;
    extPeriodCntr = 0;
  }
  
  clearIntFlag(_TIMER_3_IRQ);
}

/* Initialize Timer 2 */
void start_timer(uint32_t frequency) {
  uint32_t period;  
  uint32_t periodMS; // Most significant bits
  uint32_t periodLS; // Least Significant bits

  // Calculate period in clock ticks
  period = TICKS_PER_SECOND / frequency;

  // Seperate the period into 16 upper and lower bits
  periodMS = period & 0xFFFF0000;
  periodMS = periodMS >> 16;
  periodLS = period & 0x0000FFFF;

  //if ( period < 0xFFFF ){
    //extPeriod = floor(period / 0xFFFF);
    //period = 0xFFFF;
  //}
  
  //CONbits.T32 = 1;        // Turn TIMER2 into a 32-bit timer
  T2CONCLR = T2_ON;         /* Turn the timer off */
  T2CON = T2_PS_1_1;        /* Set the prescaler  */
  TMR2 = 0;                 /* Clear the counter  */
  TMR3 = 0;
  PR2 = periodLS;             /* Set the period     */
  PR3 = periodMS;
  T2CONSET = T2_ON;         /* Turn the timer on  */
} 

void setup() {
  // put your setup code here, to run once:

  // Begin the serial monitor at 9600 baud
  Serial.begin(9600);

   // Initialize I2C interface
  Wire.begin();

  // Initialize LCD Display 
  lcd.init();
  lcd.backlight();
  
  // Setp timer to trigger 100 times per second
  start_timer(100);  /* 8 kHz */

  // Declare Interrupt
  setIntVector(_TIMER_3_VECTOR, myISR);
  setIntPriority(_TIMER_3_VECTOR, 4, 0);

  // Enable interrupt and remove flag
  clearIntFlag(_TIMER_3_IRQ);
  setIntEnable(_TIMER_3_IRQ);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Display results to the LCD screen
  lcd.setCursor(0,0);
  lcd.print("Counter Value: ");
  lcd.print(counter);

  // Display results to serial monitor for debuging
  Serial.print("Counter Value: ");
  Serial.print(counter);
  Serial.print(" Time since last reset: ");
  realtime = counter*(1.0/100.0);
  Serial.print(realtime);
  Serial.println(" s");
  //Serial.print("delta is now: ");
  //Serial.println(counter - lastCnt);
  //Serial.print(" Time: ");
  //Serial.println(realtime);
  //lastCnt = counter;

  // Reset counter
  counter = 0;
  delay(1000);
}
