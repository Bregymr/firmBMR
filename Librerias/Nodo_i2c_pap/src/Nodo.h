#ifndef Nodo_h
#define Nodo_h

#include<stdlib.h>

#ifndef Wire_h
#include <Wire.h>
#endif

#define HORARIO 0
#define ANTIHORARIO 1

#define GRADOS 0
#define RADIANES 1


class Nodo{
  public:
    Nodo(int direccion, unsigned char tag);

    Nodo(int direccion, int quequePin, unsigned char tag);
    Nodo(int direccion, unsigned char tag, float posicionInicial, float velocidadInicial, float aceleracionInicial);
    Nodo(int direccion);
    

    void setVelocidad(float velocidad);
    float getVelocidad();
    void setPosicion(float posicion);
    float getPosicion();
    void setAceleracion(float aceleracion);
    float getAceleracion();

    void moverMm(float magnitud);
    void rotar(float angulo, int sistemaNumerico);
    void backup();
    void actualizarDatos();



  private:
    float posicion;
    float velocidad;
    float aceleracion;
    float angulo;
    unsigned char tag;
    int direccion;
    int quequePin;
    static unsigned int quequePinCont; 
    static unsigned char contador;


};


#endif
