/*===============================================================
                    Módulo de definições
----------------------------------------------------------------*/
#define	Port_Read	0
#define Port_Write	1
/*============================================================================

		Módulo com definições Numéricas do Sistema

------------------------------------------------------------------------------*/
//Bits do Bit_Ram_00

unsigned char Bit_Ram_00;
#define 	bHabLed			0
#define 	bHab_UsartRx	1
//#define 	bit_WR			2
#define 	Bit_3			3
#define 	Bit_4			4
#define 	Bit_5			5
#define 	Bit_6			6
#define 	Bit_7			7
//----------------------------
unsigned char Bit_Ram_01;

//Bits do Bit_Ram_01
#define 	Bit_8	8
#define 	Bit_9	9
#define 	Bit_10	10
#define 	Bit_11	11
#define 	Bit_12	12
#define 	Bit_13	13
#define 	Bit_14	14
#define 	Bit_15	15
//----------------------------
unsigned char Bit_Ram_02;

//Bits do Bit_Ram_02
#define 	Bit_16	16
#define 	Bit_17	17
#define 	Bit_18	18
#define 	Bit_19	19
#define 	Bit_20	20
#define 	Bit_21	21
#define 	Bit_22	22
#define 	Bit_23	23
//--------------------------------------------
//Constantes do hardware

/*------------------------------------------
	BITS DOS PÓRTICOS DO PROCESSADOR
-------------------------------------------*/
#define		SW1		0b00000110
#define		SW2		0b00000101
#define		SW3		0b00000011
#define		Ch_Off	0b00000111

//----------------------------------
//								Pinos do
//								ATmega328
#define		Sel_DSP					4
#define		Sel_7Seg_Data			5
#define		Sel_Led					6
#define		Rs_Display				11
#define		E_Display				12
#define		Buzzer					13
//#define							14
//#define							15
//#define 							16
//#define							17
//#define          					18
//#define							19
//#define      						23		//Chave de seleção para acertar hora
//#define							24		//Chave de seleção para acertar minuto
//#define							25
#define		SCL						28
#define		SDA						27
//#define							28
//=======================================
//MACROS do Sistema
#define  LOAD_DATA_LED(data)			\
PORTB = data;							\
/*Gerando um click no latch dos leds*/	\
PORTD &= (1 << 4);						\
asm("nop");								\
PORTD |= (1 << 4)
//--------------------------------------
#define  LOAD_DATA_Display_7Seg(data)	\
PORTB = data;							\
/*Gerando um click no latch dos leds*/	\
PORTD &= ~(1 << 3);						\
asm("nop");								\
PORTD |= (1 << 3)
//---------------------------------------------


/*---------------------------------------------------------
	Definições das constantes dos Displays de 7 segmentos
---------------------------------------------------------*/
//Display de 7 segmentos		   pgfedcba
#define		num_0		0x3F   //0b00111111
#define		num_1		0x06   //0b00000110
#define		num_2		0x5B   //0b01011011
#define		num_3		0x4F
#define		num_4		0x66
#define		num_5		0x6D
#define		num_6		0x7D
#define		num_7		0x07
#define		num_8		0x7F
#define		num_9		0x6F
#define		DotPoint	0x80

#define		Let_A		0x77
#define		Let_B		0x7C
#define		Let_C		0x39
#define		Let_D		0x5E
#define		Let_E		0x79
#define     Let_F		0x71
#define		Let_G		0x7D
#define		Let_H		0x76
#define		Let_I		0x06
#define		Let_J       0x1E
#define		Let_L		0x38
#define		Let_N		0x54
#define		Let_O		0x3F
#define		Let_P		0x73
#define		Let_R		0x50
#define		Let_S		0x6D
#define		Let_T		0x78
#define		Let_U		0x3E
#define		Let_Z		0x5B
//----------------------------------------------------------------------
//Constantes do controle do display Display_LCD

#define		CLR_DLY 	0x01	//Comando para limpar display
#define		CSR_HOME   	0x80	//Comando para posicionar cursor no inicio
#define		MODO_DCB   	0x0F    //Display no modo: display on, cursor ;on e "blink" on (pisc.)
#define		MODO_DC    	0x0E    // Display on, cursor on, blink off
#define		MODO_CB    	0x0B    //Display off, cursor on, blink on
#define		MODO_C     	0x0A    // Display off, cursor on, blink off
#define		MODO_D     	0x0C    //Display on, cursor off, blink off
#define		INC_CSR    	0x06    //Incrementa posicao do cursor (dir.)
#define		DEC_CSR    	0x04    //Decrementa posicao do cursor (esq.)
#define		WRD_WR     	0x05    //Desloca palavra para direita mantem posicao do cursor
#define		WRD_LF     	0x07    //Desloca palavra para esquerda; mantem posicao do cursor
#define		CSR_WR     	0x14    //Desloca cursor para direita
#define		CSR_LF     	0x10    //Desloca cursor para esquerda
#define		WR_WR      	0x18    //Desloca display interiro p/ direita
#define		WR_LF      	0x1C    // Desloca display interiro p/ esquerda
#define		M8B5X1     	0x34    //Dados de 8 bits; fonte em 5x10 pts(para displays de 2 linhas)
#define		M8B5X7     	0x38    //Dados de 8 bits; fonte em 5x7 pts (para displays de 2 linhas)
#define		M4B5X10    	0x24	//Dados de 4 bits; fonte em 5x10 pts (para displays de 2 linhas)
#define		M4B5X7     	0x20    // Dados de 4 bits; fonte em 5x7 pts
#define 	Linha1		0x80	//Endereco da linha 1, coluna -1
#define		Linha2      0xC0    //Endereco da linha 2, coluna -1
//=======================================

#ifdef Mod_Main
#define subs /**/
#else
#define subs extern
#endif // Mod_Main

/*========================================
			Rotinas do Sistema
-----------------------------------------*/
subs void Set_Portbit(uint8_t pino);
subs void Clr_Portbit(unsigned char num_bit);
subs void Cpl_Portbit(unsigned char pino);
subs void Config_Portbit(uint8_t pino, uint8_t RW);
subs void Setbit(unsigned char num_bit);
subs void Clrbit(unsigned char num_bit);
subs unsigned char TesteBit(unsigned char num_bit);
subs void Cplbit(unsigned char num_bit);
subs void Wait (unsigned int delay);
subs void Buzzer_On (unsigned char Qtd_Buzzers,unsigned int Timer_Buzzer,unsigned int Time_Betw_Buzzer);
subs void Load_Data_Led(uint8_t data);
subs unsigned int Calcula_Check_Sum (unsigned int Tamanho_da_Flash);
subs uint16_t Le_ADC(uint8_t CH_ADC);
subs unsigned char Teste_Sysbit(unsigned char Endereco, unsigned char bit);
subs void Config_ADC();
subs void Ini_System();

/*---------------------------------------------
				Display LCD
-----------------------------------------------*/
subs void Inicilaiza_LCD ();
subs void Move_Cursor (unsigned char Pos_Cursor);
subs void Mens_LCD (uint8_t Pos_Cursor, __flash const uint8_t *pont_mens);
subs void EnvDataLCD (unsigned char data_display);
subs void EnvCmdLCD (uint8_t Inst_LCD);
subs void Escreve_String_LCD (unsigned char Pos_LCD, char *Strg_LCD);
subs void Escreve_Num_LCD(uint8_t Pos_Cursor,uint8_t Qtde_Algarismos, uint32_t Num_LCD);
subs void LCD_Num_Hex (unsigned char PosCursor,unsigned int Num);
subs void EnvCaracterEspecialLCD(unsigned char PosCursor,__flash const unsigned char* Caracter);
/*---------------------------
            Usart
------------------------------*/
subs void Config_Usart ();
subs void Envia_String_Usart (char *pont_mens);
subs void Wr_Data_Usart(uint8_t Dado);
subs void Rd_Byte_Usart();
subs void Config_Timer1_OVF();
/*=======================================================
                    Variáveis
------------------------------------------------------*/
subs uint8_t		Pont_Led;
subs uint8_t		Chaves;
subs uint8_t		TChaves;
subs uint8_t		Reg_SW_Bkp;
subs uint8_t		Reg_SW;
subs uint8_t		Cad_Seq_Led;
subs uint8_t		Unid_Dsp7Seg;
subs uint8_t		Dezena_Dsp7Seg;
subs uint8_t		Display;		//Seleciona o display a ser escrito
subs uint8_t		Data_Usart;
 //-----------------------------------------------------
#ifdef Mod_Main
#define subs /**/

/*=======================================================
			Tabelas do Sistema
------------------------------------------------------*/
subs __flash const uint8_t Tab_DSP7Seg_Num[]=
{
	num_0,num_1,num_2,num_3,num_4,num_5,num_6,
	num_7,num_8,num_9,DotPoint
};
//-----------------------------------------------
subs __flash const uint8_t Tab_DSP7Seg_Letra[]=
{
	Let_A,Let_B,Let_C,Let_D,Let_E,Let_F,Let_H,Let_J,
	Let_L,Let_N,Let_P,Let_R,Let_T,Let_U,DotPoint
};

__flash const unsigned char Tab_Num_Hex []  ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//-----------------------------------------------------
#else
extern __flash const uint8_t Tab_DSP7Seg_Num[];
extern __flash const uint8_t Tab_DSP7Seg_Letra[];
extern __flash const unsigned char Tab_Num_Hex [];
//-----------------------------------------------------
#endif // Mod_Main
