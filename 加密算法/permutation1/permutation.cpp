#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Permutation_en(char *plaintext,char *key);
void Permutation_de(char *ciphertext,char *key);

int main()
{

    char key[100];
    char plaintext[100];
    char ciphertext[100];
    int choose;
    printf("permutation 加密算法！\n");
    printf("请选择：1、加密 2、解密\n");
    scanf("%d",&choose);
    if(choose==1)
    {
        printf("请输入密钥：");
        scanf("%s",key);
        printf("请输入明文：");
        scanf("%s",plaintext);
        Permutation_en(plaintext,key);
    }
    else if(choose==2)
    {
        printf("请输入密文：");
        scanf("%s",ciphertext);
        printf("请输入密钥：");
        scanf("%s",key);
        Permutation_de(ciphertext,key);
    }
    else
        printf("请重新输入选择！\n");

    return 0;
}

void Permutation_en(char *plaintext,char *key)
{

    int length,len,i,j,n;
    int num[10];
    char plaintextfinish[1000];
    char table[100][100];
    char table2[100][100];
    length=strlen(key);
    for(i=0;i<length;i++)
    {
        num[i]=1;   //把num[]赋值为1
        for(j=0;j<length;j++)
        {
            if(key[j]<key[i])
            {
                num[i]++;
            }//把key[i]和key的所有字符比较，有比key[i]小的num[i]就+1
        }
        for(j=0;j<i;j++)
        {	if(key[j] == key[i])
                num[i]=num[i]+1;
        }	//有相同字符，就把后面字符位置的k[i]+1
    }
    int length_pl=strlen(plaintext);//明文长度
    int re=length_pl%length;        //需要补上的长度
	int row;
	if(re!=0)
		row=((length_pl-re)/length+1);//行数
	else
		row=length_pl/length;//行数
    //复制操作
    for(i=0;i<length_pl;i++)
    {
        plaintextfinish[i]=plaintext[i];
    }
    //若明文长度不是密钥长度的整数倍则用无效字符补齐
    if(re!=0)
    {
        for(i=0;i<(length-re);i++)
        {
            plaintextfinish[length_pl]='q';//用无效字符补全明文
            length_pl++;//明文长度+1
        }
    }
    //转化为二维数组
    for(i=0;i<row;i++)
    {
        for(j=0;j<length;j++)
        {
            len=i*length+j;
            table[i][j]=plaintextfinish[len];
        }
    }
    for(i=0;i<=length;i++)
    {
        for(j=0;j<length;j++)
        {
            if(num[j]==i+1)
            {
                for(n=0;n<len;n++)
                {
                    table2[n][i]=table[n][j];
                }
            }
        }
    }
    printf("密文为:\n");
	for(i=0;i<row;i++)
	{
		for(j=0;j<length;j++)
		{
			printf("%c",table2[i][j]);
		}
	}
	printf("\n");
}
void Permutation_de(char *ciphertext,char *key)
{
    int i,j;
    int num[10];
    char table[100][100];
    char table2[100][100];
    int length=0;
    int length_ci;
    length=strlen(key);
    for(i=0;i<length;i++)
    {
        num[i]=1;   //把num[]赋值为1
        for(j=0;j<length;j++)
        {
            if(key[j]<key[i])
            {
                num[i]++;
            }//把key[i]和key的所有字符比较，有比key[i]小的num[i]就+1
        }
        for(j=0;j<i;j++)
        {	if(key[j] == key[i])
                num[i]=num[i]+1;
        }
    }
    length_ci=strlen(ciphertext);//密文长度
    int row=length_ci/length;  //行数
    for(i=0;i<length;i++)
    {
        for(j=0;j<row;j++)
        {
            table[j][i]=ciphertext[j*length+i];
        }
    }
    for(i=0;i<row;i++)
    {
        for(j=0;j<length;j++)
        {
            table2[i][j]=table[i][num[j]-1];
        }
    }
    printf("明文为:\n");
    for(i=0;i<row;i++)
    {
        for(j=0;j<length;j++)
        {
                printf("%c",table2[i][j]);
        }
    }
}




