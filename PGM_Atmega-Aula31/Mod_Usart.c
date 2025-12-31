/*===================================================================
		Rotinas de tratamento do módulo Usart
===================================================================*/
#define Mod_Usart

#include <stdint.h>
#include <avr/io.h>
#include "Mod_Def.h"


/*==============================================================================================
Rotina: Config_Usart ()
Função: Inicializa a Usart com a velocidade definida em "BAUD"
Chamada: Config_Usart()
Comentários: Os valores de U2X0 e UBRRH e UBRRL são calculados pela macro setbaud.h que deve
			 ser declarada. F_CPU também deve estar decladada. Pela razão desse parâmetro poder
			 ser	usado por outras funções, está declarado no Mod_def.h, tornando-se assim
			 "conhecido"	por todos os módulos.
			 Foi adotado modo assíncrono, 8 bits de dados, sem paridade e 1 stop bit.
			 Não gera interrupção na recepção.
-----------------------------------------------------------------------------------------------*/
void Config_Usart()
{

  	 UCSR0A |= (1 << U2X0);				//Modo 2x

    // Set the Mode & Frame Parameters

    UCSR0C = 0x06;                   // Asynchronous, 8-data, No parity, 1-stop

    // Enable USART0 Transmitter and Receiver

    //UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1<< RXCIE0); //Habilita Tx, Rx e Interrupção de Rx;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0B &= ~(1<< RXCIE0); 			//Não habilita Interrupção de Rx Tx, Rx

    UBRR0H = 0x00;
    UBRR0L = 0x0C;					//9600 Frequência Sistema 1MHz Modo 2x
}
/*========================================================================
 ROTINA  : Envia_String_Uart
 FUNCAO  : Transmite uma string previamnete definida via UART
 Comentário: A string deve estar previamente criada.
 Exemplo: Deseja-se enviar a string "PCI UNIVERSAL" para UART .
		 1 - Cria-se a string.
		 __flash code unsigned char Mens_Exemplo[]="PCI UNIVERSAL"

		 2-Chamada da função:
		 Envia_String_Uart (Mens_Exemplo);
========================================================================*/
void Envia_String_Usart (char *pont_mens)
{
	while (*pont_mens != 0)
	{
		while (Teste_Sysbit(UCSR0A,UDRE0)== 0)		//Espera o buffer esvasiar
		{}
		UDR0 = *pont_mens;
		pont_mens++;					//Próximo caracter
	}
}
/*==============================================================================================
Rotina:	Wr_Data_Usart()
Função:	Escreve um byte na Usart
Chamada: Wr_Data_Usart(<data>)
Comentários: O dado não é convertido para ASCII
-----------------------------------------------------------------------------------------------*/
void Wr_Data_Usart(uint8_t Dado)
{

	while (Teste_Sysbit(UCSR0A,UDRE0)== 0)		//Espera o buffer esvaziar
	{}
	UDR0 = Dado;		//Manda

}
/*==============================================================================================
Rotina: Rd_Byte_Usart
Função: Lê um byte da Usart
Chamada: Rd_Byte_Usart()
Comentários: Devolve em Data_Usart o byte Lido.
			 Sinaliza em bRX com zero se não o buffer de leitura está vazio para leitura
-----------------------------------------------------------------------------------------------*/
void Rd_Byte_Usart()
{
	if(TesteBit(bHab_UsartRx) == 1)
	{
		while (Teste_Sysbit(UCSR0A, RXC0) == 0) {}	//Tem um dado na Usart?
		UCSR0A |= (1  << UCSR0A);
		Data_Usart = UDR0;
	}
}
/*====================================================
Rotina:
Função:
Chamada:
Comentário:
Exemplo:
-----------------------------------------------------*/
