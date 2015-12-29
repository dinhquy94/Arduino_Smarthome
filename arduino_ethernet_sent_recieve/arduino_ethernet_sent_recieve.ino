#include <Regexp.h> 
#include <UIPEthernet.h> 
#include <stdlib.h>
#include <string.h> 
#include "DHT.h"            
String FIRMWARE = "41FA580671945AFH2";
char serveradds[] = "192.168.0.100";
/*ĐỘ ẩm, nhiệt độ */
const int DHTPIN = 13;       //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22  
DHT dht(DHTPIN, DHTTYPE);
/*Kết thúc ĐỘ ẩm, nhiệt độ */
/* cảm biến mưa */
int israining;
int rainSensor = 6;
/*ket thuc cam bien mua */
/*cam bien chuyen dong */
unsigned long previousMillis = 0;  
unsigned long previousMillisalarm = 0;   
boolean securemode = 0;
boolean motion_state = 0;
int pirinput = 12;
/*ket thuc cam bien chuyen dong */
boolean device[55];
int i;
String temp;
          // we start, assuming no motion detected
int inputPin = 3;  
int relay[8] = {31,33,35,37,39,41,43,45};
byte mac[] = {0x00,0x01,0x02,0x03,0x04,0x05};   //physical mac address
byte ip[] = { 192,168,0,177 };                      // ip in lan (that's what you need to use in your browser. ("***.***.*.***")
byte gateway[] = { 192,168,0,1 };       
EthernetServer server(80);        
EthernetClient aruinoclient; //server port     
String readString;
float h = 0;    //Đọc độ ẩm
float t = 0; //Đọc nhiệt độ
void updateTemp(){
    h = dht.readHumidity();  
    t = dht.readTemperature();
    sento_webserver("temp", (String)t);
     delay(500);
    sento_webserver("humidity", (String)h);
  Serial.print("Nhiet do: ");
  Serial.println(t);               //Xuất nhiệt độ
  Serial.print("Do am: ");
  Serial.println(h);               //Xuất độ ẩm 
  Serial.println();  
}
void setup() {
    israining = 0;
    for(i = 0; i<8; i++){
      pinMode(relay[i], OUTPUT);
      digitalWrite(relay[i], HIGH);      
 }
  
   for( i = 1; i<54; i++) device[i] = 0;
 // Open serial communications and wait for port to open:
  Serial.begin(9600); 
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip );
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  delay(500);
   sento_webserver("motionsensor", "0");
    delay(500);
    updateTemp();
} 
 
void loop() {
israining = digitalRead(rainSensor);
if(securemode == 0){
  delay_time_motion(); 
}else{
  motion_alarm(); 
}
   for( i = 1; i<55; i++){
        for(int j = 0; j < 8; j++){
          if(i == relay[j]){
            if(device[i] == 0){
              digitalWrite(relay[j], HIGH);              
              }else{
               digitalWrite(relay[j], LOW); 
                }            
            }
          }
     }
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         } 
         //if HTTP request has ended
         if (c == '\n') {   
          Serial.println (ParseRequestStr(readString, "rdw", false)); 
            for( i = 1; i<55; i++){
             temp = (String) i; 
               if (readString.indexOf("?relay"+temp+"On") >0){
                device[i]= 1;              
             }
              if (readString.indexOf("?relay"+temp+"Off") >0){
                device[i]= 0;              
             }
           }
             if (readString.indexOf("?securemodeOn") >0){
                  securemode = 1;
             }
                if (readString.indexOf("?securemodeOff") >0){
                  securemode = 0;
             }
             if (readString.indexOf("?offMotion") >0){
                  motion_state = 0;
             }     
             if (readString.indexOf("?updateTemp") >0){
                 updateTemp();
             }    
            /*for relay only*/              
           
            readString=""; 
            client.print("<html>"); 
           //return state of device
            for( i = 1; i<55; i++){ 
              if (device[i] == 0){
                    client.print("<relay");  client.print(i);  client.print(">off");  client.print("</relay");  client.print(i);  client.print(">");client.println("");  
                    
                }else{
                    
                    client.print("<relay");  client.print(i);  client.print(">on");  client.print("</relay");  client.print(i);  client.print(">");client.println(""); 
                  }
            }
            client.print("<rainsensor>"); client.print(israining);  client.print("</rainsensor>"); client.println("");
            client.print("<temp>"); client.print(t);  client.print("</temp>"); client.println("");
            client.print("<humidity>"); client.print(h);  client.print("</humidity>"); client.println("");
            //motion_state
            client.print("<motion_state>"); client.print(motion_state);  client.print("</motion_state>"); client.println(""); 
            client.print("</html>"); 
            client.stop();
          //end return device
         }
       }
    }
}
} 
/* This is code */
String ParseRequestStr(String reqStr, String Name, boolean type)
{ 
  //type =1: get String
  //Type = 0: get Number
    MatchState ms;
    unsigned int index = 0;
    char bufferReqStr[reqStr.length() + 1]; //Buf for Reg Ex
    char bufferStr[reqStr.length()+1]; // Buf for char array
    reqStr.toCharArray(bufferStr, reqStr.length() + 1); // String to Char Array 
    //to char array
    String myWord = Name;
    char pattent[myWord.length()+1];//as 1 char space for null is also required
    ms.Target(bufferStr);
    strcpy(pattent, myWord.c_str()); 
   (type == true) ? strcat(pattent, "=(%a+)") : strcat(pattent, "=(%d+)");  
  //  Serial.println (pattent);  
    //end to char array 
   
    while (true)
    {
      //(%a+)(%d)=(%a+)
      //r0=on&r1=on&postTemp=123
      char result = ms.Match ( pattent , index);
      if (result == REGEXP_MATCHED)
      {
      //  Serial.print ("Matched on: ");
      //  Serial.println (ms.GetMatch(bufferReqStr));
       // Serial.println ("Captures:");
        for (int j = 0; j < ms.level; j++)
          return (ms.GetCapture(bufferReqStr, j));
        index = ms.MatchStart + ms.MatchLength;
      }
      else
        break;
        
    } 
    return "0";
}
 int m = 0;
 void motion_alarm(){
   unsigned long currentMillisalarm = millis();  
    if(motion()){
     while( m < 1){ 
       sento_webserver("motionsensor", "1");
       m = m+1;
     } 
    previousMillisalarm = currentMillisalarm;
    }else{  
      int val = currentMillisalarm - previousMillisalarm;
       if (val >= 15*60*1000) {  
          m= 0;
       }     
    } 
 }
void delay_time_motion(){
   unsigned long currentMillis = millis();  
  if(motion()){
     motion_state = 1;   
     previousMillis = currentMillis;
  }else{ 
    int val = currentMillis - previousMillis;
    Serial.println(val);
     if (val >= 3000) { 
        previousMillis = currentMillis;
        motion_state = 0; 
     }     
  }
  if(motion_state == 1){
    device[31] = 1; 
  }else{
    device[31] = 0; 
  }
      // set the LED with the ledState of the variable: 
   }
 
      /* This is code */
boolean motion(){  
   //int israining = digitalRead(rainSensor);  
   return digitalRead(pirinput);    
}
 
void sento_webserver(String data_name, String value){
     int retry = 0;
 while(retry < 1 ){
  if (aruinoclient.connect(serveradds, 80)) {
    Serial.println("-> Connected");
    Serial.println(data_name);
//      // Make a HTTP request:
      aruinoclient.print( "GET /smarthome/index.php/main/updateData/?");
      aruinoclient.print(data_name);
      aruinoclient.print("=");
      aruinoclient.print(value);
      aruinoclient.print("&&");
      aruinoclient.print("firmware=");
      aruinoclient.print(FIRMWARE);
      aruinoclient.println( " HTTP/1.1");
      aruinoclient.print( "Host: " );
      aruinoclient.println(serveradds);
      aruinoclient.println( "Connection: close" );
      aruinoclient.println();
      aruinoclient.println();
      aruinoclient.stop();
      retry = retry + 1;
  }
  else { 
    Serial.println("--> connection failed/n");
    Serial.println(data_name);
  }
  delay(500);
  }
}

