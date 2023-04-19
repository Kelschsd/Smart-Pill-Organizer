#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <base64.h>


// Wifi Information
const String wifi_SSID ="SpectrumSetup-6E";
const String wifi_Pass ="bettercounty221"; 

// SMS Client Information
const String sms_host = "api.twilio.com";
const int sms_port = 443; //443
const String acc_sid = "AC79417f7a0b3be6e23ff367df948440f7";
const String auth_token = "a885ffc093236fe106020682edad664d";
const char fp[] = "93 B5 96 8C BC 63 8D A1 D5 AD 1E DA 30 A1 DB 04 73 DC BC 24"; //SHA1 Fingerprint
String from_num = "%2B18339991397"; //replace the + from your number with %2B
String to_num = "%2B16147457958"; //replace the + from your number with %2B



//Sends message to client
//Make sure message is in the following format: "This+is+an+acceptable+msg"
void sendSMS(String msg){
  WiFiClientSecure smsClient;
  smsClient.setFingerprint(fp);
  Serial.println("Connecting to Client...");
  while(!smsClient.connect(sms_host,sms_port)){
    delay(1000);
  }
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
  Serial.println("Starting...");

  // establish Wifi connection
  WiFi.begin(wifi_SSID,wifi_Pass);
  Serial.println("Waiting for connection...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); 
  }
  Serial.println("Connection Found");
  sendSMS("Testing msg");
}








void loop() {
  delay(1000);
}
