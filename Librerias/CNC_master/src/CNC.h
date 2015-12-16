#ifndef CNC_h
#define CNC_h

#ifndef Nodo_h
#include <Nodo.h>
#endif
#include <Arduino.h>

#define MODO_NORMAL 0
#define MODO_DELTA 1
#define NODOS_MAXIMOS 8




class CNC{
    struct ejeCoordenada{
        float x;
        float y;
        float z;
    };
    public:
        CNC(char nombre, int baudeaje);
        CNC(int baudeaje);
        void tipoDeCNC(int modo);
        void inicalizarCNC();
        void anadirEje(Nodo &nodo);
        void anadirEje(Nodo &nodo, float posicionInicial, float velocidadInicial, float aceleracionInicial);
        void anadirEje(Nodo &nodo, float posicionInicial);

        void anadirExtrusor(Nodo *nodo);
        void anadirExtrusor(Nodo &nodo);
        char posicionar(float x, float y, float z);
        int posicionar(float x, float y, float z, float velocidad);
        void setVelocidad(char eje, float velocidad);
        void setVelocidad(float velocidad);
        void activarExtrusor();
        void desactivarExtrusor();


    private:
        Nodo *nodos[8];
        Nodo *extrusor;
        char nombre;  
        int baudeaje;
        int modo;
        int numeroDeEjes;
        int modoDeOperacion;
        int devolverTagInt(char tag);

};


#endif
