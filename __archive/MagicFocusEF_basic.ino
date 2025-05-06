#include <BlueMagic32.h>
//#include "WiFi.h"

#define ENC_A 27
#define ENC_B 26

#include <rom/gpio.h>

#define REC_BUTTON_PIN 15
#define PLAY_BUTTON_PIN 25
#define REC_LED 32

int REC_buttonState;           // the current reading from the input pin
int REC_lastButtonState = LOW; // the previous reading from the input pin
int PLAY_buttonState;           // the current reading from the input pin
int PLAY_lastButtonState = LOW; // the previous reading from the input pin

unsigned long REC_lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long PLAY_lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

float focus_value;

int counter_old;

void setup()
{
    Serial.begin(115200);   // this line has to come before BMCConnection when connecting to the camera for the first time. Use serial input to set the passcode.
    BMDConnection.begin("MagicFocusEF");
    BMDControl = BMDConnection.connect();

    pinMode(REC_BUTTON_PIN , INPUT);
    pinMode(PLAY_BUTTON_PIN , INPUT);
    pinMode(REC_LED,OUTPUT);
    digitalWrite(REC_LED,LOW);

    pinMode(ENC_A, INPUT_PULLUP);
    digitalWrite(ENC_A, HIGH);
    pinMode(ENC_B, INPUT_PULLUP);
    digitalWrite(ENC_B, HIGH);

    //Serial.begin(115200);
    delay(20);

//    // Power conservation Wifi OFF
//    WiFi.disconnect();
//    WiFi.mode( WIFI_OFF );
//    //WiFi.forceSleepBegin();
//    delay( 1 );


}

void loop()
{
    //Serial.println("start");
    int REC_reading = digitalRead(REC_BUTTON_PIN);
    int PLAY_reading = digitalRead(PLAY_BUTTON_PIN);

    /**/
    static uint8_t counter = 0;      //this variable will be changed by encoder input
    int8_t tmpdata;
    tmpdata = read_encoder();
     
      if( tmpdata ) {
        counter += tmpdata;
        //Serial.println("Counter changed");
        //Serial.println(counter, DEC);
      }

        if(counter_old > counter+1){
          //Serial.println("CW");
          uint16_t focusVal = -3;          
          uint8_t focusL = focusVal & 0xff;
          uint8_t focusH = (focusVal >> 8);
          uint8_t data[12] = {255, 6, 0, 0, 0, 0, 128, 1, focusL, focusH, 0, 0};
          BMDControl->custom(data, 12); 
          counter_old = counter;
        }
        
        else if (counter_old < counter-1)
        {
          //Serial.println("CCW");
          uint16_t focusVal = 3;          
          uint8_t focusL = focusVal & 0xff;
          uint8_t focusH = (focusVal >> 8);
          // On all cameras, 'add focusVal' to the current focus.
          uint8_t data[12] = {255, 6, 0, 0, 0, 0, 128, 1, focusL, focusH, 0, 0};
          BMDControl->custom(data, 12);
          counter_old = counter;
          }

          delay(100);
     
    
    // If the switch changed, due to noise or pressing:
    if (REC_reading != REC_lastButtonState)
    {
        REC_lastDebounceTime = millis();
    }

        // If the switch changed, due to noise or pressing:
    if (PLAY_reading != PLAY_lastButtonState)
    {
        PLAY_lastDebounceTime = millis();
    }

    // REC
    if ((millis() - REC_lastDebounceTime) > debounceDelay)
    {
        if (REC_reading != REC_buttonState)
        {
            REC_buttonState = REC_reading;

            // only toggle the LED if the new button state is HIGH
            if (REC_buttonState == HIGH)
            {
                if (BMDConnection.available())
                {   
                    BMDControl->toggleRecording();               
                    
                }
            }
        }
    }

    
    /* Turn the REC LED on, if the camera is recodring */
    if (BMDControl->isRecording())
        {
            digitalWrite(REC_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
        }
        else
        {
            digitalWrite(REC_LED, LOW);    // turn the LED off by making the voltage LOW
        }

        // PLAY
    if ((millis() - PLAY_lastDebounceTime) > debounceDelay)
    {
        if (PLAY_reading != PLAY_buttonState)
        {
            PLAY_buttonState = PLAY_reading;

            // only toggle the LED if the new button state is HIGH
            if (PLAY_buttonState == HIGH)
            {
                if (BMDConnection.available())
                {   
                    uint8_t data[12] = {255, 4, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}; // 
                    BMDControl->custom(data, 12);
                    //BMDControl->iso(800);  
                    //Serial.write("Focus Button");

                    //uint8_t data[12] = {255, 5, 0, 0, 10, 1, 1, 0, 0, 0, 0, 0};                                         {255, 5, 0, 0, 10, 1, 1, 0, 0, 0, 0, 0};
                }
            }
        }
    }

    REC_lastButtonState = REC_reading;
    PLAY_lastButtonState = PLAY_reading;
}

////alternative set Focus function
//int set_focus(float focus_value) {
//    float setFocus;
//    float currentFocus = BMDControl->getFocus();
//    float focusDelta = 0.0;
//
//    if( currentFocus > setFocus ){
//      focusDelta = -( currentFocus - focus_value );
//    } else {
//      focusDelta = ( focus_value - currentFocus );
//    }
//
//    // Jumble of math below for focusVal is just doing what BlueMagicCameraController::mapFloat() does
//    uint16_t focusVal = (uint32_t)( ( focusDelta - 0.0 ) * (2047.0 - 0.0) / (1.0 - 0.0) + 0.0 );             
//    uint8_t focusL = focusVal & 0xff;
//    uint8_t focusH = (focusVal >> 8);
//
//    // On all cameras, 'add focusVal' to the current focus.
//    uint8_t data[12] = {255, 6, 0, 0, 0, 0, 128, 1, focusL, focusH, 0, 0};
//    
//    BMDControl->custom(data, 12);
//}

float fmap (float sensorValue, float sensorMin, float sensorMax, float outMin, float outMax)
    {
  return (sensorValue - sensorMin) * (outMax - outMin) / (sensorMax - sensorMin) + outMin;
     }


/* returns change in encoder state (-1,0,1) */
int8_t read_encoder()
{
  
  static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t old_AB = 0;
  static uint32_t curval = 0;
  static uint32_t curtmpA = 0;
  static uint32_t curtmpB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  //bit shift old_AB two positions to the left and store.

  curval = gpio_input_get();  // returns gpio pin status of pins - SEE DEFINE 
  //note to self: these curval bits are probably backwards...
  curtmpA = (curval & 1<< ENC_A ) >> ENC_A;
  curtmpB = (curval & 1<< ENC_B ) >> (ENC_B - 1);
  old_AB |= ( ( curtmpA | curtmpB ) & 0x03 ); 
  //add current state and hopefully truncate to 8bit 
 
  return ( enc_states[( old_AB & 0x0f )]);
  // return the array item that matches the known possible encoder states

  // Thanks to kolban in the esp32 channel, who has a great book on everything iot,
  // for his initial help at my panic on the esp32 gpio access. long live IRC :)
}
