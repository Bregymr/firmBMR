#include "Nodo.h"

Nodo::Nodo(int direccion, unsigned char tag){
  this->tag = tag;
  this->direccion = direccion;

}
Nodo::Nodo(int direccion){
  if(this->contador=='X'){
    this->contador++;//X, Y, Z
    this->tag = this->contador;
  }else{
    this->contador = 'X';
    this->tag = this->contador;
  }
  this->direccion = direccion;

}
Nodo::Nodo(int direccion, unsigned char tag, float posicionInicial, float velocidadInicial, float aceleracionInicial){
  this->tag = tag;
  this->direccion = direccion;

}
Nodo::Nodo(int direccion, int quequePin, unsigned char tag){
  this->tag = tag;
  this->direccion = direccion;
  this->quequePin = quequePin;
}
void Nodo::setVelocidad(float velocidad){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x51);
  Wire.write(0x08);
  char valorChar[8];
  Wire.write(dtostrf(velocidad, -5, 2, valorChar));
  this->velocidad = velocidad;
  Wire.endTransmission();
}
float Nodo::getVelocidad(){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x50);
  Wire.write(0x08);
  Wire.endTransmission();
  Wire.requestFrom(this->direccion,5);
  char lectura[8];
  int i = 0;
  while(Wire.available()>0){
      lectura[i] = Wire.read();
      i++;
  }
  
  float valor = atof(lectura);
  this->velocidad = valor;
  return valor;
}
void Nodo::setPosicion(float posicion){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x51);
  Wire.write(0x04);
  char valorChar[8];
  Wire.write(dtostrf(posicion, -5, 2, valorChar));
  this->posicion = posicion;
  Wire.endTransmission();
}
float Nodo::getPosicion(){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x50);
  Wire.write(0x04);
  Wire.endTransmission();
  Wire.requestFrom(this->direccion,5);
  char lectura[8];
  int i = 0;
  while(Wire.available()>0){
      lectura[i] = Wire.read();
      i++;
  }
  
  float valor = atof(lectura);
  this->posicion = valor;
  return valor;
}

void Nodo::setAceleracion(float aceleracion){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x51);
  Wire.write(0x0C);
  char valorChar[8];
  Wire.write(dtostrf(aceleracion, -5, 2, valorChar));
  this->aceleracion = aceleracion;
  Wire.endTransmission();
}
float Nodo::getAceleracion(){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x50);
  Wire.write(0x0C);
  Wire.endTransmission();
  Wire.requestFrom(this->direccion,5);
  char lectura[8];
  int i = 0;
  while(Wire.available()>0){
      lectura[i] = Wire.read();
      i++;
  }
  
  float valor = atof(lectura);
  this->aceleracion = valor;
  return valor;
}

void Nodo::moverMm(float magnitud){
  float posicionActual = Nodo::getPosicion();
  this->posicion = posicionActual;
  float posicionFinal = posicionActual + magnitud;
  Nodo::setPosicion(posicionFinal);
  this->posicion = posicionFinal;

}
void Nodo::rotar(float angulo, int sistemaNumerico){
  float gradosFinales = 0.0;
  if (sistemaNumerico == 0){
    gradosFinales = angulo;
  }else if(sistemaNumerico == 1){
    gradosFinales = angulo*57.2958;
  }
  Wire.beginTransmission(this->direccion);
  Wire.write(0x51);
  Wire.write(0x33);
  char valorChar[8];
  Wire.write(dtostrf(gradosFinales, -5, 2, valorChar));
  Wire.endTransmission();

}

void Nodo::backup(){
  Wire.beginTransmission(this->direccion);
  Wire.write(0x51);
  Wire.write(0x98);
  Wire.endTransmission();
}

void Nodo::actualizarDatos(){
  this->posicion = Nodo::getPosicion();
  this->aceleracion = Nodo::getAceleracion();
  this->velocidad = Nodo::getVelocidad();

}

