#include <Adafruit_LiquidCrystal.h>

// Caracter LCD
byte CaracterCima[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B01010
};
byte CaracterBaixo[] = {
  B01010,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
  B00000
};

byte aumentaLinha[] = {
  B11111,
  B11111,
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100
};
byte diminuirLinha[] = {
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000,
  B11111,
  B11111
};

// Definição dos pinos
const int sensorEntrada = 6;
const int sensorSaida = 7;
const int LedLimite = 5;
const int botaoCima = 4;
const int botaoBaixo = 3;
const int botaoEspecial = 2;
const int leds[] = {8, 9, 10, 11, 12, 13};

// Variáveis globais
int cm1 = 0;
int cm2 = 0; 
int Count = 0; 
int Ret = 0;
int Geral = 0; 
int Controle = 0;
int Menu = 0; 
int Perdidos = 0; 
int Limites = 70;
int Especial = 0;
bool cont = false;
bool Desligado = false;
static float prev_cm1 = 0;
static float prev_cm2 = 0;
unsigned long timerEntrada;
unsigned long timerSaida;
unsigned long timerLimite;
Adafruit_LiquidCrystal lcd(0);

void setup() {
  timerEntrada = millis();
  timerSaida = millis();
  timerLimite = millis();
  
  for(int i = 0; i < 6; i++){
    pinMode(leds[i], OUTPUT);
  }
  pinMode(LedLimite, OUTPUT);
  Serial.begin(9600);
  pinMode(sensorEntrada, INPUT);
  pinMode(sensorSaida, INPUT);
  pinMode(LedLimite, OUTPUT);
  pinMode(botaoEspecial, INPUT);
  IniciarLCD();

}

void loop() {
  verificarBotoes();
  Serial.println(Menu);  
  switch(Menu) {
    case 0: 
    	Desligado = false;
    	Inicio(); 
    	break;
    case 1: 
    	Desligado = false;
    	TotalDia(); 
    	break;
    case 2: 
    	Desligado = false;
    	TotalPerdidos(); 
    	break;
    case 3: FimSistema(); break;
  }
  LigarLedMenu(Menu);
}

void verificarBotoes() {
  if (digitalRead(botaoCima) == HIGH) {
    if (!cont) {
      Menu = (Menu + 1) % 4;
      lcd.clear();
      cont = true;
    }
  } else if (digitalRead(botaoBaixo) == HIGH) {
    if (!cont) {
      Menu = (Menu == 0) ? 3 : Menu - 1;
      lcd.clear();
      cont = true;
    }
  } else {
    cont = false;
  }
}

long learUltrasonicDistancia(int triggerPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(1);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(triggerPin, LOW);
  pinMode(triggerPin, INPUT);
  return pulseIn(triggerPin, HIGH);
}

void Limpar(int valor) {
  String valorStr = String(valor);
  if (valorStr.length() != Controle) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASSAGEIROS");
    Setas();
  }
  Controle = valorStr.length();
}

void Inicio() {
  lcd.setBacklight(1);
  cm1 = 0.01723 * learUltrasonicDistancia(sensorEntrada);
  cm2 = 0.01723 * learUltrasonicDistancia(sensorSaida);

  if (digitalRead(botaoEspecial) == HIGH) {
    digitalWrite(leds[5], HIGH);
    Serial.println(digitalRead(botaoEspecial));
  }

  if (cm1 != 336 && prev_cm1 == 336) {
    if (Count >= Limites) {
      Perdidos++;
      digitalWrite(LedLimite, HIGH);
      timerLimite = millis();
    } else {
      Count++;
      digitalWrite(leds[3], HIGH);
      timerEntrada = millis();
      if (digitalRead(leds[5]) == HIGH) {
        Especial++;
        digitalWrite(leds[5], LOW);
      } else {
        Geral++;
      }
    }
  }
  
  if (cm2 != 336 && prev_cm2 == 336 && Count != 0) {
    Count--;
    digitalWrite(leds[4], HIGH);
    timerSaida = millis();
    digitalWrite(LedLimite, LOW);
  }
  if ((millis() - timerSaida) > 200) {
    digitalWrite(leds[4], LOW);
  }
  if ((millis() - timerEntrada) > 200) {
    digitalWrite(leds[3], LOW);
  }
  
  Limpar(Count);
  lcd.setCursor(0, 1);
  lcd.print("Total: " + String(Count));
  lcd.setBacklight(1);
  
  prev_cm1 = cm1;
  prev_cm2 = cm2;
  Ret = 1;
  delay(0);
}

void TotalDia() {
  Setas();
  lcd.setBacklight(1);
  lcd.setCursor(0, 0);
  lcd.print("ATE A PROXIMA.");
  lcd.setCursor(0, 1);
  lcd.print("TOTAL DO DIA: " + String(Geral));
}

void TotalPerdidos() {
  Setas();
  lcd.setBacklight(1);
  lcd.setCursor(0, 0);
  lcd.print("Valores Perdido");
  lcd.setCursor(0, 1);
  lcd.print("Pass: " + String(Perdidos));
}

void FimSistema() {
  if(Desligado == false){
    FimdeRota();
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Desligando");
    lcd.setCursor(4, 1);
    lcd.print("Sistema");
    Controle = 0;
    Geral = 0;
    delay(5000);
    Desligado = true;
  }else{
  	lcd.clear();
  	lcd.setBacklight(0);   
    lcd.print("DESLIGADO"); 
    delay(5000);
  }
}

void LigarLedMenu(int led) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
  if (led != 3) {
    digitalWrite(leds[led], HIGH);
  }
}

void IniciarLCD(){
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  lcd.setCursor(3, 0);
  lcd.print("Iniciando");
  lcd.setCursor(4, 1);
  lcd.print("Sistema");
  delay(2000);
}
void Setas(){
  lcd.createChar(0, CaracterCima);
  lcd.setCursor(15, 0);
  lcd.write(byte(0));
  lcd.createChar(1, CaracterBaixo);
  lcd.setCursor(15, 1);
  lcd.write(byte(1));
}

void FimdeRota(){
  	lcd.clear();
    lcd.setCursor(0, 0);
  	lcd.print("CARREGANDO"); 
  for(int i = 0; i < 2; i++){ 
	Especial; 
	Geral;   
  delay(2000);
  	lcd.clear();
  	lcd.setCursor(0, 0);
  	lcd.print("TOTAL PAS ESP:" + String(Especial));   
  	lcd.setCursor(0, 1);  	    
    int ArrEspe = Especial * 2;
  	lcd.print("ARREC R$: " + String(ArrEspe) + ",00");
  delay(5000);
  	lcd.clear();
  	lcd.setCursor(0, 0);
  	lcd.print("TOTAL PASS: " + String(Geral));
  	lcd.setCursor(0, 1);
    int ArrPass = (Geral * 4); 
    lcd.print("ARREC R$: " + String(ArrPass) + ",00");   
  delay(5000); 
  	lcd.clear();
  	lcd.setCursor(0, 0);
  	lcd.print("TOTAL GERAL: " + String(Geral + Especial));
  	lcd.setCursor(0, 1);
    int ArrGeral = ArrPass + ArrEspe; 
    lcd.print("ARREC R$: " + String(ArrGeral) + ",00");   
  delay(5000);
  	lcd.clear();
  	float Porcentual = Perdidos * (100.0 / Limites);
    lcd.setCursor(0, 0);
    lcd.print("PERDIDOS " + String(Perdidos));   
    lcd.setCursor(0, 1);
    lcd.print("ARREC R$: " + String(Perdidos * 4) + ",00");   
    delay(5000);
    if(Porcentual > 10){      
  		lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PERDIDOS " + String(Porcentual) + "%");   
      lcd.setCursor(0, 1);
      lcd.print("AUMENTAR LINHA");   
      lcd.createChar(1, aumentaLinha);
      lcd.setCursor(15, 0);
      lcd.write(byte(1));
  	  delay(5000);
    }else if((Geral + Especial) < 20){      
  	  lcd.clear();
      lcd.setCursor(0, 0);  
      lcd.print("DIMINUIR LINHA"); 
      lcd.createChar(1, diminuirLinha);
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
  	  delay(5000);
    } else{
  	  lcd.clear();
      lcd.setCursor(0, 0);  
      lcd.print("MANTER LINHA");      
  	  delay(5000);      
    }
  }  	
}

//calcule a quantidade de usuários
//valor arrecadado na viagem
//quantidade usuários e valores perdidos pela lotação do ônibus
//execução uma análise se deve ampliar, 
//diminuir ou manter os ônibus nessa linha.
