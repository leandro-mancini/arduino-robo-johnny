// DEFINIÇÕES DE BIBLIOTECAS.
#include <Servo.h>  // inclui biblioteca de manipulação de servos motores. 
#include <HCSR04.h> // inclui biblioteca ultrasonico.

// DEFINIÇÕES DE PIENOS
#define pinTrig 8
#define pinEcho 9
#define pinBuzzer 10

// DEFINIÇÕES DE PARAMETROS
#define distObstaculo 10 // DISTANCIA EM CM PARA CONSIDERAR
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

// INSTANCIANDO OBJETOS
HCSR04 sensorHCSR04(pinTrig, pinEcho);

// INSTANCIANDO METODOS
bool validarLeitura(int tempo);
bool validarParada(bool freio);
void ligarBuzzer(bool freio);

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("| ============================================ |"));
    Serial.println(F("| Arduino com sensor de obstáculo Ultrassonico |"));
    Serial.println(F("| ============================================ |"));
  #endif

  pinMode(pinBuzzer, OUTPUT);

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
  // VALIDAR LEITURA
  validarLeitura(tempoLeitura);

  if (validarLeitura) {
    medidaSensor[posicao] = obstaculo;
    posicao++;

    for (byte i = 0; i < medidaDist; i++) {
      if (medidaSensor[i] == 1) contaObstaculo++;
    }

    validarParada();

    ligarBuzzer(validarParada());

    contaObstaculo = 0;

    if(posicao > medidaDist) {
      posicao = 0;
    }
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

      // tone(pinBuzzer, 2500, 100);
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
    tone(pinBuzzer, 2500, 100);
  } else {
    digitalWrite(pinBuzzer, LOW);
  }
}
