#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void RC4_swap(char *a,char *b)//交换函数
{
	char temp;
	temp=*a;
	*a=*b;
	*b=temp;
}
void RC4_Initial_SBox(char *S,int n)//初始化S BOX
{
	int RC4_i;
	for(RC4_i=0;RC4_i<n;RC4_i++)
		S[RC4_i]=RC4_i;
}
void RC4_Initial_TBox(char *T,char *key)
{
	int RC4_i;
	int keylen;
	keylen=strlen(key);
	for(RC4_i=0;RC4_i<256;RC4_i++)
		T[RC4_i]=key[RC4_i%keylen];
}
void RC4_Change_SBox(char *S,char *T)
{
	int RC4_i;
	int RC4_j=0;
	for(RC4_i=0;RC4_i<256;RC4_i++)
		RC4_j=(RC4_j+S[RC4_i]+T[RC4_i])%256;
	RC4_swap(&S[RC4_i],&T[RC4_j]);
}
void RC4_Encryption(char p[],char *key)
{
	char S[260]={'0'};
	char T[260]={'0'};
	RC4_Initial_SBox(S,256);
	RC4_Initial_TBox(T,key);
	RC4_Change_SBox(S,T);
	int i=0,j=0;
	int t,l=strlen(p),z=0;
	while(z!=l)
	{
	   i = (i + 1) % 256;
	   j = (j + S[i]) % 256;
	   RC4_swap(&S[i],&S[j]);
	   t = (S[i] + (S[j] % 256)) % 256;
	   p[z]=p[z]^S[t];
	   putchar(p[z]);
	   memset(p,0,1);
	   z++;
	}
}
int main()
{
	char key[]="123456",p[100];
	gets(p);
	RC4_Encryption(p,key);
	return 0;
}