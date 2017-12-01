#pragma once
#include"Types.h"
#include "Tables.h"
#include"GenDiffDistributionTables.h"
#include<math.h>
#include<stdio.h>

double Prob_Sbox[8]={1,1,1,1,1,1,1,1};    //记录八个s盒输入输出差分的概率
int Prob_Sbox_exponent[8]={0,0,0,0,0,0,0,0};

double Prob_table[9]={0,1,1,1.5,1,1.25,1.5,1.75,1};        //对概率进行转化，对应count-index矩阵，例如count=8，对应的概率就是1*2^-3
int Prob_table_exponent[9]={0,-5,-4,-4,-3,-3,-3,-3,-2};

double P2=1;                       //对应概率为	P2*2^ P2_exponent
int P2_exponent=0;

int deltX[8]={0},deltY[8]={0};       //记录八个S盒的输入输出差分

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
	//从第aj个s盒搜到第八个s盒
	for(aj=a_count+1;aj<9;aj++){                                                    
		//fprintf(fp4,"aj=%d\n",aj);    
		for(int i=a_count;i<8;i++){      //第a_count+1到第8个s盒都不活跃，输入输出差分为零。
		    deltX[i]=0;
			deltY[i]=0;
		    Prob_Sbox[i]=1;
		    Prob_Sbox_exponent[i]=0;
	    }
		//限制条件，相邻S盒之间的输入差分存在相互制约
		if((aj-a_count)>1){
			if((deltX[a_count-1]&0x3)!=0) break;
		}
			         /*如果前一轮搜的是S4，下一轮递归搜S5,S6，S7，S8，那么deltX[S3]的最后两位必须为0，否则不能搜。*/
		                                                                
		int Si=aj-1;
		//fprintf(fp4,"si=%d\n",Si);
		if((aj==8)&&((deltX[6]&0x3)==0)) round_three();                              //首先考虑S8不活跃的情况,
		                                                                             //S7的最后两位必须为0,则存在S8不活跃的情况，直接输出S8不活跃时候的结果。         
		for(int count=8;count>0;count--){                           //count的取值为1-8
			//fprintf(fp4,"aj=%d\n",aj); 
			//fprintf(fp4,"count=%d\n",count);
			//fprintf(fp4,"si    =%d\n",Si);

			for(int i=aj-1;i<8;i++){      //第a_count+1到第8个s盒都不活跃，输入输出差分为零。
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
			if(P2_exponent<-6) break;      //跳出这个count循环，进行下一个aj的搜索             
				                            //8个s盒允许出现的最大概率为2的-4次方。（16/64 * 16/64）
			for(int index=0;index<256;index++){
				//fprintf(fp4,"index=%d\n",index);
				if(DDT_SearchInOrderX[Si][count][index]==0) break;     //本行搜索完，进入下一个count的搜索

				//限制条件，相邻S盒之间的输入差分存在相互制约
				int deltx=DDT_SearchInOrderX[Si][count][index];
				if((Si==7)&&((((deltx>>4)&0x3)!=(deltX[Si-1]&0x3))||((deltx&0x3)!=((deltX[0]>>4)&0x3)))) continue;    //S8最后两bit等于S1前两bit，S8前2bit=S7后2bit，若不满足，则进入下一个index搜索
				else if(((deltx>>4)&0x3)!=(deltX[(Si+7)%8]&0x3))continue;                                      //后一个S盒前2bit=前一个S盒后2bit，若不满足，则进入下一个index搜索
				
				//fprintf(fp4,"DDT_SearchInOrderX=%d\n",DDT_SearchInOrderX[Si][count][index]);
				deltX[Si]=DDT_SearchInOrderX[Si][count][index];
				deltY[Si]=DDT_SearchInOrderY[Si][count][index];
				

				//本轮遍历的s盒不是第八个，递归进入下一个 round_two_j
				if(aj<8)round_two_j(aj);
				//本轮遍历的s盒是第八个，且概率乘积足够大，进入第三轮的搜索
				//但是没有包含进第八个S盒不活跃的情况
				else round_three();
			}
		}
	}
}