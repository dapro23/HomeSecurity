#include <Time.h>//Incluimos la librería Time


//Servo Motor*********************************************************************************************************
#include <Servo.h>
Servo myservo;


//Keypad*************************************************************************************************************
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'} };
byte rowPins[ROWS] = { 36, 34, 32, 30 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 28, 26, 24, 22 }; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


//RFID******************************************************************************************************************
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   5     // Configurable, see typical pin layout above
#define SS_PIN    53   // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

/* Set your new UID here! */
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}

MFRC522::MIFARE_Key key;


//Matriz******************************************************************************************************************
#include "LedControl.h"
LedControl lc = LedControl(12, 10, 11, 1);


//Sensor Utrasonico*****************************************************************************************************
#include "SR04.h"
#define TRIG_PIN 4
#define ECHO_PIN 3
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long a;



//SETUP****************************************************************************************************************
void setup() {    

    //Servo******
    myservo.attach(7);
    puerta(false);
    //**********
        

    //RFID***********************
    Serial.begin(9600);  // Initialize serial communications with the PC
    while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();         // Init SPI bus
    mfrc522.PCD_Init();  // Init MFRC522 card    

    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    //matrix************************************************************************************************************************
    /*
   The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
   */
    lc.shutdown(0, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0, 8);
    /* and clear the display */
    lc.clearDisplay(0);
}

void loop() {
    a = sr04.Distance();
    // Detectar tarjeta

    if (a<10)
    {        
        escribir("RFID");
        delay(500);
        if (comprobacionRFID()) {
            escribir("OK");
            delay(500);
            escribir("TECLADO");
            if (comprobacionTeclado()) {
                escribir("OK");
                escribir("PUERTA");
                puerta(true);
                matrixOff();
            }
            else {
                escribir("F");
                delay(250);
                escribir("RFID");
                delay(250);
                matrixOff();
            }
        }
        else {
            escribir("F");
            matrixOff();
        }
    }
    

}


//RFID***********************
boolean comprobacionRFID() { 
    

    //Clave***************************************
    byte validKey1[4] = { 0xB9, 0xB5, 0xB5, 0xB0 };

    while(true) {

        if (mfrc522.PICC_IsNewCardPresent())
        {
            if (mfrc522.PICC_ReadCardSerial())
            {

                Serial.print(F("Card UID:"));
                printArray(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.println();

                if (isEqualArray(mfrc522.uid.uidByte, validKey1, 4)) {
                    Serial.println("Tarjeta valida");
                    return true;
                }
                    
                else {
                    Serial.println("Tarjeta invalida");
                    return false;
                }
                    

                
            }
        }
    
    
    
    }
}

boolean isEqualArray(byte* arrayA, byte* arrayB, int length)
{
    for (int index = 0; index < length; index++)
    {
        if (arrayA[index] != arrayB[index]) return false;
    }
    return true;
}

void printArray(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}


//Password*****************************************************************************************
boolean comprobacionTeclado() {

    char temp[5];
    int pos = 0;

    while (true) {

        char customKey = customKeypad.getKey();

        if (customKey == '*') {
            return false;
        }

        if (customKey) {
            temp[pos] = customKey;
            pos++;   
            Serial.println(customKey);
            escribir("DOT");
        }

        if (pos == 5) {
            pos = 0;
            break;
        }

        

    }
    if (comprobacionPassword(temp)) {
        return true;
    }
    else {
        return false;
    }
    

}

boolean comprobacionPassword(char password[]) {    

    //********************PASSWORD
    char password1[5] = { '1','2','3','4','5' };

    //********************PASSWORD
    char password2[5] = { '5','4','3','2','1' };    


    boolean out = false;
    for (int i = 0; i < 5; i++) {
        
        //se agregan tantos if como contraseñas existan
        if(password[i] == password1[i]) {
            out = true;            
        }else if(password[i] == password2[i]) {
            out = true;           
        }
        else {
            out = false;
        }

    }    

    return out;

}


//Abrir_Puerta*****************************************************************************************
void puerta(boolean a) {
    
    if (a == false) { //Cierra la puerta

        myservo.write(0);// move servos to center position -> 90°

    }
    else if (a == true) {//Abre la puerta

        myservo.write(180);// move servos to center position -> 90°
        delay(3000);
        puerta(false);
    }
    
}


//Escribir*****************************************************************************************
void escribir(String a) {
    /* here is the data for the characters */

    byte flechaD[8] = {
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B10011001,
        B01011010,
        B00111100,
        B00011000
    };

    byte flechaI[8] = {
        B00011000,
        B00111100,
        B01011010,
        B10011001,
        B00011000,
        B00011000,
        B00011000,
        B00011000
    };

    byte adelante[8] = {
        B00001000,
        B00000100,
        B00000010,
        B11111111,
        B11111111,
        B00000010,
        B00000100,
        B00001000
    };

    byte ok[8] = {
        B00011000,
        B00100100,
        B00100100,
        B00011000,
        B00000000,
        B00111100,
        B00011000,
        B00100100
        
    };
    byte f[8] = {
        B00000000,
        B00000000,
        B01111110,
        B00010010,
        B00010010,
        B00000010,
        B00000000,
        B00000000

    };

    byte DOT[8] = {
        B00000000,
        B00000000,
        B00000000,
        B00011000,
        B00011000,
        B00000000,
        B00000000,
        B00000000

    };

    if(a == "DOT"){
        matrixOff();
        

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, DOT[i]);
        }  
        delay(250);
        matrixOff();
        
    }else if(a == "RFID") {
        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, flechaI[i]);
        }        
    }else if (a == "TECLADO") {
        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, flechaD[i]);
        }
    }else if (a == "PUERTA") {

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, adelante[i]);
        }
    }else if (a == "OK"){

        matrixOff();
        delay(250);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, ok[i]);
        }

        delay(250);
        matrixOff();
        delay(250);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, ok[i]);
        }

        delay(250);
        matrixOff();
        delay(150);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, ok[i]);
        }

    }else if (a == "F") {

        matrixOff();
        delay(250);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, f[i]);
        }

        delay(250);
        matrixOff();
        delay(250);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, f[i]);
        }

        delay(250);
        matrixOff();      

    }
}

void matrixOff() { 
    lc.shutdown(0, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0, 8);
    /* and clear the display */
    lc.clearDisplay(0);
}