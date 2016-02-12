
//#include <SoftwareSerial.h>
#include <String.h>
#include <Wire.h>
#include <DHT.h> // this lib need addition in folder Arduino IDE

#define NAME_NET_WI_FI "my_wifi" // name wi-fi network
#define PASSWD_WI_FI "my_passwd" //password wi-fi network
#define IP_WI_FI "192.168.77.177" // ip adress server arduino
#define GATEWAY_WI_FI "192.168.77.7" // geteway
#define MASK_WI_FI "255.255.255.0" //mask

#define REMOTE_SERVER "dev.autumnapp.com"    //

//----------------------------------------------------------------------

//data send
#define TIME_PERIOD_SEND_DATA 30000
//#define TIME_WAIT_ANSWER_SERVER 2000

#define JSON_STR_1 " {\"data\":{\"type\": \"SensorData\",\"id\": null,\"attributes\": {\"value\": \""
#define JSON_STR_2 "\"},\"relationships\":{\"sensor\":{\"data\":{\"type\": \"Sensor\",\"id\": \""
#define JSON_STR_3 "\"}}}}}"

#define ADRR_SENSOR_1  "/api/external/v1/sensors/senE3Upg1VVV/sensordata/"
#define ADRR_SENSOR_2  "/api/external/v1/sensors/senPXQq8QlIz/sensordata/"
#define USR_PASS "YW5keS5jbGFya0BkYWlvZG8uY29tOnA0c3N3MHJk"

//name seonsors
#define SENSOR_1 "senE3Upg1VVV"
#define SENSOR_2 "senPXQq8QlIz"

//----------------------------------------------------------------------

// DHT22 config
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN 3    // pin DHT22
DHT dht(DHTPIN, DHTTYPE);

float DHT22_Humidity = 0;
float DHT22_temperature = 0;



//-----------------------------------------------------------------------

#define PIN_LIGHT 9

//namber compare byte string
volatile uint16_t compare_char_string_1 = 0;
volatile uint16_t compare_char_string_2 = 0;

// !!! this line should not contain spaces
 char json_string_on[]  = "{\"data\":{\"type\":\"ActuatorCommand\",\"id\":\"actyTO8Y9P8B\",\"attributes\":{\"actuator\":\"actyTO8Y9P8B\",\"command\":{\"value\":true}}}}";
 char json_string_off[] = "{\"data\":{\"type\":\"ActuatorCommand\",\"id\":\"actyTO8Y9P8B\",\"attributes\":{\"actuator\":\"actyTO8Y9P8B\",\"command\":{\"value\":false}}}}";

//----------------------------------------------------------------------

 int8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout);

 uint32_t last_time_loop =0;
 
void setup(){

    pinMode(PIN_LIGHT, OUTPUT);
    digitalWrite(PIN_LIGHT, LOW);
          
    delay(500); //delay for inicializate wi-fi module

    Serial.begin(115200); //debug serial port WI_FI MODULE

    while (!Serial) {}

    delay(3000); //delay for inicializate sensor DHT22

  
 sendATcommand("AT", "OK", 1000);// means Station mode
 sendATcommand("AT+CWQAP", "OK", 1000); 
 sendATcommand("AT+CIPCLOSE=1","CLOSED" ,1000);
// sendATcommand("AT+CWLAP", "OK", 10000);
 sendATcommand("AT+CWJAP=\"" NAME_NET_WI_FI "\",\"" PASSWD_WI_FI "\"", "WIFI CONNECTED", 5000); // connet to wi-fi
 sendATcommand("AT+CIPSTA=\"" IP_WI_FI "\",\"" GATEWAY_WI_FI "\",\"" MASK_WI_FI "\"", "OK", 3000); //manual IP, gateway and mask
 sendATcommand("AT+CIPMUX=1", "OK", 1000);
 sendATcommand("AT+CIPSERVER=1,80", "OK", 1000);
 delay(2000); 

 }
   
  //------------------------------------------------------------------------------ 
void loop(){

  if(millis()>last_time_loop){
    last_time_loop = millis();
    last_time_loop += TIME_PERIOD_SEND_DATA;
    sensor_read();
    send_data_sensor();     
  }  
  listen_server(); 
}

//------------------------------------------------------------------------------

void send_data_sensor(){
   
//start the TCP connection:
 sendATcommand("AT+CIPSTART=1,\"TCP\",\"" REMOTE_SERVER "\",80","CONNECT", 1000);

uint16_t byte_string_json = ( (sizeof(JSON_STR_1)-1) + (sizeof(JSON_STR_2)-1) + (sizeof(JSON_STR_3)-1) + (sizeof(SENSOR_1)-1) + String(DHT22_temperature).length() );
uint16_t byte_string_all = ((sizeof("POST " ADRR_SENSOR_1 " HTTP/1.1""\r\n")-1) + (sizeof("Host: " REMOTE_SERVER "\r\n")-1) + (sizeof("Authorization: Basic " USR_PASS "\r\n")-1) + (sizeof("User-Agent: Arduino_DHT22" "\r\n" )-1) + (sizeof("Accept: */*" "\r\n" )-1) + (sizeof("Content-Type: application/vnd.api+json" "\r\n" )-1) + (sizeof("Content-Length: ")-1) + (String(byte_string_json).length()) + (sizeof("\r\n" "\r\n")-1) + (sizeof("\r\n")-1) +  byte_string_json );


    Serial.print(F("AT+CIPSEND=1,"));
    Serial.print((byte_string_all),DEC);
    sendATcommand("",">", 1000);  
    
    Serial.print(F("POST " ADRR_SENSOR_1 " HTTP/1.1""\r\n")); //adress sensor 
    Serial.print(F("Host: " REMOTE_SERVER "\r\n"));
    Serial.print(F("Authorization: Basic " USR_PASS "\r\n"));    
    Serial.print(F("User-Agent: Arduino_DHT22" "\r\n" ));
    Serial.print(F("Accept: */*" "\r\n" ));  
    Serial.print(F("Content-Type: application/vnd.api+json" "\r\n" )); 
    Serial.print(F("Content-Length: ")); 
    Serial.print(byte_string_json, DEC);    
    Serial.print(F("\r\n" "\r\n"));
    Serial.print(F(JSON_STR_1)); 
    Serial.print(DHT22_temperature);  // value sensor
    Serial.print(F(JSON_STR_2));
    Serial.print(F(SENSOR_1));    //name sensor  
    Serial.println(F(JSON_STR_3));  
    Serial.print(F("\r\n"));
    sendATcommand("","SEND OK", 3000);  
    
//sensor2
 byte_string_json = ( (sizeof(JSON_STR_1)-1) + (sizeof(JSON_STR_2)-1) + (sizeof(JSON_STR_3)-1) + (sizeof(SENSOR_2)-1) + String(DHT22_Humidity).length() );
 byte_string_all = ((sizeof("POST " ADRR_SENSOR_2 " HTTP/1.1""\r\n")-1) + (sizeof("Host: " REMOTE_SERVER "\r\n")-1) + (sizeof("Authorization: Basic " USR_PASS "\r\n")-1) + (sizeof("User-Agent: Arduino_DHT22" "\r\n" )-1) + (sizeof("Accept: */*" "\r\n" )-1) + (sizeof("Content-Type: application/vnd.api+json" "\r\n" )-1) + (sizeof("Content-Length: ")-1) + (String(byte_string_json).length()) + (sizeof("\r\n" "\r\n")-1) + (sizeof("\r\n")-1) +  byte_string_json );

    Serial.print(F("AT+CIPSEND=1,"));
    Serial.print((byte_string_all),DEC);
    sendATcommand("",">", 1000);  
    
    Serial.print(F("POST " ADRR_SENSOR_2 " HTTP/1.1""\r\n")); //adress sensor 
    Serial.print(F("Host: " REMOTE_SERVER "\r\n"));
    Serial.print(F("Authorization: Basic " USR_PASS "\r\n"));    
    Serial.print(F("User-Agent: Arduino_DHT22" "\r\n" ));
    Serial.print(F("Accept: */*" "\r\n" ));  
    Serial.print(F("Content-Type: application/vnd.api+json" "\r\n" )); 
    Serial.print(F("Content-Length: ")); 
    Serial.print(byte_string_json, DEC);    
    Serial.print(F("\r\n" "\r\n"));
    Serial.print(F(JSON_STR_1)); 
    Serial.print(DHT22_Humidity);  // value sensor
    Serial.print(F(JSON_STR_2));
    Serial.print(F(SENSOR_2));    //name sensor  
    Serial.println(F(JSON_STR_3));  
    Serial.print(F("\r\n"));    
    
    sendATcommand("","SEND OK", 3000);  
    sendATcommand("AT+CIPCLOSE=1","OK" ,1000);


}

//-----------------------------------------------------------------------------------
int8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout){

    uint8_t i=0,  answer=0;
    char response[500];

    memset(response, '\0', 500);    // Initialize the string

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.print(ATcommand);    // Send the AT command 
    Serial.print(F("\r\n"));    // Send the AT command 
    
    uint32_t previous_time = millis();
    previous_time += timeout;
    
    // this loop waits for the answer
    do{
        if(Serial.available() != 0){    
            response[i] = Serial.read();
            i++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL) { answer = 1; }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && (millis() < previous_time));    

    delay(100);
    
    return answer;
}

//----------------------------------------------------------------
void sensor_read(){
    DHT22_Humidity = dht.readHumidity();
    DHT22_temperature = dht.readTemperature();   
}

//----------------------------------------------------------------
void listen_server(){
  while( Serial.available() > 0){
    
      char c = Serial.read();   

      if(compare_string_1(c))  break;
      if(compare_string_2(c))  break;        
    
  }  
}

//---------------------------------------------------

uint8_t compare_string_1(char data){

  uint8_t k = 0;
  if(!(data == ' ')){

    if(json_string_on[compare_char_string_1] == data){
       compare_char_string_1++;
       if( compare_char_string_1 >= (sizeof(json_string_on)-1)){
          answer_for_server("PIN_LIGHT_ON");
          digitalWrite(PIN_LIGHT, HIGH);
          k=1;
          compare_char_string_1 = 0;
        }
     }
     else {compare_char_string_1 = 0;}
  }
     return k;
}
//--------------------------------------
  uint8_t compare_string_2(char data){

  uint8_t k = 0;
  if(!(data == ' ')){

    if(json_string_off[compare_char_string_2] == data){
       compare_char_string_2++;
       if( compare_char_string_2 >= (sizeof(json_string_off)-1)){
          answer_for_server("PIN_LIGHT_OFF");
          digitalWrite(PIN_LIGHT, LOW);
          k=1;
          compare_char_string_2 = 0;
        }
     }
     else {compare_char_string_2 = 0;}
  }
     return k;
  }
//-------------------------------------------------------------

void answer_for_server(const char* answer){
   
  uint16_t byte_string_all = ((sizeof("Server: arduino" "\r\n")-1)  + (String(answer).length()) + (sizeof("\r\n")-1));

    Serial.print(F("AT+CIPSEND=0,"));
    Serial.print((byte_string_all),DEC);
    sendATcommand("",">", 1000);  
    
    Serial.print(F("Server: arduino" "\r\n"));    
    Serial.print(answer);  
    Serial.print(F("\r\n"));
    
    sendATcommand("","SEND OK", 3000);  
    sendATcommand("AT+CIPCLOSE=1","OK" ,1000);

}
