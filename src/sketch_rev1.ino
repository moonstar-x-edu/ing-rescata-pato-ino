#include <Servo.h>

// Variables Iniciales
Servo servoBrazo, servoPinza;
int posServoBrazo = 0;
int posServoPinza = 0;
int pinServoBrazo = 5;
int pinServoPinza = 6;
int trigger = 4;
int echo = 2;
bool enRescate = false;
double tiempoSonido, alturaMedida;
int alturaDefecto = 15; // Por definir
int alturaPato = alturaDefecto - 7; // Solo pato, sin contar plataforma de elevacion ni edificio.
int alturaSeguridad = alturaDefecto - 2;

void setup() {
  Serial.begin(9600);
  servoBrazo.attach(pinServoBrazo);
  servoPinza.attach(pinServoPinza);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  Serial.println("Empieza Medicion.");
  if (!enRescate) {
    medicionConstante(alturaPato);
  } else {
    medicionConstante(alturaSeguridad);
  }
  controlServos(enRescate);
  enRescate = !enRescate;
  Serial.print("Valor de Rescate: ");
  Serial.println(enRescate);
  Serial.println("Esperando...");
  // Darle tiempo al vehiculo de retroceder antes de que vuelva a medir valores de altura.
  for (int i = 1; i < 6; i++) {
    Serial.println(i);
    delay(1000);
  }
}

/*
  Esta funcion se ocupa de realizar las mediciones de altura del sensor de distancia con relacion al suelo.
  Como forma de verificacion, se imprime en la consola serial la altura que mide el sensor. Todo esto se encuentra
  dentro de un loop while() para asegurarse de que el sensor mida al menos cinco valores positivos para poder accionar
  los servos (de manera a evitar un falso positivo). La altura que necesita medir para poder accionar los servos
  corresponde a un valor inferior a la altura relativa al suelo (el valor por defecto) y la altura del objeto que
  se mide: el pato o la zona de seguridad, este objeto se pasa como argumento de la funcion.
*/
void medicionConstante(int alturaPorMedir) {
  int i = 0;
  while (i < 3) {
    digitalWrite(trigger, HIGH);
    delay(1000);
    digitalWrite(trigger, LOW);
    tiempoSonido = pulseIn(echo, HIGH);
    alturaMedida = tiempoSonido/2*0.0343;

    Serial.print("Altura: ");
    Serial.println(alturaMedida);

    if (alturaMedida < alturaDefecto && alturaMedida >= alturaPorMedir) {
      i++;
    } else {
      i = 0;
    }
  }
  return;
}

/*
  Esta funcion se ocupa de controlar los motores servo: el brazo y la pinza. Acepta un argumento booleano el cual
  corresponde al valor del estado del vehiculo (si se encuentra en rescate o no). EL primer loop baja el brazo, el
  segundo loop cierra la pinza si no se encuentra en rescate y la abre si se encuentra en rescate, el ultimo loop
  eleva el brazo.
*/
void controlServos(bool estado) {
  Serial.println("Empieza Movimiento.");
  Serial.println("Extension Brazo...");
  for (posServoBrazo = 0; posServoBrazo <= 90; posServoBrazo++) {
    servoBrazo.write(posServoBrazo);
    delay(50);
  }
  delay(1000);
  if (!estado) {
    Serial.println("Cerrando Pinza...");
    for (posServoPinza = 0; posServoPinza <= 180; posServoPinza++) {
      servoPinza.write(posServoPinza);
      delay(20);
    }
  } else {
    Serial.println("Abriendo Pinza...");
    for (posServoPinza = 180; posServoPinza >= 0; posServoPinza--) {
      servoPinza.write(posServoPinza);
      delay(20);
    }
  }
  delay(1000);
  Serial.println("Retraccion Brazo...");
  for (posServoBrazo = 90; posServoBrazo >= 0; posServoBrazo--) {
    servoBrazo.write(posServoBrazo);
    delay(50);
  }
  Serial.println("Termina Movimiento.");
  return;
}
