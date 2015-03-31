
#include <LiquidCrystal.h>  // Required by LCDKeypad
#include <LCDKeypad.h>

#define TIME_OUT 5  // One of the system's FSM transitions

enum states
{
    SHOW_HUMIDITY,
    SHOW_HUMIDITY_THRESHOLD,
    SET_HUMIDITY_THRESHOLD
};

LCDKeypad lcd;

states state;
int8_t button;
int humidityThreshold = 60;
int tmpHumidityTreshold;
unsigned long timeRef;

void setup()
{
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    //         1234567890123456
    lcd.print("  VENTILAZIONE  ");
    lcd.setCursor(0,1);
    lcd.print("  ARCHIVIO 1.0  ");
    delay(2000);
    // read eeprom value for humidity treshold
    state = SHOW_HUMIDITY;
}

// Has the main control of the FSM (1Hz refresh rate)
void loop()
{
    timeRef = millis();

    // Uses the current state to decide what to process
    switch (state)
    {
        case SHOW_HUMIDITY:
            showHumidity();
            break;
        case SHOW_HUMIDITY_THRESHOLD:
            showHumidityThreshold();
            break;
        case SET_HUMIDITY_THRESHOLD:
            setHumidityThreshold();
    }

    // Waits about 1 sec for events (button presses)
    // If a button is pressed, it blocks until the button is released
    // and then it performs the applicable state transition
    while ( (unsigned long)(millis() - timeRef) < 970 )
    {
        if ( (button = lcd.button()) != KEYPAD_NONE )
        {
            while ( lcd.button() != KEYPAD_NONE ) ;
            transition(button);
            break;
        }
    }
}

// Looks at the provided trigger (event) 
// and performs the appropriate state transition
// If necessary, sets secondary variables
void transition(uint8_t trigger)
{
    switch (state)
    {
        case SHOW_HUMIDITY:
            if ( trigger == KEYPAD_SELECT )      state = SET_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_LEFT )   state = SHOW_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_RIGHT )  state = SHOW_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_UP )     state = SHOW_HUMIDITY_THRESHOLD;
            else if ( trigger == KEYPAD_DOWN )   state = SHOW_HUMIDITY_THRESHOLD;
            break;
        case SHOW_HUMIDITY_THRESHOLD:
            if ( trigger == KEYPAD_SELECT )     state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_LEFT )  state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_RIGHT ) state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_UP )    state = SHOW_HUMIDITY;
            else if ( trigger == KEYPAD_DOWN )  state = SHOW_HUMIDITY;
            else if ( trigger == TIME_OUT )     state = SHOW_HUMIDITY;
            break;
        case SET_HUMIDITY_THRESHOLD:
            if ( trigger == KEYPAD_SELECT ) state = SHOW_HUMIDITY;
            else if ( trigger == TIME_OUT ) state = SHOW_HUMIDITY;
            break;
    }
}

void showHumidity()
{
    lcd.clear();
    //         1234567890123456
    lcd.print( "V=Hi>He e Hi>" );
    lcd.print( humidityThreshold );
    lcd.print( "%" );
    lcd.setCursor(0,1);
    lcd.print( "Hi 30% He 33%" );
}

void showHumidityThreshold()
{
    lcd.clear();
    //         1234567890123456
    lcd.print("Soglia umidita'");
    lcd.setCursor(0,1);
    lcd.print( "interna: " );
    lcd.print( humidityThreshold );
    lcd.print( '%' );
    delay(2000);
    transition(TIME_OUT);
}

void checkHumidity()
{
    // if (( Hi > He ) && ( Hi > Htreshold)) { activate_ventilation; }
}


void setHumidityThreshold()
{
    boolean timeOut = true;
    
    lcd.clear();
    //         1234567890123456
    lcd.print("Impostazione %");

    tmpHumidityTreshold = 50;
    timeRef = millis();
    lcd.setCursor(0,1);
    //         1234567890123456
    lcd.print("Umidita':  ");
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
            // to do write value in eeprom
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
            // to do write value in eeprom
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

    if ( !timeOut ) transition(KEYPAD_SELECT);
    else transition(TIME_OUT);
}

