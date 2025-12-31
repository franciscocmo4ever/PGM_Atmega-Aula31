/*===================================================================
		Inicialização do Sistema
===================================================================*/
#define Mod_Ini

#include <stdint.h>
#include <avr/io.h>
#include "Mod_Def.h"

/*---------------------------------------------------------------------
					Inicialização do Sistema
---------------------------------------------------------------------*/
void Ini_System()
{
	DDRB = 0xFF;			//Programando todos bits do PORTB como escrita
	DDRD = 0xFF;			//Todos bits PORTD para escrita
	DDRC &= ~(1 << PC0);	//Configura as portas das chaves para leitura
	DDRC &= ~(1 << PC1);
	DDRC &= ~(1 << PC2);

	PORTB = 0x00;
	PORTD = 0xFF;

	Chaves = 0x00;					//Inicializa o ponteiro da leitura das chaves
	Reg_SW = Ch_Off;				//Limpa o buffer das chaves
//	Config_Usart();

//	Config_Timer1_OVF(); 				//Configura o Timer 1 no modo overflow
//	SREG |= (1 << 7);					//Habilita Global interrupção

	Inicilaiza_LCD();
	//Config_ADC();
}
