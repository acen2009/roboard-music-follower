/*
v1 : 從麥克風讀取音訊  將其存成Capture.wav  用FFT找出成分最多的頻率
v1a: 將頻率的比例呈現出來 0~2205*10Hz  取偶數
v1b: 以LOG來顯示頻率成分  
	 畫出聲音 平均振幅,平均Amp,平均頻率,最主要頻率
	 以時間軸顯示聲音資料
v1c: 顯示波形圖(Waveform)
*/
#include <cv.h>
#include <highgui.h>
#include <al.h>
#include <alc.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <fftw3.h>
#include <math.h>
#include "stdio.h"
#include "stdlib.h"
#include "roboard.h"
#include <windows.h>

#define BUFFERSIZE 8820
#define Nr  4410						// # real number
#define Nc  floor( (double)Nr/2.0 )+1		// # fourier number
#define RadianceToAngle 57.29577
#define PI 3.14159

using namespace std;

double Freq_Avg(double* );
double Amp_Avg(double* );
double Voice_Avg(double* );

const int SRATE = 44100;  // sampling rate. means 44100 samples per second (CD quality)
const int SSIZE = 4410;   // the size of the ring buffer, Sampling-Rate * Seconds * Resolution * Trackse 

ALshort buffer[BUFFERSIZE];  // ALbyte: signed  8-bit 2's-complement integer 
ALint sample;          // ALint: signed 32-bit 2's-complement integer

//===========================Variables for servo========================//
struct frames
{
     unsigned char command[5];             
};

struct frames RobotMotion[100];

int LoadMotion (FILE *);
double Abs (double);
FILE *sav;

char Txt_Name[20] = "AI_Motion.txt";
//======================================================================//


int main(int argc, char *argv[]) 
{    
//==================Variables for servo================================//
	float TargetAngle = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition = 0x00;
	unsigned char Speed = 3; //0 ~ 4 = fast ~ slow
	unsigned char MotorID = 1;//0~30
	unsigned char send[4] = {0xff, 0x00, InputPosition, 0x00};
	
    float TargetAngle1 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition1 = 0x00;
	unsigned char MotorID1 = 2;//0~30
	unsigned char send1[4] = {0xff, 0x00, InputPosition1, 0x00};
    
    float TargetAngle2 = 167.0;		//degree = 0~332  position = 0 ~ 254
	unsigned char InputPosition2 = 0x00;
	unsigned char MotorID2 = 3;//0~30
	unsigned char send2[4] = {0xff, 0x00, InputPosition2, 0x00};
    		
	unsigned char AllSend[8] = {0xff, 0x00, MotorID2 + 1, 0x00, InputPosition, InputPosition1, InputPosition2, 0x00};
	unsigned char PowerOffSend[4] = {0xff, 0xdf, 0x2f, (((0xdf) ^ (0x2f)) & 0x7f)};
    unsigned char read4[2] = {0};

	int end = 0;
	unsigned char c;
//======================================================================//
	FILE *file1;
	int i,j,k,l;
	double freq_avg,amp_avg,voice_avg;
	CvPoint Pt1,Pt2;
	int Scroll_start, Scroll_end, Data1, Data2;


	double *FT_in;
	fftw_complex *FT_out;  //直角坐標
	double *FT_Amp;   //極座標大小
	fftw_plan FT_plan;
	int MaxFreq = 0, MaxFreqVal = 0;

	FT_in = (double*) fftw_malloc(sizeof(double) * Nr);
    FT_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * Nc);
	FT_Amp = (double*) fftw_malloc(sizeof(double) * Nc);
	FT_plan = fftw_plan_dft_r2c_1d(Nr, FT_in, FT_out, FFTW_ESTIMATE);


	IplImage *image1;
    image1 = cvCreateImage(cvSize(1103,500),IPL_DEPTH_8U,3); // FFT Transform
    
//===================================init=====================================//

	roboio_SetRBVer(RB_100);

	if (com3_Init(COM_FDUPLEX) == false)
	{
		printf("error init com3");
		return 1;
	}
	
	com3_SetFormat(COM_BYTESIZE8, COM_STOPBIT1, COM_NOPARITY);
	com3_SetBaud(COMBAUD_19200BPS);
 	
//===================================init motion==============================//

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

//============================================================================//   

    alGetError();  // This function returns the current error state and then clears the error state
    /*
	ALCdevice *device = alcCaptureOpenDevice(actualDeviceName, SRATE, AL_FORMAT_MONO8, SSIZE); //(device name, freq, format, buffersize)
	AL_FORMAT_MONO"16" means 16bit resolution (2 bytes per sample)
	*/

	/*  //音訊裝置
	ALCdevice *device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_MONO16, SSIZE);

    if (alGetError() != AL_NO_ERROR) {
		cout << "Error!" << endl;
        return 0;
    }
    alcCaptureStart(device);

	const ALchar *actualDeviceName; 
	actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);  
	
	cout << "actualDeviceName: " << actualDeviceName << endl;
	cout << "device adress: " << device << endl;
	*/

	ALCdevice     *device;
	const ALCchar *szDefaultCaptureDevice;

	if (alGetError() != AL_NO_ERROR) {

			printf("Failed to generate Source or Buffers\n");

			return 0;

		}

	const ALchar *pDeviceList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

		if (pDeviceList) {

			printf("Available Capture Devices are:\n");

			while (*pDeviceList) {

				printf("**%s**\n", pDeviceList);

				pDeviceList += strlen(pDeviceList) + 1;

			}

		}

	szDefaultCaptureDevice = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	device = alcCaptureOpenDevice(szDefaultCaptureDevice,SRATE, AL_FORMAT_MONO16, SSIZE);

	printf("Starting **%s** Capture Device\n", alcGetString(device, ALC_CAPTURE_DEVICE_SPECIFIER));
	alcCaptureStart(device);

	system("PAUSE");
	
//============================================================================//
    com3_ServoTRX(PowerOffSend, 4, read4, 2);  
//============================================================================//
    
	int Times = 0, MMAX = 0;
	long TimeIndex = 0;
    //while (!_kbhit())
	while(1)
	{
		
		Sleep(1);
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &sample);  // Get number of captured samples, 一個音訊單元要用幾個byte(sizeof)，數目存在sample裡
        
		if(sample >= 4410)
		{
			alcCaptureSamples(device, (ALCvoid *)buffer, sample);
			
			for(i = 0; i < 4410; i++) FT_in[i] = buffer[i];
			fftw_execute(FT_plan);
			for(i = 0; i < Nc; i++)
			{
				FT_Amp[i] = sqrt( pow(FT_out[i][0],2) + pow(FT_out[i][1],2) );  // amplitude of polar coordinate
				if(i == 0) 
				{
					MaxFreq = 0;
					MaxFreqVal = FT_Amp[0];
				}
				else if(FT_Amp[i] > MaxFreqVal) 
				{
					MaxFreq = i;
					MaxFreqVal = FT_Amp[i];
				}
			}
			//cout << "MaxFreq = " << MaxFreq << endl;
			cout << "MaxFreq = " <<  MaxFreq*10 << " Hz" << endl;
			if(MaxFreqVal > MMAX) MMAX = MaxFreqVal;

			cvSetZero(image1);
			for(int j = 0; j < image1->widthStep; j+=3)
			{
				k = j/3;	// Freq
				l = (float) log10(FT_Amp[k]/10.0+1)*80.0;	// #

				for(int i = image1->height -1 - l; i < image1->height; i++)
				{

					image1->imageData[i*image1->widthStep + j   ] = 255;
					image1->imageData[i*image1->widthStep + j +1] = 255;
					image1->imageData[i*image1->widthStep + j +2] = 255;
				}
			}

			
			
			// ********** 畫出聲音 平均振幅,平均Amp,平均頻率,最主要頻率 **********

			//平均振幅
			voice_avg = Voice_Avg(FT_in);
			Pt1 = cvPoint(0,100);
			Pt2 = cvPoint((int)voice_avg*2,100);
			cvLine(image1,Pt1,Pt2,CV_RGB(0,255,255),5,CV_AA,0);

			//平均Amp
			amp_avg = Amp_Avg(FT_Amp);
			Pt1 = cvPoint(0,110);
			Pt2 = cvPoint((int)amp_avg/10,110);
			cvLine(image1,Pt1,Pt2,CV_RGB(0,255,0),5,CV_AA,0);

			//平均頻率
			freq_avg = (voice_avg > 20) ? Freq_Avg(FT_Amp) : 0;
			Pt1 = cvPoint(0,120);
			Pt2 = cvPoint((int)freq_avg,120);
			cvLine(image1,Pt1,Pt2,CV_RGB(255,0,0),5,CV_AA,0);

			//最主要頻率
			Pt1 = cvPoint(0,130);
			Pt2 = cvPoint(MaxFreq,130);
			cvLine(image1,Pt1,Pt2,CV_RGB(255,0,255),5,CV_AA,0);
			
//============================================================================//
			// FengMing, Start from here...
			if (voice_avg > 500)
			{
                    i = 0;
    	            end = LoadMotion(sav);
    	            printf("\nend = %d\n\n", end);
    	            while ( i < end )
    	            {
                          InputPosition = RobotMotion[i].command[1];
	                      InputPosition1 = RobotMotion[i + 1].command[1];
                          InputPosition2 = RobotMotion[i + 2].command[1];
	                      AllSend[4] = InputPosition;
                          AllSend[5] = InputPosition1;
                          AllSend[6] = InputPosition2;
	                      AllSend[7] = (AllSend[3] ^ AllSend[4] ^ AllSend[5] ^ AllSend[6]) & 0x7f;
	
                          if (com3_Send(AllSend, 8) == false)  printf("%s\n", roboio_GetErrMsg());
                          
                          printf("%d\t\n", RobotMotion[i].command[1]);
                          printf("%d\t\n", RobotMotion[i + 1].command[1]);        
                          printf("%d\t\n", RobotMotion[i + 2].command[1]);
                          
                          i = i + 3;
                          Sleep(400);                           
                    }
            }
			// FengMing, It's End~u7
//============================================================================//

			cvNamedWindow("Freq",1);
			cvShowImage("Freq",image1);

			if(cvWaitKey (1) == 'q') break;
		}
			
        // ... do something with the buffer
    }
	
	cout << "---------------------------"<<endl<<"MMAX = " << MMAX << endl;

    alcCaptureStop(device);
    alcCaptureCloseDevice(device);

	cvDestroyWindow("Freq");
	cvReleaseImage(&image1);

    return 0;
}

//=============================== fucntions ==================================//

double Freq_Avg(double* FT_Amp)
{
	double Avg, sum_FN = 0, sum_N = 0;
	double cutting;
	for(int i = 0; i < 1000; i++)
	{
		cutting = floor(FT_Amp[i]/1000.0);
		sum_FN += cutting*i;
		sum_N += cutting;
	}
	Avg = sum_FN/sum_N;
	return Avg;
}

double Amp_Avg(double* FT_Amp)
{
	double Avg, sum_N = 0;
	for(int i = 0; i < Nc; i++)
	{
		sum_N += FT_Amp[i];
	}
	Avg = sum_N/Nc;
	return Avg;
}

double Voice_Avg(double* FT_in)
{
	double Avg, sum_N = 0;
	for(int i = 0; i < Nr; i++)
	{
		sum_N += abs(FT_in[i]);
	}
	Avg = sum_N/Nr;
	return Avg;
}

//======================= Read File Function =================================//
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
//============================================================================//

/**************************************************
鋼琴頻率: 27.5Hz ~ 4186Hz (中央C頻率: 262Hz)
女聲: 200 ~ 1000 Hz
男聲:  80 ~ 520  Hz

Freq顯示: 0 ~ 22050Hz (ImageWidth = 1103, 1 pixel = 20Hz)

**************************************************/
