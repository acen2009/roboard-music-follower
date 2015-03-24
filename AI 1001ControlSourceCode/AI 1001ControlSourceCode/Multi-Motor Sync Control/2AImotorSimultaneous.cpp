#include <stdio.h>
#include <roboard.h>
#include <conio.h>

int main (void)
{
	float TargetAngle = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition = 0x00;
	unsigned char Speed = 3; //0 ~ 4 = fast ~ slow
	unsigned char MotorID = 2;//0~30
	
    float TargetAngle1 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition1 = 0x00;
	unsigned char MotorID1 = 3;//0~30
    
    const int ServoNum = 4;		
	unsigned char send[ServoNum + 4] = {0xff, 0x00, MotorID1 + 1, 0x00, 0x00, InputPosition, InputPosition1, 0x00};
	unsigned char PowerOffSend[4] = {0xff, 0xdf, 0x2f, (((0xdf) ^ (0x2f)) & 0x7f)};
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
	InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
    send[1] = ( (((Speed) & 0x07) << 5) + (0x1f & 0x1f) ) & 0xff;
	send[5] = InputPosition;
    send[6] = InputPosition1;
	send[7] = (send[3] ^ send[4] ^ send[5] ^ send[6]) & 0x7f;
	
    if (com3_Send(send, 8) == false)  printf("%s\n", roboio_GetErrMsg());                           
	
	//for(i = 0 ; i < 2; i++)
	//{
	//	printf("%d, read data = %d\n", i, read[i]);
	//}

//=======================Control the Motor========================================/

	printf("Please key in L or R, key in 'Esc' to stop\n");

	while ((c = getch()) != 27)
	{
		printf("fuck you\n");
		
		switch (c)
		{
		case 'r':
		case 'R':
			TargetAngle += 5;
			TargetAngle1 += 5;
			if(TargetAngle > 257) TargetAngle = 257;
			if(TargetAngle1 > 257) TargetAngle1 = 257;
			InputPosition = (unsigned char)(TargetAngle / 1.30709);
			InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
	        send[5] = InputPosition;
            send[6] = InputPosition1;
            send[7] = (send[3] ^ send[4] ^ send[5] ^ send[6]) & 0x7f;
			com3_Send(send, 8);
			break;

		case 'l':
		case 'L':
			TargetAngle -= 5;
			TargetAngle1 -= 5;
			if(TargetAngle < 77) TargetAngle = 77;
			if(TargetAngle1 < 77) TargetAngle1 = 77;
            InputPosition = (unsigned char)(TargetAngle / 1.30709);
            InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
	        send[5] = InputPosition;
            send[6] = InputPosition1;
            send[7] = (send[3] ^ send[4] ^ send[5] ^ send[6]) & 0x7f;
			com3_Send(send, 8);
			break;
		
		default:
			printf("Error!!! You Fail.\n");
			break;
		}
		
		//for(i = 0 ; i < 2; i++)
		//{
		//	printf("%d, read data = %d\n", i, read[i]);
		//}

	}
	
	printf("\n See You Next Time^^\n");

//=======================Remove the torque of the motor===========================/

	com3_ServoTRX(PowerOffSend, 4, read, 2);
	
    printf("ID %d, read data = %d\n", read[i], read[i + 1]);

	com3_Close();
	return 0;

}
