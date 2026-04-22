#define ESP8266 Serial3

//Network Information
String SSID = "";
String PASSWORD = "";
String SERVER_IP = "";
int PORT = 9999;

//Relay Pins
int RelayPin[] = {39, 41, 43, 45, 47, 49, 51, 53};
int pinCount = 8;

//Voltage Sensor
#define Volt_Sen_1 A3
#define Volt_Sen_2 A4
#define Volt_Sen_3 A5

float C1 = 0.0;
float C2 = 0.0;
float C3 = 0.0;
float V1 = 0.0;
float V2 = 0.0;
float V3 = 0.0;

float in_voltage = 0.0;
float ref_voltage = 5.0;

int R1 = 1000;
int R2 = 1000;
int R3 = 1000;
//--------------------------

boolean FAIL_8266 = false;

void setup() {
  
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    pinMode(RelayPin[thisPin], OUTPUT);
  }
  delay(1000);

  do{

    //Setup serial connections
    Serial.begin(115200);
    ESP8266.begin(115200);
  
    //Wait Serial Monitor to start
    while(!Serial);
    Serial.println("Entering Setup...");

    //Check if ESP8266 exists
    ESP8266.println("AT");
    delay(1000);
    Serial.println();
    if(ESP8266.find("OK"))
    {
      Serial.println("OK");

      //Enable station mode
      ESP8266.println("AT+CWMODE=2");
      delay(2000);
      clearBuffer();

      /**
      //Connect to WiFi
      do{
        Serial.println("Connecting to WiFi...");
        String cmd="AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
        ESP8266.println(cmd);
      }while(!isReady(3));

      String DEVICE_IP = getIPAddress();
      Serial.println("Connected as " + DEVICE_IP);
      */
      
      //Setup Access Point
      do{
        Serial.println("Setting up Access Point");
        String cmd = "AT+CWSAP=\"" + SSID + "\",\"" + PASSWORD + "\",5,3";
        ESP8266.println(cmd);
      }while(!isReady(3));

      Serial.println("Access point ready: " + SSID + ", " + PASSWORD);
      
      delay(1000);

      //Connect to TCP Server
      do
      {
        Serial.println("Connecting to Server...");
        String cmd="AT+CIPSTART=\"TCP\",\"" + SERVER_IP + "\"," + PORT;
        ESP8266.println(cmd);
      }while(!isReady(3));
      Serial.println("Established connection with " + SERVER_IP + " on port " + PORT);
      
      delay(1000);
      
      //Enable WiFi passthrough transmission
      do
      {
        Serial.println("Setting transmission mode...");
        String cmd="AT+CIPMODE=1"; //WiFi Passthrough mode
        ESP8266.println(cmd);
      }while(!isReady(3));


    }else{
      Serial.println("ERROR - NO DEVICE FOUND");
      delay(500);
      FAIL_8266 = true;
    }
  }while(FAIL_8266);
  
  Serial.println("SETUP DONE");
  delay(1000);

  ESP8266.println("AT+CIPSEND");
  while(!ESP8266.find(">"));

}

void loop() {

 /**double v1 = random(240);
  double c1 = random(50);
  double v2 = random(240);
  double c2 = random(50);
  double v3 = random(240);
  double c3 = random(50);**/

   
  //Serial.print(response);
  String response = ESP8266.readString(); 


  //Busbar 1
  if(response == "ON-1\n"){
    digitalWrite(RelayPin[0],HIGH);
  }
  if(response == "OFF-1\n"){
    digitalWrite(RelayPin[0],LOW);
  }

  //Busbar 2
  if(response == "ON-2\n"){
    digitalWrite(RelayPin[1],HIGH);
  }
  if(response == "OFF-2\n"){
    digitalWrite(RelayPin[1],LOW);
  }

  //Load 1
  if(response == "ON-3\n"){
    digitalWrite(RelayPin[2],HIGH);
  }
  if(response == "OFF-3\n"){
    digitalWrite(RelayPin[2],LOW);
  }

  //Load 2
  if(response == "ON-4\n"){
    digitalWrite(RelayPin[3],HIGH);
    
  }
  if(response == "OFF-4\n"){
    digitalWrite(RelayPin[3],LOW);
  }

  //High or Low
  if(response == "ON-5\n"){
    digitalWrite(RelayPin[4],HIGH);
    R2 = 2000;
  }
  if(response == "OFF-5\n"){
    digitalWrite(RelayPin[4],LOW);
    R2 = 1000;
  }

  //Battery
  if(response == "ON-6\n"){
    digitalWrite(RelayPin[5],HIGH);
  }
  if(response == "OFF-6\n"){
    digitalWrite(RelayPin[5],LOW);
  }

  //7
  if(response == "ON-7\n"){
    digitalWrite(RelayPin[6],HIGH);
  }
  if(response == "OFF-7\n"){
    digitalWrite(RelayPin[6],LOW);
  }

  //8
  if(response == "ON-8\n"){
    digitalWrite(RelayPin[7],HIGH);
  }
  if(response == "OFF-8\n"){
    digitalWrite(RelayPin[7],LOW);
  }

  // Read the Analog Input

  V1 = analogRead(Volt_Sen_1)/100;

  V2 = analogRead(Volt_Sen_2)/100;

  V3 = analogRead(Volt_Sen_3)/100;

  
  C1 = (V1*100)/R1;
  C2 = (V2*100)/R2;
  C3 = (V3*100)/R3;


  String message = String(V1) + "," + String(C1) + "," + String(V2) + "," + String(C2) + "," + String(V3) + "," + String(C3);

  ESP8266.println(message);

  
}

/**
  Gets the device IP Address if connected to an Access Point
*/
String getIPAddress(){
  clearBuffer();
  ESP8266.println("AT+CIFSR");
  delay(1000);
  
  String response = "";

  while (ESP8266.available() > 0) {
    String a = ESP8266.readString();
    response = response + a;
  }
  
  int start = response.indexOf("\"") + 1;
  int end = response.indexOf("\"",start);
  String espIP = response.substring(start,end);
  return espIP;
}


/**
  Checks if ESP8266 responds with 'OK' after recieving a command.
  Command refreshes every 'n' seconds denoted by the 'timeout' variable
*/
boolean isReady(int timeout)
{
  do{
    Serial.println("waiting...");
    delay(1000);
    if(ESP8266.find("OK"))
    {
      return true;
    }

  }while((timeout--)>0);
  Serial.println("ERROR - RETRYING");
  return false;
}

void clearBuffer()
{
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
  Serial.println();
}

