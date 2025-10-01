// Settings
// WiFi credentials
const char* ssid = "";      //write SSID (name of your WiFi)
const char* password = "";  //write password of your SSID

// Input pin
#define BUTTON_PIN 6

// IP address of the target PC and broadcast network
const char* BROADCAST_IP = "192.168.0.255";
const char* PC_IP = "192.168.0.1";    //for long time use, make IP address static (via DHCP reservation on router or in OS settings)
byte mac[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };  // MAC address of the PC (for example aa:bb:cc:dd:ee:ff)
const int WOL_PORT = 9; //you can try different ports (for example 0, 7, 8), if this doesn't work)
#define PACKET_SIZE 102 // leave as is

// Libraries and instances
#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP udp;

bool debugEnabled = true;
#define LED_PIN 8   // onboard LED (ESP32-WROOM32 has GPIO 2, ESP32-C3 super mini has GPIO 8)
// Some boards have inverted onboard LED logic (for example ESP32-C3 super mini)
#define LED_ON LOW
#define LED_OFF HIGH
#define REFRESH_RATE 10 // can reduce heating, high performance, and power consumption

// WOL packet logic
void sendWOL() {
  byte packet[PACKET_SIZE];

  // 6x FF
  for (int i = 0; i < 6; i++) {
    packet[i] = 0xFF;
  }

  // 16x MAC address repetition
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 6; j++) {
      packet[6 + i * 6 + j] = mac[j];
    }
  }

  // Broadcast
  udp.beginPacket(BROADCAST_IP, WOL_PORT);
  udp.write(packet, sizeof(packet));
  int sentBroadcast = udp.endPacket();

  // Direct unicast to PC
  udp.beginPacket(PC_IP, WOL_PORT);
  udp.write(packet, sizeof(packet));
  int sentUnicast = udp.endPacket();

  Serial.printf("Broadcast bytes: %d, Unicast bytes: %d\n", sentBroadcast, sentUnicast);
  Serial.println("WOL packet sent!");
}

/* Debug LED - logic for feedback (for example, if the WiFi is connected, if the packet was sent), if the Serial isn't used
   I'd prefer leave it on for normal use
   2 fast blinks means start/restart
   5 blinks means WiFi is successfully connected
   1 long blink means that the magic packet was sent
*/
void ledDebug(int pauseMs, int cycles) {
  if(debugEnabled) {
    for(int i = 0; i < cycles; i++) {
      digitalWrite(LED_PIN, LED_ON);
      delay(pauseMs);
      digitalWrite(LED_PIN, LED_OFF);
      delay(pauseMs);
    }
  }
}

// Serial/WiFi connection logic
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // debug
  ledDebug(200, 5);
}

void setup() {
  // Button pin as input with pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  Serial.begin(115200);
  delay(1000);

  ledDebug(100, 2);

  // Setup WiFi
  WiFi.begin(ssid, password);
  connectWiFi();
}

// WOL state
bool packetSent = false;

void loop() {
  //if the button is pressed and packet is not sent yet, send magic packet
  if (digitalRead(BUTTON_PIN) == LOW && !packetSent) {
    sendWOL();
    packetSent = true; //update WOL state

    ledDebug(1000, 1);
  }

  //debounce logic
  if(digitalRead(BUTTON_PIN) == HIGH && packetSent) {
    packetSent = false;
  }

  //WiFi reconnect logic
  if (WiFi.status() != WL_CONNECTED) {
    ledDebug(50, 10);
    WiFi.reconnect();
    delay(5000); // wait before trying to reconnect to avoid rapid loops
    connectWiFi();
  }

  // you can comment this out if you want
  delay(REFRESH_RATE);
}
