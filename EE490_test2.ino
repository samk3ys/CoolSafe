const int keySwitch = 5;
const int  Relay= 6;
const int buzzer = 9;
const int orLED = 10;
const int rdLED = 11;
const int gnLED = 12;
//const int fp = 5;
const int Relaysig = A0;
const int wLED = A1;
//int gdfp = 0;
//int bdfp = 0;
//int regSi = 0;
//int infp = 0;
//unsigned long interval = 60000;
//unsigned long endtime;
//unsigned long starttime;
void setup() {
  // put your setup code here, to run once:
  pinMode(orLED, OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(keySwitch, INPUT_PULLUP);
  pinMode(rdLED, OUTPUT);
  pinMode(gnLED, OUTPUT);
   pinMode(wLED, OUTPUT);
 // pinMode(fp, INPUT);
   pinMode(Relaysig, INPUT);
  digitalWrite(Relay, LOW);
// attachInterrupt(5 , openBin, CHANGE);

}

void openBin() {
  
   while (digitalRead(keySwitch) == HIGH)
   {
   digitalWrite(Relay, LOW);
   }
   
}

void loop() {
  int key = digitalRead(keySwitch);
   
  // put your main code here, to run repeatedly:
  if (key == HIGH)
  {
    digitalWrite(Relay, HIGH);
    delay(2000);
  digitalWrite(Relay, LOW);
  //openBin();
  }
 /* 
  else
  {
    digitalWrite(Relay, LOW);
  }*/
//digitalWrite(Relaysig, LOW);
/*digitalWrite(wLED, LOW);
digitalWrite(orLED, LOW);
digitalWrite(rdLED, LOW);
digitalWrite(gnLED, LOW);
 // regSi = digitalRead(reg);
//  gdfp = digitalRead(goodfp);
 // bdfp = digitalRead(badfp);
if (regSi == HIGH){
 
 operation();
}
else{}
infp = digitalRead(fp);
if (infp == HIGH){
  checkuser();
}
else{}
}

void operation(){
   
   
 starttime = millis();
 //gdfp = digitalRead(goodfp);
 //bdfp = digitalRead(badfp);
while ((bdfp != HIGH) && (gdfp != HIGH)){
  digitalWrite(orLED, HIGH);
//  gdfp = digitalRead(goodfp);
 // bdfp = digitalRead(badfp);
  endtime = millis();
  if ((endtime-starttime)>60000){
    tone(buzzer, 1000, 2500);
  noTone(buzzer);
  delay(10);
    break;
  }
  }
if (gdfp == HIGH){
  
   digitalWrite(gnLED, HIGH);
  tone(buzzer, 500);
  delay(1000);
  noTone(buzzer);
  delay(1000);
  tone(buzzer, 500);
 delay(1000);
 noTone(buzzer);

}
if (bdfp == HIGH){
  digitalWrite(rdLED, HIGH);
  tone(buzzer, 500);
  delay(3000);
  noTone(buzzer);
 delay(500);
}
}

void checkuser(){
while ((bdfp == LOW) && (gdfp == LOW)){
//  gdfp = digitalRead(goodfp);
   
//  bdfp = digitalRead(badfp);
}
if (gdfp == HIGH){
  digitalWrite(Relay, HIGH);
  digitalWrite(gnLED, HIGH);
    if(digitalRead(Relaysig) == HIGH);
   {
    digitalWrite(wLED, HIGH);
   }
   
  tone(buzzer, 500);
  delay(1000);
  noTone(buzzer);
  delay(1000);
  tone(buzzer, 500);
 delay(1000);
 noTone(buzzer);
}

if (bdfp == HIGH){
  digitalWrite(rdLED, HIGH);
  tone(buzzer, 500);
  delay(3000);
  noTone(buzzer);
 delay(500);
}
*/}
