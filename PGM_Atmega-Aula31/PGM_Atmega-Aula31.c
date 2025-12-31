/*================================================================================================
		Projeto: PGM_Atmega_Aula 31
		Propósito: Comunicação Serial usando protocolo I2C emulado
					Como capturar o resposta do disposivo escravo (DS1307)
		Processador: Atmega328
		Frequência do Sistema: 8MHz/8 (RC Interno)
		Configuração Fusíveis: L=0x62;H=0xD9;E=0xFF

		Data:Fevereiro/2020
		Autor: José Carlos

		Revisão: 2.0
		Alterado modo de manipulação dos pórticos. Agora, os pórticos estão
		relacionados aos pinos do processador e não mais a constantes arbitrárias
=================================================================================================*/
#define Mod_Main

#include <avr/io.h>
#include <avr/io.h>
#include <stdint.h>
#include "Mod_Def.h"


unsigned char nClock;
unsigned char Mask;
unsigned int delayus;
unsigned char ctedelay100us;
unsigned char LeitDS1307;
unsigned char Buffer_Uso_Geral;
/*===========================================================================
						DEFINIÇÕES DO PROGRAMADOR
===========================================================================*/
#define	RelWr			0
#define RelRd			1
#define DS1307_Address	0b01101000
#define Cte_Delay_100us		5		//Cte para 100us @ 1Mz

//-----------------------------------------------------------------------------
#define WAIT_100US(data)													\
for(delayus = data/100;delayus > 0; delayus--)								\
{																			\
	for(ctedelay100us = Cte_Delay_100us;ctedelay100us > 0; ctedelay100us--)	\
	asm("nop");																\
}
//-----------------------------------------------------------------------------
#define CLOCK												\
PORTC |=(0b00001000);		/*Coloca SCl em 1*/				\
WAIT_100US(100);			/*100us de largura de pulso*/	\
PORTC &=(0b11110111);		/*Coloca SCl em 0*/				
//-----------------------------------------------------------------------------
void I2C_Control_Write(unsigned char Addres_Device, unsigned char Addres_Data,unsigned char Data_Write);
void I2C_Control_Read(unsigned char Addres_Device,unsigned char Address_Data);

//------------------------------------------------------------------------------
int main(void)
{
	Ini_System();		//Prepara o sistema para começar a trabalhar

	Config_Portbit(SDA,Port_Write);
	Config_Portbit(SCL,Port_Write);

	Escreve_String_LCD(1,"VIDEO AULA 31");

	I2C_Control_Write(DS1307_Address,0x22,0x0F);

	while(1)
	{
		I2C_Control_Read(DS1307_Address,0x22);			//Le o Segundo do relógio e guarda
		Wait(1000);
	}//Fim while(1)
}//Fim main
/*=====================================================================================
	Módulo I2C para dispositivos com 7 bits de endereço + 1 bit de escrita/leitura
=====================================================================================*/

/*====================================================
Rotina:I2C_Control_Read
Função:Ĝera um sinal de leitura para o DS1307 com endereço definido
Chamada:I2C_Control_Read(<endereço que se deseja ler>
Comentário:Rotina encerra comunicação após a leitura do dado
Exemplo:
-----------------------------------------------------*/


/*====================================================
Rotina:
Função:
Chamada:
Comentário:
Exemplo:
-----------------------------------------------------*/
