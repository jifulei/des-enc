#pragma once
#include"Types.h"
#include "Tables.h"
#include<math.h>
#include<stdio.h>
//以下三个数组用来将输入输出差分按照概率大小进行排序。概率共有0,2,4,6,8,10,12,14,16九种情况，没有计数频数为64的情况.
u8 DDT_SearchInOrderX[8][9][256]={0};
u8 DDT_SearchInOrderY[8][9][256]={0};
int DDT_SearchInOrderLength[8][9]={0};

//存放（输入差分，输出差分）对的个数
unsigned int DDT_int[8][64][16]={0};
//将（输入差分，输出差分）对转化为A*2^B的形式。DDT数组存放A的值，1<=A<2;DDT_exponent数组存放B的值，B的值再减6，将频数转化为概率。
double DDT[8][64][16]={0};
int DDT_exponent[8][64][16]={0};
//S盒变换
void Substitution(u8* output,u8 input,int index){
	u8 x=input,x1,x2;
	x1=(x&0x1)|((x>>4)&0x2);
	x2=(x>>1)&0xf;
	*output=S[index][x1][x2];
}
//将（输入差分，输出差分）对转化为A*2^B的形式。DDT数组存放A的值，1<=A<2;DDT_exponent数组存放B的值，B的值再减6，将频数转化为概率。
void set(double frequency,int Si,int i,int j)
{
	frequency=frequency/2;
	DDT_exponent[Si][i][j]++;
	if(frequency<2) DDT[Si][i][j]=frequency;
	else set(frequency,Si,i,j);
}


//生成差分分布表，单独考虑八个S盒，8*（2^6）*（2^4）
void GenDiffDistributionTable(){
	double frequency;
	for(int Si=0;Si<8;Si++){
		for(u8 Input1=0;Input1<64;Input1++){
			for(u8 Input2=0;Input2<64;Input2++){
				u8 Output1,Output2;
				Substitution(&Output1,Input1,Si);
				Substitution(&Output2,Input2,Si);
				DDT_int[Si][Input1^Input2][Output1^Output2]++;                  //得到了差分分布表的值，2,4,6,8,10,12,14,16
			}
		}
		for(int i=0;i<64;i++){
			for(int j=0;j<16;j++){
				frequency=DDT_int[Si][i][j];        
				if(frequency!=0) {
					set(frequency,Si,i,j);
					DDT_exponent[Si][i][j]=DDT_exponent[Si][i][j]-6;        //变成概率，除以2的6次方
				} 
			}
		}
	}
}
//将输入输出差分按照概率大小排序
void GenSearchInOrder(){
	int count,index;
	for(int Si=0;Si<8;Si++){
		for(u8 x=0;x<64;x++){
			for(u8 y=0;y<16;y++){
				count=DDT_int[Si][x][y]/2;
				index=DDT_SearchInOrderLength[Si][count];         //index记录满足频数为2count的差分对的个数，初始值为0。
				DDT_SearchInOrderX[Si][count][index]=x;           //x为频数为2count的第index个（输入差分，输出差分）对的输入差分。
				DDT_SearchInOrderY[Si][count][index]=y;
				DDT_SearchInOrderLength[Si][count]++;
			}
		}
	}
}
