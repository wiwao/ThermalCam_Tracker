//SPDX-FileCopyrightText: 2023  Ono Takeshi<xono10@cloud.com>
//SPDX-License-Identifier: MIT
//Copyright (c) 2023, Ono Takeshi
//All rights reserved.

//　2023/6/28 変更　Pi4のLepton読み取りおよびJetson Nanoとのデータ送受信
// 読み取り及び最小限の表示しかしていないので、さらに追加すれば面白いものが作れる
// 送った画像データはNanoでAI認識された場合b_countにデータが渡される。座標など使い方は
// ofApp::draw() の最後の方を参照。Class番号、認識率なども同時に送られてくる


#include "ofApp.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
//***************************::
typedef struct Bound{
	int x;
	int y;
	int w;
	int h;
	int c;
	float pl;
} bound_x;
const int colormap_contrast[] =  {16, 16, 16, 23, 16, 22, 30, 15, 30, 37, 16, 37, 46, 15, 45, 53, 15, 52, 60, 15, 60, 67, 15, 67, 75,
                     15, 75, 82, 15, 81, 89, 15, 90, 98, 14, 96, 105, 14, 105, 112, 14, 111, 120, 15, 121, 127, 15, 127,
                     135, 15, 135, 143, 14, 142, 150, 14, 150, 158, 14, 157, 165, 14, 165, 172, 14, 172, 179, 14, 180,
                     186, 14, 187, 195, 14, 195, 202, 14, 201, 209, 14, 210, 217, 14, 216, 209, 15, 214, 202, 14, 211,
                     194, 15, 209, 187, 14, 206, 179, 15, 204, 172, 14, 201, 165, 15, 199, 157, 14, 196, 150, 15, 194,
                     141, 14, 191, 135, 15, 189, 126, 14, 186, 120, 15, 184, 112, 14, 181, 105, 15, 179, 97, 14, 176,
                     91, 15, 174, 82, 14, 171, 74, 15, 169, 67, 14, 166, 60, 15, 164, 52, 14, 161, 45, 15, 159, 38, 14,
                     156, 30, 15, 154, 23, 14, 151, 15, 15, 149, 16, 23, 152, 14, 30, 155, 15, 38, 156, 15, 44, 158, 14,
                     53, 162, 14, 59, 164, 15, 67, 165, 13, 74, 168, 14, 82, 171, 15, 89, 174, 13, 96, 176, 14, 104,
                     178, 14, 111, 180, 13, 119, 183, 13, 125, 185, 14, 133, 187, 12, 140, 189, 13, 148, 192, 14, 155,
                     195, 12, 162, 198, 13, 170, 199, 13, 177, 201, 12, 185, 205, 12, 191, 207, 13, 199, 208, 11, 206,
                     211, 12, 214, 214, 12, 208, 206, 12, 205, 200, 12, 199, 192, 12, 194, 185, 12, 190, 176, 12, 185,
                     169, 14, 181, 162, 12, 176, 155, 13, 170, 147, 14, 166, 141, 13, 161, 133, 13, 156, 125, 14, 151,
                     119, 13, 147, 110, 14, 142, 103, 14, 137, 96, 13, 132, 88, 14, 128, 81, 14, 122, 74, 13, 118, 66,
                     14, 113, 60, 15, 108, 52, 14, 104, 44, 15, 99, 36, 15, 93, 29, 15, 90, 22, 15, 84, 15, 23, 89, 14,
                     28, 93, 13, 36, 97, 15, 42, 103, 14, 49, 106, 13, 55, 112, 13, 63, 116, 12, 69, 120, 13, 76, 125,
                     12, 84, 129, 12, 90, 134, 11, 97, 138, 10, 104, 143, 12, 111, 147, 11, 117, 152, 10, 124, 156, 9,
                     130, 161, 10, 138, 165, 10, 144, 170, 9, 151, 174, 9, 159, 179, 8, 165, 183, 9, 172, 187, 8, 179,
                     193, 8, 186, 196, 7, 192, 202, 6, 200, 206, 8, 205, 210, 7, 213, 215, 6, 209, 208, 7, 207, 201, 7,
                     204, 194, 7, 200, 187, 7, 196, 180, 8, 194, 173, 8, 191, 166, 8, 187, 159, 9, 183, 153, 9, 181,
                     145, 9, 178, 139, 10, 174, 132, 10, 172, 124, 10, 168, 118, 11, 166, 112, 10, 162, 105, 10, 160,
                     98, 11, 156, 91, 11, 153, 84, 11, 150, 77, 12, 147, 70, 12, 143, 63, 12, 140, 57, 13, 137, 49, 13,
                     134, 43, 13, 130, 36, 14, 127, 29, 14, 124, 22, 14, 121, 15, 15, 124, 16, 17, 128, 17, 19, 130, 20,
                     19, 133, 21, 21, 135, 21, 22, 139, 23, 24, 141, 25, 24, 144, 26, 26, 148, 28, 28, 151, 29, 30, 153,
                     31, 30, 156, 32, 32, 160, 34, 34, 163, 35, 36, 164, 36, 36, 168, 38, 38, 171, 39, 40, 174, 40, 42,
                     176, 43, 42, 180, 44, 44, 183, 45, 46, 187, 46, 48, 189, 49, 48, 191, 49, 49, 194, 50, 51, 198, 52,
                     53, 200, 54, 53, 203, 55, 55, 204, 60, 61, 205, 67, 68, 206, 73, 72, 207, 79, 79, 208, 84, 84, 209,
                     91, 91, 210, 96, 97, 211, 103, 104, 212, 109, 108, 213, 115, 114, 214, 120, 120, 215, 127, 127,
                     216, 132, 133, 217, 139, 139, 218, 145, 143, 219, 151, 150, 220, 156, 156, 221, 163, 163, 222, 168,
                     169, 223, 175, 175, 224, 181, 179, 225, 187, 186, 226, 192, 192, 227, 199, 199, 228, 204, 204, 229,
                     211, 211, 230, 217, 215, 231, 223, 222, 232, 228, 228};
                   
const int colormap_glowBow[] = {16, 16, 16, 19, 17, 18, 22, 16, 16, 25, 17, 18, 28, 17, 19, 31, 17, 20, 34, 17, 19, 36, 18, 20, 39,
                    18, 19, 43, 19, 21, 45, 18, 21, 48, 20, 21, 52, 19, 22, 54, 20, 23, 58, 20, 23, 63, 21, 23, 68, 21,
                    25, 70, 21, 26, 73, 22, 27, 75, 22, 26, 79, 22, 27, 81, 22, 28, 84, 23, 27, 87, 22, 28, 91, 24, 30,
                    96, 23, 30, 102, 24, 33, 104, 25, 32, 108, 25, 33, 110, 25, 34, 117, 25, 34, 120, 27, 34, 122, 27,
                    35, 127, 28, 35, 129, 27, 35, 132, 29, 37, 135, 27, 37, 138, 29, 38, 141, 29, 39, 143, 29, 40, 147,
                    29, 41, 150, 31, 41, 152, 30, 41, 155, 29, 42, 158, 30, 41, 165, 31, 44, 167, 32, 43, 170, 32, 44,
                    175, 33, 45, 177, 33, 46, 178, 32, 46, 182, 32, 45, 186, 33, 47, 188, 34, 48, 190, 34, 47, 194, 34,
                    48, 195, 35, 49, 195, 35, 47, 197, 38, 48, 196, 39, 46, 198, 39, 45, 199, 41, 44, 200, 42, 43, 201,
                    43, 43, 200, 44, 41, 201, 45, 42, 203, 46, 41, 204, 47, 42, 204, 47, 40, 205, 49, 40, 205, 49, 38,
                    206, 52, 38, 207, 52, 36, 208, 53, 37, 209, 54, 36, 210, 55, 36, 210, 58, 35, 211, 59, 34, 212, 60,
                    33, 213, 60, 33, 214, 61, 33, 213, 62, 31, 215, 64, 33, 215, 64, 31, 216, 66, 30, 218, 66, 30, 218,
                    66, 30, 218, 68, 29, 219, 70, 28, 220, 69, 28, 221, 72, 26, 223, 73, 26, 222, 74, 24, 223, 75, 25,
                    224, 76, 24, 225, 78, 23, 225, 78, 22, 226, 79, 23, 227, 80, 22, 227, 81, 20, 228, 82, 21, 229, 83,
                    20, 230, 83, 18, 231, 86, 19, 231, 86, 17, 232, 87, 16, 233, 88, 17, 234, 90, 16, 235, 91, 14, 235,
                    91, 14, 236, 93, 13, 237, 94, 12, 236, 96, 13, 237, 97, 13, 237, 99, 14, 237, 101, 13, 236, 103, 12,
                    236, 105, 13, 237, 106, 12, 236, 108, 11, 236, 112, 12, 237, 113, 13, 236, 115, 12, 236, 117, 13,
                    235, 119, 12, 236, 122, 12, 237, 123, 13, 237, 125, 13, 236, 127, 12, 236, 129, 13, 237, 130, 12,
                    236, 132, 13, 236, 134, 12, 237, 135, 12, 237, 137, 13, 237, 142, 12, 236, 144, 13, 236, 146, 12,
                    237, 147, 13, 237, 149, 12, 236, 151, 13, 237, 152, 13, 237, 154, 12, 236, 156, 13, 236, 158, 12,
                    236, 160, 11, 235, 161, 12, 236, 163, 12, 236, 165, 13, 235, 167, 12, 236, 170, 12, 236, 172, 11,
                    235, 173, 12, 236, 176, 12, 235, 179, 12, 236, 180, 13, 236, 182, 12, 236, 184, 11, 237, 185, 12,
                    236, 187, 11, 236, 188, 12, 235, 190, 12, 236, 191, 13, 235, 194, 12, 236, 196, 11, 235, 199, 11,
                    236, 201, 11, 235, 202, 12, 235, 204, 14, 236, 204, 19, 236, 205, 23, 236, 204, 27, 235, 206, 30,
                    236, 206, 34, 236, 207, 37, 236, 207, 41, 235, 208, 45, 236, 208, 48, 236, 209, 52, 236, 209, 56,
                    235, 211, 65, 236, 212, 68, 235, 212, 71, 236, 212, 74, 234, 212, 78, 235, 213, 82, 235, 214, 87,
                    236, 214, 91, 236, 215, 94, 235, 217, 97, 235, 216, 100, 235, 217, 105, 235, 216, 108, 234, 218,
                    111, 235, 218, 116, 235, 219, 122, 235, 220, 127, 236, 220, 131, 235, 221, 134, 235, 221, 138, 235,
                    222, 142, 235, 221, 146, 234, 222, 148, 235, 223, 153, 235, 224, 157, 235, 225, 160, 236, 225, 165,
                    234, 225, 168, 235, 226, 171, 235, 225, 175, 236, 227, 182, 235, 228, 187, 234, 228, 190, 234, 229,
                    195, 235, 230, 197, 236, 230, 202, 234, 230, 205, 235, 231, 208, 235, 232, 213, 235, 231, 216, 234,
                    232, 219, 234, 234, 224, 235, 234, 228, 235, 235, 235};
int colormap[672];
int fd;
static uint8_t result[164*240];
static uint16_t *frameBuffer;ofxOscMessage mm;
ofImage img,img1,img2; 
ofPixels pixels;
char Pi3_ip[15];

struct buffer_x {
   uint8_t resultb[164*240];
};
buffer_x bufferxx;
std::vector<buffer_x>bufferx;

int    touchX = int(79 % 80), touchY = int(59 / 30) * 60 + int(59 % 30) * 2  + int( 79 / 80);
float  sl_256 = 0.0f;
float  value_min = 0, value_max = 0, tempXY  = 0;
int    th_l=999,th_h=999;
float  av3;
int    	av5,huri1=0;
float  diff;
bool 	ok;
float 	ttt,tttx,aux;
float 	point_l[9][9];
int 	past=0;
float 	xreset=0;
ofImage myImage1;

int     Lepton_3x,read_err=0,test_err=0,F_H=0;
string  ver,baf1="",baf2="",baf3="",baf4="",baf5="";
string  class_name[4]={"Goo","Tyoki","Paa","Face"};
int b_count=0;
bound_x b_1[100];  //100 kono baundingbox kakuho;
float x_time=0,y_time=0;
//-------LEPTON Reset---LEPTONのリセットコマンド--------
void L_reset(){
		tttx=ofGetElapsedTimef();
		SpiClosePort(0);
		usleep(6000);
		SpiOpenPort(0);
		lepton_restart();
}
//---------Get my IP---------------------------------
int IP_Adr (char *abc) {
struct ifaddrs * ifAddrStruct=NULL;
struct ifaddrs * ifa=NULL;
void * tmpAddrPtr=NULL;
char addressBuffer[INET_ADDRSTRLEN];

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP4 Address %s\n", ifa->ifa_name, addressBuffer); 
        }
    }
     for(int ii=0;ii<INET_ADDRSTRLEN;ii++){
           abc[ii]=addressBuffer[ii]; 
	   }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return 0;
}
//-----------Lepton読み込みルーチンの最下位　裏タスクで回し続けている-----------------------
void R_flame(){
int ecount=0,ecount2=0;
int byte1=164*20;  //164*X
uint8_t result[164*240];

usleep(2000); //usleep(2000); x2600 x2000
baka:    result[3280] = 0;      //164*20=3280
    int ecount1=-1;
    while ((result[3280] & 0b01110000) != 0x10) {    //確実に１セグメント目を捕まえるための処理。２０行目にセグメント番号が出力される。
        uint16_t data = 0x0F;
        while ((data & 0x0f) == 0x0f ) //エラー行を読み飛ばすための処理 セグメントの一番最初の行を捕まえる。0x0fよりこの方が早い
        {
            usleep(68);    //BEST 70    184    60  70  75 60 70
            ecount1++;
            read(spi_cs0_fd, result, sizeof(uint8_t)*164);
            data =  result[0];
            if (ecount1>120){  //900はだめ　　100,120はOK=120
                            usleep(550);  //500
                            ecount2++;
                            baf2=ofToString(ecount2);
                            if (ecount2>50)  //50
                                {
                                    ecount2=0;
                                    read_err++;
                                    baf4=ofToString(read_err);
                                    L_reset();
                                }
                            goto baka;
                            }
        }
        for (int frame_number = 164; frame_number < 9840; frame_number+=byte1) {
            read(spi_cs0_fd, result+sizeof(uint8_t)*frame_number, sizeof(uint8_t)*byte1);
        }
    }
    for(int ii = 9840; ii < 39360; ii+=9840){
        usleep(6060);  // BEST!!6070  x6100

        for(int i = ii; i < ii+9840; i+=byte1){
            read(spi_cs0_fd, result+sizeof(uint8_t)*i, sizeof(uint8_t)*byte1);
        }
    }
    int ii=0;
    //ここはエラーが出なくなったので実行しなくても良いが取るとリセット倍増
    for (int i= 0;i<39360;i+=164)
    {
        if (result[i+1] != ii) {
                ecount ++;
                if(ecount>10){
                    L_reset();
                    test_err++;
                    baf5=ofToString(test_err);
                    ecount=0;
                }
                baf3=ofToString(ecount);
                ecount2=0;
                goto baka;
        }
        ii++;
        if (ii==60) ii=0;
    }
    //LEPTON Rawを　バッファに保存
    memcpy(bufferxx.resultb,result,sizeof(result));
    bufferx.emplace_back(bufferxx);
}
//-------------Leptonで読み取ったデータを取り出し　変形、温度に変換する-----------------------
void read_lepton(){
	frameBuffer = (uint16_t *)result;
	int row, column;
	uint16_t value;
	uint16_t minValue = 65535;
	uint16_t maxValue = 0;
	uint16_t ondox[170][120]; //*************hanten!!
	int tempXY1;
	int col1,row1;
    //バッファ確認
redo:    while(bufferx.size() < 1)  usleep(100);  //R_flameより実行サイクルが早いのでバッファに１個貯まるまでウエイトさせる
                                                  //１個以上だとかなりの遅延発生
    memcpy(result,bufferx[0].resultb,sizeof(bufferx[0].resultb)); //1画面読み込み
    bufferx.erase(bufferx.begin());//読み込んだら先頭の画像データを削除
    frameBuffer = (uint16_t *)result; //ポインタだけなのでスピード問題なし
    //配列に変換
		for(int i=0;i<19680;i++) {
			if(i % 82 < 2) {
				continue;
			}
			int temp = result[i*2];
			result[i*2] = result[i*2+1];
			result[i*2+1] = temp;
			value = frameBuffer[i];
			if(value> maxValue) {
				maxValue = value;
			}
			if(value < minValue) {
				if(value != 0)
					minValue = value;
			}
		}	
		//tempXY1= (result[touchY * 164 + 2 * (touchX+2)+1] <<8 | result[touchY * 164 + 2 * (touchX+2) ]); 
		float rrr = (float)(maxValue - minValue) * (1.0f - abs(sl_256)); //高温側を256等分 iPhoneスライダーに連動 標準は0。最高で0.9
		
		diff = rrr / 255.0f; 
		if(diff < 1.0f) diff = 1.0f;  //256階調で表現できなくなったらノイズを減らすため強制的に256階調演算をさせる
		rrr = (float)(maxValue - minValue) * sl_256;         //開始低温度側温度
		if (sl_256 > 0) minValue = (float)minValue + rrr; //最低温度表示も開始温度に連動させる
		if (sl_256 < 0) maxValue = (float)maxValue + rrr; //最高温度表示も開始温度に連動させる minはそのまま
		float fpatemp_f = 273.15 ;  //for LEPTON3.5
		//for LEPTON3.5
		if(Lepton_3x==1){
		
		value_min = 0.01 * (float)minValue     - fpatemp_f;
		value_max = 0.01 * (float)maxValue     - fpatemp_f;
		//tempXY    = 0.01 * (float)tempXY1 - fpatemp_f;
		}
		//for LEPTON3.0
		else {
		fpatemp_f = - 535.4999f + aux/ 100.0f;
		value_min = 0.03262 * (float)minValue     + fpatemp_f;
		value_max = 0.03262 * (float)maxValue     + fpatemp_f;
		//tempXY    = 0.03262 * (float)tempXY1 + fpatemp_f;
		}		
		if(th_l != 999){   //一応エラーチェック　温度範囲指定の時の事前計算
            float 	av1=255.0f/(th_h-th_l);
					av3=av1/(255.0f/(value_max-value_min));
					av5=int((value_min-th_l)*av1);
            }
		else {av3=1; av5=0;}   //iPhoneから送られてくる温度差が0以下なら通常の色設定にする
		float avx=av3/diff;
		float avxx=-minValue*avx+av5;		
		int tx=touchY * 164 + 2 * (touchX+2) ;
		int ttty=ofGetElapsedTimef();
		
		for(int k=0; k<19680; k++) {
			if(k % 82 < 2) {
				continue;
			}
			value = (int)frameBuffer[k]*avx+avxx;
			if (value<0) value=0;
			if (value>255) value=255;
			if((k/82) % 2 == 0){
					column = (k % 82 - 2);
					row = (k / 82)/2;
			}
			else{
					column = ((k % 82 - 2))+(82-2);
					row = (k / 82)/2;
			}	
			if(ttty-tttx<2) value=0;
			
			if(F_H==0){
				img.setColor(column, row, value); //*************hanten!!
				ondox[column][row]=(int)frameBuffer[k]; //*************hanten!!
			}
			else {
				img.setColor(159-column, row, value); 
				ondox[159-column][row]=(int)frameBuffer[k];
			}
			
			if(k*2==tx )
			{
				col1=column;row1=row;		
			}
		}
		int x1=col1-4;	if(x1<0	) 	x1=0;
		int x2=col1+4;	if(x2>159) 	x2=159;
		int y1=row1-4;	if(y1<0	) 	y1=0;
		int y2=row1+4;	if(y2>119) 	y2=119;
		for(int i=x1;i<=x2;i++)
		{
			for (int j=y1;j<=y2;j++){
				if(Lepton_3x==1)
				{ 
					point_l[i-x1][j-y1]=0.01*ondox[i][j] - fpatemp_f; 
					if(j==y1+4 && i==x1+4) tempXY=point_l[i-x1][j-y1];
				}//for LEPTON3.5
				else 
				{
					point_l[i-x1][j-y1]= 0.03262 *ondox[i][j] + fpatemp_f; 
					if(j==y1+4 && i==x1+4) tempXY=point_l[i-x1][j-y1];
				}//for LEPTON3.0
			}
        }
}
//--------------LEPTON読み込みスレッド--マルチスレッド設定--------------------
class Xxlepton:public ofThread{
public:
private:
  void threadedFunction(){
	  float timex;
	  while(1){
		 R_flame();
		}
  }
};
Xxlepton ll;
//----------------温度、画像変換スレッド--マルチスレッド設定------------------------------
class Xxlepton1:public ofThread{
public:
private:
  void threadedFunction(){
      float timex;
      while(1){
        read_lepton();
        }
  }
};
Xxlepton1 ll1;
//--------------------------------------------------------------
void ofApp::setup(){
    for(int i=0;i<224;i++){
		int r,g,b;
		r=colormap_contrast[i*3];
		g=colormap_contrast[i*3+1];
		b=colormap_contrast[i*3+2];
		colormap[i*3]=b;
		colormap[i*3+1]=g;
		colormap[i*3+2]=r;
		
	}	
	//ofSetFrameRate(30);
	Lepton_3x=lepton_no();  //1=V3.5  2=V3.0 0=none;
	if(Lepton_3x==1) ver="3.5";
	else ver="3.0";
	baf1="It's 0 minits Past!!";
    
    font.load("verdana.ttf", 20, true, true, true);
    myImage1.load("WTlogoH.png");
	ofSetFrameRate(60); // run at 60 fps
	ofSetVerticalSync(true);
	HOST=arg_x;
	sender.setup(HOST, SPORT);
    receiver.setup( RPORT );
    
	img.allocate(160,120,OF_IMAGE_GRAYSCALE);
	img2.allocate(160,30,OF_IMAGE_GRAYSCALE);
    img1.allocate(160,120,OF_IMAGE_COLOR);
	IP_Adr(Pi3_ip);printf("Pi3 IP= %s\n",Pi3_ip); 
	
	SpiOpenPort(0);
	usleep(10000);
	
	ttt=ofGetElapsedTimef();	
	tttx=ofGetElapsedTimef();
	
	for(int i=0;i<160;i++){
		for (int j=0;j<120;j++){
			img.setColor(i, j, 0);
		}
	}
    	ll.startThread();   //LEPTON読み込みタスク開始。ll,ll1の２つのタスクはバッファで通信させている
    	ll1.startThread();  //RAWデータ変換タスク開始	
	if(Lepton_3x != 1){
		aux=lepton_temperature();  // if LEPTON3.0
	}
}
//--------------------------------------------------------------
void ofApp::update(){
	
	//b_count=0;  //boundindbox kazu no syokika
	if(ofGetElapsedTimef()-ttt > 60) {
						ttt=ofGetElapsedTimef();
						past++;
						baf1="It's "+ofToString(past)+" minits Past!!";
						baf2="";
						baf3="";
						//for LEPTON3.5
						if(Lepton_3x != 1){
							aux=lepton_temperature();
						}			
	}
		img.update();
		pixels=img.getPixels();
		ofSaveImage(pixels,imgAsBuffer);
		ofxOscMessage m;
		m.setAddress("/i");
		m.addBlobArg(imgAsBuffer);
		sender.sendMessage(m);
        usleep(10000);  //かなり重要
	while( receiver.hasWaitingMessages() ){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        string GM=m.getAddress() ;
        //*********Resive Point & Config
        if(GM == "/t" ){   //３種類の温度受信処理
			touchX  = m.getArgAsInt(0);
			touchY  = m.getArgAsInt(1);
			sl_256  = m.getArgAsFloat(2);  //Dynamic range
			F_H		= m.getArgAsInt(3);    //Sayu Hanten 
			th_l    = m.getArgAsInt(4);    //Color renge
			th_h    = m.getArgAsInt(5);
         }
         //********Resive Lepton reset
         else if(GM == "/r" ){ 
				SpiClosePort(0);
				int sss=m.getArgAsInt(0);
				if (sss==1){
					usleep(5000);
					SpiOpenPort(0);
					lepton_restart();
				}
			}
		else if(GM == "/y" ){   //３種類の温度受信処理
			x_time=ofGetElapsedTimef();
			b_count=m.getArgAsInt(0);
			for (int b=0;b<b_count;b++){
				b_1[b].x=m.getArgAsInt(b*6+1);
				b_1[b].y=m.getArgAsInt(b*6+2);
				b_1[b].w=m.getArgAsInt(b*6+3);
				b_1[b].h=m.getArgAsInt(b*6+4);
				b_1[b].c=m.getArgAsInt(b*6+5);
				b_1[b].pl=m.getArgAsFloat(b*6+6);
			}
			if (b_count>=100) b_count=100;  //100KOHYOUZIWO GENNDOTOSURU
			}	
        }		
        //*******Send  Ondo
		ofxOscMessage mm;
			mm.setAddress("/m");
			mm.addFloatArg(value_min);
			mm.addFloatArg(value_max);
			mm.addFloatArg(tempXY);
			mm.addStringArg(Pi3_ip);
			sender.sendMessage(mm);
		//********Send 9X9 Ondo
		ofxOscMessage mx;
			mx.setAddress("/x");
			for(int i=0;i<9;i++)
			{
				for (int j=0;j<9;j++) mx.addFloatArg(point_l[i][j]);
			}
			sender.sendMessage(mx);
}
//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackgroundGradient(0, 0);
       img1.update();
	unsigned char *idata;
            idata=img.getPixels().getData();
     float color_bai=672.0f/768.0f;
	for(int i=0;i<160;i++){
                for (int j=0;j<120;j++){
                       int vv=int(idata[i+j*160]*color_bai)*3;
                        img1.setColor(i, j, ofColor(colormap[vv],colormap[vv+1],colormap[vv+2],100));
                }
        }
	img1.draw(25,25,800,600);
	// display instructions	
	string buf = "sending osc to: " + string(HOST);
	buf += " " + ofToString(SPORT)+" LEPTON "+ver;
	//ofDrawBitmapStringHighlight(buf, 10, 20);
	font.drawString(buf, 120,20);
	font.drawString(baf1,30,50);
	ofSetColor(180,180,180);
	font.drawString(baf2,30,74);
	font.drawString(baf3,30,96);
	font.drawString(baf4,30,118);
	font.drawString(baf5,30,140);
	//myImage1.draw(470,550,360,72);  //kanden logo
	myImage1.draw(651,550,178.9,72);  //w&t logo
	if(b_count>0){
		//if(b_count==1) x_time=ofGetElapsedTimef();
	  for(int b=0;b<b_count;b++){

		switch(b_1[b].c){
			case 0:
				ofSetColor( 0,255,255);
				break;
			case 1:
				ofSetColor( 125,255,125);
				break;
			case 2:
				ofSetColor( 125,125,255);
				break;
			case 3:
				ofSetColor( 255,125,125);
				break;
			}
		ofNoFill();
		ofSetLineWidth(6);
		ofDrawRectangle(b_1[b].x,b_1[b].y,b_1[b].w,b_1[b].h);
		ofSetColor(255,255,255);
		string ss;
		ss=" "+ class_name[b_1[b].c]+" "+ofToString(b_1[b].pl*100,1);
		font.drawString(ss,b_1[b].x+4,b_1[b].y+24);
	  }
	if(ofGetElapsedTimef()-x_time>0.2) b_count=0;
	}
	ofSetColor( 255,255,255);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
