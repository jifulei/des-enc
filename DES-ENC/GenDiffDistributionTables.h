#pragma once
#include"Types.h"
#include "Tables.h"
#include<math.h>
#include<stdio.h>
//���������������������������ְ��ո��ʴ�С�������򡣸��ʹ���0,2,4,6,8,10,12,14,16���������û�м���Ƶ��Ϊ64�����.
u8 DDT_SearchInOrderX[8][9][256]={0};
u8 DDT_SearchInOrderY[8][9][256]={0};
int DDT_SearchInOrderLength[8][9]={0};

//��ţ������֣������֣��Եĸ���
unsigned int DDT_int[8][64][16]={0};
//���������֣������֣���ת��ΪA*2^B����ʽ��DDT������A��ֵ��1<=A<2;DDT_exponent������B��ֵ��B��ֵ�ټ�6����Ƶ��ת��Ϊ���ʡ�
double DDT[8][64][16]={0};
int DDT_exponent[8][64][16]={0};
//S�б任
void Substitution(u8* output,u8 input,int index){
	u8 x=input,x1,x2;
	x1=(x&0x1)|((x>>4)&0x2);
	x2=(x>>1)&0xf;
	*output=S[index][x1][x2];
}
//���������֣������֣���ת��ΪA*2^B����ʽ��DDT������A��ֵ��1<=A<2;DDT_exponent������B��ֵ��B��ֵ�ټ�6����Ƶ��ת��Ϊ���ʡ�
void set(double frequency,int Si,int i,int j)
{
	frequency=frequency/2;
	DDT_exponent[Si][i][j]++;
	if(frequency<2) DDT[Si][i][j]=frequency;
	else set(frequency,Si,i,j);
}


//���ɲ�ֲַ����������ǰ˸�S�У�8*��2^6��*��2^4��
void GenDiffDistributionTable(){
	double frequency;
	for(int Si=0;Si<8;Si++){
		for(u8 Input1=0;Input1<64;Input1++){
			for(u8 Input2=0;Input2<64;Input2++){
				u8 Output1,Output2;
				Substitution(&Output1,Input1,Si);
				Substitution(&Output2,Input2,Si);
				DDT_int[Si][Input1^Input2][Output1^Output2]++;                  //�õ��˲�ֲַ����ֵ��2,4,6,8,10,12,14,16
			}
		}
		for(int i=0;i<64;i++){
			for(int j=0;j<16;j++){
				frequency=DDT_int[Si][i][j];        
				if(frequency!=0) {
					set(frequency,Si,i,j);
					DDT_exponent[Si][i][j]=DDT_exponent[Si][i][j]-6;        //��ɸ��ʣ�����2��6�η�
				} 
			}
		}
	}
}
//�����������ְ��ո��ʴ�С����
void GenSearchInOrder(){
	int count,index;
	for(int Si=0;Si<8;Si++){
		for(u8 x=0;x<64;x++){
			for(u8 y=0;y<16;y++){
				count=DDT_int[Si][x][y]/2;
				index=DDT_SearchInOrderLength[Si][count];         //index��¼����Ƶ��Ϊ2count�Ĳ�ֶԵĸ�������ʼֵΪ0��
				DDT_SearchInOrderX[Si][count][index]=x;           //xΪƵ��Ϊ2count�ĵ�index���������֣������֣��Ե������֡�
				DDT_SearchInOrderY[Si][count][index]=y;
				DDT_SearchInOrderLength[Si][count]++;
			}
		}
	}
}
