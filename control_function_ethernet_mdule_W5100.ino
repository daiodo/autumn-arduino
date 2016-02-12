
#include <SPI.h>
#include <Ethernet.h>

#define PIN_LIGHT 9

//namber compare byte string
volatile uint16_t compare_char_string_1 = 0;
volatile uint16_t compare_char_string_2 = 0;

// !!! this line should not contain spaces
 char json_string_on[]  = "{\"data\":{\"type\":\"ActuatorCommand\",\"id\":\"actyTO8Y9P8B\",\"attributes\":{\"actuator\":\"actyTO8Y9P8B\",\"command\":{\"value\":true}}}}";
 char json_string_off[] = "{\"data\":{\"type\":\"ActuatorCommand\",\"id\":\"actyTO8Y9P8B\",\"attributes\":{\"actuator\":\"actyTO8Y9P8B\",\"command\":{\"value\":false}}}}";

 
//configure server arduino

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //mac adress arduibo

IPAddress ip(192, 168, 77, 178); //IPadress arduino

EthernetServer main_server(80);

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

uint8_t compare_string_1(char data){

  uint8_t k = 0;
  if(!(data == ' ')){

    if(json_string_on[compare_char_string_1] == data){
       compare_char_string_1++;
       if( compare_char_string_1 >= (sizeof(json_string_on)-1)){
          Serial.println("PIN_LIGHT_ON");
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
          Serial.println("PIN_LIGHT_OFF");
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
        if(compare_string_1(c))  break;
        if(compare_string_2(c))  break;        
      }
    }

    serv_client.println("Server: arduino");

    compare_char_string_1 = 0;
	  compare_char_string_2 = 0;
    
	// give the web browser time to receive the data
    delay(1);
    // close the connection:
    serv_client.stop();
    Serial.println("client disconnected");
    Ethernet.maintain();
 
 }
}
