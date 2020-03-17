#include <SoftwareSerial.h> //serial do bluetooth
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
//Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
//define as portas
//portas digitais
#define buzzer  13
#define boia  12
#define fan  11
#define led  10
#define waterbomb  9
#define DHTPIN 7
#define DHTTYPE DHT22
//portas analogicas
#define umi1  A0
#define umi2  A1
#define lumi  A2
SoftwareSerial bluetooth(2, 3); // TX, RX do Arduino
String command = "";
char b1;
String b2,b3,b4,b5; //b2 umidade e b3 nivel do tanque b4 bomba em funcionamento b5 temperatura
int a,b,luz,umi,nivel; //nivel (nivel da agua em relação ao tanque)
int valorlog; //valor logico
int u,t; //umidade e temp do ar
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  lcd.begin (16,2);
  pinMode(led, OUTPUT);
  Serial.begin(9600);  
  Serial.println("Digite algo para enviar por bluetooth:");  //Printa uma frase no monitor serial para saber que está tudo pronto
  bluetooth.begin(9600); 
  //pinos analogicos
  pinMode(umi1, INPUT);  //A0
  pinMode(umi2, INPUT);  //A1
  pinMode(lumi, INPUT);  //A2
  //pinos digitais
  pinMode(buzzer, OUTPUT); //pino 13
  pinMode(boia, INPUT); //pino 12
  pinMode(fan, OUTPUT); //pino 11
  pinMode(led, OUTPUT); //pino 10
  pinMode(waterbomb, OUTPUT); //pino 9
  delay (5000);
}
void loop() {
  // Lê os dados do serial do Arduino e envia por Bluetooth
  if (Serial.available()>0) {  // Se existem dados para leitura (sem uso mas importante manter)
    delay(10); 
    bluetooth.print(b2);
  }
  // Lê os dados do bluetooth
  if (bluetooth.available()>0) { // 
    
    b1 = bluetooth.read(); //Variável para armazenar o dado atual
    command += b1; //Variável para armazenar todos os dados
    
      if (b1 == '\n'){ //Se o dado atual for um pulador de linha (\n)
        Serial.print(command); //Printa o comando
      
        command.trim(); //Remove o \n para comparar o texto
      
          if (command == "hortela"){
            digitalWrite(buzzer, HIGH);
          }else if(command == "cebolinha"){
            digitalWrite(buzzer, LOW);
          }
        command = ""; //Limpa o comando para futuras leituras
    }
  }
  
  lcd.setBacklight(HIGH);
  digitalWrite(waterbomb, HIGH); //desligada
  digitalWrite(led, HIGH); //desligada
  digitalWrite(fan, HIGH); //desligada
  delay(10000);
  
  verificasolo:
    luminosidade ();
    solo ();
    Serial.print(umi);
    if (umi > 0 && umi < 400)
  {
      b2 = "1"; //solo umido
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print("solo umido");
  }
 
  //Solo com umidade moderada
    if (umi > 400 && umi < 800)
  {
      b2 = "2"; //umidade moderada
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print("umidade");
      lcd.setCursor (0,1);
      lcd.print("moderada");
  }
 
  //Solo seco
    if (umi > 800 && umi < 1024)
  {
      b2 = "3"; //solo seco
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print("solo seco");
  }
    delay (500);
    luminosidade();
    delay (500);
    bluetooth.println(b2); //envia os dados para o bluetooth
    luminosidade();


  
  irrigacao:
    solo ();
    nivel = digitalRead(boia); //ler os dados da boia e enviar a variavel nivel
    if (nivel == LOW) {
      nivel = digitalRead(boia);
      digitalWrite(waterbomb, HIGH); //desligada
      digitalWrite(buzzer, LOW);
      b3 = "4";  //Nivel de agua baixo, preencha o tanque
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("nivel da agua");
      lcd.setCursor(0,1);
      lcd.print("baixo preencha");
      delay (1250);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("baixo preencha");
      lcd.setCursor(0,1);
      lcd.print("o tanque");
      luminosidade();
      nivel = digitalRead(boia);
      delay (1250);
      bluetooth.println(b3);
      goto irrigacao;
    }
    
      if (nivel == HIGH) {
        digitalWrite(led, LOW);
        nivel = digitalRead(boia);
        b3 = "5"; //nivel da agua alto,
        delay (500);
        delay (500);
        bluetooth.println(b3);
        solo ();
        //leitura da umidade da terra necessaria aqui
        while (umi > 805) {
          solo ();
          Serial.print (umi);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Tanque em");
          lcd.setCursor(0,1);
          lcd.print("funcionamento");
          b4 = "6"; //tanque em funcionamento
          delay (1000);
          bluetooth.println(b4);
          digitalWrite(waterbomb, LOW); //ligada
          digitalWrite(buzzer, HIGH);    //mudar para HIGH apos finalização, necessaria para o funcionamento do alarme
          delay (250);
          if (nivel == LOW) {
            goto irrigacao;
      }
      solo ();
      nivel = digitalRead(boia);
          digitalWrite(buzzer, LOW);    //mudar para HIGH apos finalização, necessaria para o funcionamento do alarme
          delay (250);
          if (nivel == LOW) {
            goto irrigacao;
      }   
      solo ();
      nivel = digitalRead(boia);
          digitalWrite(buzzer, HIGH);
          delay (250);
          if (nivel == LOW) {
            goto irrigacao;
      }
      solo ();
      nivel = digitalRead(boia);
          digitalWrite(buzzer, LOW);
          delay (250);
          if (nivel == LOW) {
            goto irrigacao;
            }
      solo ();
      nivel = digitalRead(boia);
    }
 } 
      luminosidade();
      posIrrigacao:
      luminosidade();
      lcd.clear();
      b4 = "7"; //bomba desligado manda vazio ao app
      delay (1000);
      bluetooth.println(b4);
      nivel = digitalRead(boia);
      if (nivel == LOW) {
        goto irrigacao;
        } 
        
      solo ();

      luminosidade();
      if (luz < 850) {
        b4 = "8"; //led  ligado
        delay(1000);
        bluetooth.println(b4);
        }
      if (luz > 851) {
        b4 = "9"; //led desligado
        delay(1000);
        bluetooth.println(b4);
        }

        
      if (umi < 805) { //valor imaginario
      delay(2000);
      umidade();
      luminosidade();
      goto verificasolo;
   }

      if (umi > 805) { 
      delay (100);
      goto irrigacao;
      luminosidade();
      }
}

void luminosidade(){
  luz = analogRead(lumi);
  if (luz < 650) {
    digitalWrite(led, LOW); //liga
  }
  else if(luz > 651){
    digitalWrite(led, HIGH); //desliga
    }
}

void solo(){ 
    umi = (analogRead(umi1) + analogRead(umi2)) /2 ;
    
}

void umidade() {
      luminosidade();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Umidade: "); //IMPRIME O TEXTO NA SERIAL
      u = dht.readHumidity();
      lcd.print(u); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO
      lcd.print("%"); //IMPRIME O TEXTO NA SERIAL 
      lcd.setCursor(0,1);
      lcd.print("Temperatura: "); //IMPRIME O TEXTO NA SERIAL
      t = dht.readTemperature(), 0;
      lcd.print(t); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO E REMOVE A PARTE DECIMAL
      lcd.println("C"); //IMPRIME O TEXTO NA SERIAL
      digitalWrite(waterbomb, LOW);
      digitalWrite(buzzer, LOW);
      if (t >= 28) {
        digitalWrite(fan, LOW);
        }
      else {
        digitalWrite (fan, HIGH);
      }
      
      if (t >= 20 && t <= 40)
   {
        b5 = t - 10;
        bluetooth.println(b5)
    }
      delay(2000);
}
