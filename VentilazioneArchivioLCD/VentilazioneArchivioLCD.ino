
#include <LiquidCrystal.h>  // Required by LCDKeypad
#include <LCDKeypad.h>
#include <EEPROM.h>
#include <dht.h>

#define TIME_OUT 5

#define DHToutsidePin 3 // 4th digital pin = 3
#define DHTinsidePin  2 // 3rd digital pin = 2
#define outsideVentRelayPin 13 // 14th digital pin = 13
#define insideVentRelayPin  12 // 13th digital pin = 12

#define DHTTYPE DHT11
//#define DHTTYPE DHT22

#define LCDbacklight() pinMode( 10, INPUT) // turn on backlight
#define LCDnoBacklight() pinMode( 10, OUTPUT) // turn off backlight

#define pinBuzzer 11

LCDKeypad lcd;
dht DHT;

enum states
{
    SHOW_HUMIDITY,
    SHOW_HUMIDITY_AND_TEMP1,
    SHOW_HUMIDITY_AND_TEMP2,
    SHOW_HUMIDITY_THRESHOLD,
    SET_HUMIDITY_THRESHOLD,
    SHOW_ERROR_MESSAGE
};

states state;
int8_t button;
int humidityThreshold = 0;
int tmpHumidityTreshold;
unsigned long timeRef;

float HIn  =  0;
float TIn  =  0;
float HOut =  0;
float TOut =  0;
boolean OverThreshold = false;
boolean Vent = false;

// http://omerk.github.io/lcdchargen/
byte customCharArrowUp[8] = {
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b10001
};
byte customCharArrowDown[8] = {
	0b10001,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100,
	0b00000
};

/*
           _               
          | |              
  ___  ___| |_ _   _ _ __  
 / __|/ _ \ __| | | | '_ \ 
 \__ \  __/ |_| |_| | |_) |
 |___/\___|\__|\__,_| .__/ 
                    | |    
                    |_|    
*/
void setup()
{
    digitalWrite(10,LOW);
    //LCDnoBacklight(); // turn off the backlight
    pinMode( DHToutsidePin, INPUT);
    pinMode( DHTinsidePin,  INPUT);
    pinMode( outsideVentRelayPin, OUTPUT);
    pinMode( insideVentRelayPin,  OUTPUT);
    pinMode( pinBuzzer, OUTPUT);
    beep();    beep();    beep();    
    lcd.createChar(0, customCharArrowUp);
    lcd.createChar(1, customCharArrowDown);
    
    
    // recover humidityThreshold value from eeprom after poweroff
    humidityThreshold = EEPROM.read(0);
    if ( ( humidityThreshold > 95 ) || ( humidityThreshold < 5 ) ) {
      // if no proper value (ie. never set, first run) set default
      humidityThreshold = 60;
      EEPROM.write(0, humidityThreshold);
    }
    checkHumidity();
    
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    //         1234567890123456
    lcd.print(" ECO DEHUMIFIER ");
    lcd.setCursor(0,1);
    lcd.print("     1.0.1      ");
    delay(1500);
    // read eeprom value for humidity treshold
    if (TIn >= 0)
      state = SHOW_HUMIDITY;
    else
      state = SHOW_ERROR_MESSAGE;

}

/*
  _                   
 | |                  
 | | ___   ___  _ __  
 | |/ _ \ / _ \| '_ \ 
 | | (_) | (_) | |_) |
 |_|\___/ \___/| .__/ 
               | |    
               |_|    
*/
void loop()
{
    timeRef = millis();

    // Uses the current state to decide what to process
    switch (state)
    {
        case SHOW_HUMIDITY:
            showHumidity();
            break;
        case SHOW_HUMIDITY_AND_TEMP1:
            showHumidityTemp1();
            break;
        case SHOW_HUMIDITY_AND_TEMP2:
            showHumidityTemp2();
            break;
        case SHOW_HUMIDITY_THRESHOLD:
            showHumidityThreshold();
            break;
        case SET_HUMIDITY_THRESHOLD:
            setHumidityThreshold();
            break;
        case SHOW_ERROR_MESSAGE:
            showErrorMessage();
    }

    while ( (unsigned long)(millis() - timeRef) < 2970 )
    {
        if ( (button = lcd.button()) != KEYPAD_NONE )
        {
            while ( lcd.button() != KEYPAD_NONE ) ;
            transition(button);
            beep();
            break;
        }
    }
    checkHumidity();
    if ( Vent ) {
      digitalWrite( outsideVentRelayPin, HIGH );
      digitalWrite( insideVentRelayPin, HIGH );
    } else {
      digitalWrite( outsideVentRelayPin, LOW );
      digitalWrite( insideVentRelayPin, LOW );
    }
}

/*
  _                       _ _   _             
 | |                     (_) | (_)            
 | |_ _ __ __ _ _ __  ___ _| |_ _  ___  _ __  
 | __| '__/ _` | '_ \/ __| | __| |/ _ \| '_ \ 
 | |_| | | (_| | | | \__ \ | |_| | (_) | | | |
  \__|_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_|
*/
void transition(uint8_t trigger)
{
    switch (state)
    {
        case SHOW_HUMIDITY:
            if ( trigger == KEYPAD_SELECT )     state = SET_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_UP )    state = SHOW_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_DOWN )  state = SHOW_HUMIDITY_AND_TEMP1;
            break;
        case SHOW_HUMIDITY_AND_TEMP1:
            if ( trigger == KEYPAD_SELECT )     state = SET_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_UP )    state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_DOWN )  state = SHOW_HUMIDITY_AND_TEMP2;
            else if ( trigger == TIME_OUT )     state = SHOW_HUMIDITY;
            break;
        case SHOW_HUMIDITY_AND_TEMP2:
            if ( trigger == KEYPAD_SELECT )     state = SET_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_UP )    state = SHOW_HUMIDITY_AND_TEMP1;
            else if ( trigger == KEYPAD_DOWN )  state = SHOW_HUMIDITY_THRESHOLD;
            else if ( trigger == TIME_OUT )     state = SHOW_HUMIDITY;
            break;
        case SHOW_HUMIDITY_THRESHOLD:
            if ( trigger == KEYPAD_SELECT )     state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_UP )    state = SHOW_HUMIDITY_AND_TEMP2;
            else if ( trigger == KEYPAD_DOWN )  state = SHOW_HUMIDITY;
            else if ( trigger == TIME_OUT )     state = SHOW_HUMIDITY;
            break;
        case SET_HUMIDITY_THRESHOLD:
            if ( trigger == KEYPAD_SELECT ) state = SHOW_HUMIDITY;
            else if ( trigger == TIME_OUT ) state = SHOW_HUMIDITY;
            break;
//        case SHOW_ERROR_MESSAGE:
//            if ( trigger == KEYPAD_SELECT ) state = SHOW_HUMIDITY;
//            else if ( trigger == TIME_OUT ) state = SHOW_HUMIDITY;
//            break;
    }
}

/*
   _____      _   _    _ _______ 
  / ____|    | | | |  | |__   __|
 | |  __  ___| |_| |__| |  | |   
 | | |_ |/ _ \ __|  __  |  | |   
 | |__| |  __/ |_| |  | |  | |   
  \_____|\___|\__|_|  |_|  |_|   
*/
dht GetHT(int onPIN)
{
  // if defined DHT11
  int chk = DHT.read11(onPIN);
  // if defined DHT22
  //int chk = DHT.read22(onPIN);
 
  switch (chk)
  {
    case DHTLIB_OK:  
		//Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//Serial.print("Checksum error,\t"); 
                DHT.humidity = -1;
                DHT.temperature = -1;
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//Serial.print("Time out error,\t");
                DHT.humidity = -2;
                DHT.temperature = -2;
		break;
    default: 
		//Serial.print("Unknown error,\t"); 
                DHT.humidity = -3;
                DHT.temperature = -3;
		break;
  }
  return DHT;
}
/*
       _               _      _    _                 _     _ _ _         
      | |             | |    | |  | |               (_)   | (_) |        
   ___| |__   ___  ___| | __ | |__| |_   _ _ __ ___  _  __| |_| |_ _   _ 
  / __| '_ \ / _ \/ __| |/ / |  __  | | | | '_ ` _ \| |/ _` | | __| | | |
 | (__| | | |  __/ (__|   <  | |  | | |_| | | | | | | | (_| | | |_| |_| |
  \___|_| |_|\___|\___|_|\_\ |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |
                                                                    __/ |
                                                                   |___/ 
*/
void checkHumidity()
{
    // find out a way not to read values to soon than 2 sec apart
    dht dhtTemp = GetHT(DHTinsidePin);
    HIn = dhtTemp.humidity;
    TIn = dhtTemp.temperature;
    (TIn < -10) ? TIn = 0 : TIn = TIn;
    
    dhtTemp = GetHT(DHToutsidePin);
    HOut = dhtTemp.humidity;
    TOut = dhtTemp.temperature;
    (TOut < -10) ? TOut = 0 : TOut = TOut;

    // if (( Hi > Htreshold) && ( Hi > He )) { activate_ventilation; }
    if ( HIn > humidityThreshold ) {
      OverThreshold = true;
      if ( HIn > HOut ) {
       Vent = true;
      } else {
         Vent = false;
      }
    } else {
       OverThreshold = false;
       Vent = false;
    }
}

/*
      _                     _    _                 _     _ _ _         
     | |                   | |  | |               (_)   | (_) |        
  ___| |__   _____      __ | |__| |_   _ _ __ ___  _  __| |_| |_ _   _ 
 / __| '_ \ / _ \ \ /\ / / |  __  | | | | '_ ` _ \| |/ _` | | __| | | |
 \__ \ | | | (_) \ V  V /  | |  | | |_| | | | | | | | (_| | | |_| |_| |
 |___/_| |_|\___/ \_/\_/   |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |
                                                                  __/ |
                                                                 |___/ 
*/
void showHumidity()
{
    lcd.clear();
    //          1234567890123456
    lcd.print( "ECO DEHUMIFIER  " );
    lcd.setCursor(0,1);
    lcd.print( "Hi ");
    lcd.print( (int)HIn );
    lcd.print( "% He " );
    lcd.print( (int)HOut );
    lcd.print( "%" );

    lcd.setCursor(15,0);
    lcd.write((uint8_t)0);
    lcd.setCursor(15,1);
    lcd.write((uint8_t)1);

    lcd.setCursor(14,1);
    if ( Vent ) lcd.print( "*" ); else lcd.print( "." );
}

/*
      _                     _    _                 _     _ _ _           _______                     __ 
     | |                   | |  | |               (_)   | (_) |         |__   __|                   /_ |
  ___| |__   _____      __ | |__| |_   _ _ __ ___  _  __| |_| |_ _   _     | | ___ _ __ ___  _ __    | |
 / __| '_ \ / _ \ \ /\ / / |  __  | | | | '_ ` _ \| |/ _` | | __| | | |    | |/ _ \ '_ ` _ \| '_ \   | |
 \__ \ | | | (_) \ V  V /  | |  | | |_| | | | | | | | (_| | | |_| |_| |    | |  __/ | | | | | |_) |  | |
 |___/_| |_|\___/ \_/\_/   |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |    |_|\___|_| |_| |_| .__/   |_|
                                                                  __/ |                     | |         
                                                                 |___/                      |_|         
*/
void showHumidityTemp1()
{
    lcd.clear();
    //         1234567890123456
    lcd.print( "EXTERNAL" );
    lcd.setCursor(0,1);
    lcd.print( "He ");
    lcd.print( (int)HOut );
    lcd.print( "% Te " );
    lcd.print( (int)TOut );
    lcd.print( "C" );

    lcd.setCursor(15,0);
    lcd.write((uint8_t)0);
    lcd.setCursor(15,1);
    lcd.write((uint8_t)1);
}

/*
      _                     _    _                 _     _ _ _           _______                     ___  
     | |                   | |  | |               (_)   | (_) |         |__   __|                   |__ \ 
  ___| |__   _____      __ | |__| |_   _ _ __ ___  _  __| |_| |_ _   _     | | ___ _ __ ___  _ __      ) |
 / __| '_ \ / _ \ \ /\ / / |  __  | | | | '_ ` _ \| |/ _` | | __| | | |    | |/ _ \ '_ ` _ \| '_ \    / / 
 \__ \ | | | (_) \ V  V /  | |  | | |_| | | | | | | | (_| | | |_| |_| |    | |  __/ | | | | | |_) |  / /_ 
 |___/_| |_|\___/ \_/\_/   |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |    |_|\___|_| |_| |_| .__/  |____|
                                                                  __/ |                     | |           
                                                                 |___/                      |_|           
*/
void showHumidityTemp2()
{
    lcd.clear();
    //         1234567890123456
    lcd.print( "INTERNAL" );
    lcd.setCursor(0,1);
    lcd.print( "Hi ");
    lcd.print( (int)HIn );
    lcd.print( "% Ti " );
    lcd.print( (int)TIn );
    lcd.print( "C" );

    lcd.setCursor(15,0);
    lcd.write((uint8_t)0);
    lcd.setCursor(15,1);
    lcd.write((uint8_t)1);
}

/*
      _                     _    _                 _     _ _ _           _______            _           _     _ 
     | |                   | |  | |               (_)   | (_) |         |__   __|          | |         | |   | |
  ___| |__   _____      __ | |__| |_   _ _ __ ___  _  __| |_| |_ _   _     | |_ __ ___  ___| |__   ___ | | __| |
 / __| '_ \ / _ \ \ /\ / / |  __  | | | | '_ ` _ \| |/ _` | | __| | | |    | | '__/ _ \/ __| '_ \ / _ \| |/ _` |
 \__ \ | | | (_) \ V  V /  | |  | | |_| | | | | | | | (_| | | |_| |_| |    | | | |  __/\__ \ | | | (_) | | (_| |
 |___/_| |_|\___/ \_/\_/   |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |    |_|_|  \___||___/_| |_|\___/|_|\__,_|
                                                                  __/ |                                         
                                                                 |___/                                          
*/
void showHumidityThreshold()
{
    lcd.clear();
    //         1234567890123456
    lcd.print("HUMIDITY THRES..");
    lcd.setCursor(0,1);
    lcd.print( "SET IT TO: " );
    lcd.print( humidityThreshold );
    lcd.print( '%' );

    lcd.setCursor(15,0);
    lcd.write((uint8_t)0);
    lcd.setCursor(15,1);
    lcd.write((uint8_t)1);
//    transition(TIME_OUT);
}

/*
           _     _    _                 _     _ _ _           _______            _           _     _ 
          | |   | |  | |               (_)   | (_) |         |__   __|          | |         | |   | |
  ___  ___| |_  | |__| |_   _ _ __ ___  _  __| |_| |_ _   _     | |_ __ ___  ___| |__   ___ | | __| |
 / __|/ _ \ __| |  __  | | | | '_ ` _ \| |/ _` | | __| | | |    | | '__/ _ \/ __| '_ \ / _ \| |/ _` |
 \__ \  __/ |_  | |  | | |_| | | | | | | | (_| | | |_| |_| |    | | | |  __/\__ \ | | | (_) | | (_| |
 |___/\___|\__| |_|  |_|\__,_|_| |_| |_|_|\__,_|_|\__|\__, |    |_|_|  \___||___/_| |_|\___/|_|\__,_|
                                                       __/ |                                         
                                                      |___/                                          
*/
void setHumidityThreshold()
{
    boolean timeOut = true;
    
    lcd.clear();
    //         1234567890123456
    lcd.print("Humidity % set");

    tmpHumidityTreshold = humidityThreshold;
    timeRef = millis();
    lcd.setCursor(0,1);
    //         1234567890123456
    lcd.print("thres. is: ");
    lcd.print( humidityThreshold );
    lcd.print("%");
    
    while ( (unsigned long)(millis() - timeRef) < 5000 )
    {
        uint8_t button = lcd.button();

        if ( button == KEYPAD_UP )
        {
            tmpHumidityTreshold = tmpHumidityTreshold < 95 ? tmpHumidityTreshold + 5 : tmpHumidityTreshold;
            lcd.setCursor(11,1);
            lcd.print("  ");
            lcd.setCursor(11,1);
            if ( tmpHumidityTreshold < 10 ) lcd.print(" ");
            lcd.print( tmpHumidityTreshold );
            lcd.print( '%' );
            humidityThreshold = tmpHumidityTreshold;
            timeRef = millis();
        }
        else if ( button == KEYPAD_DOWN )
        {
            tmpHumidityTreshold = tmpHumidityTreshold > 5 ? tmpHumidityTreshold - 5 : tmpHumidityTreshold;
            lcd.setCursor(11,1);
            lcd.print("  ");
            lcd.setCursor(11,1);
            if ( tmpHumidityTreshold < 10 ) lcd.print(" ");
            lcd.print( tmpHumidityTreshold );
            lcd.print( '%' );
            humidityThreshold = tmpHumidityTreshold;
            timeRef = millis();
        }
        else if ( button == KEYPAD_SELECT )
        {
            while ( lcd.button() != KEYPAD_NONE ) ;
            timeOut = false;
            break;
        }
        delay(150);
    }

    if ( !timeOut ) {
      EEPROM.write(0, humidityThreshold);
      transition(KEYPAD_SELECT);
    } else {
      transition(TIME_OUT);
    }
}

void showErrorMessage()
{
    lcd.clear();
    //          1234567890123456
    lcd.print( "ERROR:          " );
    lcd.setCursor(0,1);
    lcd.print( "MISSING SENSORS ");
}


void beep() {
  digitalWrite( pinBuzzer, HIGH);
  delay( 100 );
  digitalWrite( pinBuzzer, LOW);
  delay( 50 );
}
