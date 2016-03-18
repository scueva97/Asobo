void InitDHT(){
  pinMode(dht_dpin,OUTPUT);
  digitalWrite(dht_dpin,HIGH);
}

void ReadDHT(){
  bGlobalErr=0;
  byte dht_in;
  byte i;
  digitalWrite(dht_dpin,LOW);
  delay(20);
  digitalWrite(dht_dpin,HIGH);
  delayMicroseconds(40);
  pinMode(dht_dpin,INPUT);
  //delayMicroseconds(40);
  dht_in=digitalRead(dht_dpin);
  if(dht_in){
    bGlobalErr=1;
    return;
  }
  delayMicroseconds(80);
  dht_in=digitalRead(dht_dpin);
  if(!dht_in){
    bGlobalErr=2;
    return;
  }
  delayMicroseconds(80);
  for (i=0; i<5; i++)
  dht_dat[i] = read_dht_dat();
  pinMode(dht_dpin,OUTPUT);
  digitalWrite(dht_dpin,HIGH);
  byte dht_check_sum =
  dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];
  if(dht_dat[4]!= dht_check_sum)
  {bGlobalErr=3;}
};

byte read_dht_dat(){
  byte i = 0;
  byte result=0;
  for(i=0; i< 8; i++){
  while(digitalRead(dht_dpin)==LOW);
  delayMicroseconds(30);
  if (digitalRead(dht_dpin)==HIGH)
  result |=(1<<(7-i));
  while (digitalRead(dht_dpin)==HIGH);
  }
  return result;
}
