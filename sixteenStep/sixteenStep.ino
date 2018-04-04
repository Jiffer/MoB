//Mux control pins
int s0 = 8;
int s1 = 9;
int s2 = 10;
int s3 = 11;
//Mux in “SIG” pin
int SIG_pin = 0;

const int numChannels = 16;

// int analogPins[] = {A0, A1}; // A1, A2, A3, A4, A5, A6, A7
int channelValues[numChannels];
const int hLength = 4;
int history[numChannels][hLength];

unsigned long debounceTime[numChannels];
int pointer = 0;

void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);


}

void loop() {

  for (int i = 0; i < numChannels; i++) {
    // read the analog in value:
    channelValues[i] = readMux(i); // analogRead(analogPins[i]);
    history[i][pointer] = channelValues[i]; // analogRead(analogPins[i]);

    int largestDelta = 0;
    // if not waiting on this channel

    if ((unsigned long)millis() > debounceTime[i]) {

      //    if (largestDelta > 5 ) {
      if (channelValues[i] > 5 ) {
        int lev = (int)map(channelValues[i], 5, 50, 40, 127);
        if (lev > 127) { // clip
          lev = 127;
        }

        // Note On messages when each button is pressed
        usbMIDI.sendNoteOn(60 + i, lev, 0);  // 60 = C4, vel, channel

        // got a hit, print it:
        Serial.print(i);
        Serial.print(" delta: ");
        Serial.print(largestDelta);
        Serial.print(" channelValue " );
        Serial.print(channelValues[i]);
        Serial.print(" velocity: ");
        Serial.print(lev);
        Serial.print(" mils " );
        Serial.print(millis());
        Serial.print(" dbounc " );
        Serial.print(debounceTime[i]);
        
        
        debounceTime[i] = millis() + 250;
        Serial.print(" mils " );
        Serial.print(millis());
        Serial.print(" dbounc " );
        Serial.print(debounceTime[i]);
        
        Serial.println();
        


      }
    }
  }
  pointer++;
  pointer %= hLength;


  // MIDI Controllers should discard incoming MIDI messages.
  while (usbMIDI.read()) {
  }
}

/////////////////////////////////////////
// MUX function
/////////////////////////////////////////
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4] = {
    {0, 0, 0, 0}, //channel 0
    {1, 0, 0, 0}, //channel 1
    {0, 1, 0, 0}, //channel 2
    {1, 1, 0, 0}, //channel 3
    {0, 0, 1, 0}, //channel 4
    {1, 0, 1, 0}, //channel 5
    {0, 1, 1, 0}, //channel 6
    {1, 1, 1, 0}, //channel 7
    {0, 0, 0, 1}, //channel 8
    {1, 0, 0, 1}, //channel 9
    {0, 1, 0, 1}, //channel 10
    {1, 1, 0, 1}, //channel 11
    {0, 0, 1, 1}, //channel 12
    {1, 0, 1, 1}, //channel 13
    {0, 1, 1, 1}, //channel 14
    {1, 1, 1, 1} //channel 15
  };

  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);

  //return the value
  return val;
}

