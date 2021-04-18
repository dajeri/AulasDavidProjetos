float Kp =2; //2
float Kd = 35; //35
float Ki =0.1; //0.1

int Rint = 8;
int Rext = 40;

unsigned long time = 0; // tiempo de ejecucion del ultimo ciclo
int period = 50; // Periodo de muestreo en ms
int sensorPin=0; //Pin Analogico donde esta conectada la señal del Sensor de distancia
int measure; // Lo que mide el sensor. Son ADCs.
int dcal [] = { // Calibracion de ADC a Distancia
  -193, -160, -110, -60, 0, 40, 60, 90, 120};
int ADCcal [] = {
  177, 189, 231, 273, 372, 483, 558, 742, 970};
  
int lastDist; // Valor anterior de Distancia para calcular la Velocidad
int dist; // distancia en mm con el 0 en el centro de la barra
int nvel=5; //  numero de valores de velocidad sobre los que calculamos la media
int v[5];
int vel; // valor medio de las nvel velocidades ultimas
float I; // Valor Integral

#include <Servo.h> 
Servo myservo;  // create servo object to control a servo
float pos;
float reposo = 1350; // valor que mantiene la barra horizontal

int ledPin=13; // Pin de led verde.

void setup() 
{ 
  analogReference(EXTERNAL); // AREF conectado a 3.3V
  myservo.attach(12);  // attaches the servo on pin X to the servo object 
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
} 

void loop() 
{ 
  if (millis()>time+period){ // ¿Ha transcurrido el periodo?
    time = millis();

    // Medimos DISTANCIA
    measure = analogRead(sensorPin);
    measure = constrain(measure, ADCcal[0], ADCcal[8]);
    lastDist = dist; // Guardamos el valor anterior de dist para calcular la velocidad
    for(int i =0; i<8; i++){ // Aplicamos curva de Calibracion de ADC a mm
      if (measure >= ADCcal[i] && measure< ADCcal[i+1]){
        dist = map(measure,ADCcal[i],ADCcal[i+1],dcal[i],dcal[i+1]);
      }      
    }
    // Calculo de la media de la VELOCIDAD
    for (int i=0; i<nvel-1; i++){ // Movemos todas hacia la izq para dejar libre la ultima.
      v[i] =v[i+1];
    }
    v[nvel-1]= (dist - lastDist); // Ponemos un dato nuevo
    vel=0;
    for (int i=0; i<nvel; i++){ // Calculamos la media
      vel = vel+ v[i];
    }
    vel = vel/nvel;
    // Integral
    if(abs(dist)>Rint && abs(dist)<Rext){ // Solo si esta dentro de (-Rext,Rext) y fuera de (-Rint,Rint)
      I=I+dist*Ki;
    } 
    else {
      I=0;
    }
    // Calculamos posicion del servo
    pos=Kp*dist+Kd*vel+I;
    myservo.writeMicroseconds(reposo+pos); 

    if(abs(dist)<Rint){ // Si estamos dentro de Rint enciende Led
      digitalWrite(ledPin,HIGH);
    }
    else{
      digitalWrite(ledPin,LOW);
    }

    if(1){ //Envio para PROCESSING
      Serial.print(dist+200);
      Serial.print(",");
      Serial.print(dist+200);
      Serial.print(",");   
      Serial.print(vel);
      Serial.print(",");
      Serial.print(vel);
      Serial.print("$");
    }
    if(0){ // Debug
      Serial.print(millis());
      Serial.print(" ms|dist: ");
      Serial.print(dist);
      Serial.print("|vel: ");
      Serial.print(vel);
      Serial.print("|Kp*dist: ");
      Serial.print(Kp*dist);
      Serial.print("|Kd*vel: ");
      Serial.print(Kd*vel);    
      Serial.print("|Int: ");
      Serial.print(I);    
      Serial.print("|pos: ");
      Serial.println(pos);
    }
    if(0){// Para calibrar sensor de Distancia
      Serial.print(dist);
      Serial.print("mm     ADC: ");    
      Serial.println(measure); 
    }
    if(0){ // DeBug Velocidades
      for (int i=0; i<(nvel); i++){
        Serial.print(v[i]);
        Serial.print(",");
      }
      Serial.print("       vel:");
      Serial.println(vel);
    }
  }
}
