/**
 * Microcontroladores II - IFTS14
 * Protocolo RS232 - Arduino
 * @author Bárbara Menares
 * @date 20160621
*/

#include <stdio.h>
#include <wire.h> // comunicador rs232

void reset_input_buffer();

// Variables para la gestión del input desde RS232
unsigned char    input_buffer[PONG_PROTOCOLO_INPUT_BUFFER] ;
unsigned char    input_buffer_index    = 0;
char             buffering             = 0;


int main (void)
{    
    
    return 0;    
}

//No sé qué onda las interrupciones en arduino
//high_priority interrupt
void high_priority interrupt MyHighIsr(void) 
{
    
    // Interrumpio Timer2 ?
    if ( PIR1bits.TMR2IF ) 
    {
        PIR1bits.TMR2IF = 0;    // Limpia el flag de interrupcion    
            
    }
    
    //Interrumpio USART RX (llego un dato por el puerto serie ?)
    //VERIFICAR COMO SE LLAMA EN ARDUINO !!1!
    if(PIR1bits.RCIF) 
    {
        PIR1bits.RCIF = 0;
    
        //Finalizador de paquete
        if ( RCREG == '>') 
        {
            //aca función que mande diga paquete ok
            reset_input_buffer();
        }
         
        //Si inició paquete válido y no detectó cierre
        if ( buffering > 0 ) 
        {
            input_buffer[ input_buffer_index ] = RCREG;
            input_buffer_index++;
            //TXREG = RCREG; //esto era para eco
        }
        
        //Inicializador de paquete
        if ( RCREG == '<' ) 
        {
            reset_input_buffer();
            buffering = 1;
        }
    }
}

void reset_input_buffer() 
{
    //borro el buffer y reinicio el índice.
    input_buffer_index    = 0;
    buffering            = 0;
    memset(input_buffer,0,sizeof(input_buffer));
}
