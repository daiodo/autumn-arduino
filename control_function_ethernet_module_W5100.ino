
#include <SPI.h>
#include <Ethernet.h>

#define PIN_LIGHT 9

// This code is unique for each controller, you have to take the value of autumn
#define ID_ACTUATOR "actYDFGj7srI"

 // !!! this line should not contain spaces
//these terms should be changed, if changed json format
 const  char json_string_on[]  = "\"command\":{\"value\":true";
 const  char json_string_off[] = "\"command\":{\"value\":false";
 const  char json_string_id_actuator[] = "\"actuator\":\"" ID_ACTUATOR "\"";

//configure server arduino
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //mac adress arduibo

IPAddress ip(192, 168, 1, 111); //IPadress arduino

EthernetServer main_server(80);

//---------------------------------------------------

//global variables, NO change them
//namber compare byte string
volatile uint16_t compare_char_string_on = 0;
volatile uint16_t compare_char_string_off = 0;
volatile uint16_t compare_char_id_actuator = 0;

//---------------------------------------------------
void setup() {
 
  Serial.begin(115200);
  while (!Serial) {}

  pinMode(PIN_LIGHT, OUTPUT); //configure pin
  digitalWrite(PIN_LIGHT, LOW);
    
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  main_server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  }
  
//---------------------------------------------------

void loop() {

	listenServer();

}

//---------------------------------------------------
//This function compares the bytes received parcels
uint8_t json_compare(char data){

  if(data == ' '){return 0;}//Remove spaces

    //compare string_on
      if( compare_char_string_on < (sizeof(json_string_on)-1)){
        if(json_string_on[compare_char_string_on] == data){
          compare_char_string_on++;
        }
        else {compare_char_string_on = 0;}
     }
    //compare string_off
      if( compare_char_string_off < (sizeof(json_string_off)-1)){
        if(json_string_off[compare_char_string_off] == data){
          compare_char_string_off++;
        }
        else {compare_char_string_off = 0;}
     }

    //compare string_id_actuator
     if( compare_char_id_actuator < (sizeof(json_string_id_actuator)-1)){
        if(json_string_id_actuator[compare_char_id_actuator] == data){
          compare_char_id_actuator++;
        }
        else {compare_char_id_actuator = 0;}
     }


 if( (compare_char_id_actuator >= (sizeof(json_string_id_actuator)-1)) && (compare_char_string_on >= (sizeof(json_string_on)-1)) ){
          Serial.println("PIN_LIGHT_ON");
          digitalWrite(PIN_LIGHT, HIGH);
          compare_char_string_on = 0;
          compare_char_string_off = 0;
          compare_char_id_actuator = 0;
          return 1;
     
 }
  if( (compare_char_id_actuator >= (sizeof(json_string_id_actuator)-1)) && (compare_char_string_off >= (sizeof(json_string_off)-1)) ){
          Serial.println("PIN_LIGHT_OFF");
          digitalWrite(PIN_LIGHT, LOW);
          compare_char_string_on = 0;
          compare_char_string_off = 0;
          compare_char_id_actuator = 0;
          return 1;
     
 }

return 0;
     
}

//--------------------------------------------------------------

void listenServer(){
  // listen for incoming clients
  EthernetClient serv_client = main_server.available();
  if (serv_client) {
    Serial.println("new client");
    while (serv_client.connected()) {
      if (serv_client.available()) {
        char c = serv_client.read();
        Serial.write(c);
        //serv_client.print(c); //echo to client
        if(json_compare(c))  break;             
      }
    }

    serv_client.println("Server: arduino");
    
	// give the web browser time to receive the data
    delay(1);
    // close the connection:
    serv_client.stop();
    Serial.println("client disconnected");
    Ethernet.maintain();
 
 }
}
