
/*
Amid Ale
Trabajo Practico Final PCSE
 */

/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>
#include "ff.h"       // <= Biblioteca FAT FS


/*==================[definiciones y macros]==================================*/

#define UART_PC        UART_USB
#define UART_BLUETOOTH UART_232
#define FILENAME "pase.txt"
/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

MPU9250_address_t addr = MPU9250_ADDRESS_0; // If MPU9250 AD0 pin is connected to GND

static FATFS fs;           // <-- FatFs work area needed for each volume
static FIL fp;             // <-- File object needed for each open file

char buff[16];
char blt[18];
uint8_t cid[32];
uint8_t data = 0;
bool_t logok;

BYTE resp;
BYTE stt;
const  char CODESD  [] = {0x03,0x53,0x44,0x53,0x55,0x30,0x32,0x47,0x80,0x72,0x11,0x02,0xf6,0x00,0x01,0x37};
const  char CODEMAC [] = "50:01:D9:41:B0:9B";



/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
bool_t hm10bleTest ( int32_t uart );
void hm10blePrintATCommands ( int32_t uart );
void doProgram (void);
void diskTickHook( void *ptr );
void allConfig(void);
bool_t login(void);
void logout(void);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();
	allConfig();

	gpioToggle(LEDR);
	while(!logok)
	{
		logok = login();
	}

	while (TRUE)
	{
		doProgram();
	}
	return 0;
}
/*==================[definiciones de funciones internas]=====================*/


/*==================[definiciones de funciones externas]=====================*/


bool_t hm10bleTest ( int32_t uart )
{
	uartWriteString( uart, "AT\r\n" );
	return waitForReceiveStringOrTimeoutBlocking( uart,
			"OK\r\n", strlen("OK\r\n"),
			50 );
}

void hm10blePrintATCommands ( int32_t uart )
{
	delay(500);
	uartWriteString( uart, "AT+HELP\r\n" );
}

void doProgram (void)
{
	bool_t valor;
	//Programa a ejecutar si el usuario está autorizado
	valor = !gpioRead( TEC1 );
	gpioWrite( LED1, valor );

	valor = !gpioRead( TEC2 );
	gpioWrite( LED2, valor );

	valor = !gpioRead( TEC3 );
	gpioWrite( LED3, valor );

}

// FUNCION que se ejecuta cada vezque ocurre un Tick
void diskTickHook( void *ptr ){
	disk_timerproc();   // Disk timer process
}

void allConfig(void)
{
	// Inicializar UART_USB para conectar a la PC
	debugPrintConfigUart( UART_PC, 9600 );
	debugPrintlnString( "UART_PC configurada." );

	// Inicializar UART_232 para conectar al modulo bluetooth
	consolePrintConfigUart( UART_BLUETOOTH, 9600 );
	debugPrintlnString( "UART_BLUETOOTH para modulo Bluetooth configurada." );


	uartWriteString( UART_PC, "Testeto si el modulo esta conectado enviando: AT\r\n" );
	if( hm10bleTest( UART_BLUETOOTH ) ){
		debugPrintlnString( "Modulo conectado correctamente." );
	}

	// SPI configuration
	spiConfig( SPI0 );


	// Inicializar el conteo de Ticks con resolucion de 10ms,
	// con tickHook diskTickHook
	tickConfig( 10 );
	tickCallbackSet( diskTickHook, NULL );

}

bool_t login(void)
{
	uint8_t i;
	disk_initialize(0);

	// Give a work area to the default drive
	if( f_mount( &fs, "", 0 ) != FR_OK ){
		// If this fails, it means that the function could
		// not register a file system object.
		// Check whether the SD card is correctly connected
	}
	resp = disk_ioctl(0,12,cid);
	if(resp == 0)
	{
		if( strcmp(cid , CODESD ) == 0 )
		{
			gpioToggle(LEDR);
			gpioToggle(LEDG);
			return TRUE;

		}
	} else
	{
		i = 0;
		while(i < 18)
		{
			if (uartReadByte( UART_BLUETOOTH, &data ))
			{
				blt[i]= data;
				i++;
			}
		}

		if( strcmp( blt , CODEMAC ) == 0 )
		{
			uartWriteString( UART_BLUETOOTH, "Identificado\r\n" );
			delay(1000);
			gpioToggle(LEDR);
			gpioToggle(LEDG);
			return TRUE;

		} else
		{
			uartWriteString( UART_BLUETOOTH, "No Identificado\r\n" );
			delay(1000);
			return false;
		}
	}





	return false;
}

/*==================[fin del archivo]========================================*/

