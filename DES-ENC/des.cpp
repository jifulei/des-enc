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


//明文和密钥写在只读文件input.txt中，程序结束后将明文密钥密文写入只写文件output.txt中
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
	//读取明文和密钥
	fscanf(fp1,"%s%s",user_supplied_plaintext,user_supplied_key);
	if(fclose(fp1)){
		printf("Can not close the file!\n");
		exit(0);
	}

	//初始化明文
	int Plain[64],Cipher_binary[64];
	//存储16进制明文数组
	char a[16];
	char *p;              
	p=&user_supplied_plaintext[0];
	int before_IP[64];
    for(int count=0;count<16;count++){
		a[count]=*p;
		//将16进制数组转化位二进制数组
		if(a[count]>96) a[count]=a[count]-87;
		before_IP[4*count]=(a[count]%16)/8;
	    before_IP[4*count+1]=(a[count]%8)/4;
	    before_IP[4*count+2]=(a[count]%4)/2;
	    before_IP[4*count+3]=a[count]%2;
        p++;
	}

	//明文通过IP置换
	for(int count=0;count<64;count++) Plain[count]=before_IP[initial_message_permutation[count]-1];
	
	//初始化密钥，经过PC1变换
	key_schedule_initial(user_supplied_key);
	//生成第1轮的轮密钥
	key_schedule(Key,0);
	//轮变换，从1开始计数，1-16轮,第16轮左右不交换
	for(int round_number=1;round_number<17;round_number++) round_function(Plain,round_key,round_number);
	
	//经过最后的IP逆置换,生成的是二进制数组
	for(int count=0;count<64;count++) Cipher_binary[count]=Plain[final_message_permutation[count]-1];

	//将二进制数组转化为16进制密文数组
	int Cipher[16];
	for(int count=0;count<16;count++) Cipher[count]=8*Cipher_binary[4*count+0]+4*Cipher_binary[4*count+1]+2*Cipher_binary[4*count+2]+Cipher_binary[4*count+3];
	
	FILE *fp2;
	if((fp2=fopen("output.txt","wb"))==NULL){
		printf("File open error!\n");
		exit(0);
	}
	//写入数据，明文，密钥，密文
	fprintf(fp2,"明文%s  密钥%s  密文",user_supplied_plaintext,user_supplied_key);
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
	printf("程序运行时间为：%f\n",(double)(end-start)/CLK_TCK);
	return 0;
}