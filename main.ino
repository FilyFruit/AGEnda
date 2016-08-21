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

// Variables para la gestión de datos vía serial.
unsigned char   input_buffer[200];
unsigned char   input_buffer_index  = 0;
char            buffering           = 0;


void setup() {
  // initialize serial:
  Serial.begin(9600);
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
        char input_char = (char)Serial.read();

        if ( input_char == '>') // cerrador de paquete
        {
            agenda_parse();
            reset_input_buffer();
        }

        if ( buffering > 0 ) // paquete valido iniciado
        {
            input_buffer[ input_buffer_index ] = input_char;
            input_buffer_index++;
        }

        if ( input_char == '<' ) // inicio de paquete
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
    String lista[10];
    int id;

    if ( strcmp( input_buffer , "guardar"  ) == 0 )
    {
        obtener_lista( &lista );
        if (no_hay == 1)
        {
            Serial.println("no hay cosos");
        }
        else
        {
            Serial.println("hay cosos");
        }
    } 
    else if ( strcmp( input_buffer , "guardar"  ) == 0 )
    {
        mostrar_cumples();
    } 
    else 
    {
        // Necesito realizar un split.
        char * id;
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
            
            id = get_id_for_name( nombre );
            guardar(nombre, ddmmaa, id);
        }
        else if ( strcmp( token , "borrar"  ) == 0 )
        {
            token       = strtok( NULL, search);
            id          = atoi( token );
            borrar(id);
        }
        else 
        {
            Serial.println("ERROR: UNKNOWN COMMAND.");
        }
    }
}

void reset_input_buffer() {
    //borro el buffer y reinicio el índice.
    input_buffer_index   = 0;
    buffering            = 0;
    memset( input_buffer , 0 , sizeof(input_buffer) );
}
