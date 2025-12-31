/*===================================================================
		Rotinas de Interrupção do Timer 1
===================================================================*/
#define Mod_Timer

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Mod_Def.h"


#define T_Timer1_OVF 0xFB1E		//Inincia timer com valor para que estouro ocorra em 10ms
								// 65536-(1MHz/8/100Hz) = 64286 =hxd 0xFB1E

//#define T_Timer1_OVF 0xF63C	// Inincia timer com valor para que estouro ocorra em 20ms
										// 65536-(1MHz/8/50Hz) = 63036 = 0xF63C

//------------------------------------------------------------------
ISR(TIMER1_OVF_vect)          // interrupção por Overflow no TIMER1
{
	TCNT1 = T_Timer1_OVF;	//Recarrega o Timer 1

/*----------------------------------------------------
	Tratamento da escrita nos displays de 7 segmentos
-----------------------------------------------------*/
	switch(Display)
	{
		case	0:					//Começa pela Unidade
		Set_Portbit(Sel_DSP);		//Seleciona o display da unidade
		LOAD_DATA_Display_7Seg (Tab_DSP7Seg_Num[Unid_Dsp7Seg]);
		Display++;
		break;

		case 1:
		Clr_Portbit(Sel_DSP);		//Seleciona o display da Dezena
		LOAD_DATA_Display_7Seg (Tab_DSP7Seg_Num[Dezena_Dsp7Seg]);
		Display = 0;				//Volta para o display da unidade
		break;
	}
/*----------------------------------
			Leitura das Chaves
----------------------------------*/
	switch (Chaves)
	{
		case	0:
		if((PINC & Ch_Off) != Ch_Off)		//Não tem chave acionada 	0b00000111
		{
			TChaves = 20/10;				// 20ms para bouncing
			Reg_SW_Bkp = (PINC & Ch_Off);	//Guarda a leitura somente das chaves
			Chaves++;						//Próximo
		}
		break;

		case 	1:
		if(--TChaves > 0)					//Esperando tempo do bouncing
		break;
		else
		{
			if((PINC & Ch_Off) == Reg_SW_Bkp)	//A segunda leitura é igual a primeira?
			Chaves++;							//Continua
			else
			{
				Chaves = 0;						//Prepara para iniciar nova leitura
				Reg_SW = Ch_Off;				//Devolve chaves abertas
			}
		}
		break;

		case	2:
		if((PINC & Ch_Off) == Ch_Off)		//Espera soltar a chave
		{
			Chaves = 0;						//Prepara para iniciar nova leitura
			Reg_SW = Reg_SW_Bkp;			//Pega o valor lido
		}
		break;
	}
}
/*===============================================================
Rotina: Config_Timer1_OVF
Função: Configura o Timer 1 para Overflow
Chamada: Config_Timer1_OVF
Comentário: Prescaler = 8; Tempo de Interrupção = T_Timer1_OVF
Exemplo: N/A
-----------------------------------------------------------------*/
void Config_Timer1_OVF()
 {
	TCCR1A = 0;                     //Configura timer para operação normal
	TCCR1B = 0x00;                    //Limpa registrador
	TCCR1B |= (1<< CS11);   		//Configura prescaler para 8: CS11 = 1

	TCNT1 = T_Timer1_OVF;


	//TCCR0A |= ((1 << WGM00) |(1 << WGM01) |(1 << WGM02));		//Normal Mode

	TIMSK1 |= (1 << TOIE1);           // habilita a interrupção do TIMER1

	//SREG |= (1 << 7);					//Habilita Global interrupção
 }
/*===============================================================
Rotina: Config_Timer1_Compare_Mode
Função: Configura o Timer 1 para Compare_Mode
Chamada: Config_Timer1_cm
Comentário: Prescaler = 8; Tempo de Interrupção = T_Timer1_OVF
Exemplo: N/A
-----------------------------------------------------------------*/
/*=========================================================================
 * Configura o Timer 1 para 10 milissegundos
 * prescler=8, Compare Mode
 *----------------------------------------------------------------------*/
void Configura_Timer_1_Modo_CTC()
{

	TCCR1A = 0;          	//Configura timer para operação normal
	TCCR1B = 0;          	//limpa registrador

	TCNT1 = 0x00;
	OCR1A = 0x4E2;      	// carrega os registradores de comparação (10ms 8MHz/8; Prescaler 8)
							//1MHz/8/100	(10ms)

	TCCR1B |= (1 << WGM12)|(1 << CS11);   // modo CTC, prescaler de 64: CS11 = 1
	TIMSK1 |= (1 << OCIE1A);  			// habilita interrupção por igualdade de comparação

}
