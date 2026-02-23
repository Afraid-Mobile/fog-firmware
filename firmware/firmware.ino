/*
 * ESP32-C3 Co-prossesor firmware
 */

#include <Arduino.h>
#include "esp_sleep.h"

/*
 * Start of config
 */

// Board identity
#define BOARD_MAKE     "GNUfault"
#define BOARD_CODENAME "PHONE"
#define BOARD_REV      "1.0"

// Firmware identity
#define FIRMWARE_VERSION

// Battery level checking variables
const int battery_level_pin = 2; // Battery is connected to a voltage divider and then GPIO2
const float R1 = 100000.0; // R1 in voltage divider
const float R2 = 100000.0; // R2 in voltage divider

// Power button
const int power_button_pin = 10;

// Power plane enable
const int power_plane_enable = 8;

/*
 * End of config
 */

/*
 * Returns int of battery voltage
 */
float get_battery_voltage(void) {
  int raw = analogRead(battery_level_pin); // Read voltage from battery
  float battery_voltage_adc = (raw / 4095.0) * 3.3; // Convert the ADC reading into a voltage
  float battery_voltage = battery_voltage_adc * ((R1 + R2) / R2); // Take the voltage divider into acount

  return battery_voltage; // Return the battery voltage
}

/*
 * Main function for when it cold boots
 */
void cold_boot_setup() {
  // Set wake up pin (power button)
  Serial.println("Setting power button as wake up pin...");
  esp_deep_sleep_enable_gpio_wakeup(1 << power_button_pin, ESP_GPIO_WAKEUP_GPIO_HIGH);
  
  // Go into deep sleep until power button is pressed
  Serial.println("Going into deep sleep until power button pressed...");
  esp_deep_sleep_start();
}

/*
 * Main function for when it wakes from deep sleep
 */
void deep_sleep_setup() {
  Serial.println("Enabling power plane...");
  pinMode(power_plane_enable, OUTPUT);
}

/*
 * Returns ESP_SLEEP_WAKEUP_EXT0 if woke up from deep sleep, somthing else if not.
 */
int get_bootup_reason() {
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause(); // Check wakeup cause
  return cause; // Return it
}

void setup() {
  pinMode(power_plane_enable, OUTPUT);

  Serial.begin(115200); // Start UART at 115200 buad
  Serial.printf("%s %s Rev %s", BOARD_MAKE, BOARD_CODENAME, BOARD_REV); // Print information over serial

  analogReadResolution(12); // ADC is 12-bit

  Serial.println("Checking if woke up from deep sleep...");
  
  int cause = get_bootup_reason(); // Get bootup reason

  // Check if woke up from deep sleep
  if (cause == ESP_SLEEP_WAKEUP_EXT0) {
    // Woke up from deep sleep
    Serial.println("Woke up from deep sleep.");
    deep_sleep_setup();
  } else {
    // Cold boot
    Serial.println("Woke up from cold boot.");
    cold_boot_setup();
  }
}

// UNUSED
void loop() {}
