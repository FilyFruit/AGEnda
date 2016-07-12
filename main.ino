/*
* Microcontroladores II - IFTS14
* Protocolo RS232 - Arduino v. Hola Mundo - pruebas
* @author Bárbara Menares
* @date 20160628
 */


#include <stdio.h>
#include <wire.h> //comunicador rs232 - arduino
#include <SoftwareSerial.h>
#include <funcionalidades.h>

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String respuesta = "como te va"; // para el hola_mundo
boolean buffering = false;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(300);
}

void loop() {


}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

      if ( inChar == '>') // cerrador de paquete
        {
          stringComplete = true;
          agenda_parse();
          reset_input_string();
        }

      if ( buffering == true ) // paquete valido iniciado
        {
          inputString += inChar;
        }

      if ( inChar == '<' ) // inicio de paquete
        {
          reset_input_string();
          buffering = true;
        }

    }
  }

void agenda_parse () {
  if (inputString == "lista")
  {
     Serial.println("no hay cosos");
    //mostrar_lista();
  }

  if (inputString == "cumples")
  {
    //mostrar_cumples();
  }

  if (inputString == "guardar")
  {
    //guardar();
  }

  if (inputString == "borrar")
  {
	//borrar();
  }
}


String getValue(String data, char separator, int index)
{
   int found = 0;
    int strIndex[] = {
  0, -1  };
    int maxIndex = data.length()-1;
    for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
    found++;
    strIndex[0] = strIndex[1]+1;
    strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
   }
    return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void reset_input_string()
{
    //borro el buffer y string
    buffering = false;
    inputString = "";
}
