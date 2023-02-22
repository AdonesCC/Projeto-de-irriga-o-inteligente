/* Código do projeto de um sistema de irrigação
   automatizado utilizando a plataforma blynk e ESP8266 ou ESP32.

   Autores(a): Prof Flávio Murilo, Adones, Rosiany, Junior.
   Centro de Ensino Tecnológico CENTEC - FATEC CARIRI
   data: 13/02/2023

   OBS: Pode-se utilizar tanto a placa ESP8266 quanto a ESP32,
   porém, deve-se modificar as configurações de placas no aplicativo Blynk.io.
   Assim como utilizar as bibliotecas específicas para cada placa.
*/

#include<WiFi.h>
#include<BlynkSimpleEsp32.h>

//configurações de pinagem do sensor ultrassônico
#define trig 19
#define echo 21

#define BLYNK_TEMPLATE_ID "TMPL8q2Fbbg0"
#define BLYNK_TEMPLATE_NAME "ESP32 Automação"
#define BLYNK_AUTH_TOKEN "  " //inserir o token fornecido pela plataforma Blynk

//credenciais da rede wifi
char ssid[] = " "; //inserir o nome da rede WiFi
char pass[] = " "; //inserir a senha da rede WiFi

//configurações do pino do réle
uint8_t pinRelay = 14;
uint8_t statePin = 1;
uint8_t forceRelay;
uint8_t value;

//variáveis para parâmetros da função  do sensor ultrassônico
long temp = 0;
uint8_t distance = 0;
uint8_t blynkDistance = 0;

//Nível máximo do reservatório
uint8_t nivelMaximo = 40;

//variável pino sensor
uint8_t pino_sensor = 32;

//variáveis temporizadoras
unsigned long temp_atual = 0;
unsigned long temp_anterior = 0;

/*criação de um objeto para receber os parâmetros do tempo de envio
dos dados*/
BlynkTimer timer;

void setup(){
  
  Serial.begin(115200);
  connectWifi(); //função de configuração de conexão do WiFi
  
  //inicialização do relé
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, statePin);
  
  //inicialização dos pinos do sensor ultrassônico
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);

  //conexão com o blynk.cloud
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80); //inicia o servidor

  //tempo de envio das instruções contidas nas funções utilizadas
  timer.setInterval(5000L, umidade);
  timer.setInterval(5000L, reservatorio);
  timer.setInterval(5000L, indicadoresReservatorio);
  timer.setInterval(5000L, porcentagemUmidadeSolo);
}

void loop(){
  
  //caso perca a conexão com a rede WiFi, tenta se reconectar
  if(WiFi.status() != WL_CONNECTED){
    
    connectWifi();
    return;
  } 
  acionamentoBomba(); //função que realiza a leitura e toma as decisões baseadas na umidade de solo e nível do reservatório
  indicadoresReservatorio(); //mostra na interface o nível do reservatório através de leds virtuais

  Blynk.virtualWrite(V1, digitalRead(pinRelay));
  Blynk.run(); // roda o servidor blynk
  timer.run(); //seta a função temporizadora de envio dos valores para o servidor blynk
}

void connectWifi(){
  
  Serial.print("Conectando a rede WiFi " + String(ssid));
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while(WiFi.status() != WL_CONNECTED){
    
    Serial.print(".");
    delay(500);
  } 
  Serial.print("\nConectado a rede WiFi: " + String(ssid) + " Com IP: " + WiFi.localIP().toString());
}

void acionamentoBomba(){
  
  temp_atual = millis();
  while((temp_atual - temp_anterior) > 5000){
    
    uint16_t umidadeSolo = leituraSensorUmidade();
    uint8_t  nivelAgua = nivelReservatorio(temp, distance, blynkDistance);

    if((umidadeSolo <= 1200)){

      if((nivelAgua > 20 && nivelAgua <= 40)){
      
        digitalWrite(pinRelay, LOW);
        Blynk.logEvent("temp_hight", String("O solo está seco!: ") + "Bomba acionada!");
      }
    }
    if(umidadeSolo > 1200 && umidadeSolo <= 2894){

      if(nivelAgua > 20 && nivelAgua <= 40){
        
        digitalWrite(pinRelay, forceRelay);
        Blynk.logEvent("temp_hight", String("O solo está umido") + umidadeSolo);
      }
    }
    if(umidadeSolo > 2894 || nivelAgua < 20){
      
      digitalWrite(pinRelay, HIGH);
      Blynk.logEvent("temp_hight", String("O solo está irrigado!: ") + "A bomba foi desligada.");
    }
    temp_anterior = temp_atual;
  }
}

float leituraSensorUmidade(){ //função com retorno
  
  float valor_sensor = 0;  
  valor_sensor = analogRead(pino_sensor);

  return valor_sensor;
}

void porcentagemUmidadeSolo(){

  float porcen_umidit = map(leituraSensorUmidade(), 0, 4095, 0, 100);
  Blynk.virtualWrite(V5, porcen_umidit);
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
  
  float umidit = leituraSensorUmidade();
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
  
  uint8_t pinsSync[] = {V0, V1, V2, V3, V4, V5};

  for(uint8_t i = 0; i < 6; i++){
    
    Blynk.syncVirtual(pinsSync[i]);
  }
}
