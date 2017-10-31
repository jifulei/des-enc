#pragma once
#include"Types.h"
#include "Tables.h"
#include<math.h>
#include<stdio.h>


//double DDT_MaxOutput[8][64]={0};
//unsigned int DDT_MaxOutput_Index[8][64]={0};


//��ţ������֣������֣��Եĸ���
unsigned int DDT_int[8][64][16]={0};
//��ţ������֣������֣��Գ���Ƶ������2Ϊ�׵�����
double DDT[8][64][16]={0};

//S�б任
void Substitution(u8* output,u8 input,int index){
	u8 x=input,x1,x2;
	x1=(x&0x1)|((x>>4)&0x2);
	x2=(x>>1)&0xf;
	*output=S[index][x1][x2];
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
				DDT_int[Si][Input1^Input2][Output1^Output2]++;
			}
		}
		
		for(int i=0;i<64;i++){
			for(int j=0;j<16;j++){
				frequency=DDT_int[Si][i][j];        
				if(frequency!=0) DDT[Si][i][j]=log(frequency)/log(2.0);       //Ƶ��Ϊ0��ʱ�����������   //Ƶ��ת��Ϊ2�Ķ������������С�����᲻��Ӱ�쾫�ȣ�
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

//���������������������������ְ��ո��ʴ�С�������򡣸��ʹ���0,2,4,6,8,10,12,14,16���������û�м���Ƶ��Ϊ64�����.
u8 DDT_SearchInOrderX[8][9][256]={0};
u8 DDT_SearchInOrderY[8][9][256]={0};
int DDT_SearchInOrderLength[8][9]={0};

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

//����8��s�е������֣�����֮������໥��Լ
double Prob[8]={64,64,64,64,64,64,64,64};    //��¼����Ƶ�����Զ�Ϊ�׵�����ֵ,��ʼֵ��Ϊ���ֵ6
double P2=0;
int deltX[8]={0},deltY[8]={0};
void round_three(){
	printf("(%f,%f,%f,%f,%f,%f,%f,%f)\n",Prob[0],Prob[1],Prob[2],Prob[3],Prob[4],Prob[5],Prob[6],Prob[7]);
}
void round_two_j(int a_count){  
	int aj=(a_count)+1;
	//�ӵ�aj��s���ѵ��ڰ˸�s��
	for(aj;aj<9;aj++){                                                    
		printf("aj=%d\n",aj);                                                                  //��������������S��֮��������ִ����໥��Լ
		if(((aj-a_count)>1)&&(((deltX[a_count-1])&0x3)!=0))return;         /*���ǰһ���ѵ���S4����һ�ֵݹ���S6��S7��S8����ôdeltX[S3]�������λ����Ϊ0���������ѡ�
		                                                                  ����ط���ԥ��һ��Ӧ����break����continue����return
		                                                                 ����˵�������S2��Ծ����S3��Ծ�����ʧ���ˣ�Ҫ��S4��Ծ�������deltX[1]�ĺ���λ��Ϊ�㣬
		                                                                 �Ǳ��뷵����һ��ݹ飬��һ��deltX[1]�ĺ���λ�������ֵ������Ӧ����return */
		for(int i=(a_count)+1;i<aj;i++){      //��a_count+1����aj-1��s�ж�����Ծ������������Ϊ�㡣
			Prob[i-1]=64;
			deltX[i-1]=deltY[i-1]=0;
		}

		int Si=aj-1;
		if((aj==8)&&((deltX[6]&0x3)==0)){                             //���ȿ���S8����Ծ�����,S7�������λ����Ϊ0
			double temp=Prob[7];
			Prob[7]=64;
			deltX[Si]=0;
			deltY[Si]=0;
			round_three();
			Prob[7]=temp;                  //���round_three�Ѳ�����������أ�������S8��Ծ�����	
		}
		for(int count=8;count>0;count--){                           //count��ȡֵΪ0-8
			printf("count=%d\n",count);
			//�жϸ����Ƿ������֦����
			double temp=Prob[Si];
			Prob[Si]=2*count;
			P2=Prob[0]+Prob[1]+Prob[2]+Prob[3]+Prob[4]   +Prob[5]+Prob[6]+Prob[7];
			if(P2<464) {
				Prob[Si]=temp;
				break;                            //�������countѭ����������һ��aj������
			}

			for(int index=0;index<256;index++){
				printf("index=%d,DDT_SearchInOrderX=%d\n",index,DDT_SearchInOrderX[Si][count][index]);
				if(DDT_SearchInOrderX[Si][count][index]==0) break;     //���������꣬������һ��count������
				//��������������S��֮��������ִ����໥��Լ
				int deltx=DDT_SearchInOrderX[Si][count][index];
				if((Si==7)&&((((deltx>>4)&0x3)!=(deltX[Si-1]&0x3))||((deltx&0x3)!=((deltX[0]>>4)&0x3)))) continue;    //S8�����bit����S1ǰ��bit��S8ǰ2bit=S7��2bit���������㣬�������һ��index����
				else if(((deltx>>4)&0x3)!=(deltX[(Si+7)%8]&0x3))continue;                                      //��һ��S��ǰ2bit=ǰһ��S�к�2bit���������㣬�������һ��index����

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