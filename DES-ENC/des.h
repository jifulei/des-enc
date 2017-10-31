#include<stdio.h>
#include "tables.h"
#include<iostream>
#include<string>
using namespace std;


//��ʼ��Կ����PC1�任���ֵ��
int Key[56];
//round key��ֵ��ÿ�ָ��¡�
int round_key[48];

//�����Կ���ŵĵ�һ����PC1�任��
void key_schedule_initial(string user_supplied_key)
{  
	char *p;              
	p=&user_supplied_key[0];
	char a[16];
	int temp[64];
    for(int count=0;count<16;count++){
		a[count]=*p;

		//��16��������ת��λ����������
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
//����ÿһ�ֵ���ѭ����λ��PC2�任������round key.
void key_schedule(int Key[],int count)
{
	//printf("count=%d\n",count);
	
	//ѭ�����Ʋ���
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
	
	//PC2�û�
	for(int i=0;i<48;i++){
		round_key[i]=Key[PC_2[i]-1];
	}
	
}
//�ֺ����任
void round_function(int Plain[],int round_key[],int round_number){
	
	//������ʱ����洢P_L_i+1��ֵ
	int Plain_Left_nextround[32];
	for(int count=0;count<32;count++){
		Plain_Left_nextround[count]=Plain[count+32];
	}

	//����E���ź���Կ�Ӳ���
	int Plain_right_after_EK[48];     //�����м�����
	for(int count=0;count<48;count++){
		Plain_right_after_EK[count]=Plain[E_expansion[count]+31]^round_key[count];
	}

	//����S�в���
	int Plain_right_after_Sboxes[32];     //�����м�����
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
		//����P�û������P_L_i,�õ�P_R_i+1
		for(int count=0;count<32;count++) Plain[count+32]=Plain[count]^Plain_right_after_Sboxes[P_permutation[count]-1];
		//����P_L_i+1��ֵ
	    for(int count=0;count<32;count++) Plain[count]=Plain_Left_nextround[count];
		//������һ�ֵ�����Կ
	    key_schedule(Key,round_number);
	}
	else{
		//��16�ֵı任
		//����P�û������P_L_i,�õ�P_L_16
		for(int count=0;count<32;count++) Plain[count]=Plain[count]^Plain_right_after_Sboxes[P_permutation[count]-1];
		//����P_L_i+1��ֵ
	    for(int count=0;count<32;count++) Plain[count+32]=Plain_Left_nextround[count];
	}
}