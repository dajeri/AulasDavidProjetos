#include <Servo.h>

#define pinTRIG 2 // sensor 
#define pinECHO 3 // sensor 

int pulso; // saber quanto demora em ir e voltar
float distancia; // o valor de distancia do objeto ao sensor
float ultima_distancia; // o valor de distancia do objeto ao sensor

// Coeficientes de calibração do controlador PID
float kp = 1;
float ki = 1;
float kd = 1;

float I = 0; // Valor Integral
float velocidade = 0;

int Dmin = 2;
int Dmax = 24;

Servo myservo; // Objeto do servo motor

void setup() {
  Serial.begin(9600); //Comuicação

  // COnfiguro pinos entrada e saida
  pinMode(pinTRIG, OUTPUT);
  pinMode(pinECHO, INPUT);

  // Inicializo o pino trigger em 0 para nao medir nada ainda
  digitalWrite(pinTRIG, 0);

  // Configuro o pino do Servo
  myservo.attach(5);
  // Valor inicial do servo em repouso e/ou equilibrio
  myservo.writeMicroseconds(850);

  // Valores de funcionamento do sistema >> Angulos do motor
  // Min: 700
  // Equilibrio: 850
  // Max : 1050

  // Valores de funcionamento do sistema >> distancas no sensor
  // Min : 2Cm
  // Equilobrio : 13Cm
  // Max : 24Cm
}

void loop() {
  /*  1- Medir a distancia
      2- Medir velocidade
      3- Medir o erro
      4- Mover motor
  */

  // 1- Medir a distancia
  float vmdf = v_d(5); // Valor medido da distancia final
  Serial.println(vmdf, 1);

  // Testar mover o motor
  float ds = kp * vmdf + ki * velocidade + I;

  if (ds > 13) {
    // desçe a plataforma
    int aj_am = map(ds, 700, 1050, 2, 24);
    myservo.writeMicroseconds(850 - aj_am);
  } else if (ds < 13) {
    // sobe a plataforma
    int aj_am = map(ds, 700, 1050, 2, 24);
    myservo.writeMicroseconds(850 + aj_am);
  }

  delay(50);
}

/******/
// Funçõa para fazer a medida da distancia
/******/
float med_distant() {
  digitalWrite(pinTRIG, 1);
  delayMicroseconds(10);
  digitalWrite(pinTRIG, 0);
  pulso = pulseIn(pinECHO, 1);
  distancia = pulso * 0.017221195;
  ultima_distancia = distancia;
  med_velocidade(distancia);
  return distancia;
}

/******/
// Funçõa para fazer a medida da velocidade
/******/
void med_velocidade(float dist_act) {
  velocidade = dist_act - ultima_distancia;
  // Integral
  if (abs(dist_act) > Dmin && abs(dist_act) < Dmax) {
    I = I + dist_act * ki;
  } else {
    I = 0;
  }
}

/******/
// Funçõa para fazer a filtragem da medida da distancia
/******/
float v_d(int amos) {
  int dcal = 0;
  for (int i = 0; i < amos; i++) {
    dcal = dcal + med_distant();
  }
  float dis = dcal / amos;
  return dis;
}


void mover_motor(int pwm) {

  int mov = map(pwm, 0, 254, 0, 180);

  Serial.print(mov);
  Serial.println(" grados");
}
