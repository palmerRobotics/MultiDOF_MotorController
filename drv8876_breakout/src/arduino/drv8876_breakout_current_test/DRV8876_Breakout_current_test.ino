#define ANALOG_READ_DELAY 5000 //in ms
#define ANLG_RD_INC 10 //in ms

int pmode = 12; //change pin numbers
int en = 11;
int ph = 10;
int nsleep = 9; //nsleep is active low (sleeps when low)
int nfault = 8; //active low
int vref = 7; //PWM freq: 980Hz, max 5.75V
int ipropi = A0;
//tie imode to ground - don't use as output

int Vi[ANALOG_READ_DELAY / ANLG_RD_INC];


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
  analogWrite(en, 128);
  int j = 0;
  int Vipropi = 0;
  double Vi_avg = 0;
  
  for (int i = 0; i < ANALOG_READ_DELAY; i+= ANLG_RD_INC){
    Vipropi = analogRead(ipropi);
    //Serial.println(Vipropi);
    Vi[j] = Vipropi;
    /*Serial.print("i= ");
    Serial.println(i);
    Serial.print("j= ");
    Serial.println(j);*/

    /*Serial.print("Vi= ");
    Serial.println(Vi[j]);*/
    delay(ANLG_RD_INC);
    j++;
  }
  /*Serial.print("j= ");
  Serial.println(j);*/
  Serial.print("Fwd avg: ");
  for (int i = 0; i < (sizeof(Vi) / sizeof(Vi[0])); i++){
    Vi_avg += Vi[i];
  }
  Serial.println(Vi_avg/j);
  j = 0;
  Vi_avg = 0;
  
  digitalWrite(ph, LOW); //reverse
  analogWrite(en, 192);
  for (int i = 0; i < ANALOG_READ_DELAY; i+= ANLG_RD_INC){
    Vipropi = analogRead(ipropi);
    //Serial.println(Vipropi);
    Vi[j] = Vipropi;
    /*
    Serial.print("i= ");
    Serial.println(i);
    Serial.print("j= ");
    Serial.println(j);*/

    /*Serial.print("Vi= ");
    Serial.println(Vi[j]);*/
    delay(ANLG_RD_INC);
    j++;
  }
  /*Serial.print("j= ");
  Serial.println(j);*/
  Serial.print("Rvrs avg: ");
  for (int i = 0; i < (sizeof(Vi) / sizeof(Vi[0])); i++){
    Vi_avg += Vi[i];
  }
  Serial.println(Vi_avg/j);
  j = 0;
}
