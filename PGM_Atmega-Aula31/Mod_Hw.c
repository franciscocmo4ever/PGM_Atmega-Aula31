/*============================================================================

		Módulo com Rotinas Auxiliares e de Controle do Hw

		Revisão: 2.0
		Alterado modo de manipulação dos pórticos. Agora, os pórticos estão
		relacionados aos pinos do processador e não mais a constantes arbitrárias
------------------------------------------------------------------------------*/
#define Mod_Hw

#include <stdint.h>
#include <avr/io.h>
#include "Mod_Def.h"

#define Cte_Delay	97

#define Flash_Size_Atmega328P	32768		//32K

/*============================================================================
Rotina: Teste_Sysbit
Função: Testa o staus de um bit do sistema em uma posição de memória previamente informada
Chamada: Teste_Sysbit (Memŕoria_Ram_Interna,bit)
Retorna: Retorna 0 ou 1
----------------------------------------------------------------------------*/
unsigned char Teste_Sysbit(unsigned char Endereco, unsigned char bit)
{
	unsigned char Bit_Status;

	Bit_Status = Endereco & (1<< bit);

	if(Bit_Status != 0)
	Bit_Status = 1;

	return (Bit_Status);
}
/*===========================================================================================
Rotina: Config_ADC()
Função: Configura o ADC para Single Conversion mode (ADCSRA = 0)
Descrição:
=============================================================================================*/
void Config_ADC()
{
	ADCSRA |= (1<<ADPS0);    //Prescaler /8 125K sample
	ADCSRA |= (1<<ADPS1);
	ADCSRA &= ~(1<<ADPS2);

	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);                //Avcc(+5v) as voltage reference

	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));    //ADC in free-running mode
	ADCSRA &= ~(1<<ADATE);              //Single Conversion mode

	ADCSRA |= (1<<ADEN);                //Power up the ADC
	//DIDR0 = 0xFF;						//Desabilita todas as entradas dos ADC's
	//ADCSRA |= (1<<ADSC);               //Start converting
}
/*===================================================================================
 * Rotina:  Le_ADC()
 * Função:  Le o canal ADC informado
 * Chamada: Le_ADC(<canal>)
 * Retorna: Nada
 * Comentário:
 * ----------------------------------------------------------------------*/
uint16_t Le_ADC(uint8_t CH_ADC)
{

	unsigned int Leitura_ADC;

	ADMUX	&=	0xf0;					//Limpa os bits de seleção dos ADC's
	ADMUX |= CH_ADC;					//Insere o canal ADC no buffer
	//DIDR0  &= ~(1 << CH_ADC);			//Habilita somente o canal ADC que está sendo lido.
	ADCSRA |= (1<<ADSC);                //Start converting (Faz ADSC = 1)
	while((ADCSRA & (1<<ADSC)) != 0){}		//Aguarda Conversão
	Leitura_ADC = ADCW;
    return (Leitura_ADC);						//devolve o valor adc lido
}

/*===================================================================================
Rotina: Calcula Check_Sum
Função: Calcula o checksum do Fw
Descrição: 	A rotina calcula o checksum do Fw até o último endereço da memória de programa.
			Para que o chechsum calculado seja o mesmo do gravador de memória é preciso
			configurar o gravador para preencher os bytes além do espaço ocupado pelo
			programa com '0's.
			É preciso estar estento ao tamnho da memória de programa. Normamlmente
			tamnhos informados menores do que o tamnho real não causarão problemas porque
			normalmente o tamanho ocupado pelo Fw é menor. Tamanhos maiores irão fazer com
			que o contador retorne ao início do programa e some novamente os bytes.
			Na chamada da rotina o tamanho da memória de programa deve ser informado.
			A rotina devolve o valor em um inteiro.

Exemplo:	Para calcular o checksum cuja memória de programa é de 8K faz-se:
			Calcula_Check_Sum (8192);
----------------------------------------------------------------------------------------*/

unsigned int Calcula_Check_Sum (unsigned int Tamanho_da_Flash)
{

	__flash unsigned char const *Pont_CheckSum = 0X00;

	unsigned int Contador;
	unsigned int Check_Sum = 0;

	for (Contador = 0; Contador < Tamanho_da_Flash; Contador++)
	{
		Check_Sum = Check_Sum + (*Pont_CheckSum);
		Pont_CheckSum++;
	}

	return (Check_Sum);

}
/*====================================================================================
 * Rotina:  LCD_Num_Hex
 * Função: Converte um Número Decimal até 65535 em Hexadecimal e coloca no display
 *		   na posição previamente informa da na chamada da rotina.
 *		   Caso não queira definir a posição, coloque 0 (zero) no campo PosCursor.
 *         A rotina irá colocar o número onde estiver o cursor.
 * -----------------------------------------------------------------------------------*/
void LCD_Num_Hex (unsigned char PosCursor,unsigned int Num)
{
	unsigned int inteiro;


	unsigned char NumH4;
	unsigned char NumH3;
	unsigned char NumH2;
	unsigned char NumH1;


	NumH1 = Num%16;				//5620/16 = 351: Resto = 4 <== NumH1
	inteiro = Num/16;			//351

	NumH2 = inteiro%16;			//351/16 = 21: Resto = F <== NumH2
	inteiro = inteiro/16;		//351/16 = 21

	NumH3 = inteiro%16;			//21/16 = 1: Resto = 5 <== NumH3
	inteiro = inteiro/16;		//21/16 = 1

	NumH4 = inteiro%16;			//1/16 = 0: Resto = 1  <==NumH4


	if(PosCursor != 0)					//Mantêm o cursor na sua posição se for 0
	Move_Cursor(PosCursor);

	EnvDataLCD(Tab_Num_Hex[NumH4]);
	EnvDataLCD(Tab_Num_Hex[NumH3]);
	EnvDataLCD(Tab_Num_Hex[NumH2]);
	EnvDataLCD(Tab_Num_Hex[NumH1]);

}
/*=======================================================================
Rotina: Escreve_Num_LCD
Função: Escreve um número decimal até 65535 no LCD na posição desejada
Chamada:Escreve_Num_LCD(<Posição do Cursor>,<Quantidade de algarismos>,<Número a ser escrito>
Comentário: A rotina previne-se se a quantidade de algarismos for maior que 5
			colocando a string '?????'.
Exemplo:	Para escrever o número 2754 com 5 algarismos a partir da posição 8 faça:
			Escreve_Num_LCD(8,5,2754). No display irá aparecer,02754

			O mesmo número com 4 algarismos será mostrado 2754.

			Com 3 algarismos: 754
-----------------------------------------------------*/
void Escreve_Num_LCD(uint8_t Pos_Cursor,uint8_t Qtde_Algarismos, uint32_t Num_LCD)
{
	uint8_t Dez_Milhar;
	uint8_t Unid_Milhar;
	uint8_t Centena;
	uint8_t Dezena;
	uint8_t Unidade;


    Dez_Milhar = Num_LCD/10000;			//26781/10000=2 => Peguei a dezena da milhar
    Unid_Milhar = (Num_LCD/1000)%10;	//26781/1000=26 => 26/10=2 => Resto = 6 => Peguei a unidade de milhar
    Centena = (Num_LCD/100)%10;			//26781/100=267 => 267/10=26 => Resto = 7 => Peguei a centena
    Dezena = (Num_LCD/10)%10;			//26781/10=2678 => 2678/10=267 => Resto = 8 => Peguei a dezena
	Unidade = Num_LCD%10;				//26781/10=2678 => Resto=1 => Peguei a unidade

	Move_Cursor(Pos_Cursor);

	switch (Qtde_Algarismos)
	{
		case	5:
		EnvDataLCD(Dez_Milhar + '0');

		case	4:
		EnvDataLCD(Unid_Milhar + '0');

		case	3:
		EnvDataLCD(Centena + '0');

		case	2:
		EnvDataLCD(Dezena + '0');

		case	1:
		EnvDataLCD(Unidade + '0');
		break;

		default:
		Escreve_String_LCD(Pos_Cursor,"?????");
	}

}
/*========================================================================================
Rotina: 	Buzzer_On
Função: 	Gera uma sequência de Beeps
Chamada:	Buzzer_On (<Quantidade de Beeps>,<Duração do Beep>,<Tempo entre Beeps>)
Exemplo:	Gerar 2 Beeps de 50 ms, com intervalo entre os Beeps de 100ms
				Buzzer_On (2,50,100)
-----------------------------------------------------------------------------------------*/
void Buzzer_On (unsigned char Qtd_Buzzers,unsigned int Timer_Buzzer,unsigned int Time_Betw_Buzzer)
{
	unsigned char x;

	if(Qtd_Buzzers == 1)
	{
		Clr_Portbit(Buzzer);
		Wait (Timer_Buzzer);
		Set_Portbit(Buzzer);
	}
	else
	{
		for (x = Qtd_Buzzers; x > 0; x--)
		{
			Clr_Portbit(Buzzer);
			Wait (Timer_Buzzer);
			Set_Portbit(Buzzer);
			Wait (Time_Betw_Buzzer);
		}
	}
}
/*===============================================================
Rotina:	Wait
Função: Gera um delay em milessegundos
Devolve: Nada
Chamada: Wait (<tempo em milessegundos>)
Comentário: A rotina pode gerar tempos de 1 a 65535 milessegundos
Exemplo: Wait(100), gera um delay de 100ms.
-----------------------------------------------------------------*/
void Wait (unsigned int delay)
{
	unsigned char constante_1ms = Cte_Delay;		//Esse valor é o responsável pelo tempo de 1ms

	while(delay > 0)
	{
		//--------//Gerando 1ms--------
		while(constante_1ms > 0)
		{
			constante_1ms--;		//Decrementa o registro de 1ms
		}
		delay--;

		constante_1ms = Cte_Delay;
	}
}
/*=========================================================================================
				MANIPULAÇÃO DE BITS DOS PÓRTICOS
==========================================================================================*/
/*
Rotina:Set_Portbit
Função:Coloca em nível lógico 1 o bit do Portx correspondente
	Os pórticos foram assim classificados
	PORTICO			s_bits
	--------------------------------------
	PORTB	|	de 0 a 7
	PORTC	|	de 8 a 15
	PORTD	|	de 16 a 31

Chamada: Set_Portbit (<bit>)
Comentário: Apenas para os pórticos
Exemplo:	Set_Portbit(Bit_19) faz bit 3 do PORTD em 1.
-----------------------------------------------------------------*/
void Set_Portbit(uint8_t pino)
{

	if((pino >= 2 && pino <= 6) || (pino >=11 && pino <=13))		//PORTD
	{

		if(pino >= 2 && pino <= 6)
		PORTD |= (1 << (pino - 2));		//Coloca 1 no pino
		else
		PORTD |= (1 << (pino - 6));		//Coloca 1 no pino

	}
	else if(pino >= 14 && pino <= 19)	//PORTB
	PORTB |= (1 << (pino-14));		//Coloca 1 no pino

	if((pino >= 23 && pino <= 28) || (pino == 1))		//PORTC
	{
		if(pino >= 23 && pino <= 28)
		PORTC |= (1 << (pino - 23));		//Coloca 1 no pino
		else
		PORTC |= (1 << (pino - 5));		//Coloca 1 no pino
	}

}
/*=============================================================
Rotina:Clr_Portbit
Função:Coloca em nível lógico 0 o bit do PORTx correspondente
Chamada: Clr_Portbit (<bit>)
Comentário: Os pórticos foram assim classificados
			PORTICO	|	s_bits
			--------------------------------------
			PORTB	|	de 0 a 7
			PORTC	|	de 8 a 15
			PORTD	|	de 16 a 31
Exemplo:
			Clr_Portbit(Bit_19) faz bit 3 do registro PORTD em 0.
-----------------------------------------------------------------*/
void Clr_Portbit(unsigned char pino)

{
	if((pino >= 2 && pino <= 6) || (pino >=11 && pino <=13))		//PORTD
	{
		if(pino >= 2 && pino <= 6)
		PORTD &= ~(1 << (pino - 2));		//Coloca 0 no pino
		else
		PORTD &= ~(1 << (pino - 6));		//Coloca 0 no pino
	}
	else if(pino >= 14 && pino <= 19)	//PORTB
	PORTB &= ~(1 << (pino-14));		//Coloca 0 no pino

	if((pino >= 23 && pino <= 28) || (pino == 1))		//PORTC
	{

		if(pino >= 23 && pino <= 28)
		PORTC &= ~(1 << (pino - 23));		//Coloca 0 no pino
		else
		PORTC &= ~(1 << (pino - 5));		//Coloca 0 no pino

	}


}
/*=============================================================
Rotina:Cpl_Portbit
Função:Inverte o estado lógico do bit requerido
Chamada: Cpl_Portbit (<bit>)
Comentário:  Os pórticos foram assim classificados
			PORTICO	|	s_bits
			--------------------------------------
			PORTB	|	de 0 a 	7
			PORTC	|	de 8 a 	15
			PORTD	|	de 16 a 31

Exemplo:
		Cpl_Bit (Bit_19) inverte o estado lógico do bit 3 do registro Bit_Ram_02.
-----------------------------------------------------------------*/
void Cpl_Portbit(unsigned char pino)
{

if((pino >= 2 && pino <= 6) || (pino >=11 && pino <=13))		//PORTD
	{
		if(pino >= 2 && pino <= 6)
		PORTD ^= (1 << (pino - 2));		//Coloca 0 no pino
		else
		PORTD ^= (1 << (pino - 6));		//Coloca 0 no pino
	}
	else if(pino >= 14 && pino <= 19)	//PORTB
	PORTB ^= (1 << (pino-14));		//Coloca 0 no pino

	if((pino >= 23 && pino <= 28) || (pino == 1))		//PORTC
	{

		if(pino >= 23 && pino <= 28)
		PORTC ^= (1 << (pino - 23));		//Coloca 0 no pino
		else
		PORTC ^= (1 << (pino - 5));		//Coloca 0 no pino

	}
}
/*=========================================================================================
				MANIPULAÇÃO DE BITS DA ÁREA DE RAM INTERNA
==========================================================================================*/
/*
Rotina:SetBit
Função:Coloca em nível lógico 1 o bit requerido
Chamada: Setbit (<bit>)
Comentário: Apenas para área de bits criada pelo programador

Exemplo:
		Setbit(Bit_19) faz bit 3 do registro Bit_Ram_02 em 1.
-----------------------------------------------------------------*/
void Setbit(unsigned char num_bit)
{
	if(num_bit <= 7)
	{
		Bit_Ram_00 |= (1 << num_bit);		//É o mesmo que: Bit_Ram_00 = Bit_Ram_00 | (1 << num_bit);

	}

	else if (num_bit >= 8 && num_bit <= 15)
	{
		num_bit = num_bit-8;				//Corrige para a posição real no registro
		Bit_Ram_01 |= (1 << num_bit);

	}

	else if (num_bit >= 16 && num_bit <= 23)
	{
		num_bit = num_bit-16;				//Corrige para a posição real no registro
		Bit_Ram_02 |= (1 << num_bit);
	}

}

/*=============================================================
Rotina:Clrbit
Função:Coloca em nível lógico 0 o bit requerido
Chamada: Clrbit (<bit>)
Comentário: Apenas para área de bits criada pelo programador

Exemplo:
		Clrbit(Bit_19) faz bit 3 do registro Bit_Ram_02 em 0.
-----------------------------------------------------------------*/
void Clrbit(unsigned char num_bit)
{
	if(num_bit <= 7)
	{
		Bit_Ram_00 &= ~(1 << num_bit);		//É o mesmo que: Bit_Ram_00 = Bit_Ram_00 & ~(0b00000001 << num_bit);
											//Exemplo: 1) (0b00000001 << 4) = 0b00010000
											//2) ~0b00010000 = 0b11101111


	}

	else if (num_bit >= 8 && num_bit <= 15)
	{
		num_bit = num_bit-8;				//Corrige para a posição real no registro
		Bit_Ram_01 &= ~(1 << num_bit);

	}

	else if (num_bit >= 16 && num_bit <= 23)
	{
		num_bit = num_bit-16;				//Corrige para a posição real no registro
		Bit_Ram_02 &= ~(1 << num_bit);
	}

}
/*===============================================================
Rotina:Teste_Bit
Função:Testa o estado de um bit informado na chamada da função.
Devolve: Zero se o bit for zero e um valor diferente de zero, se for 1
Chamada: TesteBit(<bit>)
Comentário: Somente para os bits criados pelo programador

Exemplo:
		Suponha Bit_Ram_01 = 0b01100110

		Teste_Bit(12) devolve 0b00000000
		Teste_Bit(13) devolve 0b00100000
-----------------------------------------------------------------*/
unsigned char TesteBit(unsigned char num_bit)
{
	unsigned char Status_Bit = 0x00;

	if(num_bit <= 7)
	{
		Status_Bit = Bit_Ram_00 & (1 << num_bit);		//É o mesmo que: Bit_Ram_00 = Bit_Ram_00 & (1 << num_bit);

	}
	else if (num_bit >= 8 && num_bit <= 15)
	{
		num_bit = num_bit-8;				//Corrige para a posição real no registro
		Status_Bit = Bit_Ram_01 & (1 << num_bit);

	}

	else if (num_bit >= 16 && num_bit <= 23)
	{
		num_bit = num_bit-16;				//Corrige para a posição real no registro
		Status_Bit = Bit_Ram_02 & (1 << num_bit);
	}
	if(Status_Bit != 0)
	Status_Bit = 1;

	return (Status_Bit);
}
/*=============================================================
Rotina:Cpl_Bit
Função:Inverte o estado lógico do bit requerido
Chamada: Cpl_Bit (<bit>)
Comentário: Apenas para área de bits criada pelo programador
			Carrega o PORTx quando o registro é usado como imagem de PORTx
Exemplo:
		Cpl_Bit (Bit_19) inverte o estado lógico do bit 3 do registro Bit_Ram_02.
-----------------------------------------------------------------*/
void Cplbit(unsigned char num_bit)
{
	if(num_bit <= 7)
	{
		Bit_Ram_00 ^= (1 << num_bit);		//É o mesmo que: Bit_Ram_00 = Bit_Ram_00 | (1 << num_bit);

	}

	else if (num_bit >= 8 && num_bit <= 15)
	{
		num_bit = num_bit-8;				//Corrige para a posição real no registro
		Bit_Ram_01 ^= (1 << num_bit);
	}

	else if (num_bit >= 16 && num_bit <= 23)
	{
		num_bit = num_bit-16;				//Corrige para a posição real no registro
		Bit_Ram_02 ^= (1 << num_bit);
	}
}
/*=============================================================
Rotina:Config_Portbit
Função:Configura um bit do pórtico para escrita ou leitura
Chamada: Config_Portbit (<bit>,<esrita/leitura>)
Comentário: 1 faz pórtico como escrita

Exemplo:Config Portbit (bit11, 1) deixa pórtico PC3 como esrita
-----------------------------------------------------------------*/
void Config_Portbit(uint8_t pino, uint8_t RW)
{
	if((pino >= 2 && pino <= 6) || (pino >=11 && pino <=13))		//PORTD
	{
		if(RW == 0)
		{
			if(pino >= 2 && pino <= 6)
			DDRD &= ~(1 << (pino - 2));		//Configura para leitura
			else
			DDRD &= ~(1 << (pino - 6));
		}
		else
		{
			if(pino >= 2 && pino <= 6)
			DDRD |= (1 << (pino - 2));		//Configura para escrita
			else
			DDRD |= (1 << (pino - 6));
		}
	}
	else if(pino >= 14 && pino <= 19)	//PORTB
	{
		if(RW == 0)
		DDRB &= ~(1 << (pino-14));		//Configura para leitura
		else
		DDRB |= (1 << (pino-14));		//Configura para escrita
	}
	else if((pino >= 23 && pino <= 28) || (pino == 1))		//PORTC
	{
		if(RW == 0)
		{
			if(pino >= 23 && pino <= 28)		//Configura para leitura
			DDRC &= ~(1 << (pino - 23));
			else
			DDRC &= ~(1 << (pino - 5));			//Configura para leitura
		}
		else
		{
			if(pino >= 23 && pino <= 28)
			DDRC |= (1 << (pino - 23));
			else
			DDRC |= (1 << (pino - 5));
		}
	}
}
/*====================================================
		ROTINAS DE CONTROLE DO LCD 16x2
====================================================
Rotina: Inicializa LCD
Função:	Gera comandos para inicializar o LCD
Chamada: Inicializa_LCD ()
Comentário: ---
Exemplo: ---
-----------------------------------------------------------------*/
void Inicilaiza_LCD ()
{

	EnvCmdLCD (M8B5X7);		//Dados de 8 bits; fonte em 5x7 pts (para displays de 2 linhas)
	Wait (10);

	EnvCmdLCD (M8B5X7);		//Dados de 8 bits; fonte em 5x7 pts (para displays de 2 linhas)
	Wait (10);

	EnvCmdLCD (INC_CSR);	//Incrementa posicao do cursor (dir.)
	Wait (10);

	//EnvCmdLCD(MODO_DCB);	//display on, cursor on e "blink" on (pisc.)
	EnvCmdLCD (MODO_D);		//Display on, cursor off, blink off
	Wait (10);

	EnvCmdLCD (CLR_DLY);	//Comando para limpar display
	Wait (10);

	EnvCmdLCD (CSR_HOME);	//Comando para posicionar cursor no inicio
	Wait (10);

}
/*====================================================
Rotina: Move_Cursor
Função:	Coloca o cursor na posição desejada
Chamada: Move_Cursor(<posição desejada>)
Comentário: Em um LCD 16x2, a 1a linha começa com 1 e termina com 16
			e a segunda linha começa com 17 e termina com 32
Exemplo: Move_Cursor(10) coloca o cursor na 10a coluna da 1a linha
		 Move_Cursor(20) coloca o cursor na 4a coluna da 2a linha
------------------------------------------------------*/
void Move_Cursor (unsigned char Pos_Cursor)
{

 if (Pos_Cursor <= 16)
    {
		Pos_Cursor--;   		                //Corrige. A 1a posição é zero
		EnvCmdLCD (Pos_Cursor + Linha1);	   //0x80 é o endereço da 1ª posição, 1ª linha
		Wait (2);
	}
	else
	{
		EnvCmdLCD (Pos_Cursor - 17 + Linha2);
		Wait (2);
	}

}
//==============================================================================================
/* Rotina: Escreve_String_LCD
 * Função: Coloca no display a string da chamada da rotina.
 * A rotina foi feita originalmente para displays com 16 colunas então, o número máximo de caracteres
 * em cada linha deve ser de 16.
 * A primeira coluna da Linha 1 é Posição 1
 * Na chama da rotina, quando a quantidade de caracteres for menor que 16, não é preciso completar com
 * espaços para preencher a linha. A rotina ao identificar o \0 que é colocado automaticamente pelo
 * compilador depois da última aspas dupla completa com espaços o restante da linha. *
 * Quando a quantidade de caracteres for menor que 16 e se desejar preservar o restante do conteúdo da
 * linha, coloque um \03 (ETX) depois do último caracter.
 * Exemplos:
 * 1 - Suponha que no final da linha 1 do diplay tenha uma string  [          Valor =   ] e que se queira
 * escrever "Compare" preservando o final da linha. Nesse caso a chamada será:
 * Escreve_String_Display (01,"Compare\03"). No display irá aparecer [Compare Valor =   ]
 *
 * 2 - Suponha na mesma situação que se deseja escrever na linha 1 "Compare" e apagar o restante
 * da linha. A chamada será Escreve_String_Display (01,"Compare")
 * No display irá aparecer [Compare       ]
 *
 * 3 -  Pode haver casos que queira iniciar depois da posição 1 da coluna. Nesse caso, as regras
 * acima são as mesmas. Caso se estrapole o limite de colunas, a rotina irá truncar a string.
 -------------------------------------------------------------------------------------------------*/
void Escreve_String_LCD (unsigned char Pos_LCD, char *Strg_LCD)
{
	unsigned char x,y;

	Move_Cursor (Pos_LCD);
	if (Pos_LCD > 16)
	y = 33-Pos_LCD;
	else
	y = 17-Pos_LCD;

	for (x = 0; x < y; x++)
	{
		if(*Strg_LCD == 0 || *Strg_LCD == 03 ) break;

		else
		{
			EnvDataLCD (*Strg_LCD );
			Strg_LCD++;
		}

	}
	if(*Strg_LCD == 0 && x < y)	//Completa com espaços o restante da linha...
							//...se NÃO encontrou caracter finalizador ETX (03)
	{
		for(x = x; x < y; x++)
		EnvDataLCD (' ');
	}
}
/*====================================================
Rotina: void Mens_LCD (unsigned char Pos_Cursor, unsigned char *pont_mens)
Função: Escreve uma mensagem no LCD de até 16 caracteres a partir
		da posição do cursor informada na chamada da rotina
Chamada: Mens_LCD (<posição do cursor>,<ponteiro da mesagem>)
Comentário: A rotina não verifica se haverá estouro do espaço reservado
			para a mensagem. A mensagem deve estar previamente declarada
Exemplo: Supondo uma mensagem assim declarada previamente:
		_flash const uint8_t Mens_01[]="Querer Aprender;

		void Mens_LCD (unsigned char Pos_Cursor, unsigned char *pont_mens) (1,Mens_01) irá escrever Querer Aprender
		na 1a linha do LCD iniciando da posição 1.
-----------------------------------------------------*/
void Mens_LCD (uint8_t Pos_Cursor, __flash const uint8_t *pont_mens)
{
	unsigned char i;

	Move_Cursor(Pos_Cursor);

	while ((i = *pont_mens)!= 0)
	{
		EnvDataLCD (i);
		pont_mens++;
		Wait (2);
	}

}
/*===============================================================================================
Rotina: EnvDataLCD
Função: Escreve um caracter no display. O caracter deve vir no valor correspondente ascii
-------------------------------------------------------------------------------------------- */
void EnvDataLCD (unsigned char data_display)
{
	Set_Portbit(Rs_Display);		//Configura para escrever dados no LCD
	PORTB = data_display;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	Set_Portbit(E_Display);			//Gerando sinal de Load no display
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	Clr_Portbit(E_Display);
	Wait (2);
}
/*====================================================
Rotina: EnvCmdLCD
Função: Gera sinais para encrever uma instrução no LCD
Chamada: EnvCmdLCD(<comando>)
Comentário:---
Exemplo: ---
-----------------------------------------------------*/
void EnvCmdLCD (uint8_t Inst_LCD)
{
	Clr_Portbit(Rs_Display);			//Configura para escrever instrução no LCD
	PORTB = Inst_LCD;
	Set_Portbit(E_Display);			//Gerando sinal de Load no display
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	Clr_Portbit(E_Display);
}

/*====================================================
Rotina:
Função:
Chamada:
Comentário:
Exemplo:
-----------------------------------------------------*/
void EnvCaracterEspecialLCD(unsigned char PosCursor,__flash const unsigned char *Caracter)
{

	unsigned char Ponteiro;

	EnvCmdLCD(0x40);

	for(Ponteiro = 0; Ponteiro < 8; Ponteiro++)
	{
		EnvDataLCD(*Caracter);
		Caracter++;
	}

	Move_Cursor(PosCursor);
	EnvDataLCD(0x00);
}

