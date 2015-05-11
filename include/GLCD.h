#ifndef __GLCD_H__
	#define __GLCD_H_

////////////////////////////////////////////////////////////////////////////////
////                     glcd_160x80.h                        ////
//// La presente libreria gestiona la comunicación con un GLCD de 160x80   ////
//// pixeles marca Samsung, modelo LJ41, con controlador SANYO LC7981.       ////
//// No controla el touchscreen que algunas pantallas pueden incluir      ////
////                                                      ////
//// CONEXION DE GLCD AL MICROCONTROLADOR                           ////
////                                                      ////
//// Para ver que pin del uC está asociado a cada etiqueta,               ////
//// revisar la sección ETIQUETAS                                 ////
////                                                      ////
//// ---------------------------------------------------------------------   ////
//// PIN   NOMBRE   FUNCION                              ETIQUETA   ////
//// ---------------------------------------------------------------------   ////
////  1      VSS      Tierra GND                                    ////
////  2      VCC      voltaje positivo de alimentación 5v                  ////
////  3      VADJ   contraste - P1 = 10k (ver esquema abajo)            ////
////  4      RS      1 = INSTRUCCION      0 = DATO            GLCD_RS      ////
////  5      R/W      1 = uC <- GLCD      0 = uC -> GLCD         GLCD_RW      //// set pin to ground
////  6      E      pasa una instrucción flanco descendente      GLCD_E      ////
////  7      DB0      -                                 GLCD_DB0   ////
////  8      DB1       |                                 GLCD_DB1   ////
////  9      DB2       |                                 GLCD_DB2   ////
////  10   DB3       |                                 GLCD_DB3   ////
////  11   DB4        -- Bus de datos del LCD               GLCD_DB4   ////
////  12   DB5       |                                 GLCD_DB5   ////
////  13   DB6       |                                 GLCD_DB6   ////
////  14   DB7      -                                 GLCD_DB7   ////
////  15   CS      chip select, conexión = GND                        ////
////  16   RES      Reset, normalmente a 5v                         ////
////  17   VLCD   contraste, P1 = 10k (ver esquema abajo)               ////
////  18   DISPOFF   GLCD apagado cuando está a GND, normalmente a 5v      ////
////  19   EL1      backligh   sin conexión                        ////
////  20   EL1      backlight   sin conexión                        ////
////                                                      ////
////            ESQUEMA P1, POTENCIOMETRO 10K CONTRASTE               ////
////                                                      ////
////                      -----------                        ////
////                     |   _____    |                        ////
////                     |  |     |   |                        ////
////                     |  | ( ) |   |                        ////
////                     |  |_____|   |                        ////
////                     |         |                        ////
////                      -----------                        ////
////                        |  |  |                           ////
////            PIN3 - VADJ ---   |   --- 5v (VCC)                  ////
////                      PIN17 VLCD                           ////
////                                                      ////
////------------------------------------------------------------------------////
////                                                      ////
//// Funciones de usuario:                                       ////
////                                                      ////
//// glcd_init(mode)                                          ////
////    * Debe ser llamada antes de cualquier otra función, inicializa el   ////
////     GLCD en modo gráfico o modo alfanumérico.                     ////
////     - mode: CHARACTER - modo alfanumérico, GRAPHIC - modo gráfico      ////
////     NOTA: Para configurar el cursor en modo alfanumérico, ir a la    ////
////         sección CONFIGURACION INICIAL DEL GLCD, y descomentar      ////
////         los defines pertinentes.                           ////
////                                                      ////
////   <<<<<<<<<<<<<<<   ---------------------------   >>>>>>>>>>>>>>>>>>>      ////
////   <<<<<<<<<<<<<<< FUNCIONES PARA MODO GRAFICO >>>>>>>>>>>>>>>>>>>      ////
////   <<<<<<<<<<<<<<<   ---------------------------   >>>>>>>>>>>>>>>>>>>      ////
////                                                      ////
//// glcd_pixel(x,y,color)                                       ////
////   * Dibuja o borra el pixel del GLCD o del buffer, dado por las       ////
////     coordenadas (x,y).                                    ////
////     - x,y: coordenadas del GLCD o del buffer                     ////
////     - color: ON - pixel encendido, OFF - pixel apagado            ////
////                                                      ////
//// glcd_line(x1,y1,x2,y2,color)                                 ////
////   * Dibuja una linea recta en el GLCD o en el buffer, interpolando   ////
////     coordenadas iniciales y coordenadas finales.                  ////               ////
////     - x1,y1: coordenadas iniciales                           ////
////     - X2,Y2: coordenadas finales                              ////
////     - color: color de la línea, ON = negra, OFF = blanca            ////
////                                                      ////
//// glcd_rect(x1,y1,x2,y2,fill,color)                              ////
////   * Dibuja un rectángulo en el GLCD o en el buffer, dado por las       ////
////     coordenadas x1,y1 y x2,y2, puede ser con relleno o             ////
////     solamente el perímetro.                                 ////
////     - x1,y1: coordenadas superior izquierda del rectángulo         ////
////     - x2,y2: coordenadas inferor derecha del rectángulo            ////
////     - fill: YES - con relleno, NO - solo perímetro               ////
////     - color: color del perímetro y/o del relleno, ON / OFF         ////
////                                                      ////
//// glcd_bar(x1,y1,x2,y2,width,color)                              ////
////   * Dibuja una barra en el GLCD o en el buffer, dada por las          ////
////     coordenadas x1,y1 y x2,y2, de anchura variable, del primer al    ////
////     segundo punto. Si las coordenadas no se encuentran alineadas       ////
////     en el eje x, la barra tendrá angulo diferencte de 0.            ////
////     - x1,y1: coordenadas superior izquierda de la barra            ////
////     - x2,y2: coordenadas superior derecha de la barra               ////
////     - width: número de pixeles de grosor                        ////
////     - color: ON - negro, OFF - blanco                           ////
////                                                      ////
//// glcd_circle(x,y,radius,fill,color)                              ////
////   * Dibuja un circulo en el GLCD o en el buffer.                  ////
////     - x,y: coordenadas del centro del círculo                     ////
////     - radius: medida en pixeles del radio del círculo               ////
////     - fill: YES - con relleno, NO - sin relleno                  ////
////     - color: color del perímetro y/o del relleno, ON / OFF         ////
////                                                      ////
//// glcd_text57(x,y,*textptr,size,color)                            ////
////   * Escribe texto de 5x7 pixeles en el GLCD o el buffer, en modo      ////
////     gráfico, pudiendose escalar.                              ////
////     - x,y: coordenadas de la esquina superior izquierda del primer   ////
////          caracter                                       ////
////     - *textptr: puntero a la cadena de caracteres a imprimir         ////
////     - size: escala del texto, 1-5x7, 2-10x14, 3-15,21 ...            ////
////     - color: ON - negro, OFF - blanco                           ////
////   * Ejemplo de uso:                                       ////
////     char texto[]="Hola mundo"; // declara el arreglo de caracteres   ////
////     glcd_text57(5,5,texto,1,ON) // texto impreso con escala natural   ////
////                                                      ////
//// glcd_fill_screen(color)                                    ////
////   * Llena el GLCD o el buffer de un solo color.                  ////
////     - color: ON - negro, OFF - blanco                           ////
////                                                      ////
//// glcd_load_buffer()                                          ////
////   * Funcion que permite vaciar el contenido del buffer interno al    ////
////     GLCD, ESTA FUNCION SOLO ESTA DISPONIBLE SI SE USA EL BUFFER.      ////
////                                                      ////
////   <<<<<<<<<<<<<<<   -------------------------------- >>>>>>>>>>>>>>>   ////
////   <<<<<<<<<<<<<<< FUNCIONES PARA MODO ALFANUMERICO >>>>>>>>>>>>>>>   ////
////   <<<<<<<<<<<<<<<   -------------------------------- >>>>>>>>>>>>>>>   ////
////                                                      ////
//// glcd_putc(data)                                          ////
////   * Función para imprimir caracteres en el GLCD en modo alfanumérico,   ////
////     su uso es igual al de lcd_putc() de los LCD alfanuméricos.      ////
////     - data: un solo caracter o una cadena de caracteres            ////
////           las siguientes son secuencias de escape:               ////
////           \n - salta al inicio de la siguiente línea            ////
////           \f - borra la pantalla en modo de alfanúmerico y se      ////
////               posiciona en la esquina superior izquierda         ////
////   * ejemplo:                                             ////
////   glcd_putc("HOLA MUNDO\n");   // imprime "HOLA MUNDO" y salta línea   ////
////   * ejemplo con printf para imprimir variables:                  ////
////   unsigned int8 temperatura=25;   // variable entera sin signo      ////
////   printf(glcd_putc,"\fLa temperatura es: %u\n",temperatura);         ////
////   // al principio borra la pantalla y va al inicio, imprime         ////
////   // "la temperatura es: 25", y salta de línea.                  ////
////                                                      ////
//// glcd_gotoxy(x,y)                                          ////
////   * Salta a las coordenadas x,y del GLCD en modo alfanumérico         ////
////     en este modo el GLCD se compone de 26x10 caracteres.            ////
////     - x,y: coordenadas, donde pueden valer: x = 1 - 26, y = 1 - 10   ////
////                                                      ////
////////////////////////////////////////////////////////////////////////////////
////                                                      ////
//// Libreria elaborada por: Fabian Alfonso Flores                     ////
//// Fecha de liberación: 19/07/2012, México, EdoMex                  ////
//// Versión de CCS: 4.104                                       ////
//// Versión de la libreria glcd_160x80.h: 1.00                        ////
//// La presente libreria no garantiza el funcionamiento del GLCD asociado   ////
//// no obstante, respaldo la misma al decir que ha sido probada arduamente   ////
//// con resultados satisfactorios.                               ////
//// Esta libreria no se encuentra registrada, su uso es totalmente libre   ////
//// siempre y cuando no se utilice con propósitos comerciales, se puede   ////
//// modificar, siempre y cuando siga manteniendo su caracter libre y se   ////
//// redistribuyan las mejoras gratuitamente.                        ////
////                                                      ////
//// Algunas mejoras para versiones futuras:                        ////
//// * El modo de lectura del GLCD al uC, no se puede leer información      ////
////   proveniente del GLCD                                       ////
//// * El bit busy_flag no se encuentra habilitado para ser leído, en su   ////
////   lugar, se implementa un tiempo de espera después de escribir un      ////
////   comando, pudiendo el usuario variar el mismo.                  ////
//// * La secuencia de escape \b en modo alfanumérico que permite retroce-   ////
////   der una posición de memoria el cursor.                        ////
//// * Se advierte que algunas funciones en modo gráfico pudieran llegar a    ////
////   tener algunos bugs con el uso del buffer activado, puesto que este   ////
////   modo de funcionamiento no se probo tan a fondo.                  ////
//// * Cargar imagenes menores a 160x80 pixeles dando las coordenadas      ////
////   de la esquina superior izquierda, para poder cargar distintas      ////
////   imágenes más pequeñas en diferentes posiciones de pantalla.         ////
//// * No se ha escrito una función para leer multiples imágenes declaradas ////
////   en arreglos tipo const, en su lugar escribo la siguiente función      ////
////   en el programa principal:                                 ////
////                                                      ////
//// void put_image()                                          ////////////
//// {                                                            ////
////   glcd_go_address(0);      // manda la posición de memoria 0 del GLCD CGRAM   ////
////   glcd_command_write();   // manda comando para comenzar a enviar bytes      ////
////   for(i=0;i<80;i++)      // ciclo for para escribir las filas            ////
////   {                                                         ////
////      for(j=0;j<20;j++) glcd_send_byte(imagen[i][j]);   // for para las coumnas   ////
////    }                                                         ////
//// }                                                            ////
////                                                            ////
//// donde imagen[i][j] es una arreglo de caracteres de 80x20 bytes               ////
//// declarado de la siguiente forma:                                    ////
////                                                            ////
//// const int8 imagen[80][20]=   { 0x00,0x00 ... 20 bytes en total ... 0x00,0x00},   ////
////                                         .                     ////
////                                       .                     ////
////                                       .                     ////
////                              80 filas de 20 bytes c/u            ////
////                                       .                     ////
////                                       .                     ////
////                                       .                     ////
////                     { 0x00,0x00 ... 20 bytes en total ... 0x00,0x00}};   ////
////                                                            ////
//// Al llamar esta función, la misma escribirá en la pantalla del GCLD la imagen   ////
//// compuesta por los 80x20 bytes. Recomiendo el programa Image2code para generar   ////
//// el arreglo que conforma la imagen.                                    ////
////                                                            ////
////////////////////////////////////////////////////////////////////////////////////////

// PUERTO DE CONEXION DEL GLCD
//
// Solo descomenta uno de los siguientes 4 defines para configurar un puerto o
// para configurar los pines de forma separada

//#define PORT_B   // bus de datos por el puerto B
//#define PORT_C   // bus de datos por el puerto C
//#define PORT_D   // bus de datos por el puerto D
//#define NO_PORT    // bus de datos con pines separados

/* NOTA IMPORTANTE!: El configurar los pines de forma separada tiene la ventaja
                de la flexibilidad de conexión, pero también tiene la gran
                desventaja que el tiempo de procesamiento se eleva, en
                comparación de la velocidad necesaria en el GLCD, así que
                es recomendable usar un puerto completo para su control.
------------------------------------------------------------------------------------*/

// BUFFER INTERNO
//
// El siguiente define permite utilizar un buffer interno para escribir en el GLCD
// con lo que todos los comandos de impresión que se le manden al GLCD no se verán
// en la pantalla, sino que se escribirán en el buffer interno, esto ahorra mucho
// tiempo de ejecución, ya que una vez que el buffer contiene todo lo que se desea
// mostrar en la pantalla, se manda al GLCD por medio del comando glcd_load_buffer(),
// lo cuál se traduce en una elevada velocidad de impresión.

/*    NOTA IMPORTANTE: El uso del buffer interno solo está disponible a partir de los
                uC de la familia 18F, ya que la familia 16F tiene los bancos de 
                datos en forma separada y no es posible declarar una variable
                de 1600 bytes. Si se está utilizando un PIC16F, se deberá 
                comentar esta línea y no usar buffer. */

//#define USE_BUFFER   1

//------------------------------------------------------------------------------------

// ETIQUETAS
//
// Cambia los pines de conexión como más te convenga

//#ifdef NO_PORT
//#define GLCD_DB0   PIN_D7
//#define GLCD_DB1   PIN_D6
//#define GLCD_DB2   PIN_D5
//#define GLCD_DB3   PIN_D4
//#define GLCD_DB4   PIN_B0
//#define GLCD_DB5   PIN_B1
//#define GLCD_DB6   PIN_B2
//#define GLCD_DB7   PIN_B3
//#endif

//#define   GLCD_RS     PIN_B0
//#define   GLCD_E      PIN_B1
//#define   GLCD_RW     PIN_B2

/*----------------------------------------------------------------------------------

CONFIGURACION INCIAL DEL GLCD

Para configurar el GLCD, debes descomentar cada uno de los siguientes defines
dependiendo de la configuración que requieras.

NOTA: Estos defines sirven solamente en modo alfanumerico, es decir, cuando se
     inicializa el glcd con el comando glcd_init(CHARACTER) */

//#define GLCD_CURSOR_ON         1   // comenta para ocultar el cursor
//#define GLCD_CURSOR_BLINK_ON   1   // comenta para mostrar cursor sin parpadeo 
//#define GLCD_CHAR_BLINK_ON      1   // comenta para mostrar caracter sin parpadeo
#define CURSOR_HEIGHT         7   // altura del cursor (0-7) 7 = abajo, 0 = arriba

// Este valor es la frecuencia de refresco del GLCD, se recomienda no modificarla
#define DIVISION_NUMBER       80   // velocidad de refresco del GLCD (0-255) <<80>>

#define TIME   6   // Si notas que algunos pixeles se pierden al escribir en el GLCD
               // aumenta un poco este valor, es el tiempo que espera antes de
               // escribir un nuevo comando en el GLCD y darle asi tiempo para
               // procesarlo. 

// A PARTIR DE AQUI NO SE DEBE MODIFICAR NADA!
// ----------------------------------------------------------------------------------
#define GRAPHIC      1   
#define CHARACTER   0
#define YES         1
#define NO         0   
#define ON         1
#define OFF         0

#ifndef GLCD_WIDTH
#define GLCD_WIDTH 160       // tamaño en pixeles del ancho
#define GLCD_HEIGHT 80       // tamaño en pixeles de la altura
#endif



// BUFFER INTERNO
//
// El siguiente define permite utilizar un buffer interno para escribir en el GLCD
// con lo que todos los comandos de impresión que se le manden al GLCD no se verán
// en la pantalla, sino que se escribirán en el buffer interno, esto ahorra mucho
// tiempo de ejecución, ya que una vez que el buffer contiene todo lo que se desea
// mostrar en la pantalla, se manda al GLCD por medio del comando glcd_load_buffer(),
// lo cuál se traduce en una elevada velocidad de impresión.

/*    NOTA IMPORTANTE: El uso del buffer interno solo está disponible a partir de los
                uC de la familia 18F, ya que la familia 16F tiene los bancos de 
                datos en forma separada y no es posible declarar una variable
                de 1600 bytes. Si se está utilizando un PIC16F, se deberá 
                comentar esta línea y no usar buffer. */

#define USE_BUFFER   1
#ifdef USE_BUFFER
void glcd_load_buffer(void);
#endif


/////////////////////////////////////////////////////////////////////////////////
//   NOTA: FUNCION VALIDA SOLO EN MODO DE CARACTERES
//   Manda las coordenadas X y Y para escribir un caracter en la pantalla
//   Los valores que pueden tomar x y y son: x: 1-26, y: 1-10
//
//   @param   (x,y) - las coordenada en x,y del caracter
//   @return -
//
void glcd_gotoxy(char x,char y);
////////////////////////////////////////////////////////////////////////////////////
//   NOTA: FUNCION VALIDA SOLO EN MODO DE CARACTERES
//   Imprime caracteres en la pantalla, su funcionamiento es igual a la función
//   lcd_putc de los LCD alfanumericos.
//
//   @param   data - caracter o apuntador a cadena de caracteres a imprimir en el GLCD
//   @return   -
//
void glcd_putc(char data);
/////////////////////////////////////////////////////////////////////////////////
//   Grafica un solo pixel en el buffer o en la pantalla, las coordenadas pueden
//   valer en x: 1-160, en y: 1-80.
//
//   @param   (x, y) - coordenadas x,y
//         color: ON - enciende pixel, OFF - apaga pixel
//   @return -
//
void glcd_pixel(unsigned short x,unsigned short y,unsigned char color);
///////////////////////////////////////////////////////////////////////////////////
//   Dibuja una línea en el GLCD o el buffer, usando el algoritmo de Bresenham
//
//   @param   (x1, y1) - coordenadas de inicio
//          (x2, y2) - coordenadas finales
//          color - ON or OFF
//   @return   -
//
void glcd_line(signed short x1, signed short y1, signed short x2, signed short y2, unsigned char color);
//////////////////////////////////////////////////////////////////////////////////
//   Dibuja un rectángulo en el GLCD o el buffer
//    
//   @param   (x1, y1) - coordenadas de inicio
//        (x2, y2) - coordenadas finales
//         fill - YES o NO, indica si tiene relleno o no
//         color - ON or OFF, indica el color del relleno o del perímetro
//   @return   -
//
void glcd_rect(short x1, short y1, short x2, short y2, short fill, unsigned char color);

//////////////////////////////////////////////////////////////////////////////////
//   Dibuja una barra recta con una anchura variable y un angulo predeterminado
//
//   @param   (x1, y1) - coordenadas de inicio
//        (x2, y2) - coordenadas finales
//         width  - El número de pixeles de ancho
//         color - ON or OFF, color de la barra
//   @return   -
//
void glcd_bar(short x1, short y1, short x2, short y2, short width, unsigned char color);
//////////////////////////////////////////////////////////////////////////////////
//   Dibuja un circulo en el GLCD
//
//   @param   (x,y) - coordenadas del centro del circulo
//         radius - radio del circulo en pixeles
//         fill - YES or NO, si tiene relleno o no
//         color - ON or OFF, color del relleno o del perímetro
//   @return   -
//
void glcd_circle(short x, short y, short radius, unsigned char fill, unsigned char color);
//////////////////////////////////////////////////////////////////////////////////
//   Escribe texto en el GLCD en modo gráfico
//
//   @param   (x,y) - coordenada superior izquierda del primer caracter
//         textptr - puntero a una cadena de caracteres en memoria
//         size - Escala del texto, 1 - 5x7, 2 - 10x14, 3 - 15x21 ...
//          color - ON or OFF, color del texto
//   @return   -
//
void glcd_text57(short x, short y, char* textptr, int size, unsigned char color);
//////////////////////////////////////////////////////////////////////////////////
//   Limpia la pantalla o el buffer de blanco o negro
// 
//   @param   ON - deja todos los pixeles encendidos
//         OFF - apaga todos los pixeles
//   @return   -
//
void glcd_fill_screen(unsigned char color);

//////////////////////////////////////////////////////////////////////////////////
//   Inicializa la pantalla en modo gráfico o alfanumerico, debe ser llamada antes 
//   de cualquier otra funcion
//
//   @param   mode:    CHARACTER    - inicializa el GLCD en modo de caracteres
//               GRAPHIC      - inicializa el GLCD en modo gráfico
//   @return   -
//
void glcd_init(unsigned char mode);

#endif

