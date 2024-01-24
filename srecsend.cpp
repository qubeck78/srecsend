#define _CRT_SECURE_NO_DEPRECATE


// srecsend.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include < stdio.h >
#include < stdlib.h >
#include "CSerial.h"


CSerial com;


int main(int argc, char* argv[])
{
	int i;
	int newLineCount;

	int rv;
	wchar_t  comPortWs[256];
	char buf[512];
	FILE* in;
	uint8_t rb;
	int retransmission;
	int fatalError;
	int baudRate;

	printf("scecsend B20231117 -#qUBECk#-\n");

	if (argc < 4)
	{
		printf("usage: srecsend file.rec comPort baudrate\n\nExample: srecsend main.rec com1 500000\n\n");
		return 20;
	}

	swprintf( comPortWs, 256, L"%hs", argv[2] );
	baudRate = atoi( argv[3] );

	if (baudRate == 0)
	{
		baudRate = 500000;
	}

	rv = com.open(comPortWs, baudRate);
	if (rv)
	{
		printf("ERROR: can't open com port\n");
		return 21;
	}

	in = fopen(argv[1], "r");
	if (!in)
	{
		printf("ERROR: can't open input file\n");
		com.close();
		return 22;

	}

	fatalError		= 0;
	newLineCount	= 0;

	while (!feof(in))
	{
		fgets(buf, sizeof(buf) - 1, in);

		retransmission = 0;

		if (!feof(in))
		{
			

			do
			{
				//send line via uart
				com.write( (uint8_t*)buf, strlen( buf ) );

				if( ! ( ( strlen(buf) >= 2 ) && ( ( buf[1] == '9' ) || ( buf[1] == '8' ) || ( buf[1] == '7' ) ) ) )
				{
					if (!com.readByte(&rb))
					{
						printf( "%c", rb );
						//fflush( stdout );

						newLineCount++;

						if( ( newLineCount % 80 == 0 ) )
						{
							printf( "\n" );
							fflush( stdout );

						}

						if (rb == 'r')
						{
							retransmission = 1;
						}
						else if (rb == '!')
						{
							fatalError = 1;
							retransmission = 0;
						}
					}
					else
					{

						printf("\nNo response\n");
						fatalError = 1;

						break;
					}
				}
			} while (retransmission);
		}
		if (fatalError)
		{
			printf("\nFatal error reported\n");
			fclose(in);
			com.close();
			return 23;

		}
	}

	fclose(in);

	com.close();
	return 0;
}

