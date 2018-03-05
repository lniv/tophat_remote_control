/*
 * license GPL v2
 * uses a esp8266 mini board (or clone of i got these )
 * https://www.aliexpress.com/item/5-sets-D1-Mini-Mini-nodemcu-4-m-bytes-moon-esp8266-WiFi-Internet-of-things-based/32770121685.html?spm=a2g0s.9042311.0.0.ZiSmQq
 * connected to a hacked cheap remote from which i got the button / matrix but removed the uc
 * https://www.aliexpress.com/item/Top-Deals-CHUNGHOP-1PCS-L181-Combinational-Universal-Remote-Controller-MINI-Learning-remote-control-For-TV-SAT/32803364889.html
 * conenction matrix (pin numbers are for the original uc which i removed)
 * 
 * Down arrow 8,9
 * Right arrow 11, 9
 * Left arrow 8, 10
 * Up arrow 8, 11
 * On/OFF 6, 7
 * OK 6, 11
 * S 8, gnd (not actual ground - pin 1 on the removed micro, but it was connected to ground on the battery compartment)
 * ENTER (center) 10, 11
 * 
 * connections to uc board (colors on prototype)
 * 6 brown D6
 * 7 orange D4
 * 8 gray D3
 * GND/1 green D7
 * 9 yellow D1
 * 10 purple D2
 * 11 blue D5
 * 
*/

#define USE_WIFI
//#define DEBUG
#define MAX_WIFI_TRIES 10

#ifdef USE_WIFI

#include <ESP8266WiFi.h> 
#include <WiFiUdp.h>

// setup when using a hootoo + nook 
const char *ssid = "TripMateSith-0C78";
const char *password = "11111111";
const char* host = "10.10.10.2"; // cellphone was 2, kobo 4,3

const int port = 10110; // flarm on 4352, vario on 4353

WiFiUDP udp;

void send_packet(char *packetBuffer, int j) {
#ifdef DEBUG
    Serial.print("sending to ");
    Serial.print(host);
    Serial.print(", ");
    Serial.println(port);
#endif
    udp.beginPacket(host, port);
    udp.write(packetBuffer, j);
    udp.endPacket();
    return;
}

#endif

#define ARRLEN(a) (sizeof(a)/sizeof((a)[0]))

/*
 *key matrix definition 
 * two arrays, the first for pins we'll use as output and set to zero
 * the second for ones we'll set to input + pull up and check
 */

const uint8_t input_pins[]  = {D3, D5, D3, D3, D6, D6, D3, D2};
const uint8_t output_pins[] = {D1, D1, D2, D5, D4, D5, D7, D5};

// uinput code for down = 108, right = 106, left = 105, up = 103, KEY_4 = 5, KEY_6 = 7, esc = 1 enter = 28,
char keys_to_send[] = {108, 106, 105, 103, 5, 7, 1, 28}; 

uint8_t prev_state[] = {HIGH, HIGH,HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};


// send button encoded as hex with headers,, checksum
void send_coded_key(byte k) {
    byte checksum = 0;
    unsigned int i;
    String ps = String();

    ps = ps + "$FAKBD," + String(k, HEX);
    // checksum calculation from https://rietman.wordpress.com/2008/09/25/how-to-calculate-the-nmea-checksum/
    for (i=1; i< ps.length(); i++) {
	checksum ^= ps[i]; 
    }
    ps = ps + "*" + String(checksum, HEX) ;
    Serial.println(ps);
#ifdef USE_WIFI
	if ( WiFi.status() == WL_CONNECTED) {
		ps = ps + "\r\n"; // a bit odd, but matching println
		i = ps.length() + 1;
		char buf[i];
		ps.toCharArray(buf, i);
		send_packet(buf, i -1);
	}
#endif
    return;
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    // set all to input / pullup to begin with
    // pins connected - used for setting things up
    const uint8_t pins_used[] = {D6,D4,D3,D7,D1,D2,D5};
    for (uint8_t i = 0; i < ARRLEN(pins_used); i ++) {
        pinMode(pins_used[i], INPUT_PULLUP);
    }
    
#ifdef USE_WIFI
    Serial.println("Connecting to wifi");
	Serial.println(ssid);
	Serial.println(password);
    WiFi.begin(ssid, password);
    int i =0;
    while (WiFi.status() != WL_CONNECTED && i < MAX_WIFI_TRIES) {
        delay(500);
        Serial.println(i);
        i++;
    }
    Serial.println("");
	if ( WiFi.status() == WL_CONNECTED) {
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
		
		//udp
		udp.begin(port);
		Serial.print("Local port: ");
		Serial.println(udp.localPort());
	}
	else {
		Serial.println("WIFI connection failed");
	}
#endif
    
    Serial.println('Finished setup');
}

byte j =0 ;
void loop() {
    uint8_t i;
    uint8_t new_state;
    for (i = 0 ; i < ARRLEN(output_pins); i ++) {
        pinMode(output_pins[i], OUTPUT);
        digitalWrite(output_pins[i], LOW);
        delay(10); // needed?
        new_state = digitalRead(input_pins[i]);
        if (new_state != prev_state[i] && new_state == LOW) {
            send_coded_key(keys_to_send[i]) ;
        }
        prev_state[i] = new_state;
        // return to "normal" state
        pinMode(output_pins[i], INPUT_PULLUP);
        delay(10); // needed?
    }
    //server.handleClient();
    delay(100);
}