#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* 位置结构，记录字母在Key matrix中的位置 */
#define NUM_OF_LETTERS 26
#define MAX_BUFF 1024
#define ch2id(ch) ((ch) - 'A') /* id是字母在A..Z中的位置(字母A为0) */
#define id2ch(id) ((id) + 'A') /* id对应的字母 */

typedef struct tag_Position
{
  int x;
  int y;
} Position;
typedef char CharPair[2];

typedef struct tag_KeyMatrix
{
  Position auxMap[NUM_OF_LETTERS]; /* 字母到位置的辅助映射表 */
  char letters[5][5];
} KeyMatrix;
void getKeyMatrix(char* key, KeyMatrix* m);
void setPair(char ch1, char ch2, CharPair pair);
/* 从字符串p中获取1对字母(不包括字母Q，重复或单一个字母时，填入X)
 * 返回获得字母后的位置，返回NULL则表示全部获得 */
char* getPairFromString(char* p, CharPair pair);
/* 转换字母对，d=1表示从明文转成密码，d=-1表示出密码转成明文 */
void getTransformPair(KeyMatrix* m, CharPair in, CharPair out, int d);
void encrypt(KeyMatrix* m, char* in, char* out, int len);
void decrypt(KeyMatrix* m, char* in, char* out, int len);
char *Playfair(char *plaintext,char *key,int count);


int main()
{
    int choose;
    char plaintext[100];
    char ciphertext[100];
    char key[100];
    printf("请选择：1、加密 2、解密\n");
    scanf("%d",&choose);
    if(choose==1)
    {
        printf("请输入密钥：");
        scanf("%s",key);
        printf("请输入要加密的明文：");
        scanf("%s",plaintext);
        Playfair(plaintext,key,1);
    }

    else if(choose==2)
    {
        printf("请输入密钥：");
        scanf("%s",key);
        printf("请输入要解密的密文：");
        scanf("%s",ciphertext);
        Playfair(ciphertext,key,2);
    }
    else
        printf("请重新选择！");

}

/* 字母letter对应的位置 */
Position* positionOfLetter(KeyMatrix* m, char letter)
{
  return &m->auxMap[ch2id(letter)];
}
/* 记录letter对应的位置 */
void setPosition(KeyMatrix* m, char letter, int x, int y)
{
  Position* p = positionOfLetter(m, letter);
  m->letters[y][x] = letter;
  p->x = x;
  p->y = y;
}
/* 根据key生成Key matrix */
void getKeyMatrix(char* key, KeyMatrix* m)
{
  int usedChars[NUM_OF_LETTERS]; /* 记录已经使用过的字母 */
  int i = 0, k,j;
  char upch;
  memset(usedChars, 0, sizeof(usedChars));
  /* 将key中的除Q外的字母不重复填入Key matrix */
  for (j=0;key[j]!='\0';j++)
  {
    upch = toupper(key[j]);
    if(isalpha(key[j]) && upch != 'J' && !usedChars[upch - 'A'])
    {
      usedChars[ch2id(upch)] = 1;
      setPosition(m, upch, i%5, i/5);
      i++;
    }
  }
  /* 将没有使用的字母顺序填入剩下的位置 */
  for(k=0; k<NUM_OF_LETTERS; k++)
  {
    if (usedChars[k] == 0 && id2ch(k) != 'J')
    {
      setPosition(m, id2ch(k), i%5, i/5);
      i++;
    }
  }
}
/* 取得Key matrix中x列y行的字母 */
char getLetter(KeyMatrix* m, int x, int y)
{
  return m->letters[y%5][x%5];
}
/* 字母对 */

/* 加密 */
void encrypt(KeyMatrix* m, char* in, char* out, int len)
{
  char* p = in;
  int i = 0;
  CharPair pair={0, 0}, replacedPair;

  while ( (p = getPairFromString(p, pair)) != 0 )
  {
    if (i+2 > len)
      break;
    getTransformPair(m, pair, replacedPair, 1);
    out[i]   = replacedPair[0];
    out[i+1] = replacedPair[1];
    i+=2;
  }
  out[i] = '\0';
  for(i=0;i<strlen(out);i++)
  {
      printf("%c",out[i]);
  }
}
/* 解密 */
void decrypt(KeyMatrix* m, char* in, char* out, int len)
{
  int i = 0;
  CharPair pair, replacedPair;

  while ( in[i] != 0 )
  {
    if (i+2 > len)
      break;
    pair[0] = in[i];
    pair[1] = in[i+1];

    pair[0]=toupper(pair[0]);
    pair[1]=toupper(pair[1]);

    getTransformPair(m, pair, replacedPair, -1);
    out[i]   = replacedPair[0];
    out[i+1] = replacedPair[1];
    i+=2;
  }
  out[i] = '\0';
  for(i=0;i<strlen(out);i++)
  {
      printf("%c",out[i]);
  }
}
void setPair(char ch1, char ch2, CharPair pair)
{
  pair[0] = ch1;
  pair[1] = ch2;
}
/* 跳过不加密的非字母字符和字母Q */
char* skipNocrypt(char* p)
{

  while (*p != '\0' && (!isalpha(*p) || *p == 'J' || *p == 'j'))
    *p='I';
  return p;
}
char* getPairFromString(char* p, CharPair pair)
{
  char upch1, upch2;
  if (*p == '\0')
    return NULL;

  p = skipNocrypt(p);
  upch1 = toupper(*p);
  if (pair[1] == upch1) /* 上次获得的最后一个字母(pair[1])如果和第一字母相同，填入X分割 */
  {
    upch2 = upch1;
    upch1 = 'X';
  }
  else
  {
    char* q = skipNocrypt(p+1);
    upch2 = toupper(*q);
    if (upch1 == upch2 || upch2 == '\0') /* 两个字母相同或单出一个字母，填入X */
      upch2 = 'X';
    else
      p = q; /* 如果两个字母都获取了，指针p指向后一个字母 */
  }
  setPair(upch1, upch2, pair);
  return p+1;
}
void getTransformPair(KeyMatrix* m, CharPair in, CharPair out, int d)
{
  Position* p0 = positionOfLetter(m, in[0]);
  Position* p1 = positionOfLetter(m, in[1]);
  out[0] = in[0];
  out[1] = in[1];
  if (p0->x == p1->x) /* 两个字母在同一行，转换为同列的偏移d的字母 */
    setPair(
      getLetter(m, p0->x, p0->y+d),
      getLetter(m, p1->x, p1->y+d),
      out
    );
  else if (p0->y == p1->y) /* 在同一列，转换为同行的偏移d的字母 */
    setPair(
      getLetter(m, p0->x+d, p0->y),
      getLetter(m, p1->x+d, p1->y),
      out
    );
  else /* 不同行不同列，转换成列坐标相同，行坐标交换的后的字母对 */
    setPair(
      getLetter(m, p1->x, p0->y),
      getLetter(m, p0->x, p1->y),
      out
    );
}
char *Playfair(char *plaintext,char *key,int count)
{
    int i,j;
    KeyMatrix m;
	char encryptBuff[MAX_BUFF], decryptBuff[MAX_BUFF];
	getKeyMatrix(key, &m);
	printf("生成的m矩阵为：\n");
	for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            printf("%c",m.letters[i][j]);
        }
        printf("\n");
    }
	switch(count)
	{
	case 1:
		encrypt(&m, plaintext, encryptBuff, MAX_BUFF);

		return encryptBuff;
	case 2:
		decrypt(&m, plaintext, decryptBuff, MAX_BUFF);
		return decryptBuff;
	default:
		return NULL;
	}
}


