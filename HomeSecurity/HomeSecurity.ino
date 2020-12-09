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
/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example to change UID of changeable MIFARE card.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * This sample shows how to set the UID on a UID changeable MIFARE card.
 * NOTE: for more informations read the README.rst
 *
 * @author Tom Clement
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

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
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
    lc.shutdown(0, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0, 8);
    /* and clear the display */
    lc.clearDisplay(0);

}

void loop() {
    
    // Detectar tarjeta
    if (mfrc522.PICC_IsNewCardPresent())
    {
        Serial.print(F("Tarjeta detectada! => "));
        escribir("RFID");
        delay(500);
        if (comprobacionRFID()) {
            escribir("RFID OK");
            delay(1000);

            escribir("TECLADO");
            if (comprobacionTeclado()) {
                escribir("TECLADO OK");
                puerta(true);
            }
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
    char customKey;
    int pos = 0;
    int cur = 0;

    //********************PASSWORD
    char password[4] = { '1','2','3','4' };
    //********************PASSWORD

    boolean out = true;

    while (out) {
        customKey = customKeypad.getKey();
        if (customKey == password[pos]) {            
            pos++; // aumentamos posicion si es correcto el digito
            cur++;
        }

        if (pos == 4) {
            pos = 0;
            cur = 0;            
            delay(2000);
            out = false;
            return true;
        }
    }

}



void puerta(boolean a) {
    
    if (a == false) { //Cierra la puerta

        myservo.write(0);// move servos to center position -> 90°

    }
    else if (a == true) {//Abre la puerta

        myservo.write(180);// move servos to center position -> 90°
        delay(5000);
        puerta(false);
    }
    
}


void escribir(String a) {

    /* here is the data for the characters */
    //byte a[5] = { B01111110,B10001000,B10001000,B10001000,B01111110 };
    byte r[5] = { B00010000,B00100000,B00100000,B00010000,B00111110 };
    byte d[5] = { B11111110,B00010010,B00100010,B00100010,B00011100 };
    byte u[5] = { B00111110,B00000100,B00000010,B00000010,B00111100 };
    byte i[5] = { B00000000,B00000010,B10111110,B00100010,B00000000 };
    byte n[5] = { B00011110,B00100000,B00100000,B00010000,B00111110 };
    byte o[5] = { B00011100,B00100010,B00100010,B00100010,B00011100 };



    if (a == "RFID") {
        /* now display them one by one with a small delay */
        lc.setRow(0, 0, i[0]);
        lc.setRow(0, 1, i[1]);
        lc.setRow(0, 2, i[2]);
        lc.setRow(0, 3, i[3]);
        lc.setRow(0, 4, i[4]);
    }
    else if (a == "RFID OK") {
        lc.setRow(0, 0, r[0]);
        lc.setRow(0, 1, r[1]);
        lc.setRow(0, 2, r[2]);
        lc.setRow(0, 3, r[3]);
        lc.setRow(0, 4, r[4]);
    }
    else if (a == "TECLADO") {
        lc.setRow(0, 0, d[0]);
        lc.setRow(0, 1, d[1]);
        lc.setRow(0, 2, d[2]);
        lc.setRow(0, 3, d[3]);
        lc.setRow(0, 4, d[4]);
    }
    else if (a == "TECLADO OK") {
        lc.setRow(0, 0, o[0]);
        lc.setRow(0, 1, o[1]);
        lc.setRow(0, 2, o[2]);
        lc.setRow(0, 3, o[3]);
        lc.setRow(0, 4, o[4]);
    }else if (a == "PUERTA") {
        lc.setRow(0, 0, n[0]);
        lc.setRow(0, 1, n[1]);
        lc.setRow(0, 2, n[2]);
        lc.setRow(0, 3, n[3]);
        lc.setRow(0, 4, n[4]);
    }

}


































/*void leer() {

    while (true) {
        // Look for new cards, and select one if present
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            return;
        }

        // Now a card is selected. The UID and SAK is in mfrc522.uid.

        // Dump UID
        Serial.print(F("Card UID:"));
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();

        // Dump PICC type
      //  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      //  Serial.print(F("PICC type: "));
      //  Serial.print(mfrc522.PICC_GetTypeName(piccType));
      //  Serial.print(F(" (SAK "));
      //  Serial.print(mfrc522.uid.sak);
      //  Serial.print(")\r\n");
      //  if (  piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
      //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
      //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      //    Serial.println(F("This sample only works with MIFARE Classic cards."));
      //    return;
      //  }

        // Set new UID
        byte newUid[] = NEW_UID;
        if (mfrc522.MIFARE_SetUid(newUid, (byte)4, true)) {
            Serial.println(F("Wrote new UID to card."));
        }

        // Halt PICC and re-select it so DumpToSerial doesn't get confused
        mfrc522.PICC_HaltA();
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            return;
        }

        // Dump the new memory contents
        Serial.println(F("New UID and contents:"));
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

        delay(2000);

    }

}*/
