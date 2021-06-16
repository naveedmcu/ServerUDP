//Client Remote side for data sender ASCII
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// #define USEbutton

const char *ssid = "server";
const char *pass = "12345678";

unsigned int localPort = 5555; // local port to listen for UDP packets

IPAddress ServerIP(192, 168, 4, 1);
// IPAddress ClientIP(192, 168, 4, 2);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

char packetBuffer[9]; //Where we get the UDP data
void vfConnect_Disconnect();
//======================================================================
//                Setup
//======================================================================
void setup()
{
  Serial.begin(9600);
  Serial.println();
  WiFi.setOutputPower(20.5);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass); //Connect to access point

  Serial.println("");

  pinMode(LED_BUILTIN, OUTPUT);

  vfConnect_Disconnect();
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Start UDP
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  pinMode(D0, INPUT_PULLDOWN_16);
}
//======================================================================
//                MAIN LOOP
//======================================================================
void loop()
{
  int cb = udp.parsePacket();
  if (!cb)
  {
// delay(500);
// Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
#ifdef USEbutton
    if (digitalRead(D0) == 1)
    {
      // Serial.print("high");
      udp.beginPacket(ServerIP, 2000); //Send Data to Master unit
      //Send UDP requests are to port 2000

      char a[1];
      a[0] = +1;
      udp.write(a, 1); //Send one byte to ESP8266
      udp.endPacket();
    }
    else if (digitalRead(D0) == 0)
    {
      // Serial.print("low");
      udp.beginPacket(ServerIP, 2000); //Send Data to Master unit
      //Send UDP requests are to port 2000

      char a[1];
      a[0] = '0';
      udp.write(a, 1); //Send one byte to ESP8266
      udp.endPacket();
    }
#endif
    //If serial data is recived send it to UDP
    if (Serial.available() > 0)
    {
      char Data_Ascii;
      Data_Ascii = Serial.read();
      Serial.write(Data_Ascii);
      udp.beginPacket(ServerIP, localPort); //Send Data to Master unit
      //Send UDP requests are to port 2000

      char a[1];
      a[0] = Data_Ascii; //Serial Byte Read
      udp.write(a, 1);   //Send one byte to ESP8266
      udp.endPacket();
    }
  }
  else
  {
    // We've received a UDP packet, send it to serial
    udp.read(packetBuffer, 1); // read the packet into the buffer, we are reading only one byte
    Serial.print(packetBuffer);
    delay(20);
  }
}

void vfConnect_Disconnect()
{
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
    Serial.print("Station connected, IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, 0);
  });
  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
    Serial.println("Station disconnected");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  });
}
//=======================================================================