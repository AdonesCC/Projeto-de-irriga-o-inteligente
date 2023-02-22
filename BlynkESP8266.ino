/* Código do projeto de um sistema de irrigação
   automatizado utilizando a plataforma blynk e ESP8266 ou ESP32.

   Autores(a): Prof Flávio Murilo, Adones, Rosiany, Junior.
   Centro de Ensino Tecnológico CENTEC - FATEC CARIRI
   data: 13/02/2023

   OBS: Pode-se utilizar tanto a placa ESP8266 quanto a ESP32,
   poré m, deve-se modificar as configurações de placas no aplicativo Blynk.io.
 */

#include<ESP8266WiFi.h>
#include<BlynkSimpleEsp8266.h>

//configurações de pinagem do sensor ultrassônico
#define trig D6
#define echo D7

//credenciais do blynk e da rede wifi
char auth[] = " "; //adicionar o token fornecido pela plataforma blynk
char ssid[] = " "; //adicionar o nome da rede WiFi
char pass[] = " "; //adicionar a senha da rede WiFi

//configurações do pino do réle
uint8_t pinRelay = D0;
uint8_t statePin = 1;
uint8_t forceRelay;
uint8_t value;

//variáveis sensor ultrassônico
long temp = 0;
uint8_t distance = 0;
uint8_t blynkDistance = 0;
uint8_t nivelMaximo = 40;

//variável pino sensor
uint8_t pino_sensor = A0;

//variáveis temporizadoras
unsigned long temp_atual = 0;
unsigned long temp_anterior = 0;

/*criação de um objeto para receber os parâmetros do tempo de envio
dos dados*/
BlynkTimer timer;

void setup(){
  
  Serial.begin(115200);
  setupWifi();

  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, statePin);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(trig, LOW);
   
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80); //inicia o servidor
  timer.setInterval(5000L, umidade);
  timer.setInterval(5000L, reservatorio);
  timer.setInterval(5000L, indicadoresReservatorio);
}

void loop(){
  
  acionamentoBomba(); //função que realiza a leitura e toma as decisões baseadas na umidade de solo e nível do reservatório
  indicadoresReservatorio();

  Blynk.virtualWrite(V1, digitalRead(pinRelay));
  Blynk.run(); // roda o servidor blynk
  timer.run(); //seta a função temporizadora de envio dos valores para o servidor blynk
}

void setupWifi(){
  
  Serial.print("Conectando a rede WiFi " + String(ssid));
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while(WiFi.status() != WL_CONNECTED){
    
    Serial.print(".");
    delay(500);
  } 
  if(WiFi.status() != WL_CONNECTED){

    return;
  } 
  Serial.print("\nConectado a rede WiFi: " + String(ssid) + " Com IP: " + WiFi.localIP().toString());
}

void acionamentoBomba(){
  
  temp_atual = millis();
  while((temp_atual - temp_anterior) > 5000){
    
    temp_anterior = temp_atual;

    uint16_t umidadeSolo = leituraSensorUmidade();
    uint8_t  nivelAgua = nivelReservatorio(temp, distance, blynkDistance);

    if((umidadeSolo <= 300)){

      if((nivelAgua > 20 && nivelAgua <= 40)){
      
        digitalWrite(pinRelay, LOW);
        digitalWrite(D1, HIGH);
        digitalWrite(D2, LOW);
        Blynk.logEvent("temp_hight", String("O solo está seco!: ") + "Bomba acionada!");
      }
    }
    if(umidadeSolo > 300 && umidadeSolo <= 723){

      if(nivelAgua > 20 && nivelAgua <= 40){
        
        digitalWrite(pinRelay, forceRelay);
        digitalWrite(D1, LOW);
        Blynk.logEvent("temp_hight", String("O solo está umido") + umidadeSolo);
      }
    }
    if(umidadeSolo > 723 || nivelAgua < 20){
      
      digitalWrite(pinRelay, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D1, LOW);
      Blynk.logEvent("temp_hight", String("O solo está irrigado!: ") + "A bomba foi desligada.");
    }
  }
}

uint16_t leituraSensorUmidade(){ //função com retorno
  
  uint16_t valor_sensor = 0;  
  valor_sensor = analogRead(pino_sensor);

  return valor_sensor;
}

uint8_t nivelReservatorio(long tempo, uint8_t distancia, uint8_t blynkDistancia){
  
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  delayMicroseconds(10);

  tempo = pulseIn(echo, HIGH);
  distancia = tempo / 29 / 2;
  blynkDistancia = (distancia - nivelMaximo) * -1;

  return blynkDistancia;
}

//função de envio dos dados para o pino virtual
void umidade(){
  
  uint16_t umidit = leituraSensorUmidade();
  Blynk.virtualWrite(V0, umidit);
}

void reservatorio(){
  
  uint8_t distancia = nivelReservatorio(temp, distance, blynkDistance);

  if(distancia <= nivelMaximo){

    Blynk.virtualWrite(V2, distancia);  

  }else{

    Blynk.virtualWrite(V2, 0);
  }
}

void indicadoresReservatorio(){

  uint8_t indicarNivel = nivelReservatorio(temp, distance, blynkDistance);

  if(indicarNivel < 20){

    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 1);

  }else if(indicarNivel > 20  && indicarNivel <= 40){

    Blynk.virtualWrite(V3, 1);
    Blynk.virtualWrite(V4, 0);
  }
}

//configuração do pino virtual da interface blynk
BLYNK_WRITE(V1){

  value = param.asInt();
  digitalWrite(pinRelay, value);
  forceRelay = value;
}

/*conservação dos estados dos pinos em caso de queda de energia
ou desligamento*/
BLYNK_CONNECTED(){
  
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
}
