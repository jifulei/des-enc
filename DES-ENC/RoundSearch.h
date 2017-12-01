#pragma once
#include"Types.h"
#include "Tables.h"
#include"GenDiffDistributionTables.h"
#include<math.h>
#include<stdio.h>

double Prob_Sbox[8]={1,1,1,1,1,1,1,1};    //��¼�˸�s�����������ֵĸ���
int Prob_Sbox_exponent[8]={0,0,0,0,0,0,0,0};

double Prob_table[9]={0,1,1,1.5,1,1.25,1.5,1.75,1};        //�Ը��ʽ���ת������Ӧcount-index��������count=8����Ӧ�ĸ��ʾ���1*2^-3
int Prob_table_exponent[9]={0,-5,-4,-4,-3,-3,-3,-3,-2};

double P2=1;                       //��Ӧ����Ϊ	P2*2^ P2_exponent
int P2_exponent=0;

int deltX[8]={0},deltY[8]={0};       //��¼�˸�S�е�����������

FILE *fp4;

void set_pro_trans(double P2)
{
	P2=P2/2;
	P2_exponent++;
	if(P2>=2) set_pro_trans(P2);
}

void round_three()
{
	if((fp4=fopen("fprintf.txt","a+"))==NULL){
		printf("2File open error!\n");
		exit(0);
	}
	fprintf(fp4,"dx: %x,%x,%x,%x,%x,%x,%x,%x\n",deltX[0],deltX[1],deltX[2],deltX[3],deltX[4],deltX[5],deltX[6],deltX[7]);
	fprintf(fp4,"dy: %x,%x,%x,%x,%x,%x,%x,%x\n",deltY[0],deltY[1],deltY[2],deltY[3],deltY[4],deltY[5],deltY[6],deltY[7]);
	fprintf(fp4,"pro:%0.2f(%d),%0.2f(%d),%0.2f(%d),%0.2f(%d),%0.2f(%d),%0.2f(%d),%0.2f(%d),%0.2f(%d)\n",Prob_Sbox[0],Prob_Sbox_exponent[0],Prob_Sbox[1],\
		Prob_Sbox_exponent[1],Prob_Sbox[2],Prob_Sbox_exponent[2],Prob_Sbox[3],Prob_Sbox_exponent[3],\
		Prob_Sbox[4],Prob_Sbox_exponent[4],Prob_Sbox[5],Prob_Sbox_exponent[5],Prob_Sbox[6],Prob_Sbox_exponent[6],Prob_Sbox[7],Prob_Sbox_exponent[7]);
	fprintf(fp4,"=========\n");
	if(fclose(fp4)){
		printf("Can not close the file!\n");
		exit(0);
	}
}

void round_two_j(int a_count){  
	//if((fp4=fopen("fprintf.txt","a+"))==NULL){
		//printf("3File open error!\n");
		//exit(0);
	//}
	//fprintf(fp4,"a_count=%d\n",a_count);

	int aj;
	//�ӵ�aj��s���ѵ��ڰ˸�s��
	for(aj=a_count+1;aj<9;aj++){                                                    
		//fprintf(fp4,"aj=%d\n",aj);    
		for(int i=a_count;i<8;i++){      //��a_count+1����8��s�ж�����Ծ������������Ϊ�㡣
		    deltX[i]=0;
			deltY[i]=0;
		    Prob_Sbox[i]=1;
		    Prob_Sbox_exponent[i]=0;
	    }
		//��������������S��֮��������ִ����໥��Լ
		if((aj-a_count)>1){
			if((deltX[a_count-1]&0x3)!=0) break;
		}
			         /*���ǰһ���ѵ���S4����һ�ֵݹ���S5,S6��S7��S8����ôdeltX[S3]�������λ����Ϊ0���������ѡ�*/
		                                                                
		int Si=aj-1;
		//fprintf(fp4,"si=%d\n",Si);
		if((aj==8)&&((deltX[6]&0x3)==0)) round_three();                              //���ȿ���S8����Ծ�����,
		                                                                             //S7�������λ����Ϊ0,�����S8����Ծ�������ֱ�����S8����Ծʱ��Ľ����         
		for(int count=8;count>0;count--){                           //count��ȡֵΪ1-8
			//fprintf(fp4,"aj=%d\n",aj); 
			//fprintf(fp4,"count=%d\n",count);
			//fprintf(fp4,"si    =%d\n",Si);

			for(int i=aj-1;i<8;i++){      //��a_count+1����8��s�ж�����Ծ������������Ϊ�㡣
		        deltX[i]=0;
			    deltY[i]=0;
		        Prob_Sbox[i]=1;
		        Prob_Sbox_exponent[i]=0;
	        }

			Prob_Sbox[Si]=Prob_table[count];
			Prob_Sbox_exponent[Si]=Prob_table_exponent[count];

			//fprintf(fp4,"Prob_Sbox[Si] =%f\n",Prob_Sbox[Si]);
			//fprintf(fp4,"Prob_Sbox_exponent[Si] =%d\n",Prob_Sbox_exponent[Si]);

			P2=Prob_Sbox[0]*Prob_Sbox[1]*Prob_Sbox[2]*Prob_Sbox[3]*Prob_Sbox[4]*Prob_Sbox[5]*Prob_Sbox[6]*Prob_Sbox[7];
			P2_exponent=Prob_Sbox_exponent[0]+Prob_Sbox_exponent[1]+Prob_Sbox_exponent[2]+Prob_Sbox_exponent[3]+Prob_Sbox_exponent[4]+Prob_Sbox_exponent[5]+Prob_Sbox_exponent[6]+Prob_Sbox_exponent[7];
			if(P2>=2) set_pro_trans(P2);
			if(P2_exponent<-6) break;      //�������countѭ����������һ��aj������             
				                            //8��s��������ֵ�������Ϊ2��-4�η�����16/64 * 16/64��
			for(int index=0;index<256;index++){
				//fprintf(fp4,"index=%d\n",index);
				if(DDT_SearchInOrderX[Si][count][index]==0) break;     //���������꣬������һ��count������

				//��������������S��֮��������ִ����໥��Լ
				int deltx=DDT_SearchInOrderX[Si][count][index];
				if((Si==7)&&((((deltx>>4)&0x3)!=(deltX[Si-1]&0x3))||((deltx&0x3)!=((deltX[0]>>4)&0x3)))) continue;    //S8�����bit����S1ǰ��bit��S8ǰ2bit=S7��2bit���������㣬�������һ��index����
				else if(((deltx>>4)&0x3)!=(deltX[(Si+7)%8]&0x3))continue;                                      //��һ��S��ǰ2bit=ǰһ��S�к�2bit���������㣬�������һ��index����
				
				//fprintf(fp4,"DDT_SearchInOrderX=%d\n",DDT_SearchInOrderX[Si][count][index]);
				deltX[Si]=DDT_SearchInOrderX[Si][count][index];
				deltY[Si]=DDT_SearchInOrderY[Si][count][index];
				

				//���ֱ�����s�в��ǵڰ˸����ݹ������һ�� round_two_j
				if(aj<8)round_two_j(aj);
				//���ֱ�����s���ǵڰ˸����Ҹ��ʳ˻��㹻�󣬽�������ֵ�����
				//����û�а������ڰ˸�S�в���Ծ�����
				else round_three();
			}
		}
	}
}