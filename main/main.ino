#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

constexpr uint8_t RST_PIN = D3;     
constexpr uint8_t SS_PIN = D4;  

const char *mqtt_broker = "****************";
const char *topic = "***********";
const int mqtt_port = 1883;

const char *ssid = "***************"; 
const char *password = "***************";

MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;

WiFiClient espClient;
PubSubClient client(espClient);

String tag;

void setup() {

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.println("Connecting to WiFi..");
	}

	Serial.begin(9600);
	SPI.begin(); 
	rfid.PCD_Init(); 

	client.setServer(mqtt_broker, mqtt_port);
	String client_id = "cs-01-park-bot";
	
	if (client.connect(mqtt_broker)) {
            Serial.println("connected to mqtt broker");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
}

void loop() {
	scanForTags();
	if(tag != ""){
		client.publish(topic, tag.c_str());
	}
	clearCurrentTag();
	client.loop();
}


void scanForTags(){
	if ( ! rfid.PICC_IsNewCardPresent())
		return;
	if (rfid.PICC_ReadCardSerial()) {
		for (byte i = 0; i < 4; i++) {
			tag += rfid.uid.uidByte[i];
		}
	}
}


void clearCurrentTag(){
	tag = "";
	rfid.PICC_HaltA();
	rfid.PCD_StopCrypto1();
}

