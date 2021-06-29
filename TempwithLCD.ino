// LCD, LM75, and I2C libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Temperature_LM75_Derived.h>

// Instantiate LCD and temperature sensor objects
LiquidCrystal_I2C lcd(0x27,16,4);

Generic_LM75 tempSensor;

// Temperature variables
float temperatureC;
float temperatureF[] = {0, 0, 0, 0};

void setup() {
  // put your setup code here, to run once:

  // Initialize Temperature Sensor
  
  // Delay until serial monitor is ready 
  while(!Serial){}

  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize I2C interface
  Wire.begin();

  // Initialize LCD Display 
  lcd.init();
  lcd.backlight();

}

void loop() {
  // put your main code here, to run repeatedly:

  // Read Temperature
  temperatureC = tempSensor.readTemperatureC();

  // Convert to fahrenheit
  temperatureF[0] = temperatureC*(9.0/5.0) + 32.0;

  // Print to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureF[0]);
  Serial.println(" F");

  // Print to LCD
  for(int i= 0; i<4; i++){
       lcd.setCursor(0,i);
       lcd.print("Temperature: ");
       lcd.print(temperatureF[i]);
       lcd.print(" F");
    }

  // Refresh Temperature values
  for (int i=1; i<4; ++i){
       temperatureF[i] = temperatureF[i-1];
    }

  // Delay for stability 
  delay(500);
}
