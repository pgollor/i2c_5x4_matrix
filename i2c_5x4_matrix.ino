/**
 * @file i2c_5x4_mat.ino
 *
 * @date 2015-05-01
 * @author Pascal Gollor ( http://www.pgollor.de/cms/ )
 * 
 * @mainpage I2C keypad client
 * This is a i2c client wich send the last pressed key from 5x4 keypad (matrix) after request to the bus master.
 *
 * @section license License 
 * <pre>----- english version below -----</pre>
 * Dieses Material steht unter der Creative-Commons-Lizenz Namensnennung 4.0 International.
 * Um eine Kopie dieser Lizenz zu sehen, besuchen Sie http://creativecommons.org/licenses/by/4.0/.
 * <br>cc-by Pascal Gollor <br> <br>
 * This work is licensed under the Creative Commons Attribution 4.0 International License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/.
 * <br> cc-by Pascal Gollor <br> <br>
 *
 * @section other other authors
 * the keypad code comes from from Alexander Brevig <alexanderbrevig@gmail.com>
 */
#include <Keypad.h>
#include <Wire.h>


/**
 * @defgroup key pad defines
 * @{
 */
#define ROWS 5 // five rows
#define COLS 4 // four columns

// button and digit defines
enum BUTTON {BUTTON_F1 = 11, BUTTON_F2, BUTTON_ARROW_UP, BUTTON_ARROW_DOWN, BUTTON_ARROW_LEFT, BUTTON_ARROW_RIGHT, BUTTON_ESC, BUTTON_ENT, BUTTON_RAUTE, BUTTON_STERN};
enum DIGIT {DIGIT_0 = 1, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9};

// define the symbols on the buttons of the keypads 
char hexaKeys[ROWS][COLS] = {
  {BUTTON_F1, BUTTON_F2, BUTTON_RAUTE, BUTTON_STERN},
  {DIGIT_1, DIGIT_2, DIGIT_3, BUTTON_ARROW_UP},
  {DIGIT_4, DIGIT_5, DIGIT_6, BUTTON_ARROW_DOWN},
  {DIGIT_7, DIGIT_8, DIGIT_9, BUTTON_ESC},
  {BUTTON_ARROW_LEFT, DIGIT_0, BUTTON_ARROW_RIGHT, BUTTON_ENT}
};

// connect to the row pinouts of the keypad
byte rowPins[ROWS] = {2, 3, 4, 5, 6};

//connect to the column pinouts of the keypad
byte colPins[COLS] = {A3, A2, A1, A0};
/// @}


/** 
 * @defgroup alive signal to the world
 * @{
 */
#define ALIVE_LED     13
#define ALIVE_PERIOD  500 // in ms
/// @}

/// set timeout after x ms
#define KEY_PRESSED_TIMEOUT 200


/// I2C address. Last three bits are set via Pin 10, 9, 8
#define BASIC_ADDRESS 0b1110000

/// address pins
enum ADDRESS {ADR_2 = 8, ADR_1, ADR_0};


// ------ variables -------
// ---------- \/ ----------

/// global data buffer to send over i2c
volatile uint8_t g_data[2] = {100, 0};

/// last key presse
unsigned long last_key_pressed = 0;

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


// ------ functions -------
// ---------- \/ ----------


/** @brief i2c request event
 *
 * is called if data from bus master requested
 */
void i2c_requestEvent()
{
  Wire.write((const uint8_t*)g_data, 2);

  g_data[0] = 100;
  g_data[1] = 0;
}

/** @brief i2c receive event
 *
 * Is called if data is received from bus master. It's only for debugging.
 */
void i2c_receiveEvent(int data_length)
{
  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}


/// arduino setup function
void setup()
{
  byte address = BASIC_ADDRESS;
  
  // digital pin management
  pinMode(ADR_0, INPUT);
  pinMode(ADR_1, INPUT);
  pinMode(ADR_2, INPUT);
  pinMode(ALIVE_LED, OUTPUT);
  
  // read sub adress bits
  address |= digitalRead(ADR_0) | (digitalRead(ADR_1) << 1) | (digitalRead(ADR_2) << 2);
  
  // set i2c speed
  Wire.setClock(100000L);
  
  // join i2c bus with address
  Wire.begin(address);
  
  // register events
  Wire.onRequest(i2c_requestEvent);
  Wire.onReceive(i2c_receiveEvent);
  
  Serial.begin(57600);
}


// arduino main loop functino
void loop()
{
  static unsigned long last_state_change = 0;
  char customKey = customKeypad.getKey();

  if (millis() - last_state_change >= ALIVE_PERIOD)
  {
    if (digitalRead(ALIVE_LED) == HIGH)
    {
      digitalWrite(ALIVE_LED, LOW);
    }
    else
    {
      digitalWrite(ALIVE_LED, HIGH);
    }

    last_state_change = millis();
  }

  if (g_data[0] == 100 && millis() - last_key_pressed >= KEY_PRESSED_TIMEOUT)
  {
    g_data[0] = 1;
    g_data[1] = 0;
  }

  if (customKey)
  {
    g_data[0] = 100;
    g_data[1] = customKey;
    last_key_pressed = millis();

    Serial.println(customKey, DEC);
  }
}

