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
    Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));

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
    
    boolean find = false;
    while(!find) {
        if (mfrc522.PICC_IsNewCardPresent())
        {
            if (mfrc522.PICC_ReadCardSerial())
            {
                Serial.print(F("Card UID:"));
                printArray(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.println();

                // Finalizar lectura actual
                mfrc522.PICC_HaltA();
                find = true;
                return true;
            }
        }
    }
}

void printArray(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}


//Útiles*****************************************************************************************
boolean comprobacionTeclado() {
 
    char temp[5];    
    int pos = 0;
    int cur = 0;

    //********************PASSWORD
    char password1[4] = { '1','2','3','4' };
    
    //********************PASSWORD
    char password2[4] = { '4','3','2','1' };

    //Longitud_Password*****************************
    int longitud = 4;

    //boolean out = false;
    /*while (true) {
        
        customKey = customKeypad.getKey();
        if (customKey == password[pos]) {            
            pos++; // aumentamos posicion si es correcto el digito
            cur++;
        }

        if (pos == 4) {
            pos = 0;
            cur = 0;            
            delay(2000);            
            return true;
        }

        if (customKey == '*') {
            return false;
        }
    
    }*/

    while (true) {

        char customKey = customKeypad.getKey();
        pos = 0;

        //Password 1
        if (customKey == password1[0]) {
            pos++;
            escribir("OK");
            delay(500);

            while (true) {  

                customKey = customKeypad.getKey();

                if (customKey == password1[pos]) {
                    pos++;                    
                }

                if (pos == 4) {
                    pos++;
                    return true;
                }
            }
        }

        //Password 2
        


    }

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
    //byte a[5] = { B01111110,B10001000,B10001000,B10001000,B01111110 };
    byte r[5] = { B00010000,B00100000,B00100000,B00010000,B00111110 };
    byte d[5] = { B11111110,B00010010,B00100010,B00100010,B00011100 };
    byte u[5] = { B00111110,B00000100,B00000010,B00000010,B00111100 };
    byte i[5] = { B00000000,B00000010,B10111110,B00100010,B00000000 };
    byte n[5] = { B00011110,B00100000,B00100000,B00010000,B00111110 };
    byte o[5] = { B00011100,B00100010,B00100010,B00100010,B00011100 };
    
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

    if (a == "RFID") {
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
        delay(150);

        for (int i = 0; i < 8; i++) {
            lc.setRow(0, i, f[i]);
        }

    }
}
void matrixOff() { 
    lc.shutdown(0, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0, 8);
    /* and clear the display */
    lc.clearDisplay(0);
}