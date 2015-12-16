//FirmBMR2.ino

#include <EEPROM.h>
#include <Wire.h>
#include<stdlib.h>
//#include <math.h>


#define PRIMERA_VEZ  //Comentar para que los valores se carguen nuevamente y no se guarden en la EEPROM
#define BLOQUEO_DESPUES_DEL_GIRO

#define PIN_DIR 8
#define PIN_PULSE 9
#define PIN_ENABLED 7
#define PIN_FIN_DE_CARRERA_A A0
#define PIN_FIN_DE_CARRERA_B A1
#define PASOS_POR_MILIMETRO 128.5
#define GRADOS_POR_PASO 1.8
#define VELOCIDAD_MAXIMA 4.5 //mm/s
#define ACELERACION_MAXIMA 10
#define VELOCIDAD_ANGULAR 180 //grados/s         1.8 grados por 10 milisegundos


#define HORARIO 1
#define ANTIHORARIO -1
#define GRADOS 0
#define RADIANES 1


#define POSICION_ADDRESS 0x04
#define VELOCIDAD_ADDRESS 0x08
#define ACELERACION_ADDRESS 0x0C
#define ERROR_ADDRESS 0x10



int direccionDispositivo = 0x00;//LAS DIRECCIONES PUEDEN VARIAR DE 0x00 a 0x07
byte UltimoComandoRecibido1 = 0x00;
byte UltimoComandoRecibido2 = 0x00;

int dirPin = PIN_DIR;
int pulsePin = PIN_PULSE;
int enablePin = PIN_ENABLED;

int pinesDireccion[3] = {11,12,13};


float pasosPorMilimetro = PASOS_POR_MILIMETRO;
float velocidadMaxima = VELOCIDAD_MAXIMA; //cm por segundo
float aceleracionMaxima = ACELERACION_MAXIMA;


//=========TABLA DE DIRECCIONES========================
//
//----------------------------------------------------
//DIRECCION | VARIABLE QUE ALMACENA | TIPO DE VARIABLE
//----------------------------------------------------
//0x04      |POSICION(ABSOLUTA)     | float
//0x08      |VELOCIDAD              | float
//0x0C      |ACELERACION            | float
//0x10      |ERROR                  | int
//----------------------------------------------------
//
//
//=====================================================





//=========== VALORES ===========
#ifdef PRIMERA_VEZ
  float posicionValue = 0.0;   //   mm
  float velocidadValue = 3.0;//   mm/s
  float aceleracionValue = 0.0;//   mm/s2
  float anguloValue = 0.0;
  int errorValue = 0;
#else
  float posicionValue = 0.0;   //   mm
  float velocidadValue = 100.0;//   mm/s
  float aceleracionValue = 0.0;//   mm/s2
  float anguloValue = 0.0;
  int errorValue = 0;
#endif
//===============================


void paso(){
  digitalWrite(pulsePin, LOW);
  delayMicroseconds(100);
  digitalWrite(pulsePin,HIGH);
}
void rotar(float grados){
  anguloValue += grados;
  float numeroDePasos = grados/GRADOS_POR_PASO;
  float microsegundosPorGrado = (1.0/VELOCIDAD_ANGULAR)*1000000.0;

  setDireccion(grados>0?HORARIO:ANTIHORARIO);
  activarDriver();
  for(long i=0;i<numeroDePasos;i++){
      paso();
      delayMicroseconds(microsegundosPorGrado);
  }
#ifdef BLOQUEO_DESPUES_DEL_GIRO

#else
  desactivarDriver();
#endif
}

float cogerValorDeDireccion(byte direccion){
  switch (direccion) {
      case POSICION_ADDRESS:
        return getPosicion();
        break;
      case VELOCIDAD_ADDRESS:
        return getVelocidad();
        break;
      case ACELERACION_ADDRESS:
        return getAceleracion();
        break;
      case ERROR_ADDRESS:
        return float(errorValue);
      default:
        errorValue = 15;
        break;       
  }

}
void setearValorDeDireccion(byte direccion, float valor){
  switch (direccion) {
      case POSICION_ADDRESS:
        setPosicion(valor);
        break;
      case VELOCIDAD_ADDRESS:
        setVelocidad(valor);
        break;
   
      case ACELERACION_ADDRESS:
        setAceleracion(valor);
        break;
      case ERROR_ADDRESS:
        errorValue = int(valor);
      default:
        errorValue = 16;
        break;       
  }

}


void activarDriver(){
  digitalWrite(enablePin, LOW);
}
void desactivarDriver(){
  digitalWrite(enablePin, HIGH);
}
void setDireccion(int direccion){
  digitalWrite(dirPin, direccion>0?HIGH:LOW);
}
void setVelocidad(float velocidad){
  if(velocidad>velocidadMaxima){velocidad=velocidadMaxima;}
  velocidadValue = velocidad;
}
void setAceleracion(float aceleracion){
  aceleracionValue = aceleracion;
}
float getPosicion(){
  return posicionValue;
}
float getVelocidad(){
  return velocidadValue;
}
float getAceleracion(){
  return aceleracionValue;
}
void setHome(){
  posicionValue=0.0;

}
void goToHome(){
  setPosicion(0.0);
}


void desplazarMm(float milimetros){
  posicionValue += abs(milimetros);
  long cantidadDePasos = round(abs(milimetros)*pasosPorMilimetro);
  //velocidad = mm/s -> pasos/s
  //pasos/s -> pasos/microsegundo
  float pasosPorMicrosegundo = (velocidadValue*pasosPorMilimetro)/1000000.0;
  float microsegundosPorPaso = round(1.0/pasosPorMicrosegundo);
  

  setDireccion(milimetros>0?HORARIO:ANTIHORARIO);
  activarDriver();
  for(long i=0;i<cantidadDePasos;i++){
      paso();
      delayMicroseconds(microsegundosPorPaso);
  }
#ifdef BLOQUEO_DESPUES_DEL_GIRO

#else
  desactivarDriver();
#endif
}

void desplazarMm(float milimetros, float velocidadInicial){
  setVelocidad(velocidadInicial);
  desplazarMm(milimetros); 
}

void desplazarMm(float milimetros, float velocidadInicial, float aceleracionInicial){
  setVelocidad(velocidadInicial);
  for(long i=0; i< abs(milimetros)/0.1; i++){//POR PROBAR, NO SÉ SI FUNCIONE
    desplazarMm(milimetros>0?0.1:-0.1);
    setVelocidad(getVelocidad()+aceleracionInicial);    
  }
}

void setPosicion(float posicion){
  desplazarMm((posicion-posicionValue),velocidadValue);
}
void setPosicion(float posicion, float velocidad){
  setVelocidad(velocidad);
  desplazarMm((posicion-posicionValue),velocidadValue);
}
void inicializarComunicaciones(){
	for(int x=0;x<3;x++){
	    pinMode(pinesDireccion[x],INPUT);
	}

	pinMode(dirPin, OUTPUT);
	pinMode(pulsePin, OUTPUT);
	pinMode(enablePin, OUTPUT);

	for(int i=0;i<3;i++){
		int m = (digitalRead(pinesDireccion[i]))*pow(2,i);
		direccionDispositivo = direccionDispositivo + m;
	}

	digitalWrite(dirPin, LOW);
	digitalWrite(pulsePin, LOW);
	digitalWrite(enablePin, HIGH);

	Wire.begin(0x08);///////////////////////////////////LINEA DE PRUEBA SIN PLACA!, NO TE OLVIDES BREGY!
	Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

/////////////////////////////////////////////////////////////////////
//LEER = 0x50
//ESCRIBIR = 0x51
//
//FORMA DE PETICION:
//
//Wire.write(OPERACION);
//Wire.write(REGISTRO);
//
//SI ES DE LECTURA:
//  Wire.requestFrom(DIRECCION, (byte)4); //4 porque generalmente seran float los datos
//SI ES DE ESCRITURA:
//  Wire.write(unByte);
//
//
//
//
/////////////////////////////////////////////////////////////////////


void receiveEvent(int bytesReceived){
    if(Wire.available()==2){
        UltimoComandoRecibido1 = Wire.read();
        UltimoComandoRecibido2 = Wire.read();
    }else if(Wire.available()>2){
        UltimoComandoRecibido1 = Wire.read();
        UltimoComandoRecibido2 = Wire.read();
        if(UltimoComandoRecibido1 == 0x51){
          char lectura[5];
          int i = 0;
          float valor = 0.0;
          switch (UltimoComandoRecibido2) {
              
              case  0x04:
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);

                  setearValorDeDireccion(0x04,valor);
                break;

              case 0x08:
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);
                  setearValorDeDireccion(0x08,valor);
                break;

              case 0x0C:
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);
                  setearValorDeDireccion(0x0C,valor);
                break;

              case 0x03:
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);

                  rotar(valor);


          }
        }


        
    }
}      

void requestEvent(){
    if(UltimoComandoRecibido1 == 0x50){
        char valorChar[5];
        switch(UltimoComandoRecibido2){
            case 0x04:
                Wire.write(dtostrf(cogerValorDeDireccion(0x04), 2, 1, valorChar));
                break;
                
            case 0x08:
                Wire.write(dtostrf(cogerValorDeDireccion(0x08), 2, 1, valorChar));
                break;
                
            case 0x0C:
                Wire.write(dtostrf(cogerValorDeDireccion(0x0C), 2, 1, valorChar));
                break;

            default:
                Wire.write("NADA!");
            
            
        }
    }

    
    
}



void setup(){
	inicializarComunicaciones();
  delay(1000);
  
  
  
}

void loop(){
  

	
}


