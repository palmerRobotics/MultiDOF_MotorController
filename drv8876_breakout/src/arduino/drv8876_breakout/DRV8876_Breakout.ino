#define ANALOG_READ_DELAY 1000 //in ms

int pmode = 12; //control mode
int en = 11; //pwm signal
int ph = 10; //direction
int nsleep = 9; //nsleep is active low (sleeps when low)
int nfault = 8; //active low
int vref = 7; //PWM freq: 980Hz, max 5.75V
int ipropi = A0; //current sensing
//tie imode to ground - don't use as output


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pmode, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(ph, OUTPUT);
  pinMode(nsleep, OUTPUT);
  pinMode(nfault, INPUT);
  pinMode(vref, OUTPUT);
  pinMode(ipropi, INPUT);
 
  digitalWrite(pmode,LOW); //ph/en control mode
  //imode is attached to ground
  digitalWrite(nsleep, HIGH); //turn drv8876 on, pmode and imode latched
  //analogWrite(vref,255);
  digitalWrite(vref, HIGH);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ph, HIGH); //forward
  Serial.println("forward");
  analogWrite(en, 128);
  delay(ANALOG_READ_DELAY);
  int Vipropi = analogRead(ipropi);
  Serial.print("ADC read:");
  Serial.println(Vipropi);
  delay(5000 - ANALOG_READ_DELAY);
  digitalWrite(ph, LOW); //reverse
  Serial.println("reverse");
  analogWrite(en, 192);
  delay(ANALOG_READ_DELAY);
  Vipropi = analogRead(ipropi);
  Serial.print("ADC read:");
  Serial.println(Vipropi);
  delay(5000 - ANALOG_READ_DELAY);  
}
