
/* Copyright 2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>


/*==================[definiciones y macros]==================================*/

#define UART_PC        UART_USB
#define UART_BLUETOOTH UART_232
// MPU9250 Address


/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

MPU9250_address_t addr = MPU9250_ADDRESS_0; // If MPU9250 AD0 pin is connected to GND

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

bool_t hm10bleTest( int32_t uart );
void hm10blePrintATCommands( int32_t uart );
void writeToBluetooth(void);


/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();

	debugPrintConfigUart( UART_PC, 9600 );

	// Inicializar la IMU
	debugPrintlnString("Inicializando IMU MPU9250...\r\n" );
	int8_t status;
	status = mpu9250Init( addr );


	if( status < 0 ){
		debugPrintlnString( "IMU MPU9250 no inicializado, chequee las conexiones:\r\n\r\n" );
		debugPrintlnString( "MPU9250 ---- EDU-CIAA-NXP\r\n\r\n" );
		debugPrintlnString( "    VCC ---- 3.3V\r\n" );
		debugPrintlnString( "    GND ---- GND\r\n" );
		debugPrintlnString( "    SCL ---- SCL\r\n" );
		debugPrintlnString( "    SDA ---- SDA\r\n" );
		debugPrintlnString( "    AD0 ---- GND\r\n\r\n" );
		debugPrintlnString( "Se detiene el programa.\r\n" );
		while(1);
	}
	debugPrintlnString("IMU MPU9250 inicializado correctamente.\r\n\r\n" );



	// Inicializar UART_USB para conectar a la PC
	debugPrintlnString( "UART_PC configurada." );

	// Inicializar UART_232 para conectar al modulo bluetooth
	consolePrintConfigUart( UART_BLUETOOTH, 9600 );
	debugPrintlnString( "UART_BLUETOOTH para modulo Bluetooth configurada." );

	uint8_t data = 0;

	uartWriteString( UART_PC, "Testeto si el modulo esta conectado enviando: AT\r\n" );
	if( hm10bleTest( UART_BLUETOOTH ) ){
		debugPrintlnString( "Modulo conectado correctamente." );
	}

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE ) {
		//Leer el sensor y guardar en estructura de control
		mpu9250Read();
		writeToBluetooth();

		delay(1000);


	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

bool_t hm10bleTest( int32_t uart )
{
	uartWriteString( uart, "AT\r\n" );
	return waitForReceiveStringOrTimeoutBlocking( uart,
			"OK\r\n", strlen("OK\r\n"),
			50 );
}

void hm10blePrintATCommands( int32_t uart )
{
	delay(500);
	uartWriteString( uart, "AT+HELP\r\n" );
}

void writeToBluetooth(void)
{
	   // Imprimir resultados
	      printf( "Gg      (%f, %f, %f)\r\n",
	              mpu9250GetGyroX_rads(),
	              mpu9250GetGyroY_rads(),
	              mpu9250GetGyroZ_rads()
	            );
			printf( "Aa   (%f, %f, %f)\r\n",
	              mpu9250GetAccelX_mss(),
	              mpu9250GetAccelY_mss(),
	              mpu9250GetAccelZ_mss()
	            );
			printf( "Mm   (%f, %f, %f)\r\n",
	              mpu9250GetMagX_uT(),
	              mpu9250GetMagY_uT(),
	              mpu9250GetMagZ_uT()
	            );
						printf( "Tt    %f\r\n\r\n",
	              mpu9250GetTemperature_C()
	            );


}

/*==================[fin del archivo]========================================*/

