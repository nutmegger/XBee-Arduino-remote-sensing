#include <XBee.h>
#include <SPI.h>
#include <Ethernet.h>

//Instantiate XBee object
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();

ModemStatusResponse msr = ModemStatusResponse();

//I could have coded this differently but I
//wanted to make sure the address was clear
int valarray[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
String xbee_id;
String datatype;
String deviceloc;
String PostData;
char n[2];       //this is for converting the ascii number to a letter
char c;
String convstr;  //this is for converting the ascii number to a letter
int reading;
int statusLed = 7;
int dataLed = 5;
int errorLed = 3;
int i = 0;



// Enter a MAC address and IP address for your controller below.
byte mac[] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0x02 };

// In case DHCP fails, we can 'tell' the router that we're using this IP Address
byte ip[] = { 192, 168, 0, 99 };

// In case DHCP fails, this is where to find the router
byte dlink[] = { 192, 168, 0, 1 };

// internet access via router
byte gateway[] = { 192, 168, 0, 1 }; 

//subnet mask
byte subnet[] = { 255, 255, 255, 0 }; 

// Server domain to connect to
char serverName [] = "www.hofstatter.org";

// Server port
int serverPort = 80;

//Page on server
char pageName[] = "/input2.php";


EthernetClient client;

IPAddress me;

IPAddress zdns ( 167, 206, 251, 130);

int totalCount = 0; 
// insure params is big enough to hold your variables
char params[64] = "SenderID=8169&Celsius1=26.70&Fahrenheit1=80.24&Humidity1=57.40";


// set this to the number of milliseconds delay
// this is 30 seconds
// #define delayMillis 1800000UL

//unsigned long thisMillis = 0;
//unsigned long lastMillis = 0;


//////////////////////

void setup() {
  
  pinMode(statusLed, OUTPUT);
  pinMode(dataLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  
  
//start Serial library
  Serial.begin(9600);      Serial.println("Start Serial");
  
  delay(1000);
  
  flashLed(statusLed, 1, 166);

Serial.print(F("Starting ethernet...")); 
// start the Ethernet connection:
    if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Ethernet.begin(mac, ip);
}
  
  // print your local IP address:
  Serial.println(Ethernet.localIP());

  delay(3000);
  
  flashLed(dataLed, 1, 166);
  Serial.println(F("Ready"));
  
// start xbee
  xbee.begin(9600);
  
  delay(3000);
  
  flashLed(errorLed, 1, 166);


}

//////////////////////////////////////////////////////////////////////

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
        
    digitalWrite(pin, LOW);
}

//////////////////////////////////////////////////////////////////////

byte postPage(char* domainBuffer,int thisPort,char* page,char* thisData)
{
  int inChar;
  char outBuf[128];

  Serial.print(F("Connecting..."));

  if(client.connect(domainBuffer,thisPort) == 1)
  {
    Serial.println(F("Connected"));

    // send the header
    //Serial.println("***Start HTTP Header***");
    sprintf(outBuf,"POST %s HTTP/1.1",page);
    client.println(outBuf);
    //Serial.println(outBuf);
    sprintf(outBuf,"Host: %s",domainBuffer);
    client.println(outBuf);
    //Serial.println(outBuf);
    client.println(F("Connection: close\r\nContent-Type: application/x-www-form-urlencoded"));
    sprintf(outBuf,"Content-Length: %u\r\n",strlen(thisData));
    client.println(outBuf);
    //Serial.println(outBuf);
    //Serial.println(thisData);
    //Serial.println(strlen(thisData));
    //Serial.println("***End of Header***");

    // send the body (variables)
    client.print(thisData);
      } 
      else
      {
        Serial.println(F("failed"));
        return 0;
      }

  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      connectLoop = 0;
    }

    delay(1);
    connectLoop++;
    if(connectLoop > 10000)
    {
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
  }

  Serial.println();
  Serial.println(F("Disconnecting."));
  client.stop();
  return 1;
}


void loop () {

      xbee.readPacket();     
      //Serial.println("Waiting for inbound API packet in loop()");
      if (xbee.getResponse().isAvailable()) {
        Serial.println("Inbound packet available");
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {     // got a zb rx packet
            Serial.println("Inbound packet received");
            xbee.getResponse().getZBRxResponse(rx);
            
            if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {        // the sender got an ACK
              Serial.println("Sender received acknowledgement of packet");
              flashLed(statusLed, 3, 166);
              
              } else {                                             // we got it (obviously) but sender didn't get an ACK
                Serial.println("Sender did not receive acknowledgement of packet");
                flashLed(errorLed, 3, 166);
                
              }

          //Fill the array with the payload data

          for (int whichbyte = 0; whichbyte < 9; whichbyte++) {
              
             int payloadbyte = rx.getData(whichbyte);
              
              valarray[whichbyte] = payloadbyte;              // Fills each element of the array with its respective payload byte
              //Serial.print("Payloadbyte: ");
              //Serial.println(payloadbyte);                    // Debugging what is being received from sensor XBee-Arduino  
            }

              String xbid = String(rx.getRemoteAddress16());
              Serial.print("Sender = ");
              Serial.print(xbid);
              valarray[9] = xbid.toInt();
              Serial.println();
              
              Serial.println("Reading array in Main function");
              //Celsius Read
                    Serial.print(valarray[0]);
                    Serial.print(": ");
                    int val = (valarray[1]*256)+valarray[2];
                    float cel = val/100.00;
                    char celBuff[6];
                    dtostrf(cel, 5, 2, celBuff);
                    Serial.println(celBuff);
              //Fahrenheit read
                    Serial.print(valarray[3]);
                    Serial.print(": ");
                    val = (valarray[4]*256)+valarray[5];
                    float fah = (val/100.00);
                    char fahBuff[6];
                    dtostrf(fah, 5, 2, fahBuff);
                    Serial.println(fahBuff);
               //Humidity read     
                    Serial.print(valarray[6]);
                    Serial.print(": ");
                    val = (valarray[7]*256)+valarray[8];
                    float hum = (val / 100.00);
                    char humBuff[6];
                    dtostrf(hum, 5, 2, humBuff);
                    Serial.println(humBuff);
                    Serial.println();
                  
                  // params must be url encoded.
                  //sprintf(params,"temp1=%i",totalCount); 
                  sprintf( params, "SenderID=%i&Celsius1=%s&Fahrenheit1=%s&Humidity1=%s", valarray[9], celBuff, fahBuff, humBuff );
                  Serial.print("Your formatted params string is: ");
                  Serial.println( params );
                  if(!postPage(serverName,serverPort,pageName,params)) Serial.print(F("Fail "));
                  else Serial.print(F("Pass "));
                  totalCount++;
                  Serial.println(totalCount,DEC);
              
 
           } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) 
             //   the local XBee sends this response on certain events, like association/dissociation
             {               
              xbee.getResponse().getModemStatusResponse(msr);
              
              if (msr.getStatus() == ASSOCIATED) {
                  //   yay this is great.  flash led
                  flashLed(statusLed, 5, 100);
                } 
                else if (msr.getStatus() == DISASSOCIATED) {
                  // this is awful.. flash led to show our discontent
                  flashLed(errorLed, 5, 100);
                  } 
                else {
                  // another status
                  flashLed(statusLed, 2, 250);
                  }
            } else {
            // not something we were expecting
            flashLed(errorLed, 7, 72);    
        } 
  
      }
  
}





