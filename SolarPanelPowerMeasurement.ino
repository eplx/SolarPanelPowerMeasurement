/*
   Solar power measurement using ThingSpeaks - Version: 1.00 - February 2019
   Author: Esteban Pizzini - esteban.pizzini@gmail.com
   Purpose: Measure power from a solar panel. 

   Release notes:
   1.00 - Initial version
*/
#include <Wire.h>
#include <Adafruit_INA219.h> // You will need to download this library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wifi Settings
const char* ssid = "YOUR-SSI";
const char* pwd = "WIFI-PASSWORD";

/* ThingSpeaks */
int ThingSpeaks_WAIT = 15000;//15500; // if you are using free ThingSpeaks version you need to wait about 15 seconds between updates.
char ThingSpeaks_URL[100] = "http://api.thingspeak.com/update?api_key=";
char ThingSpeaks_KEY[100] = "THINGSPEAK-KEY"; // Thingspeak key

Adafruit_INA219 sensor219; // instance of INA219
float mAh = 0;
unsigned long lastread = 0; // used to calculate Ah
unsigned long tick;         // current read time - last read

void setup(void) 
{
      
  Serial.begin(115200);    
  sensor219.begin();
  Serial.print("Connecting to wifi ");
  Serial.println(ssid);
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("."); //progress until Wifi connected
  }
    Serial.println("");
    Serial.println("Wifi connected");
    Serial.print("This is your ip address: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop(void) 
{
  float busVoltage = 0;   // voltage
  float current = 0;      // milli amps
  float power = 0;        // power
  

  unsigned long newtime;

  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA() / 1000;
  power = busVoltage * (current); // Calculate the Power

  newtime = millis();
  tick = newtime - lastread;
  mAh += (current * tick)/3600000.0;
  
  Serial.print("Bus Voltage:   "); 
  Serial.print(busVoltage); 
  Serial.println(" V");  
  
  Serial.print("Current:       "); 
  Serial.print(current); 
  Serial.println(" mA");
  
  Serial.print("Power:         "); 
  Serial.print(power); 
  Serial.println(" W");  

  Serial.print("Ah:         "); 
  Serial.print(mAh); 
  Serial.println(" mA");  

  
  Serial.println("");  

  updateThingSpeaks(busVoltage,current,power);
  delay(350000);

}

void updateThingSpeaks(float voltage, float current, float power) {
 /* This function will update sensors data into www.ThingSpeaks.com 
  *  Field 1 -> Voltage
  *  Field 2 -> Current
  *  Field 3 -> Power
*/
 char tempValue[10];
 char temp[50]; // used to format URL
 char url[255]; // URL to be used to call ThingSpeaks API
 HTTPClient http2;  //Declare an object of class HTTPClient

 Serial.println(voltage);
 // Update Field 1 - voltage
 dtostrf(voltage, 2, 2, tempValue);
 sprintf(temp,"&field1=%s", tempValue);
 strcpy(url,ThingSpeaks_URL);
 strcat(url,ThingSpeaks_KEY);
 strcat(url,temp);
 http2.begin(url);
 int httpCode = http2.GET();
 //Serial.println(url);
 Serial.print("Voltage update");  
 (httpCode=200) ? Serial.println("..Completed!") : Serial.println("..Failed!");
 http2.end();
 delay(ThingSpeaks_WAIT);
 
// Update field 2 - current
 dtostrf(current, 2, 2, tempValue);
 //sprintf(tempValue,"%s", tempValue);
 sprintf(temp,"&field2=%s", tempValue);
 strcpy(url,ThingSpeaks_URL);
 strcat(url,ThingSpeaks_KEY);
 strcat(url,temp);
 //Serial.print(url);
 http2.begin(url);
 httpCode = http2.GET();                                                                  //Send the request
 Serial.print("Current");
 (httpCode=200) ? Serial.println("..Completed!") : Serial.println("..Failed!");
 http2.end();
 delay(ThingSpeaks_WAIT);

// Update field 3 - power
 dtostrf(power, 2, 2, tempValue);
 
 //sprintf(tempValue,"%s", tempValue);
 sprintf(temp,"&field3=%s", tempValue);
 strcpy(url,ThingSpeaks_URL);
 strcat(url,ThingSpeaks_KEY);
 strcat(url,temp);
 //Serial.print(url);
 http2.begin(url); 
 httpCode = http2.GET();
 Serial.print("Power");
 (httpCode=200) ? Serial.println("..Completed!") : Serial.println("..Failed!");
 http2.end(); 
 delay(ThingSpeaks_WAIT);
}


