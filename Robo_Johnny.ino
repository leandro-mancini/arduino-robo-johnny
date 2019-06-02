// DEFINIÇÕES DE BIBLIOTECAS.
#include <Servo.h>  // inclui biblioteca de manipulação de servos motores. 
#include <HCSR04.h> // inclui biblioteca ultrasonico.

// DEFINIÇÕES DE PIENOS
#define pinTrig A0
#define pinEcho A1
#define pinBuzzer A2
#define pinServo 10
#define pinMotor1PWM 11
#define pinMotor2PWM 3

// DEFINIÇÕES DE PARAMETROS
#define distObstaculo 30 // DISTANCIA EM CM PARA CONSIDERAR
#define tempoLeitura 50 // INTERVALO MINIMO ENTRE CADA LEITURA
#define medidaDist 5 // QUANTIDADE DE MEDIDAS QUE SERAO ARMAZENADAS

#define DEBUG // DEIXAR DESCOMENTADO PARA RECEBER MAIS INFORMAÇÕES NO MONITOR SERIAL

// DECLARAÇÃO DE VARIAVEIS
bool obstaculo = false;
bool novaLeitura;
bool medidaSensor[medidaDist]; // ARRAY PARA ARMAZENAR OS ULTIMOS VALORES
byte posicao;
byte contaObstaculo = 0;
unsigned long controleLeitura; // VARIAVEL PARA CONTROLAR TEMPO ENTRE AS LEITURAS
int angulo = 90;
long duracao;
long distanciaCm = 0;

// INSTANCIANDO OBJETOS
HCSR04 sensorHCSR04(pinTrig, pinEcho);
Servo servo;

// INSTANCIANDO METODOS
bool validarLeitura(int tempo);
bool validarParada(bool freio);
void ligarBuzzer(bool freio);
void posicionaCarroMelhorCaminho();

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("| ============================================ |"));
    Serial.println(F("| Arduino com sensor de obstáculo Ultrassonico |"));
    Serial.println(F("| ============================================ |"));
  #endif

  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinMotor1PWM, OUTPUT);
  pinMode(pinMotor2PWM, OUTPUT);
  
  servo.attach(pinServo);

  servo.write(angulo);

  for (byte i = 0; i < medidaDist; i++) {
    medidaSensor[i] = 0;

    #ifdef DEBUG
      Serial.print("Setando posição ");
      Serial.print(i);
      Serial.println(" do array como");
    #endif
  }

  posicao = 0;

  #ifdef DEBUG
    Serial.println("Fim do Setup");
  #endif
}

void loop() {
  pensar();
}

void pensar() {
  reposicionaServoSonar(); //Coloca o servo para olhar a frente

  int distancia = lerSonar(); // Ler o sensor de distância

  Serial.print("distancia em cm: "); 
  Serial.println(distancia);   // Exibe no serial monitor 

  if (distancia > distObstaculo) {  // Se a distância for maior que 20 cm  
    //rotacao_Frente(); //robô anda para frente 
  } else {
    //rotacao_Parado();  //para o robô
    posicionaCarroMelhorCaminho();
    pensar();
  }
}

bool validarLeitura(int tempo) {
  novaLeitura = false;
  
  if (millis() - controleLeitura > tempo) {
    if (sensorHCSR04.dist() <= distObstaculo) {
      obstaculo = true;
      novaLeitura = true;
    } else {
      obstaculo = false;
      novaLeitura = true;
    }

    controleLeitura = millis();
  }

  return novaLeitura;
}

bool validarParada() {
  static bool parar = false;
  
  if (contaObstaculo >= ((medidaDist/2) + 1)) {
    if (parar == false) {
      parar = true;

      Serial.println("PARA!!!");

      posicionaCarroMelhorCaminho();
    }
  } else {
    if (parar == true) {
      parar = false;

      Serial.println("Caminho Livre");

      // digitalWrite(pinBuzzer, LOW);
    }
  }
}

void ligarBuzzer(bool freio) {
  if (freio) {
    tone(pinBuzzer, 1000, 100);
  } else {
    digitalWrite(pinBuzzer, LOW);
  }
}

void posicionaCarroMelhorCaminho() {
  char melhorDist = calculaMelhorDistancia();
  
  Serial.print("Melhor Distancia em cm: ");
  Serial.println(melhorDist);

  switch(melhorDist) {
    case 'c':
      pensar();
      break;
    case 'd':
      Serial.println("Virar a direita");
      //rotacao_Direita();
      break;
    case 'e':
      //rotacao_Esquerda();
      Serial.println("Virar a esquerda");
      break;
  }

  reposicionaServoSonar();
}

char calculaMelhorDistancia() {
  char melhorDistancia = '0';
  int maiorDistancia = 0;

  int esquerda = calcularDistanciaEsquerda();
  int centro = calcularDistanciaCentro();
  int direita = calcularDistanciaDireita();

  reposicionaServoSonar();

  if (centro > direita && centro > esquerda){
    melhorDistancia = 'c';    
    maiorDistancia = centro; 
  } else if (direita > centro && direita > esquerda){
    melhorDistancia = 'd';
    maiorDistancia = direita;
  } else if (esquerda > centro && esquerda > direita){
    melhorDistancia = 'e';
    maiorDistancia = esquerda;
  }

  if (maiorDistancia <= distObstaculo) { //distância limite para parar o robô
    // rotacao_Re();    
    // posicionaCarroMelhorCaminho();
  }

  reposicionaServoSonar();

  return melhorDistancia;
}

int calcularDistanciaCentro() {
  servo.write(90);
  
  delay(20);
  
  int leituraDoSonar = lerSonar();  // Ler sensor de distância
  
  delay(500);
  
  leituraDoSonar = lerSonar();
  
  delay(500);
  
  Serial.print("Distancia do Centro: "); // Exibe no serial
  Serial.println(leituraDoSonar);
  
  return leituraDoSonar;       // Retorna a distância
}

int calcularDistanciaDireita() {
  servo.write(0);
  
  delay(200);
  
  int leituraDoSonar = lerSonar();
  
  delay(500);
  
  leituraDoSonar = lerSonar();
  
  delay(500);
  
  Serial.print("Distancia da Direita: ");
  Serial.println(leituraDoSonar);
  
  return leituraDoSonar;
}

int calcularDistanciaEsquerda() {
  servo.write(180);
  
  delay(200);
  
  int leituraDoSonar = lerSonar();
  
  delay(500);
  
  leituraDoSonar = lerSonar();
  
  delay(500);
  
  Serial.print("Distancia Esquerda: ");
  Serial.println(leituraDoSonar);
  
  return leituraDoSonar;
}

int lerSonar() {
  digitalWrite(pinTrig, LOW); // não envia som
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH); // envia som
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW); //não envia o som e espera o retorno do som enviado
  
  duracao = pulseIn(pinEcho, HIGH); //Captura a duração em tempo do retorno do som.

  distanciaCm = duracao/56; //Calcula a distância;
  
  return distanciaCm;
}

void reposicionaServoSonar() {
  servo.write(90);
  
  delay(200);
}
