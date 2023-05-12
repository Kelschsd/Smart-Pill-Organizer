#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <base64.h>


// Wifi Information
const String wifi_SSID ="t’s iPhone (2)"; //You can see available wifi networks printed if you upload the code
const String wifi_Pass =""; 

// SMS Client Information
const String sms_host = "api.twilio.com";
const int sms_port = 443; //443
const String acc_sid = "";
const String auth_token = "";
const char fp[] = ""; //SHA1 Fingerprint
String from_num = ""; //replace the + from your number with %2B
String to_num = ""; //replace the + from your number with %2B



//Sends message to client
//Make sure message is in the following format: "This+is+an+acceptable+msg"
void sendSMS(String msg){
  WiFiClientSecure smsClient;
  smsClient.setFingerprint(fp);
  Serial.println("Connecting to Client...");
  while(!smsClient.connect(sms_host,sms_port)){
    delay(1000);
  }
  delay(1000);
  //Creates a HTTP request based on the data input at the top
  String headers = base64::encode(acc_sid + ":" + auth_token);
  String data = "To=" + to_num + "&From=" + from_num + "&Body="+ msg;
  String http_Req ="POST /2010-04-01/Accounts/" + acc_sid + "/Messages HTTP/1.1\r\n" + 
                   "Authorization: Basic " + headers + "\r\n" + "Host: " + sms_host+ "\r\n" + 
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Content-Length: " + data.length() + "\r\n" + 
                   "Connection: close\r\n\r\n" + data + "\r\n";
  
  Serial.println("Connection Found");
  Serial.println("Sending HTTP request:");
  Serial.println(http_Req);
  smsClient.print(http_Req);
  Serial.println("Request Sent");
  Serial.println("Return from Request:");
  while (smsClient.connected()) {
    Serial.println(smsClient.readStringUntil('\n'));
  }
  Serial.println("Print Finished");
}

void setup() {
  Serial.begin(115200);
  pinMode(14, INPUT);
  delay(5000);
  int numberOfNetworks = WiFi.scanNetworks();
 
  for(int i =0; i<numberOfNetworks; i++){
 
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      Serial.print("Signal strength: ");
      Serial.println(WiFi.RSSI(i));
      Serial.println("-----------------------");
 
  }
  Serial.println("Starting...");

  // establish Wifi connection
  WiFi.begin(wifi_SSID,wifi_Pass);
  Serial.println("Waiting for connection...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); 
  }
  Serial.println("Connection Found");
  delay(1000);
}
void loop(){
  if(digitalRead(14)){ sendSMS("Time+to+take+your+medication!");}
  delay(100);
}


