/*
 * main.c
 *
 *  Created on: 04.11.2012
 *      Author: Paul
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#define colorprez 11



uint8_t pixeltime = 0;
uint8_t row = 0;
//Pointer to the current Image
unsigned char *picture;
//Pointer to the current buffer Image
unsigned char *picbuffer;
uint8_t buPos = 0;
/**
 * receive data from pc
 * TODO:implement some protocol
 * */
ISR(USART_RX_vect)
{
	//read command bit
	//IF UCSRC ...
	picbuffer[buPos++] = UDR;
	if(buPos >= 192)
	{
		unsigned char* tmp = picbuffer;
		picbuffer = picture;
		picture = tmp;
	}
}
/**
 * color rendering interrupt
 * */
ISR(TIMER0_COMP_vect)
{
	//Set all GND pins to high and all LED Pins to low
	uint8_t portA = 0xC0;
	uint8_t portB = 0xF8;
	uint8_t portC = 0xC0;
	uint8_t portD = 0x00;
	uint8_t portE = 0x00;
	uint8_t pixelpos = row * 24;

	//Clear Current Row
	switch (row)
	{
	case 0:
		portB = 0xE8; //ROW 1
		break;
	case 1:
		portB = 0xF0; //ROW 2
		break;
	case 2:
		portB = 0x78; //ROW 3
		break;
	case 3:
		portB = 0xB8; //ROW 4
		break;
	case 4:
		portC = 0x80;  //Row 5
		break;
	case 5:
		portC = 0x40;  //Row 6
		break;
	case 6:
		portA = 0x40; //ROW 7
		break;
	case 7:
		portA = 0x80;   //ROW 8
		break;

	}
	//Render each RGB Pixel for each Column
	if (picture[pixelpos] > pixeltime) //COL 0 R
	{
		portB |= 1;
	}
	if (picture[pixelpos + 1] > pixeltime)// G
	{

		portB |= 2;
	}
	if (picture[pixelpos + 2] > pixeltime)// B
	{
		portB |= 4;
	}

	if (picture[pixelpos + 3] > pixeltime) //COL 1
	{
		portA |= 1;
	}
	if (picture[pixelpos + 4] > pixeltime)
	{
		portA |= 2;
	}
	if (picture[pixelpos + 5] > pixeltime)
	{
		portA |= 4;
	}

	if (picture[pixelpos + 6] > pixeltime) //COL 2
	{
		portE |= 1;
	}
	if (picture[pixelpos + 7] > pixeltime)
	{
		portE |= 2;
	}
	if (picture[pixelpos + 8] > pixeltime)
	{
		portE |= 4;
	}

	if (picture[pixelpos + 9] > pixeltime) //COL 3
	{
		portA |= 8;
	}
	if (picture[pixelpos + 10] > pixeltime)
	{
		portA |= 0x10;
	}
	if (picture[pixelpos + 11] > pixeltime)
	{
		portA |= 0x20;
	}

	if (picture[pixelpos + 12] > pixeltime) //COL 4
	{
		portD |= 4;
	}
	if (picture[pixelpos + 13] > pixeltime)
	{
		portD |= 8;
	}
	if (picture[pixelpos + 14] > pixeltime)
	{
		portD |= 0x10;
	}

	if (picture[pixelpos + 15] > pixeltime) //COL 5
	{
		portC |= 8;
	}
	if (picture[pixelpos + 16] > pixeltime)
	{
		portC |= 0x10;
	}
	if (picture[pixelpos + 17] > pixeltime)
	{
		portC |= 0x20;
	}
	if (picture[pixelpos + 18] > pixeltime) //COL 6
	{
		portD |= 0x20;
	}
	if (picture[pixelpos + 19] > pixeltime)
	{
		portD |= 0x40;
	}
	if (picture[pixelpos + 20] > pixeltime)
	{
		portD |= 0x80;
	}
	if (picture[pixelpos + 21] > pixeltime) //COL 7
	{
		portC |= 1;
	}
	if (picture[pixelpos + 22] > pixeltime)
	{
		portC |= 2;
	}
	if (picture[pixelpos + 23] > pixeltime)
	{
		portC |= 4;
	}
	PORTA = portA;
	PORTB = portB;
	PORTC = portC;
	PORTD = portD| (PORTD & 3);
	PORTE = portE;
	pixeltime += 4;
	if (pixeltime == 0)
	{
		row++;
		if (row == 8)
		{
			row = 0;
		}
	}

}
int main()
{
	//Initialize buffer and image
	picture =(unsigned char *) malloc(192);
	picbuffer =(unsigned char *) malloc(192);

	//Set USART Parameters
	UBRRH = 25 >> 8;
	UBRRL = 25;
	UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);

	//Set Data Direction of each Pin
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFE;
	DDRE = 0x07;
	for (uint8_t i = 0; i < 64; i++)
	{

		unsigned char seg = i / colorprez;
		unsigned char arrpos = i * 3;
		unsigned int f = (i % colorprez) * 255 / colorprez;
		unsigned char q = 255 - f;
		unsigned char t = f;
		switch (seg)
		{
		case 0:
		default:
			picture[arrpos] = 255;
			picture[arrpos + 1] = t;
			picture[arrpos + 2] = 0;
			break;

		case 1:
			picture[arrpos] = q;
			picture[arrpos + 1] = 255;
			picture[arrpos + 2] = 0;
			break;
		case 2:
			picture[arrpos] = 0;
			picture[arrpos + 1] = 255;
			picture[arrpos + 2] = t;
			break;
		case 3:
			picture[arrpos] = 0;
			picture[arrpos + 1] = q;
			picture[arrpos + 2] = 255;
			break;
		case 4:
			picture[arrpos] = t;
			picture[arrpos + 1] = 0;
			picture[arrpos + 2] = 255;
			break;
		case 5:

			picture[arrpos] = 255;
			picture[arrpos + 1] = 0;
			picture[arrpos + 2] = q;
			break;
		}

	}
	//Set timer interrupt
	TCCR0 = (1 << WGM01) | (0 << WGM00) | (0 << COM01) | (0 << COM00)
			| (0 << CS02) | (0 << CS01) | (1 << CS00);
	TIMSK = (0 << TOIE0) | (1 << OCIE0);
	OCR0 = 255;
	sei();
	//Do nothing when nothing has to be done
	while (1)
	{
	}

}

