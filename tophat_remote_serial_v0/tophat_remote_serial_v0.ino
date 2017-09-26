/* 
 niv april 2014 for checking multiple buttons on remote prototype
 TODO:
  try bouncging joystick somehow? i don't want to slow resonse further
  switch to mouse if combining both buttons?  (F2+F3 seems right)
      NOTE: used a basis for a serial remote
      protoype running on a teensy 2.0
      buttons connected to B1,2,3 (aka 1,2,3
 */

const int button1 = PB1;          // input pin for pushbutton
const int button2 = PB2;
const int button3 = PB3;
const int buttonJoystick = 14;
const int JoystickX = 15;
const int JoystickY = 19;


int previousButton1State = HIGH;   // for checking the state of a pushButton
int previousButton2State = HIGH;   
int previousButton3State = HIGH;   
int previousButtonJoystickState = HIGH;

// keys coded as hex with correct header and checksum
// generated by the test code in test_fake_keyboard.make_key_msg, but it's just nema encoding; easirt to hard code, but could generate them here
/*
 * 
 * 

def make_gps_msg(header, body):
    m = '${},{}'.format(header, body)
    cksum = checksum(m)
    m = '{}*{}'.format(m, "%X" % cksum)
    return m

 
 
def make_key_msg(key):
    return make_gps_msg('FAKBD', hex(ord(key))[2:])
    
need to send make_key_msg(key) + '\n'
    
    
 * 
 * 
 */

/*
void send_coded_key(char k) {
    switch (k) {
        case a :
            Serial.println(
    
}
*/

void setup() {
  // make the pushButton pin an input:
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(buttonJoystick, INPUT_PULLUP);
  // initialize control over the keyboard:
  //Keyboard.begin();
  Serial.begin(115200);
}

// send button encoded as hex
void send_coded_key(byte k) {
    byte checksum = 0;
    unsigned int i;
    String ps = String();

    ps = ps + "$FAKBD," + String(k, HEX);
    // checksum calculation from https://rietman.wordpress.com/2008/09/25/how-to-calculate-the-nmea-checksum/
    for (i=1; i< ps.length(); i++) {
	checksum ^= ps[i]; 
    }
    ps = ps + "*" ;
    Serial.print(ps);
    Serial.println(checksum, HEX);
    return;
}

// raw uinpout values taken from https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h

void loop() {
  int i;
  // the loop is just to slow down the joystick response - the keys are fine on a 50msec debounce, but it's too short for the joystick
  for (i=1; i < 2 ; i++) {
    // read the pushbutton:
    int button1State = digitalRead(button1);
    int button2State = digitalRead(button2);
    int button3State = digitalRead(button3);
    int buttonJoystickState = digitalRead(buttonJoystick);
    // if the button state has changed, and it's currently pressed:
    if ((button1State != previousButton1State) && (button1State == LOW)) {
      send_coded_key(5); } // KEY_4      5
    if ((button2State != previousButton2State)  && (button2State == LOW)) {
      send_coded_key(7); } // KEY_6     7
    if ((button3State != previousButton3State)  && (button3State == LOW)) {
      send_coded_key(1); } // KEY_ESC      1
    if ((buttonJoystickState != previousButtonJoystickState) && (buttonJoystickState == LOW)) {
      send_coded_key(28); } // KEY_ENTER   28
    // save the current button state for comparison next time:
    previousButton1State = button1State;
    previousButton2State = button2State;
    previousButton3State = button3State;
    previousButtonJoystickState = buttonJoystickState;
    delay(150);
  }
  int X = analogRead(JoystickX);
  int Y = analogRead(JoystickY);
  if (X < 256) 
    send_coded_key(105); // KEY_LEFT    105
  if (X > 768) 
    send_coded_key(106); //KEY_RIGHT    106
  if (Y < 256) 
    send_coded_key(103); //KEY_UP      103
  if (Y > 768) 
    send_coded_key(108); //KEY_DOWN    108
}

