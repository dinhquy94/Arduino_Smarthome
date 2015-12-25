#include <Regexp.h> 
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPClient.h>
#include <stdlib.h>
#include <string.h>
int led = 2;
int led1 = 9;
int led2 = 8; 
byte mac[] = {0x00,0x01,0x02,0x03,0x04,0x05};   //physical mac address
byte ip[] = { 192,168,100,69 };                      // ip in lan (that's what you need to use in your browser. ("***.***.*.***")
byte gateway[] = { 192,168,100,1 };                   // internet access via router

EthernetServer server(80);                             //server port     
String readString;
   /* This is code */
   
  void ParseRequestStr(String reqStr, String Name, boolean type)
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
           Serial.println (ms.GetCapture(bufferReqStr, j));
        index = ms.MatchStart + ms.MatchLength;
      }
      else
        break;
    } 
}
   
   
      /* This is code */
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip );
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
} 
void loop() {
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
        //   Serial.println(readString); //print to serial monitor for debuging
     /*
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
      
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           //client.println("POST /TempGauges.php HTTP/1.1");
          // client.println("Host: MyWebPage.com");
           client.println("led=1");
               */
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("</HEAD>");
           client.println("<BODY>");
            client.println("<BODY>");
           client.println("<h1>Oh shit, It 's Me</h1>");
           client.println("</BODY>");
           client.println("</HTML>");
 
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1on") >0){
               digitalWrite(led, HIGH);
               Serial.println ("LED 0n");
           }
           if (readString.indexOf("?button1off") >0){
               digitalWrite(led, LOW);
           }
           if (readString.indexOf("?button2on") >0){
               digitalWrite(led1, HIGH);
           }
           if (readString.indexOf("?button2off") >0){
              digitalWrite(led1, LOW);
                }
           if (readString.indexOf("?button3on") >0){
             digitalWrite(led2, HIGH);
           }
           if (readString.indexOf("?button3off") >0){
             digitalWrite(led2, LOW);
         } 
       //   Serial.println (readString);
       ParseRequestStr(readString, "rdw", false);
            //clearing string for next read
            readString=""; 
           
         }
       }
    }
}
}

