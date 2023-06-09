#include <RFID.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include "pitches.h"

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 5
#define Red_LED 6
#define Green_LED 7
#define Buzzer 8

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
Servo DoorLock;
RFID rfid(SS_PIN, RST_PIN );

int My_RFID_Tag[5] = {0x23,0xC3,0x9D,0xA7,0xDA};
boolean My_Card = false;  

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() 
{
  //set the pins as an input/output
  pinMode(Red_LED,OUTPUT);
  pinMode(Green_LED,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  
  //Servo Connnected to pin Digital Pin 5
  DoorLock.attach(SERVO_PIN);
  //open the serial port at 9600 baudrate.
  Serial.begin(9600); 
  //Initialise the LCD to 16x2 Character Format
  lcd.begin(16,2);
  //Initialise Servo and RFID
  SPI.begin();
  rfid.init();
}

void loop() 
{
  
  //First Assume detected card(Or tag) is My_Card, 
  //Then later we will check is it My_Card or not! 
  My_Card = true; 
  DoorLock.write(0); //Servo at 0 Position, Door is Closed.
  lcd.clear();
  lcd.print("RFID automatic door opener");
  lcd.setCursor(0,1);
  lcd.print("wireless communication project");
  
  //Check if any RFID Tags Detected or not?
  if( rfid.isCard() )
  {
      //if RFID Tag is detected, check for the Unique ID,and print it on the Serial Window
      
      if( rfid.readCardSerial() )
      {   
          lcd.clear();      
          lcd.print("Unique_ID is: ");
          delay(500);          
          lcd.setCursor(0,1); //Second Row, First Character
   
          //Printing in HEX 
          for( int i = 0; i < 5; i++ )
          {
              Serial.print(rfid.serNum[i], HEX);
              Serial.print(" ");              
              lcd.print(rfid.serNum[i],HEX);
              lcd.print(" ");                            
          }  
          delay(500);
          
      //Compare this RFID Tag Unique ID with your My_RFID_Tag's Unique ID
          for(int i = 0; i < 5; i++)
          {   
              //if any one Unique ID Digit is not matching,
              //then make My_Card = false and come out from loop
              //No need to check all the digit!
              if( My_RFID_Tag[i] != rfid.serNum[i] )
              {
                My_Card = false;
                break;                
              }           
          }
          Serial.println(); 
          delay(1000); 

          //If RFID Tag is My_Card then give access to enter into room
          //else dont open the door.
          if(My_Card)
          {
            Serial.println("\nWelcome To Your Room, DR-heba!");
            lcd.clear();
            lcd.print("Welcome to Your");
            lcd.setCursor(0,1);
            lcd.print("Room, DR-HEBA!");
            delay(2000);                        
            
            //Turn on the Green LED as an indication of permission is given 
            //to access the room.
            digitalWrite(Green_LED,HIGH);
            
            // iterate over the notes of the melody:
            int i = 0;
            while(i < 2)
            {
              for (int thisNote = 0; thisNote < 12; thisNote++) 
              {          
                // to calculate the note duration, take one second
                // divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                int noteDuration = 1000 / noteDurations[thisNote];
                tone(8, melody[thisNote], noteDuration);                        
                // to distinguish the notes, set a minimum time between them.
                // the note's duration + 30% seems to work well:
                int pauseBetweenNotes = noteDuration * 1.30;
                delay(pauseBetweenNotes);              
                // stop the tone playing:
                noTone(8);
              }
              i =  i + 1;
              delay(500);              
            }
            delay(1000);            

            //Now, Open the Door with the help of Servo Motor
            DoorLock.write(180);            
            delay(200);
            lcd.clear();                                    
            lcd.print("Door is Open");
            lcd.setCursor(0,1);
            lcd.print("Now!");
            delay(2000);
            lcd.clear();

            //Give 10 Sec delay to enter into room
            //After that door will again closed!
            for(int i = 10; i > 0; i--)
            {
              lcd.print("the Door will close");
              lcd.setCursor(0,1);
              lcd.print("in ");
              lcd.print(i);
              lcd.print(" Sec.HurryUp!");
              delay(1000);
              lcd.clear();
            }

            //Now,Door is closed and Green LED is Turned-Off.
            DoorLock.write(0);
            digitalWrite(Green_LED,LOW);
            delay(200);
            lcd.clear();
            lcd.print("the Door is Closed");
            lcd.setCursor(0,1);
            lcd.print("Now!");
            delay(2000);                            
          }
          
          // If RFID Tag is not My_Card then Do not open the Door and Turn-On Red LED and Buzzer as an indication of Warning:
          else
          {
            Serial.println("\nGet Out of Here !");
            lcd.clear();
            lcd.print("Card isnot FOUND!");
            lcd.setCursor(0,1);
            lcd.print("Get Out of Here!");

            for(int i = 0; i < 7; i++)
            {
              digitalWrite(Buzzer, HIGH);
              digitalWrite(Red_LED,HIGH);
              delay(500);
              digitalWrite(Buzzer, LOW);
              digitalWrite(Red_LED,LOW);
              delay(500);              
            }
            delay(1000);            
          }                 
      }      
   }
  rfid.halt();
}
