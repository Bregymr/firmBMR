//FirmBMR2.ino


#include <Wire.h>
#include <EEPROM.h>
#include<stdlib.h>
//#include <math.h>


#define PRIMERA_VEZ  //Comentar para que los valores se carguen nuevamente y no se guarden en la EEPROM
#define BLOQUEO_DESPUES_DEL_GIRO

#define PIN_DIR 9
#define PIN_PULSE 8
#define PIN_ENABLED 7
#define PIN_QUEQUE 6
#define PIN_FIN_DE_CARRERA_A A0
#define PIN_FIN_DE_CARRERA_B A1


#define PASOS_POR_MILIMETRO 80.0
#define GRADOS_POR_PASO 1.8
#define VELOCIDAD_MAXIMA 100.0 //mm/s
#define ACELERACION_MAXIMA 10
#define VELOCIDAD_ANGULAR 360 //grados/s         1.8 grados por 10 milisegundos
#define DISTANCIA_MINIMA 400.0
#define DISTANCIA_MAXIMA  0.1


#define HORARIO 1
#define ANTIHORARIO -1
#define GRADOS 0
#define RADIANES 1


#define POSICION_ADDRESS 0x04
#define VELOCIDAD_ADDRESS 0x08
#define ACELERACION_ADDRESS 0x0C
#define ANGULO_ADDRESS 0x10
#define ERROR_ADDRESS 0x11



int direccionDispositivo = 0x00;//LAS DIRECCIONES PUEDEN VARIAR DE 0x00 a 0x07
byte UltimoComandoRecibido1 = 0x00;
byte UltimoComandoRecibido2 = 0x00;

int dirPin = PIN_DIR;
int pulsePin = PIN_PULSE;
int enablePin = PIN_ENABLED;
int quequePin = PIN_QUEQUE;

int pinesDireccion[3] = {4,5,6};


float pasosPorMilimetro = PASOS_POR_MILIMETRO;
float milimetrosPorPaso = 1.0/PASOS_POR_MILIMETRO;
float velocidadMaxima = VELOCIDAD_MAXIMA; //cm por segundo
float aceleracionMaxima = ACELERACION_MAXIMA;


//=========TABLA DE DIRECCIONES========================
//
//----------------------------------------------------
//DIRECCION | VARIABLE QUE ALMACENA | TIPO 
//----------------------------------------------------
//0x04      |POSICION(ABSOLUTA)     | float
//0x08      |VELOCIDAD              | float
//0x0C      |ACELERACION            | float
//0x10      |ANGULO                 | float
//0x30      |SETHOME                | funcion
//0x31      |ACTIVARDRIVER          | funcion
//0x32      |DESCACTIVARDRIVER      | funcion
//0x33      |ROTAR                  | funcion
//0x34      |GOTOHOME               | funcion  
//0x35      |BACKUPDEDATOS          | funcion
//0x36      |SETDIRECCION           | funcion
//
//=====================================================





//=========== VALORES ===========
#ifdef PRIMERA_VEZ
  float posicionValue = 10.0;   //   mm
  float velocidadValue = 35.0;//   mm/s
  float aceleracionValue = 0.0;//   mm/s2
  float anguloValue = 0.0;
  int errorValue = 0;



#else
  float posicionValue = 0.0;   //   mm
  float velocidadValue = 0.0;//   mm/s
  float aceleracionValue = 0.0;//   mm/s2
  float anguloValue = 0.0;
  int errorValue = 0;


#endif
//===============================

void inicializarComunicaciones(){
  for(int x=0;x<3;x++){
      pinMode(pinesDireccion[x],INPUT);
  }

  pinMode(dirPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(quequePin, OUTPUT);
  pinMode(13, OUTPUT);

  pinMode(PIN_FIN_DE_CARRERA_A, INPUT);
  pinMode(PIN_FIN_DE_CARRERA_B, INPUT);


  for(int i=0;i<3;i++){
    int m = (digitalRead(pinesDireccion[i]))*pow(2,i);
    direccionDispositivo = direccionDispositivo + m;
  }

  digitalWrite(dirPin, LOW);
  digitalWrite(pulsePin, LOW);
  digitalWrite(enablePin, HIGH);
  digitalWrite(quequePin, LOW);

  Wire.begin(0x01);///////////////////////////////////LINEA DE PRUEBA SIN PLACA!, NO TE OLVIDES BREGY!
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

int getFinDeCarrera(int pin){
  return digitalRead(pin)?1:-1;
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

void paso(){
  
  digitalWrite(pulsePin, LOW);
  delayMicroseconds(200);
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

void backup(){
  EEPROM.put(POSICION_ADDRESS, posicionValue);
  EEPROM.put(VELOCIDAD_ADDRESS, velocidadValue);
  EEPROM.put(ACELERACION_ADDRESS, aceleracionValue);
  EEPROM.put(ANGULO_ADDRESS, anguloValue);
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
  
  long cantidadDePasos = round(abs(milimetros)*pasosPorMilimetro);
  //velocidad = mm/s -> pasos/s
  //pasos/s -> pasos/microsegundo
  float pasosPorMicrosegundo = (velocidadValue*pasosPorMilimetro)/1000000.0;
  float microsegundosPorPaso = round(1.0/pasosPorMicrosegundo);
  

  setDireccion(milimetros>0?HORARIO:ANTIHORARIO);
  activarDriver();
  for(long i=0;i<cantidadDePasos;i++){
      if(getFinDeCarrera(PIN_FIN_DE_CARRERA_A)>0 || getFinDeCarrera(PIN_FIN_DE_CARRERA_B)>0){break;}
      paso();
      posicionValue += (milimetrosPorPaso*(milimetros>0?1:-1));
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
          char lectura[8];
          int i = 0;
          float valor = 0.0;
          switch (UltimoComandoRecibido2) {
              
              case  0x04:
                  digitalWrite(quequePin, HIGH);
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);
                  Wire.flush();
                  setearValorDeDireccion(0x04,valor);
                  digitalWrite(quequePin, LOW);
                  
                  
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

              case 0x30:
                  i = 0;
                  setHome();
                  


                  break;

              case 0x31:
                  i = 0;
                  activarDriver();
                  

                  break;

              case 0x32:
                  i = 0;
                  desactivarDriver();
                  

                  break;

              case 0x33:
                  i = 0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  valor = atof(lectura);

                  rotar(valor);
                  
                  break;

              case 0x34:
                  i = 0;
                  goToHome();
                  

                  break;

              case 0x35:
                  i = 0;
                  backup();
                  

                  break;

              case 0x36:
                  i=0;
                  while(Wire.available()>0){
                      lectura[i] = Wire.read();
                      i++;
                  }
                  if(lectura[0] == 'A'){setDireccion(1);}else if(lectura[0]=='B'){setDireccion(-1);}
                  
                  break;


          }
        }


        
    }
}      

void requestEvent(){
    if(UltimoComandoRecibido1 == 0x50){
        char valorChar[8];
        switch(UltimoComandoRecibido2){
            case 0x04:
                Wire.write(dtostrf(cogerValorDeDireccion(0x04), -5, 2, valorChar));
                break;
                
            case 0x08:
                Wire.write(dtostrf(cogerValorDeDireccion(0x08), -5, 2, valorChar));
                break;
                
            case 0x0C:
                Wire.write(dtostrf(cogerValorDeDireccion(0x0C), -5, 2, valorChar));
                break;

            default:
                Wire.write("NADA!");
            
            
        }
    }

    
    
}



void setup(){
	inicializarComunicaciones();
  activarDriver();
/*  #ifdef PRIMERA_VEZ

  EEPROM.put(POSICION_ADDRESS, posicionValue);
  EEPROM.put(VELOCIDAD_ADDRESS, velocidadValue);
  EEPROM.put(ACELERACION_ADDRESS, aceleracionValue);
  EEPROM.put(ANGULO_ADDRESS, anguloValue);

  #else

  EEPROM.get(POSICION_ADDRESS, posicionValue);
  EEPROM.get(VELOCIDAD_ADDRESS, velocidadValue);
  EEPROM.get(ACELERACION_ADDRESS, aceleracionValue);
  EEPROM.get(ANGULO_ADDRESS, anguloValue);
  #endif*/

  


  
}

void loop(){ 
  
  digitalWrite(13, HIGH);
  delay(100);  
  digitalWrite(13, LOW);
  delay(100);
  
	
}
