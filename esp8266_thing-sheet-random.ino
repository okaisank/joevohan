/*
// ----------------------------------------------------------------------------------------------
This is a program to learn and control variables of a random number for Thingspeak and Google Sheet.
It was designed to be run on an Esp8266 
Design by Yotesarun Bunpotsed 
// ----------------------------------------------------------------------------------------------
*/


#include <ThingSpeak.h>
#include <ESP8266WiFi.h>


//Google sheets
// Your WiFi credentials.
// Set password to "" for open networks.

char ssid[] = "Okaisank_2.4G";
char pass[] = "12345678";



String GAS_ID = "***********************"; //--> spreadsheet script ID

//Your Domain name with URL path or IP address with path
const char* host = "script.google.com"; // only google.com not https://google.com

// ----------------------------------------------------------------------------------------------
//ThingSpeak
String SERVER_IP = "api.thingspeak.com";

unsigned long myChannelNumber = 12345678;
const char* myWriteAPIKey     = "****************";
const char* myReadAPIKey      = "****************";

unsigned int field1 =1;
unsigned int field2 =2;


WiFiClient client;

int Temp, Humi;

// ----------------------------------------------------------------------------------------------
//#define DHT_PIN               D1
#define UPDATE_INTERVAL_HOUR  (0)
#define UPDATE_INTERVAL_MIN   (0)
#define UPDATE_INTERVAL_SEC   (30)

#define UPDATE_INTERVAL_MS    ( ((UPDATE_INTERVAL_HOUR*60*60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC ) * 1000 )

// ----------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------------
void update_google_sheet()
{
    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
    
    client.setInsecure(); // this is the magical line that makes everything work
    
    if (!client.connect(host, httpPort)) { //works!
      Serial.println("connection failed");
      return;
    }
       
    //----------------------------------------Processing data and sending data
    
    String url = "/macros/s/" + GAS_ID + "/exec?temperature=";
    int Temp = random(0,100);
    int Humi = random(0,100); 
    url += String(Temp);
    
    url += "&humidity=";
    url += String(Humi);
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
  
    Serial.println();
    Serial.println("closing connection");  
}

// ----------------------------------------------------------------------------------------------
void setup()
{
  // Debug console
 Serial.begin(9600);

//  // Digital output pin
 pinMode(LED_BUILTIN, OUTPUT);
//


//----------------------------------------Wait for connection
  Serial.print("Connecting");
  WiFi.begin(ssid, pass); //--> Connect to your WiFi router
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
    ThingSpeak.begin(client);

    Serial.println("");
    Serial.println("WiFi connected");
  
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP() );

    ThingSpeak.begin(client);
}
// ----------------------------------------------------------------------------------------------
unsigned long time_ms;
unsigned long time_1000_ms_buf;
unsigned long time_sheet_update_buf;
unsigned long time_dif;

void loop()
{
  time_ms = millis();
  time_dif = time_ms - time_1000_ms_buf;

  // Read and print serial data every 1 sec
  if ( time_dif >= 1000 ) // 1sec 
  {
    time_1000_ms_buf = time_ms;

      int Temp = random(0,100);
      int Humi = random(0,50); 
      delay(1000);

    Serial.print("Temperature: " + String(Temp) + " C");
    Serial.print("\t");
    Serial.println("Humidity: " + String(Humi) + " %");

    //Channel update LED On , Close connection LED Off
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    
    thing_speak();
    ThingSpeak.setField(1, Temp);
    ThingSpeak.setField(2, Humi);
  }
  

  // Update data to google sheet in specific period
  time_ms = millis();
  time_dif = time_ms - time_sheet_update_buf;  
  if ( time_dif >= UPDATE_INTERVAL_MS ) // Specific period
  {
    time_sheet_update_buf = time_ms;
    update_google_sheet();
  }
 
}

void thing_speak(){
  int Temp = random(0,100);
  int Humi = random(0,50); 
Serial.println("T: " + String(Temp)+", "+"H: " + String(Humi));
    
ThingSpeak.setField(1,Temp);
ThingSpeak.setField(2,Humi);

int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
   if(x == 200){
    Serial.println("Channel updated successful");
   }
   else{
    Serial.println("Problem updateing channel");
   }

delay(20000);
}
