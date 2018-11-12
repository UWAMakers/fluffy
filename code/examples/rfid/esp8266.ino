
/**
 * @author: Benny Michaels <ben@makeuwa.com>
 * 
 * This code is an example of how to interface with the RFID 
 * module and RGB LED used in Project Fluffy. It reads the uuid
 * of a present card, and changes the LED colour when doing so.
 */

#include <Wire.h>
#include "SL018.h"
#include <Adafruit_NeoPixel.h>

#define LED_BUILTIN   2   // Onboard LED for debugging (not set correct in Arduino IDE)
#define CARD_IN       13  // RFID module has a digital pin to say when card is present
#define NEO_PIN       15  // Demonstrate how RGB LED can show state change

// Once read, pass uuid variable to access.uwamakers.com/api/perms/has
// along with the relevant permission for the equipment being protected
// (ask Fraser for more details)
const char* uuid;

// Set up the RGB Neopixel ready to begin
Adafruit_NeoPixel LED = Adafruit_NeoPixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

// Set up the RFID reader for use.
// Note we didn't need to set up the other pins for the RFID module
// We're using the default I2C pins on the ESP module
SL018 rfid;


void setup() {
  rfid.address = 0x50;  // Default address
  rfid.debug = true;    // Can't remember what extra stuff this provides.. may experiment
  
  // Initialise digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialise digital pin CARD_IN as an input with built in pullup activated
  pinMode(CARD_IN, INPUT_PULLUP);
  LED.begin(); // Also initialise the Adafruit NeoPixel library (consider FastLED library)

  // Setup the serial connection to your PC
  Wire.begin();
  Serial.begin(9600);

  // Add dorky prompt for good measure...
  Serial.println("Let's get physical");
  Serial.println("Show me your tag");
}


void loop() {

  // If CARD_IN is high, no card is present
  if (digitalRead(CARD_IN) == HIGH) {

    // So make sure the user knows by changing shiny things
    digitalWrite(LED_BUILTIN, HIGH);
    LED.setPixelColor(0, LED.Color(255,0,0)); 
    LED.show();
    
  } else { // Otherwise we're in business and should do something!

    // Let the user know we are doing something with their card
    digitalWrite(LED_BUILTIN, LOW);
    LED.setPixelColor(0, LED.Color(0,0,255)); 
    LED.show();

    // And start to read the card (yay, something!)
    rfid.seekTag();

    // But it take a fraction of a second to read before we 
    // have the data and can do anything useful
    Serial.print("Reading tag ");
    while(!rfid.available()) {

      // I was using this to see how long it took.
      // Every 5 dots means it's taking a second to read
      // From memory, it didn't take long at all
      delay(200);
      Serial.print(".");
      
      // If the user takes the card away, we should probably just stop
      // The final code needs to handle this a lot more gracefully!
      if (digitalRead(CARD_IN)) {
        Serial.println("");
        Serial.println("User removed tag");
        break;
      };

      // You also need to let the ESP8266 run background tasks 
      // when looping for a while, otherwise the background
      // wifi tasks will crash the ESP8266 microcontroller!
      yield();
    };

    // And we have the uuid! What you do with it now is up to you.
    uuid = rfid.getTagString();
    Serial.println(uuid);

    // The LED should probably change to a third state to tell
    // the user to take their card away... 
  }
}
