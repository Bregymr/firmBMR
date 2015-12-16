//Firm_Maestro.ino


#include <Wire.h>
#include <Nodo.h>
#include <CNC.h>


Nodo nodoX(0x01,'X');
Nodo nodoY(0x08,'Y');

void setup(){
	Wire.begin();
	nodoX.moverMm(50.0);
	nodoY.moverMm(25.0);
	
}

void loop(){

	delay(500);

	
}
