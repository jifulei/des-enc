#pragma once
#include"Types.h"
#include "Tables.h"
#include<math.h>
#include<stdio.h>


//double DDT_MaxOutput[8][64]={0};
//unsigned int DDT_MaxOutput_Index[8][64]={0};


//存放（输入差分，输出差分）对的个数
unsigned int DDT_int[8][64][16]={0};
//存放（输入差分，输出差分）对成立频数的以2为底的真数
double DDT[8][64][16]={0};

//S盒变换
void Substitution(u8* output,u8 input,int index){
	u8 x=input,x1,x2;
	x1=(x&0x1)|((x>>4)&0x2);
	x2=(x>>1)&0xf;
	*output=S[index][x1][x2];
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
				DDT_int[Si][Input1^Input2][Output1^Output2]++;
			}
		}
		
		for(int i=0;i<64;i++){
			for(int j=0;j<16;j++){
				frequency=DDT_int[Si][i][j];        
				if(frequency!=0) DDT[Si][i][j]=log(frequency)/log(2.0);       //频数为0的时候输出有问题   //频数转化为2的对数，结果出现小数，会不会影响精度？
			}
		}

	}
	
	/*for(int i=0;i<10;i++){
		printf("i=%d\n",i);
		for(int j=0;j<16;j++){
			printf("%d %f\n",DDT_int[0][i][j],DDT[0][i][j]);	
		}
		printf("\n");
	}*/
}

//以下三个数组用来将输入输出差分按照概率大小进行排序。概率共有0,2,4,6,8,10,12,14,16九种情况，没有计数频数为64的情况.
u8 DDT_SearchInOrderX[8][9][256]={0};
u8 DDT_SearchInOrderY[8][9][256]={0};
int DDT_SearchInOrderLength[8][9]={0};

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

//遍历8个s盒的输入差分，它们之间存在相互制约
double Prob[8]={64,64,64,64,64,64,64,64};    //记录的是频数的以二为底的真数值,初始值设为最大值6
double P2=0;
int deltX[8]={0},deltY[8]={0};
void round_three(){
	printf("(%f,%f,%f,%f,%f,%f,%f,%f)\n",Prob[0],Prob[1],Prob[2],Prob[3],Prob[4],Prob[5],Prob[6],Prob[7]);
}
void round_two_j(int a_count){  
	int aj=(a_count)+1;
	//从第aj个s盒搜到第八个s盒
	for(aj;aj<9;aj++){                                                    
		printf("aj=%d\n",aj);                                                                  //限制条件，相邻S盒之间的输入差分存在相互制约
		if(((aj-a_count)>1)&&(((deltX[a_count-1])&0x3)!=0))return;         /*如果前一轮搜的是S4，下一轮递归搜S6，S7，S8，那么deltX[S3]的最后两位必须为0，否则不能搜。
		                                                                  这个地方犹豫了一下应该用break还是continue还是return
		                                                                 举例说明，如果S2活跃，搜S3活跃的情况失败了，要搜S4活跃的情况，deltX[1]的后两位不为零，
		                                                                 那必须返回上一层递归，找一个deltX[1]的后两位等于零的值，所以应该是return */
		for(int i=(a_count)+1;i<aj;i++){      //第a_count+1到第aj-1个s盒都不活跃，输入输出差分为零。
			Prob[i-1]=64;
			deltX[i-1]=deltY[i-1]=0;
		}

		int Si=aj-1;
		if((aj==8)&&((deltX[6]&0x3)==0)){                             //首先考虑S8不活跃的情况,S7的最后两位必须为0
			double temp=Prob[7];
			Prob[7]=64;
			deltX[Si]=0;
			deltY[Si]=0;
			round_three();
			Prob[7]=temp;                  //如果round_three搜不到结果，返回，继续搜S8活跃的情况	
		}
		for(int count=8;count>0;count--){                           //count的取值为0-8
			printf("count=%d\n",count);
			//判断概率是否满足剪枝条件
			double temp=Prob[Si];
			Prob[Si]=2*count;
			P2=Prob[0]+Prob[1]+Prob[2]+Prob[3]+Prob[4]   +Prob[5]+Prob[6]+Prob[7];
			if(P2<464) {
				Prob[Si]=temp;
				break;                            //跳出这个count循环，进行下一个aj的搜索
			}

			for(int index=0;index<256;index++){
				printf("index=%d,DDT_SearchInOrderX=%d\n",index,DDT_SearchInOrderX[Si][count][index]);
				if(DDT_SearchInOrderX[Si][count][index]==0) break;     //本行搜索完，进入下一个count的搜索
				//限制条件，相邻S盒之间的输入差分存在相互制约
				int deltx=DDT_SearchInOrderX[Si][count][index];
				if((Si==7)&&((((deltx>>4)&0x3)!=(deltX[Si-1]&0x3))||((deltx&0x3)!=((deltX[0]>>4)&0x3)))) continue;    //S8最后两bit等于S1前两bit，S8前2bit=S7后2bit，若不满足，则进入下一个index搜索
				else if(((deltx>>4)&0x3)!=(deltX[(Si+7)%8]&0x3))continue;                                      //后一个S盒前2bit=前一个S盒后2bit，若不满足，则进入下一个index搜索

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