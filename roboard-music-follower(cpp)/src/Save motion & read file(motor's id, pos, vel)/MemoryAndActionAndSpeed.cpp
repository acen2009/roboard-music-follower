#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "roboard.h"
#include "math.h"
#include <windows.h>

#define RadianceToAngle 57.29577
#define PI 3.14159

struct frames
{
     unsigned char command[5];             
};

struct frames RobotMotion[100];

int SaveMotion (unsigned char *, FILE *);
int LoadMotion (FILE *);
int MaxFind(int);
int SpeedAssign(FILE *);
double Abs (double);
FILE *sav;

char Txt_Name[20] = "AI_Motion_Speed.txt";


int main(void)
{
	unsigned char Speed = 3; //0 ~ 4 = fast ~ slow
	int PauseTimeConstant[5] = {5, 5, 10, 14, 20};
	
    float TargetAngle = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition = 0x00;
	unsigned char MotorID = 1;//0~30
	unsigned char send[4] = {0xff, 0x00, InputPosition, 0x00};
	unsigned char read[2] = {0};
	
    float TargetAngle1 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition1 = 0x00;
	unsigned char MotorID1 = 2;//0~30
	unsigned char send1[4] = {0xff, 0x00, InputPosition1, 0x00};
	unsigned char read1[2] = {0};
    
    float TargetAngle2 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition2 = 0x00;
	unsigned char MotorID2 = 3;//0~30
	unsigned char send2[4] = {0xff, 0x00, InputPosition2, 0x00};
	unsigned char read2[2] = {0};
    		
	unsigned char AllSend[8] = {0xff, 0x00, MotorID2 + 1, 0x00, InputPosition, InputPosition1, InputPosition2, 0x00};
	unsigned char PowerOffSend[4] = {0xff, 0xdf, 0x2f, (((0xdf) ^ (0x2f)) & 0x7f)};
    unsigned char read4[2] = {0};
	
    int baud[7] = {COMBAUD_2400BPS, COMBAUD_4800BPS, COMBAUD_9600BPS, COMBAUD_19200BPS, COMBAUD_38400BPS, COMBAUD_57600BPS, COMBAUD_115200BPS};

	int i = 0, j = 0, end = 0, max = 0, sleep_time = 0;
	unsigned char c;

//===================================init====================================/

	roboio_SetRBVer(RB_100);

	if (com3_Init(COM_FDUPLEX) == false)
	{
		printf("error init com3");
		return 1;
	}
	
	com3_SetFormat(COM_BYTESIZE8, COM_STOPBIT1, COM_NOPARITY);
	com3_SetBaud(COMBAUD_19200BPS);
 	
//===================================init motion=============================/

	InputPosition = (unsigned char)(TargetAngle / 1.30709);
	InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
    InputPosition2 = (unsigned char)(TargetAngle2 / 1.30709);
    AllSend[1] = ( (((Speed) & 0x07) << 5) + (0x1f & 0x1f) ) & 0xff;
	AllSend[4] = InputPosition;
    AllSend[5] = InputPosition1;
    AllSend[6] = InputPosition2;
	AllSend[7] = (AllSend[3] ^ AllSend[4] ^ AllSend[5] ^ AllSend[6]) & 0x7f;
	
    if (com3_Send(AllSend, 8) == false)  printf("%s\n", roboio_GetErrMsg());
    
    printf("It's game time!!!\nPress 's' to start!!!!\n");
    
    if ( (c = getch()) == 's' ) 
	{
         com3_ServoTRX(PowerOffSend, 4, read4, 2);
    }        
    
//============================================================================/ 
/*
	InputPosition = (unsigned char)(TargetAngle / 1.30709);
	send[1] = ( (((Speed) & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
	send[2] = InputPosition;
	send[3] = (send[1] ^ send[2]) & 0x7f;
	com3_ServoTRX(send, 4, read, 2);
	InputPosition1 = (unsigned char)(TargetAngle1 / 1.30709);
	send1[1] = ( (((Speed) & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
	send1[2] = InputPosition1;
	send1[3] = (send1[1] ^ send1[2]) & 0x7f;
	com3_ServoTRX(send1, 4, read1, 2);
	InputPosition2 = (unsigned char)(TargetAngle2 / 1.30709);
	send2[1] = ( (((Speed) & 0x07) << 5) + ((MotorID2) & 0x1f) ) & 0xff;
	send2[2] = InputPosition2;
	send2[3] = (send2[1] ^ send2[2]) & 0x7f;        
    com3_ServoTRX(send2, 4, read2, 2);                                                                                                                                                                  
    
    printf("It's game time!!!\nPress 's' to start!!!!\n");
    
    if ( (c = getch()) == 's' ) 
	{
         com3_ServoTRX(PowerOffSend, 4, read4, 2);
    }         	
*/
//================================instruction================================/

	printf("It's game time!!!\nPress '0' to stop!!!!\n");
	printf("Press 'a' to capture the motion!!!!\n");
    printf("Press 'b' to play the motion!!!!\n");
    //printf("Press 'c' to capture then play the repeat the motion!!!!\n");

//==================================main=====================================/	

	while((c = getch()) != '0') 
	{
             switch (c)
             {
               case 'a':
	                send[1] = ( ((0x06 & 0x07) << 5) + ((MotorID) & 0x1f) ) & 0xff;
	                send[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	                send[3] = (send[1] ^ send[2]) & 0x7f;
	                com3_ServoTRX(send, 4, read, 2);
                    SaveMotion(read, sav);
                    send1[1] = ( ((0x06 & 0x07) << 5) + ((MotorID1) & 0x1f) ) & 0xff;
	                send1[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	                send1[3] = (send1[1] ^ send1[2]) & 0x7f;
	                com3_ServoTRX(send1, 4, read1, 2);
                    SaveMotion(read1, sav);
	                send2[1] = ( ((0x06 & 0x07) << 5) + ((MotorID2) & 0x1f) ) & 0xff;
	                send2[2] = ( ((0x01 & 0x0f) << 4) + (0x0f & 0x0f) ) & 0xff;
	                send2[3] = (send2[1] ^ send2[2]) & 0x7f;                    
                    com3_ServoTRX(send2, 4, read2, 2);
                    SaveMotion(read2, sav);
                    printf("\nPress '0' to stop!!!!\n");
                    printf("Press 'a' to continue!!!!\n");
 	                break;
                       
               case 'b':
                    i = 0;
    	            end = LoadMotion(sav);
    	            printf("\nend = %d\n\n", end);
    	            while ( i < end )
    	            {
                          InputPosition = RobotMotion[i].command[1];
	                      InputPosition1 = RobotMotion[i + 1].command[1];
                          InputPosition2 = RobotMotion[i + 2].command[1];
                          Speed = RobotMotion[i + 3].command[0];
                          AllSend[1] = ( (((Speed) & 0x07) << 5) + (0x1f & 0x1f) ) & 0xff;
	                      AllSend[4] = InputPosition;
                          AllSend[5] = InputPosition1;
                          AllSend[6] = InputPosition2;
	                      AllSend[7] = (AllSend[3] ^ AllSend[4] ^ AllSend[5] ^ AllSend[6]) & 0x7f;
	
                          if (com3_Send(AllSend, 8) == false)  printf("%s\n", roboio_GetErrMsg());
                          
                          printf("%d\t\n", RobotMotion[i].command[1]);
                          printf("%d\t\n", RobotMotion[i + 1].command[1]);        
                          printf("%d\t\n", RobotMotion[i + 2].command[1]);
                          printf("%d\t\n", RobotMotion[i + 3].command[0]);
                          
                          max = MaxFind(i);
                          
                          i = i + 4;
                          
                          sleep_time = (int)(max * PauseTimeConstant[Speed]);
                          if (sleep_time > 75) Sleep(sleep_time);
                          else Sleep(500);                           
                    }                 
                    printf("\nPress '0' to stop!!!!\n");
                    printf("Press 'a' to capture the motion!!!!\n");
                    printf("Press 'b' to play the motion again!!!!\n");
                    break;
                    
               //case '3':
                              
               default:
                    printf("\nPress '0' to stop!!!!\n");
	                printf("Press 'a' to capture the motion!!!!\n");
                    printf("Press 'b' to play the motion!!!!\n");
                    break;     
              }
              
              com3_ServoTRX(PowerOffSend, 4, read4, 2);
	}

//===============================close=======================================/

	com3_ServoTRX(PowerOffSend, 4, read4, 2);
	
	for(i = 0 ; i < 2; i++)
	{
		printf("%d, read data = %d\n", i, read4[i]);
	}

	com3_Close();
	return 0;
}

//================================== functions ============================/

double Abs(double a)
{
    return (a > 0.0)? a : -a;       
}


int MaxFind(int index)
{
    int j = 1;
    int max = 0, temp = 0;
    
    if (index == 0) 
    {
         max = Abs(RobotMotion[index].command[1] -  128);
         while (j <= 2)
         {
               temp = Abs(RobotMotion[index + j].command[1] -  128);        
               if (temp > max) max = temp;        
               j = j + 1;   
         }  
    }
    else 
    {
         max = Abs(RobotMotion[index].command[1] -  RobotMotion[index - 3].command[1]);         
         while (j <= 2)
         {
               temp = Abs(RobotMotion[index + j].command[1] -  RobotMotion[index + j - 3].command[1]);        
               if (temp > max) max = temp;        
               j = j + 1;   
         }      
    }
    
    return max;                              
}


int SaveMotion(unsigned char *InputArray, FILE *fp)
{
    int i;
    char WriteIn[10] = {0};
    
    fp = fopen(Txt_Name, "a");
    
    for (i = 0 ; i < 2 ; i++)
    {
        itoa(InputArray[i], WriteIn, 10);
        fputs(WriteIn, fp);
        fputc(' ', fp);
    }
    
    fputc('\n', fp);
    fclose(fp);
    
    return 1;    
}


int LoadMotion(FILE *fp)
{
    int i = 0, j = 0, k = 0, p = 0;
    char ReadOut[250] = {0};         // the whole char from txt file
    char temp[5] = {0};              // digits of each data
    
    fp = fopen(Txt_Name, "rt");
    
    while (fgets(ReadOut ,250 ,fp) != NULL) 
    {    
         i = 0;
         //printf("A\n");
         for (j = 0 ; ReadOut[j] != '\n' ; j++)
         {   
             if (ReadOut[j] != ' ' &&  ReadOut[j + 1] != '\n')
             {
                  temp[p] = ReadOut[j]; 
                  p = p + 1;   
             }
             else if (ReadOut[j] != ' ' && ReadOut[j + 1] == '\n')
             {
                  temp[p] = ReadOut[j]; 
                  p = p + 1;
                  temp[p + 1] = '\0';
                  RobotMotion[k].command[i] = (unsigned char) atoi(temp);
                  //printf("(%d, %d):%d\n", k, i, RobotMotion[k].command[i]);
                  p = 0;  
                  i = i + 1;
             }
             else
             {
                  temp[p] = '\0';
                  RobotMotion[k].command[i] = (unsigned char) atoi(temp);
                  //printf("(%d, %d):%d\n", k, i, RobotMotion[k].command[i]);
                  p = 0;  
                  i = i + 1;
             }                                  
         }
         
         k = k + 1; 
    }
    
	fclose(fp);
    return k;             
}


int SpeedAssign(FILE *fp)
{
    unsigned char c, speed = '3';
    
    fp = fopen(Txt_Name, "a");
    
    printf("Press 'a' to assign the speed (0~4)!!!!\n");
    printf("Press 'd' to use default speed 3!!!!\n");
    
    switch (c = getch())
    {
     case 'a':
          speed = getch();
          fputc(speed, fp);
          fputc(' ', fp);
          break;
     case 'd':
          fputc(speed, fp);
          fputc(' ', fp);
          break;     
     default:
          printf("Press 'a' to assign the speed (0~4)!!!!\n");
          printf("Press 'd' to use default speed 3!!!!\n");
          break;
    }
                
    fputc('\n', fp);
    fclose(fp);
    
    return 1;    
}
