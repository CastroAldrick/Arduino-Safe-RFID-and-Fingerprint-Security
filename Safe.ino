#include <SPI.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <Servo.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Variables--------------------------

#define SS_PIN        10
#define RST_PIN       9
#define DOOR_CLOSED   0
#define DOOR_OPEN     180
int fingerprintID = 0;
const int iTrig = 4;
const int iEcho = 5;
int iPosition = DOOR_CLOSED;
int iCounter1;
int iCounter2;
int iCounter3;
int iDistance;
long lDuration;
String ValidCards[5] = {"04 8C FB 9A 5B 57 80", "04 A5 45 9A 5B 57 80", "04 0A 8F 9A 5B 57 81", "04 C8 B7 9A 5B 57 80", "EMPTY"};
String sCard = "";
String xCard = "";
boolean successRead = false;
Servo safe;
MFRC522 mfrc522(SS_PIN, RST_PIN);

int x = 0;

// ---------------------------------------

void setup(void) {
  startFingerprintSensor(); 
  pinMode (6, OUTPUT);
  pinMode (iTrig, OUTPUT);
  pinMode (iEcho, INPUT);
  safe.attach(6);
  safe.write(0);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place card on the reader");
       
}

void loop() 
{

  xCard = "";
  successRead = false;
  checkDoor();
  {
    if (iDistance < 25)
    {
      fingerprintID = getFingerprintID(); 
      delay(50);
      if(fingerprintID == 1)
      {
        helloAdmin();
      }
      else
      {
        Serial.println("Enter Card");
        getID();

        while (xCard != 0)
        {
          iCounter3 = 1;
          for (int i = 0; i < 5; i++)
          {
            if (xCard.substring(1) == ValidCards[i])
            {
              Serial.println("Open");
              return;            
            }
            else if (iCounter3 == 5)
            {
              Serial.println("No bro");
              return;
            }
            iCounter3++;
          }
        }
        return;
      }
    }
    else
    {
      Serial.println("Close box");
      delay(1000);
      return;
    }
  }
delay(1000);
}


void startFingerprintSensor()
{
  Serial.begin(9600);
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor");
  }
  Serial.println("Waiting for valid finger...");
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

void checkDoor()
{
  digitalWrite (iTrig, HIGH);
  delayMicroseconds (10);
  digitalWrite (iTrig, LOW);
  lDuration = pulseIn(iEcho, HIGH);
  iDistance = lDuration * 0.034/2;
  //Serial.print("Distance: ");
  //Serial.println(iDistance); 
  delay(1000); 
}

uint8_t getID()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }
  xCard = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     xCard.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     xCard.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  xCard.toUpperCase();
  Serial.println(xCard.substring(1));
  mfrc522.PICC_HaltA();
  return 1;
}

void printArray()
{
  for (int i = 0; i < 5; i++)
    {
      Serial.println(ValidCards[i]);
    }
}

void helloAdmin()
{
  Serial.println("Hello Admin");
  printArray();
  while (!successRead)
  {
    successRead = getID();
    if (successRead == true)
    {
      int iCounter1 = 1;
      int iCounter2 = 1;
      Serial.println("Scan was successful\n");
      delay(1000);

      for (int i = 0; i < 5; i++)
      {
        Serial.print("this is i: ");
        Serial.println(i);
        Serial.print(xCard.substring(1));
        Serial.print(" == ");
        Serial.println(ValidCards[i]);
        
        Serial.print("iCounter1: ");
        Serial.println(iCounter1);

                
        if (xCard.substring(1) == ValidCards[i])
        {
          ValidCards[i] = "EMPTY";
          Serial.println("Removed");
          printArray();
          return;
        }
        else if (iCounter1 == 5)
        {
          for (int i = 0; i < 5; i++)
          {
            Serial.print("this is i: ");
            Serial.println(i);
            Serial.print("this is iCounter2: ");
            Serial.println(iCounter2);
            if (ValidCards[i] == "EMPTY")
            {
              
              ValidCards[i] = xCard.substring(1);
              Serial.println("Added");
              printArray();
              return;
            }
            else if (iCounter2 == 5)
            {
              Serial.println("Sorry no space");
              printArray();
              return;
            }
            iCounter2++;            
          }
        }
        iCounter1++;
      }
    }
  }
}

