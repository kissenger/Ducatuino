
// ------------------------------------------------------------------------------
#define SOFT_VERSION F("0.1")  
//    rework display logic to avoid corrupted screen errors
//    recoded storage of data using data structure and associated changed to display coding
// ------------------------------------------------------------------------------

//===============================================================================
//
//                             DEFINE VARIABLES
//
//===============================================================================
 
// Library Definitions
#include      <Wire.h>                                             // library for I2C communication with accelerometer
#include      <LiquidCrystal.h>                                    // library for communicating with lcd display
#include      <OneWire.h>                                          // library for onewire communication with digital temp gauges
#include      <DallasTemperature.h>                                // library for digital temp gauges
#include      <SoftwareSerial.h>                                   // software serial emulator for control of gps
#include      <Streaming.h>                                        // enables inline send to lcd commands using <<

// Define sensor parameters
#define TEMP_IN_UPR       0
#define TEMP_IN_LWR       1
#define TEMP_IN_AVE       2
#define TEMP_OUT_FWD      3
#define TEMP_OUT_AFT      4
#define TEMP_OUT_AVE      5
#define SENSOR_ARRAY_SIZE 6

// struct sensor_reading {
//    char*   val;
//    boolean isdisplayed;
//    int     rowcol[2];
//    char*   addr;
// } sensor[SENSOR_ARRAY_SIZE];

boolean IsNewScreen = true;




class TemperatureProbes {
  public:
    static int insideTemp;
    static int outsideTemp;
    getTemps();
}

void TemperatureProbes::getTemps() {
  TmpSensors.requestTemperatures();
  insideTemp  = 0.5 * (TmpSensors.getTempC(addrInsideTemp1)  + TmpSensors.getTempC(addrInsideTemp2));         //keep as integer type - not interested in the dps
  outsideTemp = 0.5 * (TmpSensors.getTempC(addrOutsideTemp1) + TmpSensors.getTempC(addrOutsideTemp2));
}

TemperatureProbes temps;
temps.getTemps();

display1 = temps::InsideTempAve;
display2 = temps::OutsideTempAve;


class Line {
  char* preText;
  int valueName;
  char* valueUnit;
}



class Screen {

  public:
    Screen(Line, Line)
}

Screen::Screen (Line a, Line b) {
  rowOne = a;
  rowTwo = b;
}

Screen screen(
  Line a("Temperature 1 = ",  "TEMP_IN_UPR", "\337C"),
  Line b("Temperature 2 = ",  "TEMP_IN_LWR", "\337C")
)



class Display {
  public:
    static boolean isBacklightOn;
    static boolean newScreenRequest;
    static boolean updateValues;
    void turnBacklightOn();
    void turnBacklightOff();
    void changeScreen(index);
    void updateValuesOnly();
}

Display::changeScreen(index, sensors) {
  
  lcd.clear();

  // set all sensor values to not displayed
  for (int i = 0; i < SENSOR_ARRAY_SIZE; i++)
    s[i].isdisplayed = false;
  }

  // update 
  if (index == 0) {
    s[TEMP_IN_AVE ].isdisplayed = true;
    s[TEMP_OUT_AVE].isdisplayed = true;
    lcd.setCursor(0,0); lcd << "Temperature 1 =     \337C";
    lcd.setCursor(0,1); lcd << "Temperature 2 =     \337C";
  } else if (index == 1) {
    //next screen definition
  }

  Display::updateValuesOnly(s);
  return s

}

Display::updateValueOnly(s) {   
  for (int i = 0; i < SENSOR_ARRAY_SIZE; i++) {
    lcd.setCursor(s[i].col, s[i].row);
    lcd << s[i].value;
  }
}

Display::turnBacklightOn(){
  analogWrite(PIN_LCD_BACKLIGHT, LCD_LIGHT_ON); 
  isBacklightOn = true;
} 

Display::turnBacklightOff(){
  analogWrite(PIN_LCD_BACKLIGHT, LCD_LIGHT_OFF);
  isBacklightOn = false;
}

// sensor[TEMP_IN_UPR]  = {0, false, {0,0},  "0x28, 0x0F, 0x85, 0xD0, 0x04, 0x00, 0x00, 0xBC"};
// sensor[TEMP_IN_LWR]  = {0, false, {0,0},  "0x28, 0x31, 0x8F, 0x74, 0x04, 0x00, 0x00, 0x58"};
// sensor[TEMP_IN_AVE]  = {0, false, {0,12}, ""};
// sensor[TEMP_OUT_FWD] = {0, false, {0,0},  "0x28, 0x6B, 0x55, 0xC5, 0x03, 0x00, 0x00, 0x3E"};
// sensor[TEMP_OUT_AFT] = {0, false, {0,0},  "0x28, 0x7E, 0x5A, 0xC5, 0x03, 0x00, 0x00, 0xE1"};
// sensor[TEMP_OUT_AVE] = {0, false, {1,12}, ""};

  
// Define Times and Timers
#define              TIME_FOR_LONG_PRESS   1000                           // time required for a long button press
#define              TIME_BACKLIGHT_HIGH   8000                           // length of time backlight will remain high after button press
#define              DELAY_DEBOUNCE        35                             // button debounce delay
#define              READ_TEMPS_DELAY      60000                          // delay between reading temperature sensors

// Define LCD variables
LiquidCrystal        lcd(9, 8, 5, 4, 3, 2);                                // define pins for lcd display
#define              PIN_LCD_CONTRAST      10                              // define digital pin for contrast control
#define              PIN_LCD_BACKLIGHT     6                               // define digital pin for backlight control
#define              LCD_CONTRAST          20                              // define contrast level
#define              LCD_LIGHT_ON          80                              // define backlight brightness when set to low
#define              LCD_LIGHT_OFF         0                              // define backlight brightness when set to high 
unsigned long        LastDisplayTime       = 0;                            // time that the display was last updated  
boolean              IsBacklightOn         = false;                          // keeps track of backlight status

    
// Define Temperature Variables
OneWire              ds(PIN_DIGITAL_TEMP);                                 // create instance of onewire library for communication with onewire devices
DallasTemperature    TmpSensors(&ds);                                      // create instace of dallastemp library for reading temperature sensors
#define              PIN_DIGITAL_TEMP      7                               // set digital pin for temperature readings
unsigned long        LastTempReadTime      = 0;                            // time that the temperatures were last checked  
DeviceAddress        addrOutsideTemp1      = {0x28, 0x6B, 0x55, 0xC5, 0x03, 0x00, 0x00, 0x3E};    // long grey lead probe   OUTSIDE FRONT
DeviceAddress        addrOutsideTemp2      = {0x28, 0x7E, 0x5A, 0xC5, 0x03, 0x00, 0x00, 0xE1};    // long grey lead probe   OUTSIDE BACK 
DeviceAddress        addrInsideTemp1       = {0x28, 0x31, 0x8F, 0x74, 0x04, 0x00, 0x00, 0x58};    // 5m black cable         INSIDE DOWN
DeviceAddress        addrInsideTemp2       = {0x28, 0x0F, 0x85, 0xD0, 0x04, 0x00, 0x00, 0xBC};    // 10m black cable        INSIDE UP
 

// Define buton variables
#define              PIN_BUTTON            11                         // define digital pin for button   
unsigned int         ButtonState           = 0;                       // Current state of button
unsigned int         LastButtonState       = 0;                       // State of button last time it was checked
unsigned long        ButtonPressTime       = 0;                       // Length of time since button was last confirmed on
unsigned long        LastActivityTime      = 0;                       // Time since button press or warning

// -- DISPLAY VARIABLES --
const int            NumberOfScreens       = 1;      // number of unique screens to display
unsigned int         DisplayIndex          = 0;      // index of the current display, progressed by button press
char                 chrBuffer[16];                  // text buffer used for formatting variables for display output

//===============================================================================
// 
//                             SETUP 
//
//===============================================================================

void setup(){
  
  // Set pin modes
  pinMode(PIN_VOLTS_MAIN,     INPUT);   
  pinMode(PIN_VOLTS_AUX,      INPUT);  
  pinMode(PIN_DIGITAL_TEMP,   INPUT);
  pinMode(PIN_BUTTON,         INPUT);
  pinMode(PIN_LCD_CONTRAST,   OUTPUT);
  pinMode(PIN_LCD_BACKLIGHT,  OUTPUT);
  
  // Initialise hardware serial comms for debugging
  // Serial.begin(9600);   
 
  // Initialise digital temp gauges
  TmpSensors.begin();                                 // start sensors library
  TmpSensors.setResolution(addrInsideTemp1,  12);     
  TmpSensors.setResolution(addrInsideTemp2,  12);        
  TmpSensors.setResolution(addrOutsideTemp1, 12);        
  TmpSensors.setResolution(addrOutsideTemp2, 12);        
  
  // Initialise display
  // UPDATE
  lcd.begin(16,2);                                    // initialise lcd display
  analogWrite(PIN_LCD_CONTRAST, LCD_CONTRAST);        // set lcd contrast
  SetBackLightHigh();                                 // turn on backlight 
  LastActivityTime = millis();                        // initialised idle timer
 
  // Display splash screen
  lcd.setCursor(0,0); lcd << F(" Ducatuino v") << SOFT_VERSION;
  delay(300);
  
}

//===============================================================================
//
//                             MAIN LOOP
//
//===============================================================================

void loop() {

  
  //---------------------------------------------------------------------
  // Detect button press - long and short press enabled
  // On short press: 
  //---------------------------------------------------------------------
  

  ButtonState = digitalRead(PIN_BUTTON);
  if (ButtonState == HIGH && ButtonState != LastButtonState) {        
    // if button is pressed and it wasnt pressed on last loop (ie button just pressed)
    ButtonPressTime = millis();

  } else if (ButtonState == LOW && ButtonState != LastButtonState) {              
    // if the button is not pressed and it was on the last loop (ie button just unpressed) 
    if (millis() - ButtonPressTime > DELAY_DEBOUNCE) { 
                  
      if (BackLightIndex == 2) {                                    
        // if backlight is already high, do something, otherwise dont (backlight will go high because LastActivityTime was reset)
        DisplayIndex++;
        if (DisplayIndex > DisplayScreensShort) DisplayIndex = 0;                                                    
      }     

    }    
    LastActivityTime = millis();
  }

  LastButtonState = ButtonState;  // used to track debounce

  // Handle LCD backlight
  if (millis()-LastActivityTime < TIME_BACKLIGHT_HIGH) { 
    if (IsBacklightOn) IsBacklightOn = TurnBacklightOn();  
  } else if (IsBacklightOn) {
    IsBacklightOn =  TurnBacklightOff();
  }

  // Get new sensor data if its time
  if ((millis() - LastTempReadTime) > READ_TEMPS_DELAY || LastTempReadTime == 0) {  
    LastTempReadTime = millis();
    temps = ReadTemps(); 
    sensor[TEMP_IN_AVE].value = temps[0];
    sensor[TEMP_OUT_AVE].value = temps[1];
  }

  // Handle the display
  if (IsNewScreenRequest) {
    DisplayNewScreen();
  } else if (IsValuesUpdated) {
    UpdateValues();
  }

}                                                                     

//===============================================================================
//                    DISPLAY SUBROUTINES
//===============================================================================

/**
 * Takes the index number of the desired screen and the sensor data as struct
 * 
 */


//===============================================================================
//                    TEMPERATURES
//===============================================================================

/**
 * Read temperature sensors
 * Loop time approx 800 ms
 * Loop time could be improved - library effectively waits 750ms for sensors to return
 * Needs some thought but could be better.
 */

void ReadTemps() {

  TmpSensors.requestTemperatures();
  InsideTempAve  = 0.5 * (TmpSensors.getTempC(addrInsideTemp1)  + TmpSensors.getTempC(addrInsideTemp2));         //keep as integer type - not interested in the dps
  OutsideTempAve = 0.5 * (TmpSensors.getTempC(addrOutsideTemp1) + TmpSensors.getTempC(addrOutsideTemp2));

  return [PadInt(InsideTempAve,2," "), PadInt(OutsideTempAve,2," ")]

}

//===============================================================================
//                    UTILITY
//===============================================================================

/**
 * -------------------------------------------------------------------------  
 * function takes an integer and left pads to desired field width with requested char
 * int num - integer number to left-pad
 * int wdth - desired field width after padding (max 10 defined by zpi/buff array size)
 * char pad - character requested to pad with
 * -------------------------------------------------------------------------
 * eg: PadInt(56,5," ") returns "   56"
 * eg: PadInt(-12,4,"0") returns "0-12"
 * eg: PadInt(-12,4," ") returns " -12"
 * eg: PadInt(135,2," ") returns "13"
 * -------------------------------------------------------------------------
 */ 
char * PadInt(int num, const int wdth, char * pad) {
  
  static char zpi[10]="";                             //initialise zpi (zero-pad-integer) array
  char buff[10]="";                                   //initialise buffer array

  itoa(num,buff,10);                                  //convert the subject number to a string and store in buffer arrat
  strcpy(zpi,"");                                     // put nothing into zpi array                              

  if (strlen(buff) > wdth) strncat(zpi,buff,wdth);    // if buffer is longer than wdth requested then knock off end characters
  else {
    for (int i = 0; i < (wdth-strlen(buff)); i++) {   // for the number of characters in zpi not filled by the number in buff
      strcat(zpi,pad);                                // fill with pad character
    }
    strcat(zpi,buff);                                 // then copy buff onto the end 
  }
  
  return zpi;                                         // and return

}
