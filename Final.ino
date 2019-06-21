

#include "DualVNH5019MotorShield.h" // Inclusão da biblioteca para controle dos motores

unsigned long tempo1, tempo2;
DualVNH5019MotorShield motor1;
// Seguidor de linha
#define Sensor_1 A11  
#define Sensor_2 A12
#define Sensor_3 A10

// Sonar
#define trigPin1 30    //meio
#define echoPin1 31
#define trigPin2 41    //esquerda
#define echoPin2 40
#define trigPin3 32   //direita
#define echoPin3 33

int i=0;
int sonarAtivo = 0;

void stopIfFault()  // Teste dos motores
{
  if (motor1.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (motor1.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}


void setup() {  // Setup do bit rate da placa, definição dos pinos de entrada e saída de sinal e inicialização dos motores

  Serial.begin (9600);  // Setup do bit rate da placa
  // Sensores Fotovoltaicos
  pinMode(Sensor_1, INPUT);
  pinMode(Sensor_2, INPUT);
  pinMode(Sensor_3, INPUT);
  // Sonares
  pinMode(trigPin1, OUTPUT); // Definição do pino de saída do sinal
  pinMode(echoPin1, INPUT);  // Definição do pino de entrada do sinal
  pinMode(trigPin2, OUTPUT); // Definição do pino de saída do sinal
  pinMode(echoPin2, INPUT);  // Definição do pino de entrada do sinal
  pinMode(trigPin3, OUTPUT); // Definição do pino de saída do sinal
  pinMode(echoPin3, INPUT);  // Definição do pino de entrada do sinal
  motor1.init();  // Inicialização dos motores
}

void loop() {

  int RoverSpeed = 40;
  int Sensor_R = 0;
  int Sensor_M = 0;
  int Sensor_L = 0;
  
  Sensor_M = digitalRead(Sensor_1);   // meio
  Sensor_R = digitalRead(Sensor_2);   // direita
  Sensor_L = digitalRead(Sensor_3);   //esquerda

  if ((Sensor_M == 1) && (Sensor_R == 0) && (Sensor_L == 1) || (Sensor_M == 0) && (Sensor_R == 0) && (Sensor_L == 1))// curva a direita
  {
    //Serial.println("CURVA DIREITA");
    motor1.setM1Speed(-(RoverSpeed-15));
    motor1.setM2Speed(-(RoverSpeed+20));
    Sonar();
  }
  else if ((Sensor_M == 1) && (Sensor_R == 1) && (Sensor_L == 0) || (Sensor_M == 0) && (Sensor_R == 0) && (Sensor_L == 1)) // curva a esquerda
  {
    //Serial.println("CURVA ESQUERDA");
    motor1.setM1Speed(-(RoverSpeed+25));
    motor1.setM2Speed(-(RoverSpeed-20));
    Sonar();
  }
  else if ((Sensor_M == 0) && (Sensor_L == 1) && (Sensor_R == 1)) // reto
  {
    //Serial.println("RETO");
    motor1.setM1Speed(-(RoverSpeed+5));
    motor1.setM2Speed(-(RoverSpeed+5));
    Sonar();
  }
  else if ((Sensor_M == 0) && (Sensor_L == 0) && (Sensor_R == 0))
  {
    Sonar();
  }
  if(sonarAtivo == 1)
    Sonar();
}

unsigned long range (byte trig, byte echo)
{
digitalWrite(trig, LOW);
delayMicroseconds(2);
digitalWrite(trig, HIGH);
delayMicroseconds(10);
digitalWrite(trig, LOW);
unsigned long duration = pulseIn(echo, HIGH);
return (duration/58.2);
} 



void Sonar(){
  
  tempo1 = millis();

inicioSonar:
  
  long duration1, duration2, duration3;  // Declaração das variáveis utilizadas
  int vetorRuido1[2], vetorRuido2[2], vetorRuido3[2];
  int Ruido1, Ruido2, Ruido3;
  Ruido1 = 100;
  Ruido2 = 100;
  Ruido3 = 100;
  double distanciaF, distanciaE, distanciaD;
  
  // Valores máximos(cm)
  int MaxF = 40;
  int MaxE = 30;
  int MaxD = 30;

  sonarAtivo = 0;
  
    distanciaF = range(trigPin1, echoPin1);
    if (distanciaF > 1023 || distanciaF == 0){
      distanciaF = Ruido1;
    }
    else{
      Ruido1 = distanciaF;
    }
    delay(50);  
    
    distanciaE = range(trigPin2, echoPin2);
    if (distanciaE > 1023 || distanciaE == 0){
      distanciaE = Ruido2;
    }
    else{
      Ruido2 = distanciaE;
    }
    delay(50);    
    
    distanciaD = range(trigPin3, echoPin3);
    if (distanciaD > 1023 || distanciaD == 0){
      distanciaD = Ruido3;
    }
    else{
      Ruido3 = distanciaD;
    }
    delay(50);
  /* Tempo = Largura do Pulso em Echo, em micro segundos
     Logo, Distância em centímetros = Tempo / 58
     ou Distância em polegadas = Tempo / 148
     Ou você pode usar a velocidade do som, que é de 340m/s. 
     d = v.t/2, v = 340 m/s, t = tempo, divide por 2 pois o sinal deve ir e voltar*/
  
  if (distanciaF < MaxF && distanciaE < MaxE && distanciaD < MaxD) {  // Se a distância dos 3 for menor que 40cm, dá ré para esquerda
    motor1.setM1Speed(0); 
    motor1.setM2Speed(78);
    //Serial.println("todos menor");
  }

  else if (distanciaF < MaxF){

     if (distanciaD >= distanciaE){
      motor1.setM1Speed(50);
      motor1.setM2Speed(0);
      //Serial.println("direita maior que esquerda");
     }

     else{
      motor1.setM1Speed(0);
      motor1.setM2Speed(50);
      //Serial.println("esquerda maior que direita");
     }
     sonarAtivo = 1;
  }
  
  else if (distanciaE < MaxE){   // Caso a distância esquerda seja menor que 12cm

    if (distanciaD > distanciaE){   // Se distancia direita é maior que todas, vira para direita
      motor1.setM1Speed(60);
      motor1.setM2Speed(0);
    }

    else{                       // Distancia direita igual ou menor que esquerda
      motor1.setM1Speed(60);
      motor1.setM2Speed(60);
    }
    
    sonarAtivo = 1;
  }

  else if (distanciaD < MaxD){   // Caso a distancia direita seja menor que 12cm
    
    if (distanciaE > distanciaD){   // Se distancia esquerda é maior que todas, vira para esquerda
      motor1.setM1Speed(0);
      motor1.setM2Speed(70);
    }

    else{
      motor1.setM1Speed(60);
      motor1.setM2Speed(60);
    }

    sonarAtivo = 1;
  }

  else{
    sonarAtivo = 0;
  }

  if (sonarAtivo == 0){
    camera();
  }
  
  else if (sonarAtivo == 1){
    goto inicioSonar;
    delay(10);
  }

  delay(50);
}

void camera() {
  
  if (Serial.available()) //se byte pronto para leitura
  {
    switch (Serial.read())     //verifica qual caracter recebido
    {

      case '1':
        //Serial.println("CURVA ESQUERDA RAPIDO");
        motor1.setM1Speed(-60);
        motor1.setM2Speed(+20);
        break;

      case '2':
        //Serial.println("CURVA ESQUERDA DEVAGAR");
        motor1.setM1Speed(-60);
        motor1.setM2Speed(0);
        break;

      case '3':
        //Serial.println("RETO");
        motor1.setM1Speed(-60);
        motor1.setM2Speed(-50);
        break;

      case '4':
        //Serial.println("CURVA DIREITA DEVAGAR");
        motor1.setM1Speed(0);
        motor1.setM2Speed(-60);
        break;

      case '5':
        //Serial.println("CURVA DIREITA RAPIDO");
        motor1.setM1Speed(+20);
        motor1.setM2Speed(-60);
        break;
    }
  }
}
