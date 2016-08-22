/*
* Microcontroladores II - IFTS14
* Protocolo RS232 - Arduino v. Hola Mundo - pruebas
* @author Bárbara Menares
* @date 20160628
 */


#include <stdio.h>
#include <Wire.h> //comunicador rs232 - arduino
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>
#include "RTClib.h"
#include "AT24Cxx.h"

// Variables para la gestión de datos vía serial.
char input_buffer[200];
char input_buffer_index  = 0;
char buffering           = 0;

// Variables para manejar el RTC
RTC_DS1307  RTC;

// Variables para manejar la memoria
AT24Cxx     MEM;

void setup() {
  // iniciamos el puerto serial:
  Serial.begin(9600);
  
  // iniciamos también el módulo RTC.
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("<MSG:El RTC no estaba corriendo.>");
    // Si el RTC no estaba corriendo, lo ponemos en hora.
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  MEM = AT24Cxx();
}

void loop() {

    /*
    DateTime now = RTC.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(); 
    delay(1000);
    */
}

/**
 * Esta función es un callback de un evento de la librería Serial.
 **/
void serialEvent() {
    while (Serial.available()) {
        // Obtengo un byte
        char input_char = (char)Serial.read();

        if ( input_char == '>')     // cerrador de paquete
        {
            agenda_parse();
            reset_input_buffer();
        }

        if ( buffering > 0 )        // paquete valido ya iniciado
        {
            input_buffer[ input_buffer_index ] = input_char;
            input_buffer_index++;
        }

        if ( input_char == '<' )    // inicio de paquete
        {
            reset_input_buffer();
            buffering = 1;
        }
    }
}

/**
 * Función que entiende comandos del protocolo AGENDA.
 * 
 * @author  Daniel Cantarín
 * @date    20160820
 **/
void agenda_parse () {
    char *  lista;
    char    no_hay = 1;
    String  result = "";
    
    Serial.println("comando:");
    Serial.println( input_buffer );

    if ( strcmp( input_buffer , "lista"  ) == 0 )
    {
        obtener_lista( lista );
        if (no_hay == 1)
        {
            Serial.println("<MSG:La lista está vacía.>");
        }
        else
        {
            result  = "<MSG:";
            result += (String) sizeof( lista );
            result += " items en la lista.>";
            Serial.println( result );
        }
    } 
    else if ( strcmp( input_buffer , "mostrar"  ) == 0 )
    {
        mostrar_cumples();
    }
    else if ( strcmp( input_buffer , "check_memoria"  ) == 0 )
    {
        if ( MEM.isPresent() ) {
            Serial.println("<MSG:Memoria presente.>");
        } 
        else
        {
            Serial.println("<MSG:Memoria NO presente.>");
        }
    } 
    else 
    {
        // Necesito realizar un split.
        char   id;
        char * token;
        char * search = ":";
        char i        = 0;
        

        // Token va a tener la primera acepción de un texto antes del ":".
        // En adelante, sucesivos llamados devuelve el resto de los parámetros.
        // Con esto parseo cualquier parámetro.
        token = strtok( input_buffer, search);
        
        // Y ahora trabajo con el token hallado. 
        // Los tokens subsiguientes serán parámetros de cada comando.
        if ( strcmp( token , "guardar"  ) == 0 )
        {
            
            token           = strtok( NULL, search);
            char * nombre   = token;
            token           = strtok( NULL, search);
            char * ddmmyy   = token;
            char * data     = ""; 
            uint8_t * buffer;
            id              = get_id_for_name( nombre );
            
            sprintf( data, "%s:%s", ddmmyy, nombre);
            
            guardar( (uint8_t *) data, id );
            
            RTC.readnvram( buffer, strlen(data), id );
            
            if ( buffer == (uint8_t *) data ) 
            {
                Serial.println("<MSG:Guardado OK.>");
            } 
            else 
            {
                Serial.println("<ERROR:No se pudo guardar.>");
            }
            
        }
        else if ( strcmp( token , "borrar"  ) == 0 )
        {
            token       = strtok( NULL, search);
            id          = (char) atoi(token);
            //borrar(id);
        }
        else if ( strcmp( token , "leer"  ) == 0 )
        {
            token         = strtok( NULL, search);
            char * numero = token;
            char * buffer;
            //int res       = MEM.ReadMem( (int) &numero, buffer, 1);
            uint8_t res   = RTC.readnvram( (uint8_t) atoi(numero) );
            
            Serial.println( res );
        }
        else 
        {
            Serial.println("<ERROR:Comando no reconocido.>");
        }
    }
}

void reset_input_buffer() {
    //borro el buffer y reinicio el índice.
    input_buffer_index   = 0;
    buffering            = 0;
    memset( input_buffer , 0 , sizeof(input_buffer) );
}



void obtener_lista ( char *lista )
{
  /*
   * if (hay datos)
  *{
  *  how to?
  *  tiene que preguntar a la memoria y devolver todos
  *  los cumpleaños guardados con sus ids
  *  return cumpleaños_con_ids;
  }
  *  else no_hay = 1;

  */
}

void mostrar_cumples() // dfhsdf
{
  /*
   * esto es más del rtc
   * si la fecha actual coincide con una ingresada
   * return match;
   */
}

void guardar (uint8_t * que, int id)
{
  /*
   * "ok, voy a guardar"
   * busco id donde lo voy a guardar
   * pido primero nombre
   * luego fecha
   * return ok;
   */
   
   RTC.writenvram( id, que, strlen( (char *) que ) );
   delay(50);
   
}

void borrar( int cual )
{
  /*
   * dame la id a borrar
   * print si no conoces la id buscá en lista
   * o tirar la lista obtener_lista();
   * pasame la id
   * return ok;
   */
}

char get_id_for_name ( char * name )
{
    return 1;
}



/*
 * escribe en la memoria 24c32
 * recibe la direccion de la memoria
 * y el byte de dato
 *
 */
void escribeMEM (int direccion, byte data)
{
  //transforma direccion en los dos address byte direccion
  byte BYTE_1 = direccion >> 8;
  byte BYTE_2 = direccion - (BYTE_1 << 8);

  Wire.beginTransmission(MEMdir);
  Wire.write(BYTE_1);
  Wire.write(BYTE_2);
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}


/*
 * escribe una pagina en la memoria 24c32
 * recibe la direccion de la memoria
 * y el string de  dato
 *
 */
void escribePagMEM (int direccion, String data)
{
  for (int i = 0; i < 32; i++)
    {
      escribeMEM(direccion,data[i]);
      direccion++;
    }
}

/*
 * lee la memoria 24c32
 * recibe la direccion de la memoria
 * y devuelve el String de la pagina de esa
 * direccion
 */
String leeMEM (int direccion)
{
  String paginaDeMemoriaR;
  paginaDeMemoriaR.reserve(32);
  paginaDeMemoriaR = "";
  //byte data;
  byte BYTE_1 = direccion >> 8;
  byte BYTE_2 = direccion - (BYTE_1 << 8);
  Wire.beginTransmission(MEMdir);
  Wire.write(BYTE_1);
  Wire.write(BYTE_2);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(MEMdir, 32);
  delay(10);
  for(byte i=0; i < 32; i++)
  {
    paginaDeMemoriaR += (char)Wire.read();
  }
  delay(10);
  return paginaDeMemoriaR;
}
