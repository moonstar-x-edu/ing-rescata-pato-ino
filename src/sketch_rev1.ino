#include <Servo.h>

// Variables Iniciales
Servo servoBrazo, servoPinza;
int alphaMinServoBrazo = 0; // Abajo
int alphaMinServoPinza = 0; // Abierto
int alphaMaxServoBrazo = 45; // Arriba
int alphaMaxServoPinza = 40; // Cerrado

int posServoBrazo = 0;
int posServoPinza = 0;
int pinServoBrazo = 5;
int pinServoPinza = 6;

int trigger = 4;
int echo = 2;
double VEL_SND = 0.0343;

double tiempoSonido, alturaMedida;
int alturaDefecto = 15; // Por definir
int alturaPato = alturaDefecto - 7; // Solo pato, sin contar plataforma de elevacion ni edificio.
int alturaSeguridad = alturaDefecto - 2;

bool enRescate = false;
int tiempoPausa = 8;
int medicionesRangoSeguro = 3;

void setup() {
  Serial.begin(9600);
  servoBrazo.attach(pinServoBrazo);
  servoPinza.attach(pinServoPinza);
  servoBrazo.write(alphaMinServoBrazo);
  servoPinza.write(alphaMinServoPinza);
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
  for (int i = 1; i < tiempoPausa; i++) {
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
  while (i < medicionesRangoSeguro) {
    digitalWrite(trigger, HIGH);
    delay(1000);
    digitalWrite(trigger, LOW);
    tiempoSonido = pulseIn(echo, HIGH);
    alturaMedida = tiempoSonido / 2 * VEL_SND;

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
  for (posServoBrazo = alphaMinServoBrazo; posServoBrazo <= alphaMaxServoBrazo; posServoBrazo++) {
    servoBrazo.write(posServoBrazo);
    delay(50);
  }
  delay(1000);
  if (!estado) {
    Serial.println("Cerrando Pinza...");
    for (posServoPinza = alphaMinServoPinza; posServoPinza <= alphaMaxServoPinza; posServoPinza++) {
      servoPinza.write(posServoPinza);
      delay(20);
    }
  } else {
    Serial.println("Abriendo Pinza...");
    for (posServoPinza = alphaMaxServoPinza; posServoPinza >= alphaMinServoPinza; posServoPinza--) {
      servoPinza.write(posServoPinza);
      delay(20);
    }
  }
  delay(1000);
  Serial.println("Retraccion Brazo...");
  for (posServoBrazo = alphaMaxServoBrazo; posServoBrazo >= alphaMinServoBrazo; posServoBrazo--) {
    servoBrazo.write(posServoBrazo);
    delay(50);
  }
  Serial.println("Termina Movimiento.");
  return;
}
