#include <stdio.h>
#include <roboard.h>
#include <conio.h>

int main (void)
{
	float TargetAngle = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition = 0x00;
	unsigned char Speed = 3; //0 ~ 4 = fast ~ slow
	unsigned char MotorID = 2;//0~30
			
	unsigned char send[4] = {0xff, 0x00, InputPosition, 0x00};
	unsigned char read[2] = {0};
	int baud[7] = {COMBAUD_2400BPS, COMBAUD_4800BPS, COMBAUD_9600BPS, COMBAUD_19200BPS, COMBAUD_38400BPS, COMBAUD_57600BPS, COMBAUD_115200BPS};

	int i, j;
	unsigned char c;

//=================================Initialization=================================/

	roboio_SetRBVer(RB_100);

	if (com3_Init(COM_FDUPLEX) == false)
	{
		printf("error init com3");
		return 1;
	}
	
	com3_SetFormat(COM_BYTESIZE8, COM_STOPBIT1, COM_NOPARITY);
	com3_SetBaud(COMBAUD_19200BPS);

//=======================Move the motor to center=================================/

	InputPosition = (unsigned char)(TargetAngle / 1.30709);
	send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
	send[2] = InputPosition;
	send[3] = (send[1] ^ send[2]) & 0x7f;
	
    if (com3_ServoTRX(send, 4, read, 2) == false)
	{
        printf("%s\n", roboio_GetErrMsg());                           
    }
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, read data = %d\n", i, read[i]);
	}

//=======================Control the Motor========================================/

	printf("Please key in A or Z, key in '0' to stop\n");

	while ((c = getch()) != 27)
	{
		printf("fuck you\n");
		
		switch (c)
		{
		case 'a':
		case 'A':
			TargetAngle += 5;
			if(TargetAngle > 332) TargetAngle = 332;
			InputPosition = (unsigned char)(TargetAngle / 1.30709);
			send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
			send[2] = InputPosition;
			send[3] = (send[1] ^ send[2]) & 0x7f;
			com3_ServoTRX(send, 4, read, 2);
			break;

		case 'z':
		case 'Z':
			TargetAngle -= 5;
			if(TargetAngle < 0) TargetAngle = 0;
			InputPosition = (unsigned char)(TargetAngle / 1.30709);
			send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
			send[2] = InputPosition;
			send[3] = (send[1] ^ send[2]) & 0x7f;
			com3_ServoTRX(send, 4, read, 2);
			break;
		
		default:
			printf("Error!!! You Fail.\n");
			break;
		}
		
		for(i = 0 ; i < 2; i++)
		{
			printf("%d, read data = %d\n", i, read[i]);
		}

	}
	
	printf("\n See You Next Time^^\n");

//=======================Remove the torque of the motor===========================/

	send[1] = ( ((0x06 & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
	send[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	send[3] = (send[1] ^ send[2]) & 0x7f;
	com3_ServoTRX(send, 4, read, 2);
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, read data = %d\n", i, read[i]);
	}

	com3_Close();
	return 0;

}
