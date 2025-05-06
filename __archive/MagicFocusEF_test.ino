#include <BlueMagic32.h>
//#include "WiFi.h"

#define REC_BUTTON_PIN 15
#define PLAY_BUTTON_PIN 25
#define REC_LED 32

// for Hardware Seril Communication to LIDAR
#define RX2_PIN 16
#define TX2_PIN 17

int REC_buttonState;           // the current reading from the input pin
int REC_lastButtonState = LOW; // the previous reading from the input pin
int PLAY_buttonState;           // the current reading from the input pin
int PLAY_lastButtonState = LOW; // the previous reading from the input pin

unsigned long REC_lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long PLAY_lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

float focus_value;

uint16_t focusVal_current; 
uint16_t focusVal_target; 
uint16_t focusVal;

int counter_old;

void setup()
{

    /* Debugging serial connection */
    Serial.begin(115200);   // this line has to come before BMCConnection when connecting to the camera for the first time. Use serial input to set the passcode.

    /*  hardware serial setup for LIDAR sensor */
    //          baud rate  protocol
    //              v         v
    Serial2.begin(921600, SERIAL_8N1, RX2_PIN, TX2_PIN);  // Serial for LIDAR reading
    HardwareSerial Serial1( 2 );
    // Serial connected to LIDAR sensor
    Serial1.begin( 115200 );
    delay(50);
    
    /* BlueMagic32 camera connection */
    BMDConnection.begin("MagicFocusEF");
    BMDControl = BMDConnection.connect();

    delay(100);

    focusVal_current = 100;
    focusVal_target = 0;
    
    pinMode(REC_BUTTON_PIN , INPUT);
    pinMode(PLAY_BUTTON_PIN , INPUT);
    pinMode(REC_LED,OUTPUT);
    digitalWrite(REC_LED,LOW);

    delay(20);

    /* Calibrating lens by going all the way to infinity */
    for (int i = 0; i <= 20; i++) {
          uint16_t focusVal = 6; 
          setFocus(focusVal);     
          delay(10);
        }

     for (int i = 0; i <= 5; i++) {
          uint16_t focusVal = -2;
          setFocus(focusVal); 
          delay(10);
        }
}

void loop()
{
    //Serial.println("start");
    int REC_reading = digitalRead(REC_BUTTON_PIN);
    int PLAY_reading = digitalRead(PLAY_BUTTON_PIN);

     /* 1. Read LIDAR . */
     unsigned int distance = readLIDAR( 2000 );  // distance in cm up to 12m 65535
     focusVal_target = map(distance, 0, 900, 0, 100);
     Serial.print("target: ");
     Serial.print(focusVal_target);
     Serial.print(", current: ");
     Serial.print(focusVal_current);

     uint16_t range = 3;

     if (focusVal_current + range + 1 < focusVal_target){
          focusVal = range;
          focusVal_current = focusVal_current + focusVal; 
          Serial.println(" +3");    
          setFocus(focusVal);
     }

     else if (focusVal_current < focusVal_target && focusVal_current + range + 1 > focusVal_target){
          focusVal = 1;
          focusVal_current = focusVal_current + focusVal; 
          Serial.println(" +1");
          setFocus(focusVal);
     }

     else if (focusVal_current > range + 1 + focusVal_target){
          focusVal = -range;
          focusVal_current = focusVal_current + focusVal; 
          Serial.println(" -3");    
          setFocus(focusVal);
     }

     else if (focusVal_current >  focusVal_target && focusVal_current < range + 1 + focusVal_target){
          focusVal = -1;
          focusVal_current = focusVal_current + focusVal;
          Serial.println(" -1");
          setFocus(focusVal);
     }

      else {
      Serial.println(" +0");  
          delay(10);
     }

//    
//    // If the switch changed, due to noise or pressing:
//    if (REC_reading != REC_lastButtonState)
//    {
//        REC_lastDebounceTime = millis();
//    }
//
//        // If the switch changed, due to noise or pressing:
//    if (PLAY_reading != PLAY_lastButtonState)
//    {
//        PLAY_lastDebounceTime = millis();
//    }
//
//    // REC
//    if ((millis() - REC_lastDebounceTime) > debounceDelay)
//    {
//        if (REC_reading != REC_buttonState)
//        {
//            REC_buttonState = REC_reading;
//
//            // only toggle the LED if the new button state is HIGH
//            if (REC_buttonState == HIGH)
//            {
//                if (BMDConnection.available())
//                {   
//                    BMDControl->toggleRecording();               
//                    
//                }
//            }
//        }
//    }
//
//    
//    /* Turn the REC LED on, if the camera is recodring */
//    if (BMDControl->isRecording())
//        {
//            digitalWrite(REC_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
//        }
//        else
//        {
//            digitalWrite(REC_LED, LOW);    // turn the LED off by making the voltage LOW
//        }
//
//        // PLAY
//    if ((millis() - PLAY_lastDebounceTime) > debounceDelay)
//    {
//        if (PLAY_reading != PLAY_buttonState)
//        {
//            PLAY_buttonState = PLAY_reading;
//
//            // only toggle the LED if the new button state is HIGH
//            if (PLAY_buttonState == HIGH)
//            {
//                if (BMDConnection.available())
//                {   
//                    uint8_t data[12] = {255, 4, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}; // 
//                    BMDControl->custom(data, 12);
//                    //BMDControl->iso(800);  
//                    //Serial.write("Focus Button");
//
//                    //uint8_t data[12] = {255, 5, 0, 0, 10, 1, 1, 0, 0, 0, 0, 0};                                         {255, 5, 0, 0, 10, 1, 1, 0, 0, 0, 0, 0};
//                }
//            }
//        }
//    }

    REC_lastButtonState = REC_reading;
    PLAY_lastButtonState = PLAY_reading;
}


/*
 * This function reads the Serial1 until a valid packet is found or timeout passed.
 * Param timeout: Timeout in milliseconds.
 * Returns distance in cm or 0 if timeout happened.
 */
unsigned int readLIDAR( long timeout ) {

  unsigned char readBuffer[ 9 ];

  long t0 = millis();

  while ( Serial2.available() < 9 ) {

    if ( millis() - t0 > timeout ) {
      // Timeout
      return 0;
    }

    delay( 10 );
  }

  for ( int i = 0; i < 9; i++ ) {
    readBuffer[ i ] = Serial2.read();
  }

  while ( ! detectFrame( readBuffer ) ) {

    if ( millis() - t0 > timeout ) {
      // Timeout
      return 0;
    }

    while ( Serial2.available() == 0 ) {
      delay( 10 );
    }

    for ( int i = 0; i < 8; i++ ) {
      readBuffer[ i ] = readBuffer[ i + 1 ];
    }

    readBuffer[ 8 ] = Serial2.read();

  }

  // Distance is in bytes 2 and 3 of the 9 byte frame.
  unsigned int distance = ( (unsigned int)( readBuffer[ 2 ] ) ) |
                          ( ( (unsigned int)( readBuffer[ 3 ] ) ) << 8 );

  return distance;

}

bool detectFrame( unsigned char *readBuffer ) {

  return  readBuffer[ 0 ] == 0x59 &&
          readBuffer[ 1 ] == 0x59 &&
          (unsigned char)(
            0x59 +
            0x59 +
            readBuffer[ 2 ] + 
            readBuffer[ 3 ] + 
            readBuffer[ 4 ] +
            readBuffer[ 5 ] + 
            readBuffer[ 6 ] + 
            readBuffer[ 7 ]
          ) == readBuffer[ 8 ];
}

void setFocus(uint16_t focusVal) {      
  uint8_t focusL = focusVal & 0xff;
  uint8_t focusH = (focusVal >> 8);
  uint8_t data[12] = {255, 6, 0, 0, 0, 0, 128, 1, focusL, focusH, 0, 0};
  BMDControl->custom(data, 12); 
  delay(15);
}
