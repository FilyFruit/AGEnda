/*
* Microcontroladores II - IFTS14
* Protocolo RS232 - Arduino
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
char input_buffer[32];
char input_buffer_index  = 0;
char buffering           = 0;
char debug_enabled       = 0;

// Variables para manejar el RTC
RTC_DS1307  RTC;

// Variables para manejar la memoria
AT24Cxx     MEM;
#define     MEMdir 0x50
byte        filler = ' ';
char tamanio_lista = 10;

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
    char *  lista[10];
    char    cuantos = 0;
    
    debug( "comando:" );
    debug( (String) input_buffer );

    if ( strcmp( input_buffer , "lista"  ) == 0 )
    {
        char * vacio = "                                ";
        obtener_lista( lista );
        
        for (cuantos = 0; cuantos < 10; cuantos++)
        {
            if (lista[cuantos] == vacio)
            {
                break;
            }
        }
        
        if (cuantos == 0)
        {
            Serial.println("<MSG:La lista está vacía.>");
        }
        else
        {
            char * result;
            sprintf(result, "<MSG:%i items en la lista>", cuantos);
            Serial.println( result );
        }
    }
    else if ( strcmp( input_buffer , "debug"  ) == 0 )
    {
        if ( debug_enabled == 0 )
        {
            debug_enabled = 1;
            Serial.println("<MSG:debug activado.>");
        }
        else
        {
            debug_enabled = 0;
            Serial.println("<MSG:debug desactivado.>");
        }
    }
    else if ( strcmp( input_buffer , "limpiar"  ) == 0 )
    {
        clear_lista();
        Serial.println("<MSG:Lista borrada.>");
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
            String data     = ""; 
            String buffer   = "";
            id              = get_id_for_name( nombre );
            
            if ( id < 0 )
            {
                Serial.println("<ERROR:No hay espacio disponible.>");
                return;
            }
            
            //sprintf( data, "%s:%s", ddmmyy, nombre);
            data = (String) ddmmyy + ":" + (String) nombre;
            
            guardar( data, id );
            
            // confirmamos que haya sido guardada correctamente.
            
            buffer = leer_memoria( id );
            
            if ( buffer == data ) 
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
            
            // El parámetro "id" corresponde a "número de página".
            borrar( id );
            
            // Compruebo el borrado.
            String buffer = leer_memoria( id );
            if ( buffer == "                                " )
            {
                Serial.println("<MSG:Borrado OK.>");
            }
            else 
            {
                Serial.println("<ERROR:No se pudo borrar.>");
            }
        }
        else if ( strcmp( token , "leer"  ) == 0 )
        {
            token         = strtok( NULL, search);
            int    id     = (int) (token[0] - '0');
            String pagina = "";
            
            sprintf(token, "<DEBUG:numero de pagina:%i>",id);
            debug( (String) token );
            pagina  = leer_memoria( id );
            Serial.println( "<ITEM:" + pagina + ">" );
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


/**
 * Devuelvo un array con strings.
 * Cada string es una página completa de la memoria.
 * En cada página están los datos de los cumpleaños.
 * 
 * @author  Daniel Cantarín <canta@canta.com.ar>
 * @date    20160827
 * @param   char* lista Un array donde voy a guardar las páginas.
 * 
 **/
void obtener_lista ( char * lista[] )
{
    char i;
    char * tmp;
    String pagina;
    
    debug("<DEBUG:obtener_lista().>");
    for ( i = 0; i < tamanio_lista; i++) 
    {
        debug("<DEBUG:" + (String) i + ".>");
        pagina = leer_memoria( i );
        debug("<DEBUG:'" + pagina + "'.>");
        pagina.toCharArray( tmp, 32 );
        debug("<DEBUG:lista[" + (String) i + "] = '" + (String) tmp + "'.>");
        lista[i] = tmp;
    }
}

/**
 * Función que se utiliza para depurar.
 * 
 * @author  Daniel Cantarín <canta@canta.com.ar>
 * @date    20160827
 * @param   String texto Un texto a imprimir.
 **/
void debug (String texto)
{
    if ( debug_enabled == 1 )
    {
        Serial.println( texto );
    }
}

void mostrar_cumples() 
{
    
}

void guardar (String que, int id)
{
   escribir_pagina_memoria( id * 32, que  );
   delay(50); // Leí por ahí que a veces es necesario un pequeño delay.
}

/**
 * Borra completamente las páginas de la memoria.
 * 
 * @author  Daniel Cantarín <canta@canta.com.ar>
 * @date    20160827
 **/
void clear_lista()
{
    char i;
    for (i = 0; i < tamanio_lista; i++)
    {
        borrar( i );
    }
}

/**
 * Dado un número de página, la llena de espacios en blanco.
 * 
 * @author  Daniel Cantarín <canta@canta.com.ar>
 * @date    20160827
 * @param   int cual Un número de página, empezando por cero.
 **/
void borrar( int cual )
{
    // Simplemente escribo espacios en blanco en una página.
    String espacios = "                                ";
    escribir_pagina_memoria( cual * 32, espacios );
}

char get_id_for_name ( char * name )
{
    char id         = -1;
    char i          = 0;
    char temp[32]   = "";
    char * temp2;
    String temp3;
    debug("<DEBUG:get_id_for_name(" + (String) name + ").>");
    for ( i = 0; i < tamanio_lista; i++)
    {
        leer_memoria( i ).toCharArray( temp, 32 );
        debug("<DEBUG:" + (String) temp + ".>");
        temp2 = trimwhitespace( temp );
        temp3 = (String) temp2;
        if ( temp3.length() == 0 ) 
        {
            id = i;
            break;
        }
    }
    
    debug("<DEBUG:return " + (String) id + ".>");
    
    return id;
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}


/**
 * Escribe en la memoria 24c32 recibe la direccion de la memoria y el 
 * byte de dato.
 *
 * @author granjero
 * @url https://github.com/granjero
 */
void escribir_memoria (int direccion, byte data)
{
    //transforma direccion en los dos address byte direccion
    debug("<DEBUG:escribir_memoria(" + (String) direccion + ",'" + (String) data + "').>");
    byte BYTE_1 = direccion >> 8;
    byte BYTE_2 = direccion - (BYTE_1 << 8);

    Wire.beginTransmission(MEMdir);
    Wire.write(BYTE_1);
    Wire.write(BYTE_2);
    Wire.write(data);
    Wire.endTransmission();
    delay(10);
}

/**
 * Escribe una pagina en la memoria 24c32 recibe la direccion de la 
 * memoria y el string de  dato
 * 
 * @author granjero
 * @url https://github.com/granjero
 */
void escribir_pagina_memoria (int direccion, String data)
{
    char b;
    debug("<DEBUG:escribir_pagina_memoria(" + (String) direccion + ",'" + (String) data + "').>");
    for (int i = 0; i < 32; i++)
    {
        escribir_memoria(direccion,data[i]);
        direccion++;
        // Si llegué a cubrir el tamaño del string, relleno la página
        // con un valor de filler definido globalmente.
        if ( i > data.length() )
        {
            for (b = i; b < 32; b++)
            {
                escribir_memoria(direccion, filler);
                direccion++;
            }
            i = 32;
        }
    }
}

/**
 * Lee la memoria 24c32 recibe la direccion de la memoria y devuelve el 
 * String de la pagina de esa direccion.
 * 
 * @author granjero
 * @url https://github.com/granjero
 */
String leer_memoria (int direccion)
{
    
    direccion = direccion * 32; // asumo un índice, no una dirección.
    
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
    
    char tmp[32];
    paginaDeMemoriaR.toCharArray( tmp, 32 );
    //char * token  = strtok( tmp, (char * ) filler);
    return (String) tmp;
}
