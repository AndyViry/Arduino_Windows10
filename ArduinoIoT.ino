/*
  Smart home with Arduino MKR1000 and Windows 10 App

 A web server that lets you control your home via internet (WiFi), 
 with a Windows 10 application.
 
 Circuit:
 * Arduino MKR1000
 * Pir Motion Sensor (Hc-sr501) attached to pin 1
 * A relay to control light attached to pin 6
 * A relay to control fan attached to pin 7
 * A relay to control a coffe maker attached to pin 8
 * MQ2 gas sensor attached to pin A0
 * LM35 temperature sensor attached to pin A1  

 created 20 MARZO 2015
 by Andrea Soto
 */
#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "INFINITUMv4mq";      //  your network SSID (name)
char pass[] = "df31c999b9";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

int light = 6;
int fan = 7;
int coffee_maker = 8;
int water_pump = 3;

int gas = A0;
int temperature = A1;
int motion_sensor = 1;


void setup() {
  Serial.begin(9600);      // initialize serial communication 
  
  //Pin configuration
  
  pinMode(light,OUTPUT);
  pinMode(fan,OUTPUT);
  pinMode(coffee_maker,OUTPUT);
  pinMode(water_pump,OUTPUT);

  pinMode(motion_sensor,INPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid,pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Refresh: 1");  // refresh the page automatically every 5 sec
            client.println();
            client.print("<html>");
            client.print("<body>");

            float gas1 = analogRead(gas);
            int aux=0;
            if(gas1>600.0) aux=1;
            else aux=0;
            
            client.print("gas ");
            client.println(aux);
            client.println("<br>");
            client.println("<br>");
            
            float temperature1 = 0;
            float aux2 = 0;
            for(int i = 0; i<100; i++){
              aux2 = analogRead(temperature)*5.0/1024.0;
              aux2 = aux2/0.01;
              temperature1 = temperature1 + aux2;  
              
            }
            temperature1=temperature1/100.0-27.0;
            client.print("temperature ");
            client.println(temperature1);
            client.println("<br>");
            client.println("<br>");

            int motion_sensor1 = digitalRead(motion_sensor);

            client.print("alarm ");
            client.println(motion_sensor1);
            client.println("<br>");
            client.println("<br>");

            client.print("</b></body>");
            client.print("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /LIGHT=ON" or "GET /LIGHT=OFF":
        if (currentLine.endsWith("GET /LIGHT=ON")) {
          digitalWrite(light, HIGH);               // GET /LIGHT=ON turns the LIGHT on
        }
        if (currentLine.endsWith("GET /LIGHT=OFF")) {
          digitalWrite(light, LOW);                // GET /LIGHT=OFF turns the LIGHT off
        }

        // Check to see if the client request was "GET /FAN=ON" or "GET /FAN=OFF":
        if (currentLine.endsWith("GET /FAN=ON")) {
          digitalWrite(fan, HIGH);               // GET /FAN=ON turns the FAN on
        }
        if (currentLine.endsWith("GET /FAN=OFF")) {
          digitalWrite(fan, LOW);                // GET /FAN=OFF turns the FAN off
        }

                // Check to see if the client request was "GET /COFFEE_MAKER=ON" or "GET /COFFEE_MAKER=OFF":
        if (currentLine.endsWith("GET /COFFEE_MAKER=ON")) {
          digitalWrite(coffee_maker, HIGH);               // GET /COFFEE_MAKER=ON turns the COFFEE MAKER on
        }
        if (currentLine.endsWith("GET /COFFEE_MAKER=OFF")) {
          digitalWrite(coffee_maker, LOW);                // GET /COFFEE_MAKER=OFF turns the COFFEE MAKER off
        }

                // Check to see if the client request was "GET /WATER_PUMP=ON" or "GET /WATER_PUMP=OFF":
        if (currentLine.endsWith("GET /WATER_PUMP=ON")) {
          digitalWrite(water_pump, HIGH);               // GET /WATER_PUMP=ON turns the WATER PUMP on
        }
        if (currentLine.endsWith("GET /WATER_PUMP=OFF")) {
          digitalWrite(water_pump, LOW);                // GET /WATER_PUMP=OFF turns the WATER PUMP off
        }
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

