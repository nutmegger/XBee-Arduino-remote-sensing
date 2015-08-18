#include <DHT.h>
#include <SPI.h>
#include <XBee.h>
//#include <DHT22.h>
//#include <SoftwareSerial.h>




// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
// #define DHT22_PIN 7 - old!
#define DHTPIN 7

#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Setup a DHT22 instance
//DHT22 myDHT22(DHT22_PIN); - old!
DHT dht(DHTPIN, DHTTYPE);

//Instantiate xbee object
XBee xbee = XBee();


float ftempC, ftempF, fRelH;
int tempC, tempF, RelH;

// set this to the number of milliseconds delay
// this is 30 minutes
#define delayMillis 1800000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;


//////////////////////

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  
  dht.begin();
  
  xbee.begin(9600);

}

void transmit_data(int tempC, int tempF, int RelH) {

//Serial.println("Received values from calling function C:" & tempC & " F:" &tempF&" Hum:" & RelH);
Serial.print("Received values from calling function C:");
Serial.print(tempC);
Serial.print(" F:");
Serial.print(tempF);
Serial.print(" RelHum:");
Serial.println(RelH);



Serial.println("Defining array...");
  uint8_t payload[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
Serial.println("Setting address...");
  // SH + SL Address of receiving XBee
  // Sun is the coord.  DL = 40 3A A0 18
  // Moon is a node     DL = 40 3A A0 16
  // Mars is a node     DL = 40 64 F1 EC
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x403AA018); //Sun
Serial.println("Create ZBTxRequest object...");
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
Serial.println("Create ZBTxStatusResponse object...");
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

Serial.println("Fill array...");
  payload[0] = 67 & 0xff;
  payload[1] = tempC >> 8 & 0xff;
  payload[2] = tempC & 0xff;
  payload[3] = 70 & 0xff;
  payload[4] = tempF >> 8 & 0xff;
  payload[5] = tempF & 0xff;
  payload[6] = 72 & 0xff;
  payload[7] = RelH >> 8 & 0xff;
  payload[8] = RelH & 0xff;
Serial.println("Send Tx...");
  xbee.send(zbTx);
  Serial.println();
  Serial.println("read payload...");
  Serial.println(payload[0]& 0xff);
  Serial.println(payload[1]& 0xff);
  Serial.println(payload[2]& 0xff);
  Serial.println(payload[3]& 0xff);
  Serial.println(payload[4]& 0xff);
  Serial.println(payload[5]& 0xff);
  Serial.println(payload[6]& 0xff);
  Serial.println(payload[7]& 0xff);
  Serial.println(payload[8]& 0xff);

  
  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
    // got a response!

    // should be a znet tx status               
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.print("Packet received successfully");
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        Serial.println("Packet not acknowledged");
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("local XBee did not provide a timely TX Status Response -- should not happen");
  }

  delay(1000);
}


void sensor_fetch(){

  delay(2000);
//  Serial.print("Requesting data...");

// Read temperature as Celsius
float ftempC = dht.readTemperature();
 tempC = ftempC*100;

// Read temperature as Fahrenheit
float ftempF = dht.readTemperature(true);
 tempF = ftempF*100;

float fRelH = dht.readHumidity();
 RelH = fRelH*100;
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(fRelH) || isnan(ftempC) || isnan(ftempF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(ftempF, fRelH);

  Serial.print("Humidity: "); 
  Serial.print(fRelH);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(ftempC);
  Serial.print(" *C\t ");
  Serial.print(ftempF);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");

}

void loop(void) {
  
    thisMillis = millis();
    
    sensor_fetch();
    
      //only send readings every 30 minutes    
      if(thisMillis - lastMillis > delayMillis)  {
    
         lastMillis = thisMillis;

         transmit_data(tempC, tempF, RelH);
        
       }
       
}


