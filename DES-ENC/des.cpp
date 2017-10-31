#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
using namespace std;
#include "Types.h"
#include "GenDiffDistributionTables.h"
#include "RoundFunction.h"
#include<time.h>



#include "Tables.h"


//���ĺ���Կд��ֻ���ļ�input.txt�У����������������Կ����д��ֻд�ļ�output.txt��
//user_supplied_plaintext ="4e6f772069732074";
//user_supplied_key ="0123456789abcdef";
int main()
{
	clock_t start,end;
	start = clock(); 
	char user_supplied_plaintext[80],user_supplied_key[80];

	FILE *fp1;
	if((fp1=fopen("input.txt","r"))==NULL){
		printf("File open error!\n");
		exit(0);
	}
	//��ȡ���ĺ���Կ
	fscanf(fp1,"%s%s",user_supplied_plaintext,user_supplied_key);
	if(fclose(fp1)){
		printf("Can not close the file!\n");
		exit(0);
	}

	//��ʼ������
	int Plain[64],Cipher_binary[64];
	//�洢16������������
	char a[16];
	char *p;              
	p=&user_supplied_plaintext[0];
	int before_IP[64];
    for(int count=0;count<16;count++){
		a[count]=*p;
		//��16��������ת��λ����������
		if(a[count]>96) a[count]=a[count]-87;
		before_IP[4*count]=(a[count]%16)/8;
	    before_IP[4*count+1]=(a[count]%8)/4;
	    before_IP[4*count+2]=(a[count]%4)/2;
	    before_IP[4*count+3]=a[count]%2;
        p++;
	}

	//����ͨ��IP�û�
	for(int count=0;count<64;count++) Plain[count]=before_IP[initial_message_permutation[count]-1];
	
	//��ʼ����Կ������PC1�任
	key_schedule_initial(user_supplied_key);
	//���ɵ�1�ֵ�����Կ
	key_schedule(Key,0);
	//�ֱ任����1��ʼ������1-16��,��16�����Ҳ�����
	for(int round_number=1;round_number<17;round_number++) round_function(Plain,round_key,round_number);
	
	//��������IP���û�,���ɵ��Ƕ���������
	for(int count=0;count<64;count++) Cipher_binary[count]=Plain[final_message_permutation[count]-1];

	//������������ת��Ϊ16������������
	int Cipher[16];
	for(int count=0;count<16;count++) Cipher[count]=8*Cipher_binary[4*count+0]+4*Cipher_binary[4*count+1]+2*Cipher_binary[4*count+2]+Cipher_binary[4*count+3];
	
	FILE *fp2;
	if((fp2=fopen("output.txt","wb"))==NULL){
		printf("File open error!\n");
		exit(0);
	}
	//д�����ݣ����ģ���Կ������
	fprintf(fp2,"����%s  ��Կ%s  ����",user_supplied_plaintext,user_supplied_key);
	for(int count=0;count<16;count++) fprintf(fp2,"%x",Cipher[count]);
	if(fclose(fp2)){
		printf("Can not close the file!\n");
		exit(0);
	}
	GenDiffDistributionTable();
	GenSearchInOrder();
	//printf("%x\n",DDT_SearchInOrderX[0][8][0]);  
	//round_two_traverse();
	int a0=0;
	round_two_j(a0);
	//int test=(1<<4)|1;
	//printf("%d",test);
	end = clock();
	printf("��������ʱ��Ϊ��%f\n",(double)(end-start)/CLK_TCK);
	return 0;
}