#include<stdio.h>
#include "tables.h"
#include<iostream>
#include<string>
using namespace std;


//初始密钥经过PC1变换后的值。
int Key[56];
//round key的值，每轮更新。
int round_key[48];

//完成密钥编排的第一步，PC1变换。
void key_schedule_initial(string user_supplied_key)
{  
	char *p;              
	p=&user_supplied_key[0];
	char a[16];
	int temp[64];
    for(int count=0;count<16;count++){
		a[count]=*p;

		//将16进制数组转化位二进制数组
		if(a[count]>96) a[count]=a[count]-87;
		temp[4*count]=(a[count]%16)/8;
	    temp[4*count+1]=(a[count]%8)/4;
	    temp[4*count+2]=(a[count]%4)/2;
	    temp[4*count+3]=a[count]%2;

        p++;
	}
	
	for(int count=0;count<56;count++){
		Key[count]=temp[PC_1[count]-1];
	}
}
//进行每一轮的左循环移位和PC2变换，生成round key.
void key_schedule(int Key[],int count)
{
	//printf("count=%d\n",count);
	
	//循环左移操作
	if(key_shift_sizes[count]==1){
		int a=Key[0];
		for(int i=0;i<27;i++){
			Key[i]=Key[i+1];
		}
		Key[27]=a;
		int b=Key[28];
		for(int i=28;i<55;i++){
			Key[i]=Key[i+1];
		}
		Key[55]=b;
	}
	
	else{
		int a=Key[0],b=Key[1];
		for(int i=0;i<26;i++){
			Key[i]=Key[i+2];
		}
		Key[26]=a;
		Key[27]=b;
		int c=Key[28],d=Key[29];
		for(int i=28;i<54;i++){
			Key[i]=Key[i+2];
		}
		Key[54]=c;
		Key[55]=d;
	}
	
	//PC2置换
	for(int i=0;i<48;i++){
		round_key[i]=Key[PC_2[i]-1];
	}
	
}
//轮函数变换
void round_function(int Plain[],int round_key[],int round_number){
	
	//创建临时数组存储P_L_i+1的值
	int Plain_Left_nextround[32];
	for(int count=0;count<32;count++){
		Plain_Left_nextround[count]=Plain[count+32];
	}

	//经过E扩张和密钥加操作
	int Plain_right_after_EK[48];     //创建中间数组
	for(int count=0;count<48;count++){
		Plain_right_after_EK[count]=Plain[E_expansion[count]+31]^round_key[count];
	}

	//经过S盒操作
	int Plain_right_after_Sboxes[32];     //创建中间数组
	for(int count=0;count<8;count++){
		int S_box[64];
		switch(count){
		case 0:
			for(int i=0;i<64;i++) S_box[i]=S1[i];
			break;
        case 1:
			for(int i=0;i<64;i++) S_box[i]=S2[i];
			break;
        case 2:
			for(int i=0;i<64;i++) S_box[i]=S3[i];
			break;
        case 3:
			for(int i=0;i<64;i++) S_box[i]=S4[i];
			break;
		case 4:
			for(int i=0;i<64;i++) S_box[i]=S5[i];
			break;
		case 5:
			for(int i=0;i<64;i++) S_box[i]=S6[i];
			break;
		case 6:
			for(int i=0;i<64;i++) S_box[i]=S7[i];
			break;
		case 7:
			for(int i=0;i<64;i++) S_box[i]=S8[i];
			break;
		}
		int temp_after_Sboxes=S_box[16*(2*Plain_right_after_EK[6*count+0]+Plain_right_after_EK[6*count+5])+(8*Plain_right_after_EK[6*count+1]+4*Plain_right_after_EK[6*count+2]\
		      +2*Plain_right_after_EK[6*count+3]+Plain_right_after_EK[6*count+4])];
	    Plain_right_after_Sboxes[4*count+0]=(temp_after_Sboxes%16)/8;
	    Plain_right_after_Sboxes[4*count+1]=(temp_after_Sboxes%8)/4;
	    Plain_right_after_Sboxes[4*count+2]=(temp_after_Sboxes%4)/2;
	    Plain_right_after_Sboxes[4*count+3]=temp_after_Sboxes%2;
	   
	}
	if(round_number<16){
		//经过P置换，异或P_L_i,得到P_R_i+1
		for(int count=0;count<32;count++) Plain[count+32]=Plain[count]^Plain_right_after_Sboxes[P_permutation[count]-1];
		//更新P_L_i+1的值
	    for(int count=0;count<32;count++) Plain[count]=Plain_Left_nextround[count];
		//生成下一轮的轮密钥
	    key_schedule(Key,round_number);
	}
	else{
		//第16轮的变换
		//经过P置换，异或P_L_i,得到P_L_16
		for(int count=0;count<32;count++) Plain[count]=Plain[count]^Plain_right_after_Sboxes[P_permutation[count]-1];
		//更新P_L_i+1的值
	    for(int count=0;count<32;count++) Plain[count+32]=Plain_Left_nextround[count];
	}
}