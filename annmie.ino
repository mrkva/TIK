/*  internal sensors for hives on i2c bus ...temp_1(float celcius2) and temp_2(float celcius3), external one is (float celcius) 
    internal sensors for humidity are patched on analog inputs ...range of 0 to 5V....internal humidity_1 is on analog(1), internal humidity_2 is on analog(2), external on analog(0).
the analog values are not calibrated!!!!!!!
    DATA are available via http request on IP assigned to arduino with ethernet shield 
not using DHCP !! should be set up according to your network settings
adapted from arduino examples and documentation by jakub hybler for "bee monitoring workshop bruxeless 2011" 
*/

/* 
CURRENT STATE:

OUTSIDE: SHT15 (DIGITAL 4, 5)
INSIDE: ???

*/

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <SHT1x.h>

//SHT15 sensor pins
#define dataPin  5
#define clockPin 4
SHT1x sht1x(dataPin, clockPin);

int tmp102Address = 0x4B;
int tmp102Address2 = 0x48;
int tmp102Address3 = 0x49;
byte res;
//int vall;
//-----------------------------------------------------------ethernet shield settings!!!!!!!!!!
// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xAB, 0xCD, 0x12};
// assign an IP address for the controller:
byte ip[] = { 192, 168, 1, 155 };
byte gateway[] = { 192, 168, 1, 1 };	
byte subnet[] = { 255, 255, 255, 0 };
//--------------------------------------------------------------------------------------------------------------- 
  
int temperatureCommand  = B00000011;  // command used to read temperature
int humidityCommand = B00000101;  // command used to read humidity

//int clockPin = 7;  // pin used for clock
//int dataPin  = 6;  // pin used for data
int ack;  // track acknowledgment for errors
int val;  
float temperature;          
float humidity;  
float temp_external;
float humidity_external;

  
  
// initialize the library instance:


long lastConnectionTime = 0;        // last time you connected to the server, in milliseconds
boolean lastConnected = false;      // state of the connection last time through the main loop
const int postingInterval = 90000;  //delay between updates to Pachube.com

EthernetServer server(80);

void setup() {
  // start the ethernet connection 
 Ethernet.begin(mac, ip, gateway, subnet);
 server.begin();
  
  //Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(10000);
  //Serial.println("server ok");
}

void loop()
{
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          //Serial.println("got client connection");
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          //client.println(" <META HTTP-EQUIV=\"Refresh\" CONTENT=\"3;URL=http://172.16.0.114\" >");//------------important----for use with common browser view...ip address
          
//-----------------should be set as in the network settings of your arduino .....this is a refresh header for browser...........3 seconds referesh......

  
//----------------------------  

// external temp------------------------------------------------------

  /*Wire.begin();
  
  Wire.requestFrom(tmp102Address,2); 

  byte MSB = Wire.receive();
  byte LSB = Wire.receive();

  int TemperatureSum = ((MSB << 8) | LSB) >> 4; //it's a 12bit int, using two's compliment for negative

  float celcius = TemperatureSum*0.0625;
  float fahrenheit = (TemperatureSum*0.1125) + 32;  */
  
  

 // Serial.print("Celcius: ");
 // Serial.println(celcius);
  client.print("external temp in celcius is :");
  temp_external = sht1x.readTemperatureC();
  client.print(temp_external/2);
  client.println("<br />");
  //-----------------------internal temp 1 -----------------------------------------------
  //Wire.end();
  delay(100);
  Wire.begin();
  Wire.requestFrom(tmp102Address2,2); 

  byte MSB2 = Wire.read();
  byte LSB2 = Wire.read();

  int TemperatureSum2 = ((MSB2 << 8) | LSB2) >> 4; //it's a 12bit int, using two's compliment for negative

  float celcius2 = TemperatureSum2*0.0625;
  //float fahrenheit2 = (TemperatureSum2*0.1125) + 32;  

 // Serial.print("Celcius2: ");
//Serial.println(celcius2);

  client.print("internal temp_1 in celcius2 is :");
  client.print(celcius2);
  client.println("<br />");
  delay(100);
  
  //--------------------------------------internal temp 2----------------------------------------------------------
  Wire.begin();
  Wire.requestFrom(tmp102Address3,2); 

  byte MSB3 = Wire.read();
  byte LSB3 = Wire.read();

  int TemperatureSum3 = ((MSB3 << 8) | LSB3) >> 4; //it's a 12bit int, using two's compliment for negative

  float celcius3 = TemperatureSum3*0.0625;
  //float fahrenheit3 = (TemperatureSum3*0.1125) + 32;  


  client.print("internal temp_2 in celcius3 is :");
  client.print(celcius3);
  client.println("<br />");
  
  
//-------------------------system conditions----------------------------------------------


 //only for 328
/*
  client.print("temp of core is :");
  client.print(readTemp());
  client.println("<br />");
  
  client.print("voltage on core is :");
  client.print(readVcc());
  client.println("<br />"); */

//------------------------external humidity, internal 1, internal 2----------------------------------

  client.print("external humidity is :");
  //client.print(map(analogRead(0),300,700,20,90));//----------------------here is calibration formula ...map(.........)  see arduino reference :)
  humidity_external = sht1x.readHumidity();
  client.println(humidity_external);
  client.println("<br />");
  

  client.print("internal humidity_1 is :");
  client.print(map(analogRead(1),0,1024,30,100));
  client.println("<br />");

 client.print("internal humidity_2 is :");
  client.print(map(analogRead(2),0,1024,30,100));
  client.println("<br />");

//-------------------------output of raw analog readings------------------------------------------------------
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            client.print("analog input ");
            //client.print(analogChannel);
            client.print(" is ");
            client.print(analogRead(analogChannel));
            client.println("<br />");
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}



















