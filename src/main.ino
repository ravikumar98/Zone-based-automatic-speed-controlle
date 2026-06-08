/*
 * Zone-Based Automatic Speed Controller
 * Hardware: ESP32, Ublox Neo-6M GPS, L298N Motor Driver, LCD 1602 (I2C), Piezo Buzzer
 * Framework: Arduino IDE
 */

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Speed Controller"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>

// WiFi Configuration
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Pin Assignments
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define BUZZER_PIN 33
#define MOTOR_ENA  25
#define MOTOR_IN1  26
#define MOTOR_IN2  27

// Geofence Target Zone Configuration (e.g., School / Hospital Zone Center)
// Replace these coordinates with your specific location testing coordinates
const double TARGET_LAT = 12.3156; 
const double TARGET_LNG = 76.6124;
const double ZONE_RADIUS_METERS = 50.0; // 50-meter speed restriction zone

// Speeds (PWM Duty Cycles out of 255)
const int NORMAL_SPEED = 255; 
const int RESTRICTED_SPEED = 110; 

// Object Initializations
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Use ESP32 Hardware Serial 2
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27

BlynkTimer timer;
bool inRestrictedZone = false;

void checkGeofence() {
    if (gps.location.isValid()) {
        double currentLat = gps.location.lat();
        double currentLng = gps.location.lng();
        double currentSpeedKmh = gps.speed.kmph();

        // Calculate distance between current coordinates and target zone center
        double distanceMeters = TinyGPSPlus::distanceBetween(currentLat, currentLng, TARGET_LAT, TARGET_LNG);

        lcd.clear();
        if (distanceMeters <= ZONE_RADIUS_METERS) {
            // INSIDE RESTRICTED ZONE
            inRestrictedZone = true;
            digitalWrite(BUZZER_PIN, HIGH); // Sound warning buzzer
            
            // Apply speed restriction via PWM
            digitalWrite(MOTOR_IN1, HIGH);
            digitalWrite(MOTOR_IN2, LOW);
            analogWrite(MOTOR_ENA, RESTRICTED_SPEED);

            // Update Displays
            lcd.setCursor(0, 0);
            lcd.print("ZONE: RESTRICTED");
            lcd.setCursor(0, 1);
            lcd.print("Speed: Limited");

            // Push Telemetry to Blynk IoT App
            Blynk.virtualWrite(V1, currentSpeedKmh);
            Blynk.virtualWrite(V2, "RESTRICTED ZONE");
        } else {
            // OUTSIDE RESTRICTED ZONE (NORMAL DRIVING)
            inRestrictedZone = false;
            digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer

            // Standard operation speed
            digitalWrite(MOTOR_IN1, HIGH);
            digitalWrite(MOTOR_IN2, LOW);
            analogWrite(MOTOR_ENA, NORMAL_SPEED);

            // Update Displays
            lcd.setCursor(0, 0);
            lcd.print("ZONE: NORMAL  ");
            lcd.setCursor(0, 1);
            lcd.print("Speed: Unlocked");

            // Push Telemetry to Blynk IoT App
            Blynk.virtualWrite(V1, currentSpeedKmh);
            Blynk.virtualWrite(V2, "NORMAL ZONE");
        }
    } else {
        // GPS searching for satellites
        lcd.setCursor(0, 0);
        lcd.print("Searching GPS...");
        lcd.setCursor(0, 1);
        lcd.print("Satellites: 0");
    }
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(MOTOR_ENA, OUTPUT);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Initializing");

    Blynk.begin(auth, ssid, pass);
    
    // Check GPS and Geofencing boundaries every 1 second
    timer.setInterval(1000L, checkGeofence);
}

void loop() {
    Blynk.run();
    timer.run();

    // Direct data streams from the physical GPS module to parse routine
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
}
