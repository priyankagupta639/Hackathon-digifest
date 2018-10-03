//#include<GSM.h>
#include <SoftwareSerial.h>

#define Motor 13                                        //defining pins
#define ManualSwitch 5
#define UgLower 6
#define UgUpper 7
//#define RoofLower 8
//#define RoofUpper 9
 
SoftwareSerial SIM900(10, 11);                         // TX||RX  Configure software serial port

int ManualS;
String textMessage;                                    // Variable to store text message
String motorState = "off";                             // Create a variable to store motor state 
int motor;  
int ul = 0, uu = 0;
int condition = 0;
 
void setup() {
  
  pinMode(Motor, OUTPUT);                               // Mode of pin
  digitalWrite(Motor, LOW);      
  pinMode(ManualSwitch, INPUT);
  pinMode(UgLower, INPUT);
  pinMode(UgUpper, INPUT);
  //pinMode(RoofLower, INPUT);
  //pinMode(RoofUpper, INPUT);
  
  Serial.begin(19200);                                  // Initializing serial commmunication
  SIM900.begin(19200);
  delay(20000);                                         // Give time to your GSM shield log on to network
  Serial.print("SIM900 is ready to send receive sms");
 
  
  SIM900.print("AT+CMGF=1\r");                         // AT command to set SIM900 to SMS mode
  delay(100);
   
  SIM900.print("AT+CNMI=2,2,0,0,0\r");                // Set module to send SMS data to serial out upon receipt    
  delay(100);
}
 
void loop(){

  ManualS = digitalRead(ManualSwitch);                 // CHECK IF MANUAL SWITCH IS ON or OFF
  delay(50);

  if (ManualS == LOW)                                  // If OFF than,
  {
      motorState = checkSMS();                         // Check for SMS

      if(motorState == "on")                           //If SMS says motor ON
      {
        condition = checkTank(motorState);             // Check Tanks state

          if(condition == 1)                           // If Empty, Motor ON
          {
            digitalWrite(Motor, HIGH);        
            Serial.println("Motor ON");
          }
          if(condition == 0)
          {
            digitalWrite(Motor, LOW);                  // If not Empty, Motor OFF
            Serial.println("Motor OFF");            
          }
      }

      if(motorState == "off")                         // If SMS commands Motor to be OFF
      {
        motorState = checkSMS();                      // Check for new messages, and turn OFF the motor
        digitalWrite(Motor, LOW);
        Serial.println("Motor OFF");
      }
    

  }

  if (ManualS == HIGH)                               // If Manual Switch is HIGH
  {
    digitalWrite(Motor, HIGH);                       // Motor is ON, No matter what other conditions are
    Serial.println("Motor manually ON");
  }
}  
 
int checkTank(String motorState)
{
    ul = digitalRead(UgLower);                        // Read conditions
    uu = digitalRead(UgUpper);

    if(ul == HIGH && uu == HIGH)                      // If water below lower threshold
    {
      condition = 1;                                  // Motor needs to be ON
      Serial.println("tank empty");
    }
    if(ul == LOW && uu == LOW)                        // If water above upper threshold
    {
      condition = 0;                                  // Motor needs to be OFF
      Serial.println("tank full");
    }
    if(ul == LOW && uu == HIGH)
    {
      condition = 1;                                 //When water is between upper and lower threshold
      Serial.println("Water in mid");                         
    }

  return condition;
    
}


String checkSMS(void)
{
  if(SIM900.available()>0)                            // If serial available
     {
        textMessage = SIM900.readString();            // read the message string
        Serial.print(textMessage);    
        delay(10);
     } 
  
  
          if(textMessage.indexOf("ON")>=0)             // If messages says ON
          {
            motorState = "on";                         // set message state  
            Serial.println("Motor set to ON");  
            textMessage = "";   
          }
          
          if(textMessage.indexOf("OFF")>=0)            // If message says OFF
          {
             digitalWrite(Motor, LOW);                 // Turn off the motor
             motorState = "off"; 
             Serial.println("Motor set to OFF");
             textMessage = ""; 
           }
 
          if(textMessage.indexOf("STATE")>=0)          // If user asks for current motor statevia SMS
          {
             String message = "Motor is " + motorState;
             sendSMS(message);
             Serial.println("Motor state resquest");
             textMessage = "";
          }
   return motorState;

}




void sendSMS(String message)                           // Function that sends SMS
{
  
       SIM900.print("AT+CMGF=1\r");                    // AT command to set SIM900 to SMS mode
       delay(100);
       
       SIM900.println("AT + CMGS = \"+919982202001\"");   // change to your sim900's your phone number
       delay(100);
 
       SIM900.println(message);                           // Send the SMS
       delay(100);
 
  
       SIM900.println((char)26);                          // End AT command with a ^Z, ASCII code 26
       delay(100);
       SIM900.println(); 
       delay(5000);                                       // Give module time to send SMS
}


