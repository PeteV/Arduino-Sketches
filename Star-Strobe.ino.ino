/**
 * Star Trek Model LED Lights
 * 
 * 
 * 
 * Features:
 * Landing light controlled via BLE
 * 2 fading anti-collision beacons
 * Double flash strobe
 * 
 * @author Pete Verschueren
 * @version 0.1
 * @date 2016-08-15
 * 
 */

// Pins Init
#define LL_IRQ_NUMBER 0 // Interrupt number to use (0 = pin 2 on most boards)
#define LL_PIN_SERVO 2 // Servo input pin number - this needs to match whatever interrupt is used
#define LL_PIN_LIGHT 3 // Landing light output pin number
#define LL_SERVO_THRESHOLD 1500 // Servo signal threshold to turn on/off landing light (pulse width in microseconds, 1000 to 2000)
#define LL_SERVO_DEAD_ZONE 100 // Servo signal dead-zone size, eliminates flicker
#define LL_SERVO_REVERSED true   // Whether or not the servo channel is reversed

// Strobe Init
#define STB_PIN_LIGHT 4 // Pin number for strobe light output
#define STB_BLINK_INTERVAL 2000000 // Blink interval for strobe light in microseconds

// Anti-collision beacon settings
#define ACB1_PIN_LIGHT 9 // Pin number for anti-collision beacon 1
#define ACB2_PIN_LIGHT 10 // Pin number for anti-collision beacon 2
#define ACB_FADE_MIN 10 // Minimum fade level for beacon (0-255)
#define ACB_FADE_MAX 50 // Maximum fade level for beacon (0-255)
#define ACB_FADE_INTERVAL 30000 // Fade step interval, in microseconds (lower numbers = faster fade)


// 
volatile unsigned long servoPulseStartTime;
volatile int servoPulseWidth = 0;
boolean curLandingLight = false;

unsigned long lastFadeTime = 0;
unsigned long lastStrobeTime = 0;
int currentFade = ACB_FADE_MIN;
int fadeDirection = 1;

void setup()
{
  // Set up interrupt handler
 // attachInterrupt(LL_IRQ_NUMBER, measureServoSignal, CHANGE);

  // Declare output pins
  pinMode(LL_PIN_LIGHT, OUTPUT);
  pinMode(STB_PIN_LIGHT, OUTPUT);
  pinMode(ACB1_PIN_LIGHT, OUTPUT);
  pinMode(ACB2_PIN_LIGHT, OUTPUT);
}


void loop()
{
  unsigned long currentTime = micros();
  
    setLandingLight(true);
  
  // Check if it's time to fade the anti-collision lights
  if ((currentTime - lastFadeTime) > ACB_FADE_INTERVAL) {
    doFade();
    lastFadeTime = currentTime;
  }

  // Check if it's time to blink the strobes
  if ((currentTime - lastStrobeTime) > STB_BLINK_INTERVAL) {
    doStrobe();
    lastStrobeTime = currentTime; 
  }
}



// Turn on or off landing light
void setLandingLight(boolean state)
{
  float i;
  if (state && !curLandingLight) {
    digitalWrite(LL_PIN_LIGHT, HIGH);
  } else if (!state && curLandingLight) {
    digitalWrite(LL_PIN_LIGHT, LOW);
  }
  curLandingLight = state;
}

// Fade anti-collision LEDs
void doFade()
{
  currentFade += fadeDirection;
  if (currentFade == ACB_FADE_MAX || currentFade == ACB_FADE_MIN) {
    // If we hit the fade limit, flash the high beacon, and flip the fade direction
    if (fadeDirection == 1) {
      analogWrite(ACB1_PIN_LIGHT, 255);

    } else {
      analogWrite(ACB2_PIN_LIGHT, 255);
    }
    delay(50); 
    fadeDirection *= -1; 
  }

  analogWrite(ACB1_PIN_LIGHT, currentFade);
  analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX - currentFade + ACB_FADE_MIN);
}

// Strobe double-blink
void doStrobe()
{
  digitalWrite(STB_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, LOW);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, LOW);
}



