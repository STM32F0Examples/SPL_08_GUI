#include<string.h>
#include "GLCD.h"
#include "ARM_delay.h"

#define __GLCD_C__
#include "GLCD_instance.h"

//macros implemented to suport register bit clear and register bit set
#define bit_clear(dRegister,bitPos) dRegister&=~(0x1<<bitPos); 
#define bit_set(dRegister,bitPos) dRegister|=(0x1<<bitPos);

// REGISTROS INTERNOS
#define   MODE_CONTROL      0x00
#define CHAR_PITCH         0x01
#define   NUM_CHAR         0x02
#define   TIME_DIV         0x03
#define   CURSOR_POS         0x04
#define   START_LOW_ADD      0x08
#define   START_HIGH_ADD      0x09
#define CURSOR_LOW_ADD      0x0A
#define CURSOR_HIGH_ADD      0x0B
#define WRITE_GLCD         0x0C
#define READ_GLCD         0x0D
#define   CLEAR_BIT         0x0E
#define   SET_BIT            0x0F
unsigned short j,i;
unsigned char cursor_last_y=1;      // guarda la posici�n actual del cursor en el eje y
unsigned char operation_mode;

#ifdef USE_BUFFER
unsigned char pixel_buffer[1600];   // declaraci�n del buffer interno de escritura
#endif

// CARACTERES IMPRIMIBLES EN MODO GRAFICO
const char TEXT[51][5] ={0x00, 0x00, 0x00, 0x00, 0x00, // SPACE
                         0x00, 0x00, 0x5F, 0x00, 0x00, // !
                         0x00, 0x03, 0x00, 0x03, 0x00, // "
                         0x14, 0x3E, 0x14, 0x3E, 0x14, // #
                         0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
                         0x43, 0x33, 0x08, 0x66, 0x61, // %
                         0x36, 0x49, 0x55, 0x22, 0x50, // &
                         0x00, 0x05, 0x03, 0x00, 0x00, // '
                         0x00, 0x1C, 0x22, 0x41, 0x00, // (
                         0x00, 0x41, 0x22, 0x1C, 0x00, // )
                         0x14, 0x08, 0x3E, 0x08, 0x14, // *
                         0x08, 0x08, 0x3E, 0x08, 0x08, // +
                         0x00, 0x50, 0x30, 0x00, 0x00, // ,
                         0x08, 0x08, 0x08, 0x08, 0x08, // -
                         0x00, 0x60, 0x60, 0x00, 0x00, // .
                         0x20, 0x10, 0x08, 0x04, 0x02, // /
                         0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
                         0x04, 0x02, 0x7F, 0x00, 0x00, // 1
                         0x42, 0x61, 0x51, 0x49, 0x46, // 2
                         0x22, 0x41, 0x49, 0x49, 0x36, // 3
                         0x18, 0x14, 0x12, 0x7F, 0x10, // 4
                         0x27, 0x45, 0x45, 0x45, 0x39, // 5
                         0x3E, 0x49, 0x49, 0x49, 0x32, // 6
                         0x01, 0x01, 0x71, 0x09, 0x07, // 7
                         0x36, 0x49, 0x49, 0x49, 0x36, // 8
                         0x26, 0x49, 0x49, 0x49, 0x3E, // 9
                         0x00, 0x36, 0x36, 0x00, 0x00, // :
                         0x00, 0x56, 0x36, 0x00, 0x00, // ;
                         0x08, 0x14, 0x22, 0x41, 0x00, // <
                         0x14, 0x14, 0x14, 0x14, 0x14, // =
                         0x00, 0x41, 0x22, 0x14, 0x08, // >
                         0x02, 0x01, 0x51, 0x09, 0x06, // ?
                         0x3E, 0x41, 0x59, 0x55, 0x5E, // @
                         0x7E, 0x09, 0x09, 0x09, 0x7E, // A
                         0x7F, 0x49, 0x49, 0x49, 0x36, // B
                         0x3E, 0x41, 0x41, 0x41, 0x22, // C
                         0x7F, 0x41, 0x41, 0x41, 0x3E, // D
                         0x7F, 0x49, 0x49, 0x49, 0x41, // E
                         0x7F, 0x09, 0x09, 0x09, 0x01, // F
                         0x3E, 0x41, 0x41, 0x49, 0x3A, // G
                         0x7F, 0x08, 0x08, 0x08, 0x7F, // H
                         0x00, 0x41, 0x7F, 0x41, 0x00, // I
                         0x30, 0x40, 0x40, 0x40, 0x3F, // J
                         0x7F, 0x08, 0x14, 0x22, 0x41, // K
                         0x7F, 0x40, 0x40, 0x40, 0x40, // L
                         0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
                         0x7F, 0x02, 0x04, 0x08, 0x7F, // N
                         0x3E, 0x41, 0x41, 0x41, 0x3E, // O
                         0x7F, 0x09, 0x09, 0x09, 0x06, // P
                         0x1E, 0x21, 0x21, 0x21, 0x5E, // Q
                         0x7F, 0x09, 0x09, 0x09, 0x76};// R

const char TEXT2[44][5]={0x26, 0x49, 0x49, 0x49, 0x32, // S
                         0x01, 0x01, 0x7F, 0x01, 0x01, // T
                         0x3F, 0x40, 0x40, 0x40, 0x3F, // U
                         0x1F, 0x20, 0x40, 0x20, 0x1F, // V
                         0x7F, 0x20, 0x10, 0x20, 0x7F, // W
                         0x41, 0x22, 0x1C, 0x22, 0x41, // X
                         0x07, 0x08, 0x70, 0x08, 0x07, // Y
                         0x61, 0x51, 0x49, 0x45, 0x43, // Z
                         0x00, 0x7F, 0x41, 0x00, 0x00, // [
                         0x02, 0x04, 0x08, 0x10, 0x20, // back slash
                         0x00, 0x00, 0x41, 0x7F, 0x00, // ]
                         0x04, 0x02, 0x01, 0x02, 0x04, // ^
                         0x40, 0x40, 0x40, 0x40, 0x40, // _
                         0x00, 0x01, 0x02, 0x04, 0x00, // `
                         0x20, 0x54, 0x54, 0x54, 0x78, // a
                         0x7F, 0x44, 0x44, 0x44, 0x38, // b
                         0x38, 0x44, 0x44, 0x44, 0x44, // c
                         0x38, 0x44, 0x44, 0x44, 0x7F, // d
                         0x38, 0x54, 0x54, 0x54, 0x18, // e
                         0x04, 0x04, 0x7E, 0x05, 0x05, // f
                         0x08, 0x54, 0x54, 0x54, 0x3C, // g
                         0x7F, 0x08, 0x04, 0x04, 0x78, // h
                         0x00, 0x44, 0x7D, 0x40, 0x00, // i
                         0x20, 0x40, 0x44, 0x3D, 0x00, // j
                         0x7F, 0x10, 0x28, 0x44, 0x00, // k
                         0x00, 0x41, 0x7F, 0x40, 0x00, // l
                         0x7C, 0x04, 0x78, 0x04, 0x78, // m
                         0x7C, 0x08, 0x04, 0x04, 0x78, // n
                         0x38, 0x44, 0x44, 0x44, 0x38, // o
                         0x7C, 0x14, 0x14, 0x14, 0x08, // p
                         0x08, 0x14, 0x14, 0x14, 0x7C, // q
                         0x00, 0x7C, 0x08, 0x04, 0x04, // r
                         0x48, 0x54, 0x54, 0x54, 0x20, // s
                         0x04, 0x04, 0x3F, 0x44, 0x44, // t
                         0x3C, 0x40, 0x40, 0x20, 0x7C, // u
                         0x1C, 0x20, 0x40, 0x20, 0x1C, // v
                         0x3C, 0x40, 0x30, 0x40, 0x3C, // w
                         0x44, 0x28, 0x10, 0x28, 0x44, // x
                         0x0C, 0x50, 0x50, 0x50, 0x3C, // y
                         0x44, 0x64, 0x54, 0x4C, 0x44, // z
                         0x00, 0x08, 0x36, 0x41, 0x41, // {
                         0x00, 0x00, 0x7F, 0x00, 0x00, // |
                         0x41, 0x41, 0x36, 0x08, 0x00, // }
                         0x02, 0x01, 0x02, 0x04, 0x02};// ~



//////////////////////////////////////////////////////////////////////////////
//   Pone los datos a enviar al GLCD en los pines de salida configurados
//
//   @param    data - el byte que se enviara por el puerto
//   @return    -
//
void put_data_on_glcd_pins(char data)
{
	delay_us(TIME);
	__GLCD_CTRL_Pin_Write(GLCD_PIN_E,1);
	delay_us(1);//1
	__GLCD_Put_Data(data);
	delay_us(1);//1
	__GLCD_CTRL_Pin_Write(GLCD_PIN_E,0);
}

/////////////////////////////////////////////////////////////////////////////////
//   Escribe en un registro interno del GLCD
//
//   @param   reg - direcci�n del registro al que se escribir�
//         data - byte a escribir en el registro
//   @return   -
//
void glcd_send_command(char reg,char data)
{
   __GLCD_CTRL_Pin_Write(GLCD_PIN_RS,GLCD_RS_INSTRUCTION);
   put_data_on_glcd_pins(reg);
   __GLCD_CTRL_Pin_Write(GLCD_PIN_RS,GLCD_RS_DATA);
   put_data_on_glcd_pins(data);
}

//////////////////////////////////////////////////////////////////////////////
//   Manda la direcci�n de uno de los 1600 bytes que conforman la pantalla 
//   en modo grafico, cada fila se compone de 20 bytes y son 80 filas. La
//   primer direcci�n comienza en 0.
//
//   @param   address - direcci�n de 16 bits.
//   @return -
//
void glcd_go_address(short address)
{
   char MSB_address;
   char LSB_address;
   MSB_address=(char) (address>>8);
   LSB_address=(char) address;
   glcd_send_command(CURSOR_LOW_ADD,LSB_address);
   glcd_send_command(CURSOR_HIGH_ADD,MSB_address);
}

////////////////////////////////////////////////////////////////////////////////
//   Escribe la direcci�n del registro de escritura, y todos los bytes que le
//   sigan con la se�al GLCD_RS=0 se mostrar�n en la pantalla, al escribir un
//   nuevo byte, la direcci�n de memoria aumenta en 1, con lo que se pueden
//   escribir de corrido los 1600 bytes que conforman la memoria en forma serial.
//
//   @param    -
//   @return   -
//
void glcd_command_write()
{
   __GLCD_CTRL_Pin_Write(GLCD_PIN_RS,GLCD_RS_INSTRUCTION);
   put_data_on_glcd_pins(WRITE_GLCD);
}

/////////////////////////////////////////////////////////////////////////////////
//   Manda un byte al GLCD, este comando sirve para mandar los bytes a imprimir
//   en la pantalla utilizando una unica vez al comienzo, la funci�n
//   glcd_command_write(), posteriormente se pueden enviar tantos bytes se requiera
//   imprimir en pantalla.
//
//   @param   data - byte a imprimir en la pantalla
//   @return   -
//
void glcd_send_byte(char data)
{
   __GLCD_CTRL_Pin_Write(GLCD_PIN_RS,GLCD_RS_DATA);
   put_data_on_glcd_pins(data);
}

//////////////////////////////////////////////////////////////////////////////
//   Carga el contenido del buffer de escritura en la pantalla, solamente se
//   procesa esta funci�n si se ha descomentado al inicio #define USE_BUFFER 1
//
//   @param   -
//   @return   -
//
#ifdef USE_BUFFER
void glcd_load_buffer()
{
   glcd_go_address(0);   // direccion byte 0
   glcd_command_write();
   for(i=0;i<1600;i++)
   {
      glcd_send_byte(pixel_buffer[i]);
   }
}
#endif


/////////////////////////////////////////////////////////////////////////////////
//   NOTA: FUNCION VALIDA SOLO EN MODO DE CARACTERES
//   Manda las coordenadas X y Y para escribir un caracter en la pantalla
//   Los valores que pueden tomar x y y son: x: 1-26, y: 1-10
//
//   @param   (x,y) - las coordenada en x,y del caracter
//   @return -
//
void glcd_gotoxy(char x,char y)
{
   if(operation_mode==CHARACTER)
   {   
      short address;
      char MSB_address;
      char LSB_address;
      cursor_last_y=y;   // guarda la nueva fila
   
      switch(y)
      {
         case 1 :    address=0;       break;
         case 2 :    address=28;    break;
         case 3 :    address=56;    break;
         case 4 :    address=84;    break;
         case 5 :    address=112;   break;
         case 6 :    address=140;    break;
         case 7 :    address=168;    break;
         case 8 :    address=196;    break;
         case 9 :    address=224;    break;
         case 10 :   address=252;    break;
      }
      address+=x-1;
      
      // divide el short en dos bytes
      MSB_address=(char)address>>8;
      LSB_address=(char)address;
      glcd_send_command(CURSOR_LOW_ADD,LSB_address);   // registro de posici�n del cursor en memoria LSB
      glcd_send_command(CURSOR_HIGH_ADD,MSB_address);   // registro de posici�n del cursor en memoria MSB
   }
}

////////////////////////////////////////////////////////////////////////////////////
//   NOTA: FUNCION VALIDA SOLO EN MODO DE CARACTERES
//   Imprime caracteres en la pantalla, su funcionamiento es igual a la funci�n
//   lcd_putc de los LCD alfanumericos.
//
//   @param   data - caracter o apuntador a cadena de caracteres a imprimir en el GLCD
//   @return   -
//

void glcd_putc(char data)
{
   if(operation_mode==CHARACTER)
   {
      switch(data)
      {
         case '\f':
            // Manda el cursor a la primera posici�n de memoria
            glcd_gotoxy(1,1);
            for(i=0;i<280;i++)   // ciclo para borrar pantalla
            {
               glcd_send_command(WRITE_GLCD,data);
            }
            // Manda el cursor a la primera posici�n de memoria
            glcd_gotoxy(1,1);
            break;
         case '\n':
            switch(cursor_last_y)
            {
               case 1: glcd_gotoxy(1,2); break;
               case 2: glcd_gotoxy(1,3); break;
               case 3: glcd_gotoxy(1,4); break;
               case 4: glcd_gotoxy(1,5); break;
               case 5: glcd_gotoxy(1,6); break;
               case 6: glcd_gotoxy(1,7); break;
               case 7: glcd_gotoxy(1,8); break;
               case 8: glcd_gotoxy(1,9); break;
               case 9: glcd_gotoxy(1,10); break;
               case 10: glcd_gotoxy(1,1); break;
            }
            break;
         default:
            glcd_send_command(WRITE_GLCD,data);
      }
   }
}


#define abs(signedNumber) (signedNumber>0)?(signedNumber):(-signedNumber)

//short abs(short signedNumber){
//	if(signedNumber>0){
//		return signedNumber;
//	}else{
//		return -signedNumber;
//	}
//}


/////////////////////////////////////////////////////////////////////////////////
//   Grafica un solo pixel en el buffer o en la pantalla, las coordenadas pueden
//   valer en x: 1-160, en y: 1-80.
//
//   @param   (x, y) - coordenadas x,y
//         color: ON - enciende pixel, OFF - apaga pixel
//   @return -
//
void glcd_pixel(unsigned short x,unsigned short y,unsigned char color)
{
   if(x<=160&&y<=80&&x>0&&y>0)   // solo grafica si las coordenadas est�n dentro del rango
   {
      unsigned short eje_x;
      unsigned short eje_y;
      signed char bit_number;
      unsigned short address;

      eje_y=(y-1)*20;      // obtiene el valor de la fila
      eje_x=(x-1)/8;      // obtiene el valor de la columna
      address=eje_x+eje_y;   // obtiene el n�mero de byte
      
      #ifdef USE_BUFFER
      bit_number=(x-1)%8;   // obtiene la posici�n del bit
      if(color==OFF&&address<=1600) bit_clear(pixel_buffer[address],bit_number);
      if(color==ON&&address<=1600) bit_set(pixel_buffer[address],bit_number);
      #else
      bit_number=8-(x-1)%8;   // obtiene la posici�n del bit
      glcd_go_address(address);
      if(color==OFF) glcd_send_command(CLEAR_BIT,abs(bit_number-8));   // registro limpiar un bit
      else glcd_send_command(SET_BIT,abs(bit_number-8));            // registro setear un bit
      #endif
   }
}

///////////////////////////////////////////////////////////////////////////////////
//   Dibuja una l�nea en el GLCD o el buffer, usando el algoritmo de Bresenham
//
//   @param   (x1, y1) - coordenadas de inicio
//          (x2, y2) - coordenadas finales
//          color - ON or OFF
//   @return   -
//
void glcd_line(signed short x1, signed short y1, signed short x2, signed short y2, unsigned char color)
{
   signed short  x, y, addx, addy, dx, dy;
   signed short P;
   short i;
   dx = abs((signed short)(x2 - x1));
   dy = abs((signed short)(y2 - y1));
   x = x1;
   y = y1;

   if(x1 > x2)
      addx = -1;
   else
      addx = 1;
   if(y1 > y2)
      addy = -1;
   else
      addy = 1;

   if(dx >= dy)
   {
      P = 2*dy - dx;

      for(i=0; i<=dx; ++i)
      {
         glcd_pixel(x, y, color);

         if(P < 0)
         {
            P += 2*dy;
            x += addx;
         }
         else
         {
            P += 2*dy - 2*dx;
            x += addx;
            y += addy;
         }
      }
   }
   else
   {
      P = 2*dx - dy;

      for(i=0; i<=dy; ++i)
      {
         glcd_pixel(x, y, color);

         if(P < 0)
         {
            P += 2*dx;
            y += addy;
         }
         else
         {
            P += 2*dx - 2*dy;
            x += addx;
            y += addy;
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////
//   Dibuja un rect�ngulo en el GLCD o el buffer
//    
//   @param   (x1, y1) - coordenadas de inicio
//        (x2, y2) - coordenadas finales
//         fill - YES o NO, indica si tiene relleno o no
//         color - ON or OFF, indica el color del relleno o del per�metro
//   @return   -
//
void glcd_rect(short x1, short y1, short x2, short y2, short fill, unsigned char color)
{
   if(fill)
   {
      short y, ymax;                          // Find the y min and max
      if(y1 < y2)
      {
         y = y1;
         ymax = y2;
      }
      else
      {
         y = y2;
         ymax = y1;
      }

      for(; y<=ymax; ++y)                    // Draw lines to fill the rectangle
         glcd_line(x1, y, x2, y, color);
   }
   else
   {
      glcd_line(x1, y1, x2, y1, color);      // Draw the 4 sides
      glcd_line(x1, y2, x2, y2, color);
      glcd_line(x1, y1, x1, y2, color);
      glcd_line(x2, y1, x2, y2, color);
   }
}

//////////////////////////////////////////////////////////////////////////////////
//   Dibuja una barra recta con una anchura variable y un angulo predeterminado
//
//   @param   (x1, y1) - coordenadas de inicio
//        (x2, y2) - coordenadas finales
//         width  - El n�mero de pixeles de ancho
//         color - ON or OFF, color de la barra
//   @return   -
//
void glcd_bar(short x1, short y1, short x2, short y2, short width, unsigned char color)
{
   signed short  x, y, addx, addy, j;
   signed short P, dx, dy, c1, c2;
   int i;
   dx = abs((signed short)(x2 - x1));
   dy = abs((signed short)(y2 - y1));
   x = x1;
   y = y1;
   c1 = -dx*x1 - dy*y1;
   c2 = -dx*x2 - dy*y2;

   if(x1 > x2)
   {
      addx = -1;
      c1 = -dx*x2 - dy*y2;
      c2 = -dx*x1 - dy*y1;
   }
   else
      addx = 1;
   if(y1 > y2)
   {
      addy = -1;
      c1 = -dx*x2 - dy*y2;
      c2 = -dx*x1 - dy*y1;
   }
   else
      addy = 1;

   if(dx >= dy)
   {
      P = 2*dy - dx;

      for(i=0; i<=dx; ++i)
      {
         for(j=-(width/2); j<width/2+width%2; ++j)
         {
            if(dx*x+dy*(y+j)+c1 >= 0 && dx*x+dy*(y+j)+c2 <=0)
               glcd_pixel(x, y+j, color);
         }
         if(P < 0)
         {
            P += 2*dy;
            x += addx;
         }
         else
         {
            P += 2*dy - 2*dx;
            x += addx;
            y += addy;
         }
      }
   }
   else
   {
      P = 2*dx - dy;

      for(i=0; i<=dy; ++i)
      {
         if(P < 0)
         {
            P += 2*dx;
            y += addy;
         }
         else
         {
            P += 2*dx - 2*dy;
            x += addx;
            y += addy;
         }
         for(j=-(width/2); j<width/2+width%2; ++j)
         {
            if(dx*x+dy*(y+j)+c1 >= 0 && dx*x+dy*(y+j)+c2 <=0)
               glcd_pixel(x+j, y, color);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////
//   Dibuja un circulo en el GLCD
//
//   @param   (x,y) - coordenadas del centro del circulo
//         radius - radio del circulo en pixeles
//         fill - YES or NO, si tiene relleno o no
//         color - ON or OFF, color del relleno o del per�metro
//   @return   -
//
void glcd_circle(short x, short y, short radius, unsigned char fill, unsigned char color)
{
   signed short a, b, P;
   a = 0;
   b = radius;
   P = 1 - radius;

   do
   {
      if(fill)
      {
         glcd_line(x-a, y+b, x+a, y+b, color);
         glcd_line(x-a, y-b, x+a, y-b, color);
         glcd_line(x-b, y+a, x+b, y+a, color);
         glcd_line(x-b, y-a, x+b, y-a, color);
      }
      else
      {
         
         glcd_pixel(a+x, b+y, color);
         glcd_pixel(b+x, a+y, color);
         glcd_pixel(x-a, b+y, color);
         glcd_pixel(x-b, a+y, color);
         glcd_pixel(b+x, y-a, color);
         glcd_pixel(a+x, y-b, color);
         glcd_pixel(x-a, y-b, color);
         glcd_pixel(x-b, y-a, color);
      }

      if(P < 0)
         P+= 3 + 2*a++;
      else
         P+= 5 + 2*(a++ - b--);
    } while(a <= b);
}

unsigned char bit_test(char byteToTest, int bitPos){
	if(((byteToTest>>bitPos)&0x1)!=0){
		return 1;
	}else{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//   Escribe texto en el GLCD en modo gr�fico
//
//   @param   (x,y) - coordenada superior izquierda del primer caracter
//         textptr - puntero a una cadena de caracteres en memoria
//         size - Escala del texto, 1 - 5x7, 2 - 10x14, 3 - 15x21 ...
//          color - ON or OFF, color del texto
//   @return   -
//
void glcd_text57(short x, short y, char* textptr, int size, unsigned char color)
{
   int i, j, k, l, m;                     // Loop counters
   char pixelData[5];                     // Stores character data

   for(i=0; textptr[i] != '\0'; ++i, ++x) // Loop through the passed string
   {
      if(textptr[i] < 'S') // Checks if the letter is in the first text array
         memcpy(pixelData, TEXT[textptr[i]-' '], 5);
      else if(textptr[i] <= '~') // Check if the letter is in the second array
         memcpy(pixelData, TEXT2[textptr[i]-'S'], 5);
      else
         memcpy(pixelData, TEXT[0], 5);   // Default to space

      if(x+5*size >= GLCD_WIDTH)          // Performs character wrapping
      {
         x = 0;                           // Set x at far left position
         y += 7*size + 1;                 // Set y at next position down
      }
      for(j=0; j<5; ++j, x+=size)         // Loop through character byte data
      {
         for(k=0; k<7*size; ++k)          // Loop through the vertical pixels
         {
            if(bit_test(pixelData[j], k)) // Check if the pixel should be set
            {
               for(l=0; l<size; ++l)      // The next two loops change the
               {                          // character's size
                  for(m=0; m<size; ++m)
                  {
                     glcd_pixel(x+m, y+k*size+l, color); // Draws the pixel
                  }
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////
//   Limpia la pantalla o el buffer de blanco o negro
// 
//   @param   ON - deja todos los pixeles encendidos
//         OFF - apaga todos los pixeles
//   @return   -
//
void glcd_fill_screen(unsigned char color)
{
   #ifdef USE_BUFFER
   if(color==ON)    for(i=0;i<1600;i++) pixel_buffer[i]=255;
   else          for(i=0;i<1600;i++) pixel_buffer[i]=0;
   //glcd_load_buffer();

   #else
   glcd_go_address(0);   // direccion byte 0
   glcd_command_write();
   if(color==ON)    for(i=0;i<1600;i++) glcd_send_byte(255);
   else          for(i=0;i<1600;i++)   glcd_send_byte(0);
   #endif
}

//////////////////////////////////////////////////////////////////////////////////
//   Inicializa la pantalla en modo gr�fico o alfanumerico, debe ser llamada antes 
//   de cualquier otra funcion
//
//   @param   mode:    CHARACTER    - inicializa el GLCD en modo de caracteres
//               GRAPHIC      - inicializa el GLCD en modo gr�fico
//   @return   -
//
void glcd_init(unsigned char mode)  
{
   char config_byte=0x20;//0b00100000;
   __GLCD_Configure_Pins();
   if(mode==GRAPHIC)
   {
      operation_mode=GRAPHIC;
      config_byte|=0x02;//0b00000010;      // configura modo grafico
   }
   else
   {
      operation_mode=CHARACTER;
      config_byte&=0xFD;//0b11111101;   // configura modo alfanum�rico
      #ifdef GLCD_CURSOR_ON
      config_byte|=0x04;//0b00000100;   // configura cursor ON
      #else
      config_byte&=0xFB;//0b11111011;   // configura cursor OFF
      #endif

      #ifdef GLCD_CURSOR_BLINK_ON
      config_byte|=0x08;//0b00001000;   // configura cursor blink ON
      #else
      config_byte&=0xF7;//0b11110111;   // configura cursor blink OFF
      #endif

      #ifdef GLCD_CHAR_BLINK_ON
      config_byte|=0x08//0b00001000; // configura blink de caracter ON
      #endif
   }

   // ESCRITURA EN LOS REGISTROS INTERNOS
   glcd_send_command(MODE_CONTROL,config_byte);         // escribe en registro de control

   if(operation_mode==CHARACTER) glcd_send_command(CHAR_PITCH,0x75/*0b01110101*/);   // ancho de caracter (6 puntos)
   else glcd_send_command(CHAR_PITCH,0x07/*0b00000111*/);   // n�mero de bits por byte (8 puntos)

   if(operation_mode==CHARACTER) glcd_send_command(NUM_CHAR,26);   // caracteres
   else glcd_send_command(NUM_CHAR,19);   // bytes

   glcd_send_command(TIME_DIV,DIVISION_NUMBER);         // registro velocidad de refresco
   
   if(operation_mode==CHARACTER) glcd_send_command(CURSOR_POS,CURSOR_HEIGHT);
   else glcd_send_command(CURSOR_POS,0x00);   // no aplica para modo gr�fico
   
   glcd_send_command(START_LOW_ADD,0);         // registro posici�n de inicio LSB
   glcd_send_command(START_HIGH_ADD,0);      // registro posici�n de inicio MSB
   
   // limpia la pantalla
   if(operation_mode==GRAPHIC){
		glcd_fill_screen(OFF);
		glcd_load_buffer();
	 }   
   else glcd_putc('\f');
}
