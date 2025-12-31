// ======================
// I2C emulado (bit-bang)
// SDA = PC4
// SCL = PC5
// ======================

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <stdint.h>

// Se LeitDS1307 está definido em outro .c (ex: main.c), aqui tem que ser extern:
extern unsigned char LeitDS1307;

// ======================
// I2C emulado (bit-bang)
// SDA = PC4
// SCL = PC5
// ======================
#define SDA_BIT   4
#define SCL_BIT   5
#define SDA_MASK  (1 << SDA_BIT)
#define SCL_MASK  (1 << SCL_BIT)

// Delay bem simples (aprox). Se precisar, ajusta o valor  (mais = mais lento)
static inline void I2C_Delay(void)
{
	for (volatile uint16_t i = 0; i < 120; i++)
	asm volatile("nop");
}

// Open-drain no SDA via DDR:
// - Para mandar 0: saída em 0
// - Para mandar 1: solta (entrada + pull-up)
static inline void I2C_SCL_H(void) { PORTC |=  SCL_MASK; I2C_Delay(); }
static inline void I2C_SCL_L(void) { PORTC &= ~SCL_MASK; I2C_Delay(); }

static inline void I2C_SDA_RELEASE(void)
{
	DDRC  &= ~SDA_MASK;   // entrada (solta)
	PORTC |=  SDA_MASK;   // pull-up
	I2C_Delay();
}

static inline void I2C_SDA_LOW(void)
{
	DDRC  |=  SDA_MASK;   // saída
	PORTC &= ~SDA_MASK;   // força 0
	I2C_Delay();
}

static inline uint8_t I2C_SDA_READ(void)
{
	return (PINC & SDA_MASK) ? 1 : 0;
}

static void I2C_Start(void)
{
	I2C_SDA_RELEASE();
	I2C_SCL_H();
	I2C_SDA_LOW();   // SDA cai com SCL alto = START
	I2C_SCL_L();
}

static void I2C_Stop(void)
{
	I2C_SDA_LOW();
	I2C_SCL_H();
	I2C_SDA_RELEASE(); // SDA sobe com SCL alto = STOP
}

static uint8_t I2C_WriteByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & 0x80) I2C_SDA_RELEASE(); // manda 1 soltando
		else             I2C_SDA_LOW();     // manda 0 forçando

		I2C_SCL_H();
		I2C_SCL_L();
		data <<= 1;
	}

	// ACK do escravo (ACK=0)
	I2C_SDA_RELEASE();
	I2C_SCL_H();
	uint8_t ack = I2C_SDA_READ();
	I2C_SCL_L();

	return (ack == 0);
}

static uint8_t I2C_ReadByte(uint8_t send_ack)
{
	uint8_t data = 0;

	I2C_SDA_RELEASE(); // escravo dirige SDA

	for (uint8_t i = 0; i < 8; i++)
	{
		data <<= 1;
		I2C_SCL_H();
		if (I2C_SDA_READ()) data |= 1;
		I2C_SCL_L();
	}

	// ACK (0) para continuar ou NACK (1) para encerrar
	if (send_ack) I2C_SDA_LOW();      // ACK=0
	else          I2C_SDA_RELEASE();  // NACK=1

	I2C_SCL_H();
	I2C_SCL_L();
	I2C_SDA_RELEASE();

	return data;
}

// ==============================
// FUNÇÕES DO SEU PROJETO (EXPORT)
// ==============================
void I2C_Control_Write(unsigned char Addres_Device,
unsigned char Addres_Data,
unsigned char Data_Write)
{
	// Idle: SCL alto; SDA solto
	DDRC  |= SCL_MASK;
	PORTC |= SCL_MASK;
	I2C_SDA_RELEASE();

	I2C_Start();
	I2C_WriteByte((Addres_Device << 1) | 0); // write
	I2C_WriteByte(Addres_Data);
	I2C_WriteByte(Data_Write);
	I2C_Stop();
}

void I2C_Control_Read(unsigned char Addres_Device,
unsigned char Address_Data)
{
	// Idle: SCL alto; SDA solto
	DDRC  |= SCL_MASK;
	PORTC |= SCL_MASK;
	I2C_SDA_RELEASE();

	// 1) aponta registrador
	I2C_Start();
	I2C_WriteByte((Addres_Device << 1) | 0); // write
	I2C_WriteByte(Address_Data);

	// 2) repeated start + leitura 1 byte
	I2C_Start();
	I2C_WriteByte((Addres_Device << 1) | 1); // read
	LeitDS1307 = I2C_ReadByte(0);            // NACK encerra
	I2C_Stop();
}
