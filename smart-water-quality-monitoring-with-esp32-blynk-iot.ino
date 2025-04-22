#define BLYNK_TEMPLATE_ID "TMPL3pOAc0TpX"
#define BLYNK_TEMPLATE_NAME "Water Quality Monitoring"
#define BLYNK_AUTH_TOKEN "5i7WnkJmsuGEC52vcxIGAWw1CO7fEwKQ"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// LCD configuration
LiquidCrystal_PCF8574 lcd(0x27); // Use correct I2C address (0x27 or 0x3F)

// WiFi credentials
char ssid[] = "Galaxy A03 Core0945";
char pass[] = "jsqe8129";

char auth[] = BLYNK_AUTH_TOKEN;

namespace pin {
    const byte tds_sensor = 34;
}

namespace device {
    float aref = 3.3; // Vref, this is for 3.3V compatible controller boards, for Arduino use 5.0V.
}

namespace sensor {
    float ec = 0;
    unsigned int tds = 0;
    float ecCalibration = 1;
}
void displayName(const char* firstName, const char* surname)
    {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print(firstName);
      lcd.setCursor(3, 1);
      lcd.print(surname);
      delay(3000);
    }

void setup() {
    Serial.begin(115200); // Debugging on hardware Serial 0
    Blynk.begin(auth, ssid, pass);

    // Initialize LCD
    Wire.begin();
    lcd.begin(16, 2); // Set as a 16x2 LCD
    lcd.setBacklight(255); // Set backlight brightness

    // Display initial message
    lcd.setCursor(3, 0);
    lcd.print("LogiK Jets");
    delay(5000);
    
    displayName("Lakshya", "Sharma");
    displayName("Geetansh", "Kumawat");
    displayName("Jayesh", "Mangal");
    displayName("Jasmeet", "Kaur");
    displayName("Kritika", "Sharma");
    
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Water Quality");
    lcd.setCursor(3, 1);
    lcd.print("Monitoring   ");
    delay(2000);
    lcd.clear();
}

void loop() {
    Blynk.run();
    readTdsQuick();
    displayTdsEc();
    delay(1000);
}

void readTdsQuick() {
    // Read the raw analog value and convert to voltage
    float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0;
    
    // Debugging: Print the raw analog value
    Serial.print(F("Raw Analog Value: "));
    Serial.println(rawEc);

    // Adjust this offset based on the sensor's dry reading (without immersion)
    float offset = 0.14; // Set this to the observed raw analog value in air

    // Apply calibration and offset compensation
    sensor::ec = (rawEc * sensor::ecCalibration) - offset;

    // If the EC is below zero after adjustment, set it to zero
    if (sensor::ec < 0) sensor::ec = 0;

    // Convert voltage value to TDS value using a cubic equation
    sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5 / 10;

    // Debugging: Print the TDS and EC values
    Serial.print(F("TDS: "));
    Serial.println(sensor::tds);
    Serial.print(F("EC: "));
    Serial.println(sensor::ec, 2);

    // Display values on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.setCursor(5, 0);
    lcd.print(sensor::tds);

    lcd.setCursor(0, 1);
    lcd.print("EC: ");
    lcd.setCursor(5, 1);
    lcd.print(sensor::ec, 2);

    // Send data to Blynk virtual pins
    Blynk.virtualWrite(V0, sensor::tds);
    Blynk.virtualWrite(V1, sensor::ec);
}
void displayTdsEc()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TDS: ");
  lcd.setCursor(5, 0);
  lcd.print(sensor::tds);

  lcd.setCursor(0, 1);
  lcd.print("EC: ");
  lcd.setCursor(5, 1);
  lcd.print(sensor::ec, 2);
}