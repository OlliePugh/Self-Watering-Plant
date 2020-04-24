#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);  // This sets up the LCD Screen

int refresh_time = 43200000;  // How often the system checks for updates (In milliseconds) (this is 12 hours)
int moisture_sensor_pin = A3;
int moisture;

int water_level_sensor_pin = A0;
int water_level;

int water_pump_pin = 4;

double desired_moistness = 415;
double desired_water_level = 315;

String lcd_state;

bool watering_active = false; // This is used for debugging

void clear_lcd() {
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
}

void lcd_write(String type, String value = "Null") {
  if (type == "starting_system" && lcd_state != "starting_system") {
    clear_lcd();
    lcd.setCursor(0, 0);
    lcd.print("Starting System");
    lcd_state = "starting_system";
  }
  else if (type == "water_plant" && lcd_state != "water_plant") {
    clear_lcd();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Adding Water");
    lcd.setCursor(0, 1);
    lcd.print("For Seconds");
    lcd_state = "water_plant";
  }
  else if (type == "water_warning" && lcd_state != "water_warning") {
    clear_lcd();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Water Needs");
    lcd.setCursor(0, 1);
    lcd.print("Refilling");
    lcd_state = "water_warning";
  }
  else if (type != "water_warning" && type != "water_plant" && type != "starting_system") {
    if (lcd_state != "UNKNOWN") {
      clear_lcd();
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("UNKNOWN LCD");
      lcd.setCursor(0, 1);
      lcd.print("STRING");
      Serial.println(type);  // See what string is being sent to the function
    }
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd_write("starting_system");
  String lcd_state = "starting_screen";

  pinMode(moisture_sensor_pin, INPUT); 
  pinMode(water_level_sensor_pin, INPUT);
  pinMode(water_pump_pin, OUTPUT);
  
  digitalWrite(water_pump_pin, HIGH);  // The relay is inversed so HIGH = LOW (GRRR)
}

void water_plant(int duration) {
  int start_time = millis();
  
  //lcd_write("water_plant", duration);
  Serial.println("Watering Plant");

  while (millis()-start_time <= duration){
    if (watering_active == true){ // if the system is not in debug mode
      digitalWrite(water_pump_pin, LOW);  // Turn the pump on 
    }
    else {
      Serial.println("NO ACTIVE WATERING - WATER_ACTIVE NEEDS TO BE TRUE FOR WATERING");
    }
  }
  
  digitalWrite(water_pump_pin, HIGH);  // Turn the pump off
  Serial.println("Finishing Watering");
}

void water_warning() {
  if (lcd_state != "water_warning"){  // This makes sure that the screen isnt getting spammed with messages to update
    lcd_write("water_warning");
  }
}

void loop() {
  
  moisture = analogRead(moisture_sensor_pin);  // Get the value of the moisture sensor
  water_level = analogRead(water_level_sensor_pin);  // Get the value of the water level sensor
  
  
  Serial.println(moisture);

  if (water_level < desired_water_level) {
    Serial.println("Low Water Level");
    water_warning();
  }

  else if (moisture > desired_moistness) {
    water_plant(500);  // TODO This needs to be changed to compensate for the amount of water that may be requried, or just go linear
    Serial.println("Watering Plant");
  }

  delay(refresh_time);  // Save power by making the system sleep until it is time to use it again
}
