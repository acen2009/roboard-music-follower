#include <stdio.h>
#include <roboard.h>
#include <conio.h>
#include <windows.h>

int main (void)
{
	float TargetAngle = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition = 0x00;
	unsigned char Speed = 3; //0 ~ 4 = fast ~ slow
	unsigned char MotorID = 2;//0~30			
	unsigned char send[4] = {0xff, 0x00, InputPosition, 0x00};
	unsigned char read[2] = {0};
	
	float TargetAngle1 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition1 = 0x00;
	unsigned char MotorID1 = 3;//0~30			
	unsigned char send1[4] = {0xff, 0x00, InputPosition1, 0x00};
	unsigned char read1[2] = {0};
	
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
	
	InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
	send1[1] = ( (((Speed) & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
	send1[2] = InputPosition1;
	send1[3] = (send1[1] ^ send1[2]) & 0x7f;
	
	
    if (com3_ServoTRX(send, 4, read, 2) == false) printf("%s\n", roboio_GetErrMsg());
    Sleep(500);                           
	if (com3_ServoTRX(send1, 4, read1, 2) == false) printf("%s\n", roboio_GetErrMsg());
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, Servo1 read data = %d\n", i, read[i]);
	}
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, Servo2 read data = %d\n", i, read1[i]);
	}

//=======================Control the Motor========================================/

	printf("Please key in A or Z to control Servo1\n");
	printf("Please key in S or X to control Servo2\n");
	printf("or key in Esc to stop\n");

	while ((c = getch()) != 27)
	{
		printf("fuck you\n");
		
		switch (c)
		{
		case 'a':
		case 'A':
			TargetAngle += 5;
			if(TargetAngle > 257) TargetAngle = 257;
			InputPosition = (unsigned char)(TargetAngle / 1.30709);
			send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
			send[2] = InputPosition;
			send[3] = (send[1] ^ send[2]) & 0x7f;
			com3_ServoTRX(send, 4, read, 2);
			break;

		case 'z':
		case 'Z':
			TargetAngle -= 5;
			if(TargetAngle < 77) TargetAngle = 77;
			InputPosition = (unsigned char)(TargetAngle / 1.30709);
			send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
			send[2] = InputPosition;
			send[3] = (send[1] ^ send[2]) & 0x7f;
			com3_ServoTRX(send, 4, read, 2);
			break;
			
		case 's':
		case 'S':
			TargetAngle1 += 10;
			if(TargetAngle1 > 257) TargetAngle1 = 257;
			InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
			send1[1] = ( (((Speed) & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
			send1[2] = InputPosition1;
			send1[3] = (send1[1] ^ send1[2]) & 0x7f;
			com3_ServoTRX(send1, 4, read1, 2);
			break;	
		
		case 'x':
		case 'X':
			TargetAngle1 -= 10;
			if(TargetAngle1 < 77) TargetAngle1 = 77;
			InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
			send1[1] = ( (((Speed) & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
			send1[2] = InputPosition1;
			send1[3] = (send1[1] ^ send1[2]) & 0x7f;
			com3_ServoTRX(send1, 4, read1, 2);
			break;
		
		default:
			printf("Error!!! You Fail.\n");
			break;
		}
		
		for(i = 0 ; i < 2; i++)
		{
			printf("%d, Servo1 read data = %d\n\n", i, read[i]);
		}
		
		for(i = 0 ; i < 2; i++)
		{
		    printf("%d, Servo2 read data = %d\n", i, read1[i]);
		}

	}
	
	printf("\n See You Next Time^^\n");

//=======================Remove the torque of the motor===========================/

	send[1] = ( ((0x06 & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
	send[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	send[3] = (send[1] ^ send[2]) & 0x7f;
	com3_ServoTRX(send, 4, read, 2);
	
	send1[1] = ( ((0x06 & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
	send1[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	send1[3] = (send1[1] ^ send1[2]) & 0x7f;
	com3_ServoTRX(send1, 4, read1, 2);
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, Servo1 read data = %d\n\n", i, read[i]);
	}
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, Servo2 read data = %d\n", i, read1[i]);
	}

	com3_Close();
	return 0;

}
