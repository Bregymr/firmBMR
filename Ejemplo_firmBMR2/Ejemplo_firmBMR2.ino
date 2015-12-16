

//Ejemplo_firmBMR2.ino

#include <Wire.h>
#include <Nodo.h>
#include <CNC.h>

Nodo motorX(0x16, 'X');
Nodo motorY(0x18, 'Y');
Nodo motorZ(0x20, 'Z');

void moverEje(float x, float y, float z){
	motorX.setPosicion(x);
	delay(10);
	motorY.setPosicion(y);
	delay(10);
	motorZ.setPosicion(z);
	delay(10);
}


void setup(){
	Wire.begin();
	Serial.begin(9600);

	motorX.setVelocidad(20.0);
	delay(10);
	motorX.setPosicion(0.0);
	



	
}

void loop(){
	
	

}
