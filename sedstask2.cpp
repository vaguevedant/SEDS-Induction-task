#include <Adafruit_LiquidCrystal.h>

const int BUTTON_PIN = 2;
const int LED_PIN = 6;
const int BUZZER_PIN = 7;
const int PING_PIN = 9;       
const int LIGHT_PIN = A0;    

Adafruit_LiquidCrystal lcd(0x27);

const int LIGHT_THRESHOLD = 512; //The reading ranges from 0 to 1023, so half will be a reading of 512
const float DISTANCE_THRESHOLD = 100.0;
const unsigned long WRECK_TIME = 5000;
const unsigned long BLINK_INTERVAL = 500;



// STATES
enum State {
  OPEN_SEA,
  ANCHOR_DROPPED,
  STORM,
  CHARYBDIS,
  WRECKED
};

State currentState = OPEN_SEA;
State previousState = OPEN_SEA;




// When the current danger state started
unsigned long dangerStartTime = 0;

// Used for blinking the LED
unsigned long previousBlinkTime = 0;



// OTHER VARIABLES
bool ledState = false;

// Button state
bool lastButtonState = HIGH;



float getDistance() {

  unsigned long duration;

  
  
  pinMode(PING_PIN, OUTPUT);    //US = ultrasonic sensor

  digitalWrite(PING_PIN, LOW); //First makes US an output device to emit a puslse, then reads it to measure the distance of Charybdis
  delayMicroseconds(2);

  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING_PIN, LOW);

  pinMode(PING_PIN, INPUT);

  duration = pulseIn(PING_PIN, HIGH, 30000);   // if does not receive input for 30ms, function quits

  if (duration == 0) {
    return 101;
  }

  
  float distance = duration / 58.2; // 29.1ms time to cover 1cm, so a round trip takes 58.2ms/cm

  return distance;
}


void displayState() {

  lcd.clear();

  switch (currentState) {

    case OPEN_SEA:
      lcd.setCursor(0, 0);
      lcd.print("OPEN SEA");
      break;


    case ANCHOR_DROPPED:
      lcd.setCursor(0, 0);
      lcd.print("ANCHOR");
      lcd.setCursor(0, 1);
      lcd.print("DROPPED");
      break;


    case STORM:
      lcd.setCursor(0, 0);
      lcd.print("STORM");
      break;


    case CHARYBDIS:
      lcd.setCursor(0, 0);
      lcd.print("CHARYBDIS");
      break;


    case WRECKED:
      lcd.setCursor(0, 0);
      lcd.print("WRECKED");
      break;
  }
}


void setup() {

  // Push button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LED
  pinMode(LED_PIN, OUTPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  lcd.clear();

  // Starting state
  currentState = OPEN_SEA;
  previousState = OPEN_SEA;

  displayState();

  // Make sure outputs start OFF
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}



void loop() {

    int lightValue = analogRead(LIGHT_PIN);

  float distance = getDistance();


  bool buttonState = digitalRead(BUTTON_PIN);
  // INPUT_PULLUP means:
  // HIGH = not pressed
  // LOW  = pressed

  if (lastButtonState == HIGH && buttonState == LOW) {     // anchor is already down. Press button again to raise anchor


    if (currentState == ANCHOR_DROPPED) {

      currentState = OPEN_SEA;

      // Reset danger timer
      dangerStartTime = 0;

      // Make sure outputs are off
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      ledState = false;
    }

       else if (currentState != WRECKED) {

      currentState = ANCHOR_DROPPED;
      dangerStartTime = 0;

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      ledState = false;
    }

    // Small debounce
    delay(50);
  }

  lastButtonState = buttonState;


    switch (currentState) {


        case OPEN_SEA:

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      ledState = false;

      if (lightValue < LIGHT_THRESHOLD) {
 // check for storm
        currentState = STORM;

        // Start the 5-second timer NOW
        dangerStartTime = millis();

        // Start LED blinking timer
        previousBlinkTime = millis();

      }

      else if (distance < DISTANCE_THRESHOLD) {
 //check for charybdis
        currentState = CHARYBDIS;

        // Start the 5-second timer NOW
        dangerStartTime = millis();
      }

      break;

    
    case ANCHOR_DROPPED:

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      dangerStartTime = 0;

      ledState = false;

      break;



    case STORM:

      digitalWrite(BUZZER_PIN, LOW);



      if (millis() - previousBlinkTime >= BLINK_INTERVAL) {

        previousBlinkTime = millis();

        ledState = !ledState;

        digitalWrite(LED_PIN, ledState);
      }

      if (millis() - dangerStartTime >= WRECK_TIME) {

        currentState = WRECKED;

        digitalWrite(LED_PIN, LOW);
        ledState = false;
      }

      else if (lightValue >= LIGHT_THRESHOLD) {

        currentState = OPEN_SEA;

        // Reset timer
        dangerStartTime = 0;

        digitalWrite(LED_PIN, LOW);

        ledState = false;
      }

      break;

    case CHARYBDIS:

      // LED OFF
      digitalWrite(LED_PIN, LOW);

      digitalWrite(BUZZER_PIN, HIGH);

      if (millis() - dangerStartTime >= WRECK_TIME) {

        currentState = WRECKED;

        digitalWrite(BUZZER_PIN, LOW);
      }

      else if (distance >= DISTANCE_THRESHOLD) {

        currentState = OPEN_SEA;

        
        dangerStartTime = 0;

        digitalWrite(BUZZER_PIN, LOW);
      }

      break;

    case WRECKED:

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      break;
  }


    if (currentState != previousState) {

    displayState();

    previousState = currentState;
  }


  
  delay(20);
}