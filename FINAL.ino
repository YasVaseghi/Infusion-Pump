//Manual and Automatic Infusion Pump

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Keypad.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define ROOT0_MENU_CNT 2
#define ROOT_MENU_CNT 4
#define SUB_MENU1_CNT 4
#define SUB_MENU2_CNT 4
#define stepPin 2
#define dirPin 15
#define SDA_PIN 21
#define SCL_PIN 18
#define limit_1 14
#define limit_2 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const byte ROWS = 1; 
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},};
byte rowPins[ROWS] = {32}; 
byte colPins[COLS] = {26,25,33}; 
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
enum pageType {ROOT_MENU0, ROOT_MENU_AUTO, ROOT_MENU, SUB_MENU1, SUB_MENU2, SUB_MENU3, SUB_MENU4};
enum pageType currPage = ROOT_MENU0;
uint8_t root_Pos = 1;
uint8_t root0_Pos = 1;
double volume = 0;
double rate = 0;
const char* ssid = "YAS";
const char* password = "hehehehe";
const char* server = "bsnlab.ir";
const int port = 80;
const String endpointLogin = "/KNT/APPlogin";
const String endpointIndex = "/KNT/InjectionDevice";
const char* patient_ID = "637de530ca47947b2796b351";
float vol;
float batteryVoltage;
int batteryPercentage;



void setup() {
 Serial.begin(115200);
 Wire.begin(SDA_PIN, SCL_PIN);
 pinMode(stepPin,OUTPUT);
 pinMode(dirPin,OUTPUT);
  pinMode(limit_1,INPUT);
 pinMode(limit_2,INPUT);

 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();          
   
}

void loop() 
{
 
  
  
//  delay(500);
  
    switch (currPage)
    {

    case ROOT_MENU0:
    page_RootMenu0();
    break;

    case ROOT_MENU_AUTO:
    page_RootMenuAuto();
    break; 
    
    case ROOT_MENU:
    page_RootMenu();
    break;
    
    case SUB_MENU1:
    page_SubMenu1();
    break;
    
    case SUB_MENU2:
    page_SubMenu2();
    break;

    case SUB_MENU3:
    page_SubMenu3();
    break;

    case SUB_MENU4:
    page_SubMenu4();
    break;
      }

      

  
}

void page_RootMenu0(void){
  
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
  while (true){
  
yield(); 
    loopStartMs = millis();

    if (updateDisplay){
      updateDisplay = false;
      display.clearDisplay();
      batteryVoltage = readBatteryVoltage();
      batteryPercentage = mapFloat(batteryVoltage, 0, 3.3, 0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(103, 0);
  display.print(batteryPercentage);
  display.print("%");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(F("Select Mode"));

      display.setTextSize(1);
      display.setCursor(25, 20);
      printSelected(1, root0_Pos); display.println(F(" Smart"));
      

      display.setCursor(25, 30);
      printSelected(2, root0_Pos); display.println(F(" Manual"));

      display.display();
      
    }
    char keypressed = keypad.getKey();
    if (keypressed != NO_KEY){
    switch (keypressed){
    
      case '1':
      if (root0_Pos == 1) {
        root0_Pos = ROOT0_MENU_CNT;
      } else  {
      root0_Pos--;}
      updateDisplay = true;
      break;

      case '2':
      if (root0_Pos == ROOT0_MENU_CNT) {
        root0_Pos = 1;
      } else  {
      root0_Pos++;}
      updateDisplay = true;
      break;

      case '3':
      switch (root0_Pos){
        case 1: currPage = ROOT_MENU_AUTO;
        return;
        case 2: currPage = ROOT_MENU;
        return;
      }

    

    }
  }
    
    while (millis() - loopStartMs < 25) {delay(2);}
    
}

}

void page_RootMenuAuto(void){
  vol = readBatteryVoltage();
  if(vol>2.6){
  WiFi.begin(ssid, password);
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
  while (true){

    loopStartMs = millis();

    if (updateDisplay){
      
      updateDisplay = false;
      display.clearDisplay();           
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(25, 0);
      display.println(F("Smart Infusion"));
      display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 10);
    display.println(F("Connecting..."));
    display.display();     
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  if(WiFi.status() == WL_CONNECTED){
    display.clearDisplay();           
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 0);
    display.println(F("Smart Infusion"));
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 10);
    display.println(F("Connected and Infusing"));
  }
      display.display();
      
  }
 // API CONNECTION
   WiFiClient client;
   HTTPClient http;
  if (!client.connect(server, port)) {
    Serial.println("Connection failed");
    return;
  }
  
  //LOGIN API CONNECTION
  
  String dataLogin = "email=INSERT_EMAIL&password=INSERT_PASSWORD&app_type=100";
  String requestLogin = "POST " + endpointLogin + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Content-Length: " + String(dataLogin.length()) + "\r\n\r\n" +
                   dataLogin;

  client.print(requestLogin);
  delay(1000);

  String responseLogin = "";
  while (client.available()) {
    yield(); 
    char c = client.read();
    responseLogin += c;
   
  }


  int bodyIndexLogin = responseLogin.indexOf("\r\n\r\n") + 4;
  // Extract the response body
  String bodyLogin = responseLogin.substring(bodyIndexLogin);


  DynamicJsonDocument doc(1024);
  deserializeJson(doc, bodyLogin);

  String firstName = doc["first_name"].as<String>();
  String lastName = doc["last_name"].as<String>();
  String institute = doc["institute"].as<String>();
  String token = doc["token"].as<String>();
 
  Serial.println("First name: " + firstName);
  Serial.println("Last name: " + lastName);
  Serial.println("Institute: " + institute);
  Serial.println("Token: " + token);

  
  //INDEX API CONNECTION

  
  String dataIndex = "patient_ID=637de530ca47947b2796b351";

  String requestIndex = "POST " + endpointIndex + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Content-Length: " + String(dataIndex.length()) + "\r\n" +
                   "Authorization: " + token + "\r\n\r\n" +
                   dataIndex;
                  

  client.print(requestIndex);
  delay(1000);

  String responseIndex = "";
  while (client.available()) {
    yield(); 
    char d = client.read();
    responseIndex += d;
  }
  int bodyIndexIndex = responseIndex.indexOf("\r\n\r\n") + 4;

  String bodyIndex = responseIndex.substring(bodyIndexIndex);


  DynamicJsonDocument doc2(1024);
  deserializeJson(doc2, bodyIndex);


  float index = doc2["INDEX"].as<float>();
  String token2 = doc2["token"].as<String>();

  // Print the extracted values
  Serial.println("Index: " + String(index));
  Serial.println("Token: " + token2);

 //Dose calculation
  uint32_t t_auto = 60;  //auto infusion happens every 16 hours
  double n1_auto = (10.000000*index)/(M_PI*pow(0.9525, 2)*0.001800);  //number of steps required
  double n_auto = round(n1_auto);
  uint32_t td_auto = (t_auto/n_auto)*(1000000);  //required delay between each step
  uint32_t td2_auto = td_auto/2.000000;
  boolean f=false;
 //stepper motor control
    digitalWrite(dirPin, HIGH); //sets the direction of the motor    //low for forward, high for retraction
    for(int x_auto=0;x_auto<n_auto;x_auto++)
    {
      if(digitalRead(limit_1) == LOW)
      {
        yield();
        digitalWrite(stepPin, HIGH);    // Trigger a step
        delayMicroseconds(td2_auto);    // Delay for the desired duration
        digitalWrite(stepPin, LOW);     // Release the step
        delayMicroseconds(td2_auto);    // Delay for the desired duration    
      }
      else
      {
           currPage = ROOT_MENU0;
           f=true;
           break;
           
      }
    }
    if(f) {
      break;
    }

   currPage = ROOT_MENU;


while (millis() - loopStartMs < 25) {delay(2);}
  }
  }
  else{
    displayLowBatteryWarning();
  }
}


void page_RootMenu(void) {
  vol = readBatteryVoltage();
  if(vol>2.6){
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
  while (true){
yield(); 
    loopStartMs = millis();

    if (updateDisplay){
      
      updateDisplay = false;
      display.clearDisplay(); 
      batteryVoltage = readBatteryVoltage();
      batteryPercentage = mapFloat(batteryVoltage, 0, 3.3, 0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(103, 0);
  display.print(batteryPercentage);
  display.print("%");          
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(F("Manual Infusion"));
    
      display.setTextSize(1);
      display.setCursor(10, 20);
      printSelected(1, root_Pos); display.println(F(" Volume:"));
      display.setCursor(80, 20);
      display.println(volume);

      display.setCursor(10, 30);
      printSelected(2, root_Pos); display.println(F(" Rate:"));
      display.setCursor(80, 30);
      display.println(rate);

      display.setCursor(10, 40);
      printSelected(3, root_Pos); display.println(F(" Ready"));
      
      display.setCursor(10, 50);
      printSelected(4, root_Pos); display.println(F(" Start Infusion"));
      display.display();
    }

    char keypressed = keypad.getKey();
    if (keypressed != NO_KEY){
    switch (keypressed){
    
      case '1':
      if (root_Pos == 1) {
        root_Pos = ROOT_MENU_CNT;
      } else  {
      root_Pos--;}
      updateDisplay = true;
      break;

      case '2':
      if (root_Pos == ROOT_MENU_CNT) {
        root_Pos = 1;
      } else  {
      root_Pos++;}
      updateDisplay = true;
      break;

      case '3':
      switch (root_Pos){
        case 1: currPage = SUB_MENU1;
        return;
        case 2: currPage = SUB_MENU2;
        return;
        case 3: currPage = SUB_MENU3;
        return;
        case 4: currPage = SUB_MENU4;
        return;
      }

    

    }
  }
    
    while (millis() - loopStartMs < 25) {delay(2);}
   
  }
  }
  else{
    displayLowBatteryWarning();
  }
}

void page_SubMenu1(void){
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
  while (true){
yield(); 
    loopStartMs = millis();

    if (updateDisplay){
      
      updateDisplay = false;
      display.clearDisplay();
      batteryVoltage = readBatteryVoltage();
      batteryPercentage = mapFloat(batteryVoltage, 0, 3.3, 0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(103, 0);
  display.print(batteryPercentage);
  display.print("%");                     
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(F("Manual Infusion"));
    
      display.setTextSize(1);
      display.setCursor(10, 20);
      display.println(F("  Volume:"));
      display.setCursor(80, 20);
      printSelected(1, root_Pos); display.println(volume);

      display.setCursor(10, 30);
      display.println(F("  Rate:"));
      display.setCursor(80, 30);
      printSelected(2, root_Pos); display.println(rate);

      display.setCursor(10, 40);
      printSelected(3, root_Pos); display.println(F(" Ready"));
      
      display.setCursor(10, 50);
      printSelected(4, root_Pos); display.println(F(" Start Infusion"));
      display.display();
    }
    char keypressed = keypad.getKey();
    if (keypressed != NO_KEY){
    switch (keypressed){
    
      case '1':
      if (volume>=0.1 && volume < 10) {
        volume = volume - 0.1;
      } else if (volume >= 10) {
      volume = volume - 1;}
      else{
        volume = 0;
      }
      updateDisplay = true;
      break;

      case '2':
      if (volume <= 9.9 ) {
        volume = volume + 0.1;
      } else  {
      volume = volume + 1;}
      updateDisplay = true;
      break;

      case '3':
      currPage = ROOT_MENU;
      updateDisplay = true;
      return;
      } 
     

    }
  }
}


void  page_SubMenu2(void){
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
  while (true){
    yield(); 
    loopStartMs = millis();

    if (updateDisplay){
      
      updateDisplay = false;
      display.clearDisplay(); 
      batteryVoltage = readBatteryVoltage();
      batteryPercentage = mapFloat(batteryVoltage, 0, 3.3, 0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(103, 0);
  display.print(batteryPercentage);
  display.print("%");                    
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(F("Manual Infusion"));
    
      display.setTextSize(1);
      display.setCursor(10, 20);
      display.println(F("  Volume:"));
      display.setCursor(80, 20);
      printSelected(1, root_Pos); display.println(volume);

      display.setCursor(10, 30);
      display.println(F("  Rate:"));
      display.setCursor(80, 30);
      printSelected(2, root_Pos); display.println(rate);

      display.setCursor(10, 40);
      printSelected(3, root_Pos); display.println(F(" Ready"));
      
      display.setCursor(10, 50);
      printSelected(4, root_Pos); display.println(F(" Start Infusion"));
      display.display();
    }
     char keypressed = keypad.getKey();
    if (keypressed != NO_KEY){
    switch (keypressed){
    
      case '1':
      if (rate>=0.1 && rate < 10) {
        rate = rate - 0.1;
      } else if (rate >= 10) {
      rate = rate - 1;}
      else{
        rate = 0;
      }
      updateDisplay = true;
      break;

      case '2':
      if (rate <= 9.9 ) {
        rate = rate + 0.1;
      } else  {
      rate = rate + 1;}
      updateDisplay = true;
      break;

      case '3':
      currPage = ROOT_MENU;
      updateDisplay = true;
      return;
      } 
     

    }
}
} 


void page_SubMenu3(void){
  
  digitalWrite(dirPin, LOW);
      int y =0;
    while (y < 9999999 && digitalRead(limit_2) == LOW && digitalRead(limit_1) == LOW) {
    yield();
    digitalWrite(stepPin, HIGH);    // Trigger a step
    delayMicroseconds(500);    // Delay for the desired duration
    digitalWrite(stepPin, LOW);     // Release the step
    delayMicroseconds(500);    // Delay for the desired duration
    y++;                    // Increment step count

    
    char keypressed = keypad.getKey();
    if (keypressed != NO_KEY){
    break;
    }
    }
    currPage = ROOT_MENU;
    
}

  void page_SubMenu4(){
    //vol = readBatteryVoltage();
//    if(vol>2.6){
  uint32_t t = 3600*volume/rate;  //total infusion time
  double n1 = (10.000000*volume)/(M_PI*pow(0.9525, 2)*0.001800);  //number of steps required
  double n = round(n1);
  uint32_t td = (t/n)*(1000000);  //required delay between each step
  uint32_t td2 = td/2.000000;
  
  boolean updateDisplay = true;
  uint32_t loopStartMs;
  
 

    loopStartMs = millis();
  
    if (updateDisplay){
      
      updateDisplay = false;
      display.clearDisplay();           
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(30, 25);
      display.println(F("Infusing..."));
      display.display();
}  
  //stepper motor control
  

    digitalWrite(dirPin, HIGH); //sets the direction of the motor    //low for forward, high for retraction
//    int x =0;
  
//    while (x < n && digitalRead(limit_1) == LOW && digitalRead(limit_2) == LOW) {                   
//    yield();
//    digitalWrite(stepPin, HIGH);    // Trigger a step
//    delayMicroseconds(td2);    // Delay for the desired duration
//    digitalWrite(stepPin, LOW);     // Release the step
//    delayMicroseconds(td2);    // Delay for the desired duration
//    x++;                    // Increment step count
//  }
    for(int x=0;x<n;x++)
    {
      if(digitalRead(limit_1) == LOW)
      {
        yield();
        digitalWrite(stepPin, HIGH);    // Trigger a step
        delayMicroseconds(td2);    // Delay for the desired duration
        digitalWrite(stepPin, LOW);     // Release the step
        delayMicroseconds(td2);    // Delay for the desired duration    
      }
      else
      {
           //currPage = ROOT_MENU;
           break;
      }
   currPage = ROOT_MENU;
    }

  }
 


void printSelected(uint8_t p1, uint8_t p2){
  if(p1==p2){
    display.print(F(">"));
  }
  else{
    display.print(F(" "));
  }
}
float readBatteryVoltage() {

  int rawValue = analogRead(34); // Assuming battery voltage is connected to pin34
  float voltage = rawValue * (3.3 / 4095.0); // Convert raw ADC value to voltage 
  
  return voltage;
}

int BatteryIndicator(){
  float batteryVoltage = readBatteryVoltage();
   int batteryPercentage = mapFloat(batteryVoltage, 0, 3.3, 0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(103, 0);
  display.print(batteryPercentage);
  display.print("%");
  display.display();
}

void displayLowBatteryWarning() {
  display.clearDisplay();           
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(30, 26);
      display.println(F("LOW BATTERY"));
      display.display();  
}

float mapFloat(float value, float inMin, float inMax, float outMin, float outMax) {
  return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
