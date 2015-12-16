#include "CNC.h"


CNC::CNC(char nombre, int baudeaje){
  this->nombre = nombre;
  this->baudeaje = baudeaje;
}
CNC::CNC(int baudeaje){
  this->nombre = 'M';
  this->baudeaje = baudeaje;
}

void CNC::tipoDeCNC(int modo){
  this->modoDeOperacion = modo;
}
void CNC::anadirEje(Nodo &nodo){
  for(int i=0; i<8;i++){
      if(this->nodos[i]){
        i++;
      }else{
          this->nodos[i] = &nodo;
            this->numeroDeEjes++;
            break;
        }
   }
}
void CNC::anadirEje(Nodo &nodo, float posicionInicial, float velocidadInicial, float aceleracionInicial){
  for(int i=0; i<8;i++){
      if(this->nodos[i]){
        i++;
      }else{
          this->nodos[i] = &nodo;
          this->numeroDeEjes++;
          this->nodos[i]->setPosicion(posicionInicial);
          this->nodos[i]->setVelocidad(velocidadInicial);
          this->nodos[i]->setAceleracion(aceleracionInicial);
          break;
        }
   }
}

void CNC::anadirEje(Nodo &nodo, float posicionInicial){
  for(int i=0; i<8;i++){
      if(this->nodos[i]){
        i++;
      }else{
          this->nodos[i] = &nodo;
            this->numeroDeEjes++;
            this->nodos[i]->setPosicion(posicionInicial);
            break;
        }
   }
}

int CNC::devolverTagInt(char tag){
  switch (tag){
    case 'X':
      return 0;
    case 'Y':
      return 1;
    case 'Z':
      return 2;
    case 'E':
      return 3;
    case 'W':
      return 4;
    case 'V':
      return 5;
  }  
}

void CNC::anadirExtrusor(Nodo *nodo){
  this->extrusor = nodo;

}

void CNC::setVelocidad(char eje, float velocidad){
  this->nodos[devolverTagInt(eje)]->setVelocidad(velocidad);
}
void CNC::setVelocidad(float velocidad){
  for(int i=0; i<numeroDeEjes; i++){
    this->nodos[i]->setVelocidad(velocidad);     
  }

}

void CNC::activarExtrusor(){
  this->extrusor->rotar(360, GRADOS);
}

char CNC::posicionar(float x, float y, float z){
  for (int i = 0; i<NODOS_MAXIMOS; i++){
    
  }

}

void CNC::anadirExtrusor(Nodo &nodo){
  this->extrusor = &nodo;
}

void CNC::inicalizarCNC(){
  Serial.begin(this->baudeaje);
  Wire.begin();
}