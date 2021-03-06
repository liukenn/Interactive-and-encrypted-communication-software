#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
//-----------------------------------------------------
//读取路径文件头文件
//-----------------------------------------------------
#include "QFileInfo"
#include "QFileDialog"
//-----------------------------------------------------
//加密头文件
//-----------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include <time.h>
using namespace std;
//-----------------------------------------------------
//Socket头文件
//-----------------------------------------------------
#include< windows.h >
#include< winsock.h >
#include< string.h >
#pragma comment( lib, "ws2_32.lib" )

#define PORT 2046
#define BACKLOG 10
#define MAXDATASIZE 100
//-----------------------------------------------------
//RSA头文件
//-----------------------------------------------------
typedef struct  RSA_PARAM_Tag {
    unsigned __int64    p, q;   //两个素数，不参与加密解密运算
    unsigned __int64    f;      //f=(p-1)*(q-1)，不参与加密解密运算
    unsigned __int64    n, e;   //公匙，n=p*q，gcd(e,f)=1
    unsigned __int64    d;      //私匙，e*d=1 (mod f)，gcd(n,d)=1
    unsigned __int64	s;      //块长，满足2^s<=n的最大的s，即log2(n)
} RSA_PARAM;
const static long	g_PrimeTable[]= {3,5,7,11,13,17,
    19,23,29,31,37,41,
    43,47,53,59,61,67,71,73,79,83,89,97};		//小素数表
const static long	g_PrimeCount=sizeof(g_PrimeTable) / sizeof(long);


const unsigned __int64  multiplier=12747293821;
const unsigned __int64  adder=1343545677842234541;//随机数类
class    RandNumber
{
    private:
        unsigned __int64    randSeed;
    public:
        RandNumber(unsigned __int64 s=0);
        unsigned __int64    Random(unsigned __int64 n);
};

RandNumber::RandNumber(unsigned __int64 s)
{
    if(!s)
    {
        randSeed= (unsigned __int64)time(NULL);
    }
    else
    {
        randSeed=s;
    }
}

unsigned __int64 RandNumber::Random(unsigned __int64 n)
{
    randSeed=multiplier * randSeed + adder;
    return randSeed % n;
}
static RandNumber   g_Rnd;				/* 模乘运算，返回值 x=a*b mod n */

//-----------------------------------------------------
//playfair头文件
//-----------------------------------------------------

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
void Playfair(char *plaintext,char *key,int count);
//-----------------------------------------------------
//DES首部
//----------------密钥PC-1块----------------
const static int PC1_Table[56] = {
     57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
     10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
     63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
     14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
//----------------密钥PC-2块----------------
const static int PC2_Table[48] = {
     14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
     23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
     41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
     44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
//----------------密钥每阶段左移位数列表数组----------------
const static int LOOP_Table[16] = {
     1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
//----------------明文初始换位表 IP----------------
const static int IP_Table[64] = {
     58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
     62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
     57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
     61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
//----------------E-boxes----------------
static const int E_Table[48] = {
     32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
      8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
     16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
     24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
//----------------S-boxes----------------
const static int S_Box[8][4][16] = {
     // S1
     14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
      0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
      4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
     15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
     // S2
     15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
      3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
      0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
     13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
     // S3
     10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
     13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
     13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
      1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
     // S4
      7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
     13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
     10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
      3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
     // S5
      2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
     14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
      4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
     11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
     // S6
     12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
     10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
      9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
      4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
     // S7
      4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
     13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
      1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
      6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
     // S8
     13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
      1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
      7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
      2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};
//----------------P-boxes----------------
static const int P_Box[32] = {
     16,  7, 20, 21, 29, 12, 28, 17,
      1, 15, 23, 26,  5, 18, 31, 10,
      2,  8, 24, 14, 32, 27,  3,  9,
     19, 13, 30,  6, 22, 11,  4, 25
 };
//----------------明文第16次换位表----------------
const static int IPR_Table[64] = {
     40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
     38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
     36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
     34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};
//-----------------------------------------------------
//全局变量
//-----------------------------------------------------
int which=0;
QString str_Caesar,str_Caesar_temp;
QString str_keyword,str_keyword_temp;
QString str_Vigenere,str_Vigenere_temp;
QString str_Autokey,str_Autokey_temp;
QString str_Playfair,str_Playfair_temp;
QString str_Permutation,str_Permutation_temp;
QString str_C_Permutation,str_C_Permutation_temp;
QString str_RC4,str_RC4_temp;
QString str_DES,str_DES_temp;
QString str_RSA,str_RSA_temp,str_RSA_e,str_RSA_n,str_RSA_d;
unsigned __int64 *resultt;
int length_int;

QString str_server=" ",str_server_temp;
int int_socket=0;

QString str_file_lu_1,str_file_lu_2,str_filed,str_filed_temp;

unsigned __int64 DH_p,DH_g,DH_a,DH_b;
unsigned __int64 DH_AtoB,DH_BtoA;
unsigned __int64 DH_result_A,DH_result_B;
//-----------------------------------------------------
int iServerSock;
int iClientSock;

struct sockaddr_in ServerAddr;
struct sockaddr_in ClientAddr;

int sin_size;

WSADATA WSAData;
//-----------------------------------------------------
char buf[ MAXDATASIZE ];
int numbytes;


//-----------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}
//-----------------------------------------------------
//加密算法的选择
//-----------------------------------------------------
void MainWindow::on_Caesar_clicked()
{
    connect(ui->Caesar,SIGNAL(clicked()),this,SLOT(slot_1()));
}

void MainWindow::on_Keyword_clicked()
{
    connect(ui->Keyword,SIGNAL(clicked()),this,SLOT(slot_2()));
}

void MainWindow::on_Vigenere_clicked()
{
    connect(ui->Vigenere,SIGNAL(clicked()),this,SLOT(slot_3()));
}

void MainWindow::on_Autokey_clicked()
{
    connect(ui->Autokey,SIGNAL(clicked()),this,SLOT(slot_4()));
}

void MainWindow::on_Playfair_clicked()
{
    connect(ui->Playfair,SIGNAL(clicked()),this,SLOT(slot_5()));
}

void MainWindow::on_Permutation_clicked()
{
    connect(ui->Permutation,SIGNAL(clicked()),this,SLOT(slot_6()));
}

void MainWindow::on_C_Permutation_clicked()
{
    connect(ui->C_Permutation,SIGNAL(clicked()),this,SLOT(slot_7()));
}

void MainWindow::on_RC4_clicked()
{
    connect(ui->RC4,SIGNAL(clicked()),this,SLOT(slot_8()));
}

void MainWindow::on_DES_clicked()
{
    connect(ui->DES,SIGNAL(clicked()),this,SLOT(slot_9()));
}

void MainWindow::on_RSA_clicked()
{
    connect(ui->RSA,SIGNAL(clicked()),this,SLOT(slot_10()));
}

void MainWindow::on_DH_clicked()
{
    connect(ui->DH,SIGNAL(clicked()),this,SLOT(slot_11()));
}
//-----------------------------------------------------
void MainWindow::slot_1()
{
    ui->stackedWidget->setCurrentIndex(0);
    which=0;
}
void MainWindow::slot_2()
{
    ui->stackedWidget->setCurrentIndex(1);
    which=1;
}
void MainWindow::slot_3()
{
    ui->stackedWidget->setCurrentIndex(2);
    which=2;
}
void MainWindow::slot_4()
{
    ui->stackedWidget->setCurrentIndex(3);
    which=3;
}
void MainWindow::slot_5()
{
    ui->stackedWidget->setCurrentIndex(4);
    which=4;
}
void MainWindow::slot_6()
{
    ui->stackedWidget->setCurrentIndex(5);
    which=5;
}
void MainWindow::slot_7()
{
    ui->stackedWidget->setCurrentIndex(6);
    which=6;
}
void MainWindow::slot_8()
{
    ui->stackedWidget->setCurrentIndex(7);
    which=7;
}
void MainWindow::slot_9()
{
    ui->stackedWidget->setCurrentIndex(8);
    which=8;
}
void MainWindow::slot_10()
{
    ui->stackedWidget->setCurrentIndex(9);
    which=9;
}
void MainWindow::slot_11()
{
    ui->stackedWidget->setCurrentIndex(10);
    which=10;
}
//-----------------------------------------------------
//文件读取
//-----------------------------------------------------
void Read(char lu[])
{
    FILE *f;
    int i=0;
    char A[100];
    char c;
    str_filed="";
    f=fopen(lu,"rb");
    if(f==NULL)
    {
        printf("打开文件失败，请检查你的文件名格式！");
        exit(1);
    }
    while((c=getc(f))!=EOF)
    {
        A[i]=c;
        i++;
    }
    A[i]='\0';
    for(i=0;i<strlen(A);i++)
    {
        str_filed_temp.sprintf("%c",A[i]);
        str_filed+=str_filed_temp;
    }
    fclose(f);
}
//-----------------------------------------------------
//文件写入
//-----------------------------------------------------
void file_write(QString str_filed_end,int v)
{
    char B[100];
    FILE *f;
    int i;

    QByteArray ba = str_filed_end.toLatin1();
    char *c=ba.data();
    for(i=0;i<str_filed_end.length();i++)
        B[i]=c[i];
    B[str_filed_end.length()]='\0';

    if(v==1)
        f=fopen("C:/Ciphertext.txt","wb");
    else
        f=fopen("C:/Plaintext.txt","wb");
    for(i=0;B[i]!=NULL;i++)
    {
        fprintf(f,"%c",B[i]);
    }
    fclose(f);
}
//-----------------------------------------------------
//Qt文件路径读取
//-----------------------------------------------------
void MainWindow::on_file_clicked()
{
    QString file_full, file_name, file_path;
    QFileInfo fi;
    file_full = QFileDialog::getOpenFileName(this);
    fi = QFileInfo(file_full);
    file_name = fi.fileName();
    file_path = fi.absolutePath();
    ui->lu->setText(file_path+"/"+file_name);
}
//-----------------------------------------------------
//keyword
//-----------------------------------------------------
void key_str(char str[],char key_ABC[])
{
    int i=0,j=0,l=strlen(str),n=strlen(key_ABC),z=0;
    for(i=0;i<l;i++)
    {
        z=0;
        for(j=0;j<26;j++)
        {
            if(str[i]==key_ABC[j])
                z=1;
        }
        if(z==0)
            key_ABC[n++]=str[i];
    }
}
void keyword(char p[],char key[],int v)
{
    int i=0,j=0,n=strlen(p);
    char ABC[27]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    char key_ABC[27]={0};
    key_str(key,key_ABC);
    key_str(ABC,key_ABC);
    if(v==1)
    {
        for(i=0;i<n;i++)
        {
            for(j=0;j<26;j++)
            {
                if(p[i]==ABC[j])
                {
                    str_keyword_temp.sprintf("%c",key_ABC[j]);
                    str_keyword+=str_keyword_temp;
                }
            }
        }
    }
    else if(v==2)
    {
        for(i=0;i<n;i++)
        {
            for(j=0;j<26;j++)
            {
                if(p[i]==key_ABC[j])
                {
                    str_keyword_temp.sprintf("%c",ABC[j]);
                    str_keyword+=str_keyword_temp;
                }
            }
        }
    }

}
//-----------------------------------------------------
//Caesar
//-----------------------------------------------------
void Caesar(char p[],int key,int v)
{
    int l=strlen(p);
    int i=0;
    key=key%26;
    if(v==1)
    {
         for(i=0;i<l;i++)
         {
              if ('a'<=p[i] && p[i]<='z'-key || 'A'<=p[i] && p[i]<='Z'-key)
                   p[i]=p[i]+key;
              else
                   p[i]=p[i]+key-26;
         }
         for(i=0;i<l;i++)
         {
              str_Caesar_temp.sprintf("%c",p[i]);
              str_Caesar+=str_Caesar_temp;
         }
    }
    else if(v==2)
    {
        for(i=0;i<l;i++)
         {
              if ('a'+key<=p[i] && p[i]<='z' || 'A'+key<=p[i] && p[i]<='Z')
                   p[i]=p[i]-key;
              else
                   p[i]=p[i]-key+26;
         }
        for(i=0;i<l;i++)
        {
             str_Caesar_temp.sprintf("%c",p[i]);
             str_Caesar+=str_Caesar_temp;
        }
    }
}
//-----------------------------------------------------
//Vigenere
//-----------------------------------------------------
void Vigenere(char text[],char key[],int v)
{
    int i=0,j=0,P_len=0;
    char Ciphertext[100];
    char a[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    char table[26][26];
    for(i=0;i<26;i++)
    {
        for(j=0;j<26;j++)
            table[i][j]=a[(i+j)%26];
    }
    if(v==1)
    {
        P_len=strlen(text);
        i=0;
        //---------------------加密---------------------
        while(i!=P_len)
        {
            Ciphertext[i]=table[(int)key[i%strlen(key)]-97][(int)text[i]-97];           //索引Vigenere 表
            str_Vigenere_temp.sprintf("%c",Ciphertext[i]);
            str_Vigenere+=str_Vigenere_temp;
            i++;
        }
    }
    else if(v==2)
    {
        P_len=strlen(text);
        i=0;
        j=0;
        //---------------------Vigenere 表---------------------
        while(j!=P_len)
        {
            if(text[j]==table[(int)key[j%strlen(key)]-97][i])							//索引Vigenere 表逆操作
            {
                str_Vigenere_temp.sprintf("%c",table[0][i]);
                str_Vigenere+=str_Vigenere_temp;
                j++;
                i=0;
            }
            else
                i++;
        }
    }
}
//-----------------------------------------------------
//Autokey
//-----------------------------------------------------
void Autokey(char p[],char key[],int v)
{
    int i=0,j=0,P_len=strlen(p),k_len=strlen(key);
    char key_use[100];
    //---------------------Vigenere 表---------------------
    char a[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    char table[26][26];
    for(i=0;i<26;i++)
    {
        for(j=0;j<26;j++)
            table[i][j]=a[(i+j)%26];
    }
    //------------------------------------------
    if(v==1)
    {
        for(i=0;i<k_len;i++)
            key_use[i]=key[i];
        for(i=0;i<P_len-k_len;i++)
            key_use[i+k_len]=p[i];
        key_use[i+k_len]='\0';
        i=0;
        //---------------------加密---------------------
        while(i!=P_len)
        {
            str_Autokey_temp.sprintf("%c",table[(int)key_use[i]-97][(int)p[i]-97]);
            str_Autokey+=str_Autokey_temp;
            i++;
        }
    }
    else if(v==2)//---------------------解密---------------------
    {
        for(i=0;i<k_len;i++)
            key_use[i]=key[i];
        i=0;
        j=0;
        while(j!=P_len)
        {
            if(p[j]==table[(int)key_use[j]-97][i])							//索引Vigenere 表逆操作
            {
                str_Autokey_temp.sprintf("%c",table[0][i]);
                str_Autokey+=str_Autokey_temp;
                key_use[k_len++]=table[0][i];
                j++;
                i=0;
            }
            else
                i++;
        }
    }
}
//-----------------------------------------------------
//Playfair
//-----------------------------------------------------
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
      str_Playfair_temp.sprintf("%c",out[i]);
      str_Playfair+=str_Playfair_temp;
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
      str_Playfair_temp.sprintf("%c",out[i]);
      str_Playfair+=str_Playfair_temp;
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
void Playfair(char *plaintext,char *key,int count)
{
    KeyMatrix m;
    char encryptBuff[MAX_BUFF], decryptBuff[MAX_BUFF];
    getKeyMatrix(key, &m);
    switch(count)
    {
    case 1:
        encrypt(&m, plaintext, encryptBuff, MAX_BUFF);
        return ;
    case 2:
        decrypt(&m, plaintext, decryptBuff, MAX_BUFF);
        return ;
    }
}
//-----------------------------------------------------
//Permutation
//-----------------------------------------------------
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
    for(i=0;i<row;i++)
    {
        for(j=0;j<length;j++)
        {
            str_Permutation_temp.sprintf("%c",table2[i][j]);
            str_Permutation+=str_Permutation_temp;
        }
    }
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
    for(i=0;i<row;i++)
    {
        for(j=0;j<length;j++)
        {
                str_Permutation_temp.sprintf("%c",table2[i][j]);
                str_Permutation+=str_Permutation_temp;
        }
    }
}
//-----------------------------------------------------
//C_Permutation
//-----------------------------------------------------
void C_Permutation(char p[],char key[],int v)
{
    char Ciphertext[100];
    int i=0,j=0,key_numble[20]={0},len,len_P,m,n,x=0,k;
    len=strlen(key);
    for(i=0;i<len;i++)												//key字母顺序
    {
        key_numble[i]=1;
        for(j=0;j<len;j++)
            if(key[i]>key[j])
                key_numble[i]++;
    }
    for(i=0;i<len;i++)												//key里出现相同字母
    {
        for(j=i+1;j<len;j++)
                if(key[i]==key[j])
                    key_numble[j]++;
    }
    if(v==1)
    {
        len_P=strlen(p);
        m=len_P/len;
        n=len_P%len;
        if(n!=0)														//明文字符不够数用"x"填充
        {
            m++;
            for(i=0;i<len-n;i++)
                p[len_P++]='x';
        }
        //---------Column permutation encryption核心代码---------
        for(i=0;i<len;i++)
        {
            k=key_numble[i]-1;
            for(j=0;j<m;j++)
            {
                Ciphertext[x++]=p[j*len+k];
                Ciphertext[x]='\0';
            }
        }
        str_C_Permutation_temp.sprintf("%s",Ciphertext);
        str_C_Permutation+=str_C_Permutation_temp;
    }
    else if(v==2)
    {
        len_P=strlen(p);
        m=len_P/len;
        n=len_P%len;
        if(n!=0)														//明文字符不够数用"x"填充
        {
            m++;
            for(i=0;i<len-n;i++)
                p[len_P++]='x';
        }
        //---------解密---------
        x=0;
        for(i=0;i<len;i++)
        {
            k=key_numble[i]-1;
            for(j=0;j<m;j++)
                Ciphertext[j*len+k]=p[x++];
        }
        for(i=0;i<len_P;i++)
        {
            str_C_Permutation_temp.sprintf("%c",Ciphertext[i]);
            str_C_Permutation+=str_C_Permutation_temp;
        }
    }
}
//-----------------------------------------------------
//RC4
//-----------------------------------------------------
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
void RC4(char p[],char *key)
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
       str_RC4_temp.sprintf("%c",p[z]);
       str_RC4+=str_RC4_temp;
       memset(p,0,1);
       z++;
    }
}
//-----------------------------------------------------
//DES
//-----------------------------------------------------
void DES_Encrypition(int Plaintext[],int key_use[]){
    int i,j,m,ch,Plaintext_left[32],Plaintext_right[32],Plaintext_after[48],Plaintext_after_S[32],Plaintext_after_P[32],str[4];
    //---------------- Plaintext分成L、R两部分----------------
    for(i=0;i<32;i++)
        Plaintext_left[i]=Plaintext[i];
    for(i=32;i<64;i++)
        Plaintext_right[i-32]=Plaintext[i];
    //---------------- E盒扩展----------------
    for(i=0;i<48;i++)
        Plaintext_after[i]=Plaintext_right[E_Table[i]-1];
    //---------------- 与key异或----------------
    for(i=0;i<48;i++)
        if(Plaintext_after[i]==key_use[i])
            Plaintext_after[i]=0;
        else
            Plaintext_after[i]=1;
    //---------------- S盒替代----------------
    for(m=0;m<8;m++){
        i=Plaintext_after[m*6]*2+Plaintext_after[m*6+5];
        j=Plaintext_after[m*6+1]*8+Plaintext_after[m*6+2]*4+Plaintext_after[m*6+3]*2+Plaintext_after[m*6+4];
        ch=S_Box[m][i][j];
        for(i=0;i<4;i++){									//二进制转换
            str[3-i]=ch%2;
            ch=ch/2;
        }
        for(j=0;j<4;j++)
            Plaintext_after_S[m*4+j]=str[j];
    }
    //---------------- P盒替代----------------
    for(i=0;i<32;i++)
        Plaintext_after_P[i]=Plaintext_after_S[P_Box[i]-1];
    //---------------- 完成一轮加密后对L、R的操作----------------
    for(i=0;i<32;i++){
        if(Plaintext_after_P[i]==Plaintext_left[i])
            Plaintext_after_P[i]=0;
        else
            Plaintext_after_P[i]=1;
        Plaintext_left[i]=Plaintext_right[i];
        Plaintext_right[i]=Plaintext_after_P[i];
    }
    for(i=0;i<32;i++){
        Plaintext[i]=Plaintext_left[i];
        Plaintext[i+32]=Plaintext_right[i];
    }
}
void DES (char a[],char b[],int v)
{
    char Ciphertext_char[64];
    int i,j,m,ch,str[8],sum=0,key_before[64],key[16][56],key_temp[56],key_use[48],Plaintext[64],Ciphertext[64],Plaintext_temp[64],key_De[16][56];
    for(m=0;m<8;m++){									//密钥的二进制转换
        ch=a[m];
        for(j=0;j<7;j++){
            str[6-j]=ch%2;
            ch=ch/2;
        }
        for(j=0;j<7;j++)
            sum+=str[j];
        if(sum%2==0)									//奇偶校验位
            str[7]=1;
        else
            str[7]=0;
        for(j=0;j<8;j++)
            key_before[m*8+j]=str[j];
    }
    for(m=0;m<8;m++){									//明文的二进制转换
        ch=b[m];
        for(j=0;j<8;j++){
            str[7-j]=ch%2;
            ch=ch/2;
        }
        for(j=0;j<8;j++)
            Plaintext[m*8+j]=str[j];
    }
    //----------------PC1_Table置换----------------
    for(j=0;j<56;j++)
        key[0][j]=key_before[PC1_Table[j]-1];
    for(m=0;m<16;m++)									//求取16轮加密的密钥
        for(i=0;i<LOOP_Table[m];i++){					//一次或两次左移1位数操作
            for(j=0;j<56;j++)
                key_temp[j]=key[m][(j+1)%56];
            key_temp[27]=key[m][0];
            key_temp[55]=key[m][28];
            for(j=0;j<56;j++){
                key[m][j]=key_temp[j];
                key[m+1][j]=key[m][j];
                if(v==2)
                    key_De[15-m][j]=key[m][j];
            }
        }
    //----------------IP_Table置换----------------
    for(i=0;i<64;i++)
        Plaintext_temp[i]=Plaintext[IP_Table[i]-1];
    for(i=0;i<16;i++){									//16轮加密
        //----------------PC2_Table置换----------------
        for(j=0;j<48;j++)
        {
            if(v==1)
                key_use[j]=key[i][PC2_Table[j]-1];
            else {
                key_use[j]=key_De[i][PC2_Table[j]-1];
            }
        }

        DES_Encrypition(Plaintext_temp,key_use);
    }
    //----------------IPR_Table置换----------------
    for(i=0;i<64;i++)
        Ciphertext[i]=Plaintext_temp[IPR_Table[i]-1];
    //----------------输出----------------
    for(i=0;i<64;i++){
        str_DES_temp.sprintf("%d",Ciphertext[i]);
        str_DES+=str_DES_temp;
        if((i+1)%8==0)
           str_DES+=" ";
       if((i+1)%32==0)
           str_DES+="\n";
    }
}
//-----------------------------------------------------
//RSA
//-----------------------------------------------------
inline unsigned __int64 MulMod(unsigned __int64 a,
                               unsigned __int64 b,
                               unsigned __int64 n)
{
    return a * b % n;
}
/* 模幂运算，返回值 x=base^pow mod n */
unsigned __int64 PowMod(unsigned __int64 base,
                        unsigned __int64 &pow,
                        unsigned __int64 &n)
{
    unsigned __int64    a=base, b=pow, c=1;
    while(b)
    {
        while(!(b & 1))
        {
            b>>=1;        //a=a * a % n;    //函数看起来可以处理64位的整数，
                            //但由于这里a*a在a>=2^32时已经造成了溢出
                                //因此实际处理范围没有64位
            a=MulMod(a, a, n);
        }
        b--;			//c=a * c % n;  这里也会溢出，若把64位整数拆为
                        //	两个32位整数不知是否可以解决这个问题
        c=MulMod(a, c, n);
    }
    return c;
}
/* Rabin-Miller素数测试，通过测试返回1，否则返回0。 n是待测素数。
注意：通过测试并不一定就是素数，非素数通过测试的概率是1/4 */
long RabinMillerKnl(unsigned __int64 &n)
{
    unsigned __int64    b, m, j, v, i;
    m=n - 1;
    j=0;			//0、先计算出m、j，使得n-1=m*2^j，其中m是正奇数，j是非负整数
    while(!(m & 1))
    {
        ++j;
        m>>=1;
    }								//1、随机取一个b，2<=b<n-1
    b=2 + g_Rnd.Random(n - 3);    //2、计算v=b^m mod n
    v=PowMod(b, m, n);				//3、如果v==1，通过测试
    if(v == 1)
    {
        return 1;
    }								//4、令i=1
    i=1;					//5、如果v=n-1，通过测试
    while(v != n - 1)
    {
                            //6、如果i==l，非素数，结束
        if(i == j)
        {
            return 0;
        }				 //7、v=v^2 mod n，i=i+1
        unsigned __int64 squ=2;
        v=PowMod(v, squ, n);		//X：将2改换为多一个变量
        ++i;			 //8、循环到5
    }
    return 1;
}
/* Rabin-Miller素数测试，循环调用核心loop次 全部通过返回1，否则返回0 */
long RabinMiller(unsigned __int64 &n, long loop) {
    //先用小素数筛选一次，提高效率
    long i;
    for(i=0; i < g_PrimeCount; i++)
    {
        if(n % g_PrimeTable[i] == 0)
        {
            return 0;
        }
    }				//循环调用Rabin-Miller测试loop次，
                    //使得非素数通过测试的概率降为(1/4)^loop
    for(i=0; i < loop; i++)
    {
        if(!RabinMillerKnl(n))
        {
            return 0;
        }
    }
    return 1;
}
/* 随机生成一个bits位(二进制位)的素数，最多32位 */
unsigned __int64 RandomPrime(char bits)
{
    unsigned __int64    base;
    do
    {
        base= (unsigned long)1 << (bits - 1);	   //保证最高位是1
        base+=g_Rnd.Random(base);			    //再加上一个随机数
        base|=1;							 //保证最低位是1，即保证是奇数
    } while(!RabinMiller(base, 30));    //进行拉宾－米勒测试30次
    return base;				//全部通过认为是素数
}
/* 欧几里得法求最大公约数 */
unsigned __int64 EuclidGcd(unsigned __int64 &p, unsigned __int64 &q)
{
    unsigned __int64    a=p > q ? p : q;
    unsigned __int64    b=p < q ? p : q;
    unsigned __int64    t;     if(p == q)
    {
        return p;   //两数相等，最大公约数就是本身
    }
    else
    {
        while(b)    //辗转相除法，gcd(a,b)=gcd(b,a-qb)
        {
            a=a % b;
            t=a;
            a=b;
            b=t;
        }
        return a;
    }
}
/* 已知a、b，求x，满足a*x =1 (mod b) 相当于求解a*x-b*y=1的最小整数解 */
unsigned __int64 Euclid(unsigned __int64 &a, unsigned __int64 &b)
{
    unsigned __int64    m, e, i, j, x, y;
    long                xx, yy;
    m=b;
    e=a;
    x=0;
    y=1;
    xx=1;
    yy=1;
    while(e)
    {
        i=m / e;
        j=m % e;
        m=e;
        e=j;
        j=y;
        y*=i;
        if(xx == yy)
        {
            if(x > y)
            {
                y=x - y;
            }
            else
            {
                y-=x;
                yy=0;
            }
        }
        else
        {
            y+=x;
            xx=1 - xx;
            yy=1 - yy;
        }
        x=j;
    }
    if(xx == 0)
    {
        x=b - x;
    }
    return x;
}

/* 随机产生一个RSA加密参数 */
RSA_PARAM RsaGetParam(void)
{
    RSA_PARAM           Rsa={ 0 };
    unsigned __int64    t;
    Rsa.p=RandomPrime(16);          //随机生成两个素数
    Rsa.q=RandomPrime(16);
    Rsa.n=Rsa.p * Rsa.q;
    Rsa.f=(Rsa.p - 1) * (Rsa.q - 1);
    do
    {
        Rsa.e=g_Rnd.Random(65536);  //小于2^16，65536=2^16
        Rsa.e|=1;                   //保证最低位是1，即保证是奇数，
                                    //因f一定是偶数，要互素，只能是奇数
    } while(EuclidGcd(Rsa.e, Rsa.f) != 1);
    Rsa.d=Euclid(Rsa.e, Rsa.f);
    Rsa.s=0;
    t=Rsa.n >> 1;
    while(t)
    {
        Rsa.s++;                    //s=log2(n)
        t>>=1;
    }
    return Rsa;
}
/* RSA加密解密 */
void TestRSA(char pSrc[],int z) {
    RSA_PARAM           r;
    unsigned long i;
    const unsigned long l=strlen(pSrc);		//X：原先是sizeof（pSrc）
    unsigned char       *q, pDec[100];		//X：原先是n 把n换成了100
    unsigned __int64    pEnc[100];
    str_RSA="";
    if(z==1){
        r=RsaGetParam();
        str_RSA_n.sprintf("%u",r.n);
        str_RSA_e.sprintf("%u",r.e);
        str_RSA_d.sprintf("%u",r.d);
        q= (unsigned char *)pSrc;
        unsigned __int64 t;
        for(i=0; i < l; i++)
        {
            t=(unsigned __int64)q[i];			//X同样为PowMod左值引用问题
            pEnc[i]=PowMod(t, r.e, r.n);
            str_RSA_temp.sprintf("%u ",pEnc[i]);
            str_RSA+=str_RSA_temp;
        }
    }
    else if(z==2){
        i=0;
        unsigned __int64 d=str_RSA_d.toULongLong();
        unsigned __int64 n=str_RSA_n.toULongLong();
//        cout<<d<<endl<<n<<endl<<length_int<<endl;
//        cout<<resultt[0]<<endl;
//        cout<<resultt[1]<<endl;
//        cout<<resultt[2]<<endl;
//        cout<<resultt[3]<<endl;
        for(i=0; i < length_int; i++)
        {
            pDec[i]=PowMod(resultt[i],d,n);
            str_RSA_temp.sprintf("%c",pDec[i]);
            str_RSA+=str_RSA_temp;
        }
        pDec[i+1]='\0';
    }
}
//-----------------
//RSA解密
unsigned __int64* Chartoint(char example[])
{
//    cout<<example<<endl;
    unsigned __int64 temp_int=0;
    int i=0,j=0,k=0;
    length_int=0;
    for(i=0;example[i]!='\0';i++)
    {
        if(example[i]==' ')
            length_int++;
    }
    length_int++;
    unsigned __int64 *result=new unsigned __int64[length_int];

    for(i=0;i<strlen(example);i++)
    {
        j=i;
        while(example[++i]!=' ')
        {
            if(example[i]=='\0')
                break;
        }
        temp_int=0;
        while(j<i)
        {
            temp_int=temp_int*10+(example[j]-'0');
            j++;
        }
        result[k++]=temp_int;
    }
    j++;
    temp_int=0;
    while(j<i)
    {
        temp_int=temp_int*10+(example[j]-'0');
        j++;
    }
    result[k++]=temp_int;

    return result;
}
//-----------------------------------------------------
//加密按键
//-----------------------------------------------------
void MainWindow::on_Encrypt_clicked()
{
    int i;
    QString str_file=ui->lu->text();
    if(str_file==""){
        if(which==0)//Caesar
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->key_Caesar->text();
            QString Caesar_Plaintext=ui->Plaintext->toPlainText();
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Caesar);
            str_Caesar="";
        }else if(which==1)//Keyword
        {
            char Keyword_PlainText_use[100],Keyword_key_use[100];
            QString Keyword_key=ui->key_keyword->text();
            QString Keyword_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Keyword_ba = Keyword_key.toLatin1();
            char *a=Keyword_ba.data();
            for(i=0;i<Keyword_key.length();i++)
                Keyword_key_use[i]=a[i];
            Keyword_key_use[Keyword_key.length()]='\0';
            Keyword_ba = Keyword_Plaintext.toLatin1();
            char *b=Keyword_ba.data();
            for(i=0;i<Keyword_Plaintext.length();i++)
                Keyword_PlainText_use[i]=b[i];
            Keyword_PlainText_use[Keyword_Plaintext.length()]='\0';
            keyword(Keyword_PlainText_use,Keyword_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_keyword);
            str_keyword="";
        }else if(which==2)//Vigenere
        {
            char Vigenere_PlainText_use[100],Vigenere_key_use[100];
            QString Vigenere_key=ui->key_Vigenere->text();
            QString Vigenere_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Vigenere_ba = Vigenere_key.toLatin1();
            char *a=Vigenere_ba.data();
            for(i=0;i<Vigenere_key.length();i++)
                Vigenere_key_use[i]=a[i];
            Vigenere_key_use[Vigenere_key.length()]='\0';
            Vigenere_ba = Vigenere_Plaintext.toLatin1();
            char *b=Vigenere_ba.data();
            for(i=0;i<Vigenere_Plaintext.length();i++)
                Vigenere_PlainText_use[i]=b[i];
            Vigenere_PlainText_use[Vigenere_Plaintext.length()]='\0';
            Vigenere(Vigenere_PlainText_use,Vigenere_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Vigenere);
            str_Vigenere="";
        }else if(which==3)//Autokey
        {
            char Autokey_PlainText_use[100],Autokey_key_use[100];
            QString Autokey_key=ui->key_Autokey->text();
            QString Autokey_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Autokey_ba = Autokey_key.toLatin1();
            char *a=Autokey_ba.data();
            for(i=0;i<Autokey_key.length();i++)
                Autokey_key_use[i]=a[i];
            Autokey_key_use[Autokey_key.length()]='\0';
            Autokey_ba = Autokey_Plaintext.toLatin1();
            char *b=Autokey_ba.data();
            for(i=0;i<Autokey_Plaintext.length();i++)
                Autokey_PlainText_use[i]=b[i];
            Autokey_PlainText_use[Autokey_Plaintext.length()]='\0';
            Autokey(Autokey_PlainText_use,Autokey_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Autokey);
            str_Autokey="";
        }else if(which==4)//Playfair
        {
            char Playfair_PlainText_use[100],Playfair_key_use[100];
            QString Playfair_key=ui->key_Playfair->text();
            QString Playfair_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Playfair_ba = Playfair_key.toLatin1();
            char *a=Playfair_ba.data();
            for(i=0;i<Playfair_key.length();i++)
                Playfair_key_use[i]=a[i];
            Playfair_key_use[Playfair_key.length()]='\0';
            Playfair_ba = Playfair_Plaintext.toLatin1();
            char *b=Playfair_ba.data();
            for(i=0;i<Playfair_Plaintext.length();i++)
                Playfair_PlainText_use[i]=b[i];
            Playfair_PlainText_use[Playfair_Plaintext.length()]='\0';
            Playfair(Playfair_PlainText_use,Playfair_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Playfair);
            str_Playfair="";
        }else if(which==5)//Permutation
        {
            char Permutation_PlainText_use[100],Permutation_key_use[100];
            QString Permutation_key=ui->key_per->text();
            QString Permutation_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Permutation_ba = Permutation_key.toLatin1();
            char *a=Permutation_ba.data();
            for(i=0;i<Permutation_key.length();i++)
                Permutation_key_use[i]=a[i];
            Permutation_key_use[Permutation_key.length()]='\0';
            Permutation_ba = Permutation_Plaintext.toLatin1();
            char *b=Permutation_ba.data();
            for(i=0;i<Permutation_Plaintext.length();i++)
                Permutation_PlainText_use[i]=b[i];
            Permutation_PlainText_use[Permutation_Plaintext.length()]='\0';
            Permutation_en(Permutation_PlainText_use,Permutation_key_use);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Permutation);
            str_Permutation="";
        }else if(which==6)//C_Permutation
        {
            char C_Permutation_PlainText_use[100],C_Permutation_key_use[100];
            QString C_Permutation_key=ui->key_C_per->text();
            QString C_Permutation_Plaintext=ui->Plaintext->toPlainText();
            QByteArray C_Permutation_ba = C_Permutation_key.toLatin1();
            char *a=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_key.length();i++)
                C_Permutation_key_use[i]=a[i];
            C_Permutation_key_use[C_Permutation_key.length()]='\0';
            C_Permutation_ba = C_Permutation_Plaintext.toLatin1();
            char *b=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_Plaintext.length();i++)
                C_Permutation_PlainText_use[i]=b[i];
            C_Permutation_PlainText_use[C_Permutation_Plaintext.length()]='\0';
            C_Permutation(C_Permutation_PlainText_use,C_Permutation_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_C_Permutation);
            str_C_Permutation="";
        }else if(which==7)//RC4
        {
            char RC4_PlainText_use[100],RC4_key_use[]="123456";
            QString RC4_Plaintext=ui->Plaintext->toPlainText();
            QByteArray RC4_ba = RC4_Plaintext.toLatin1();
            char *b=RC4_ba.data();
            for(i=0;i<RC4_Plaintext.length();i++)
                RC4_PlainText_use[i]=b[i];
            RC4_PlainText_use[RC4_Plaintext.length()]='\0';
            RC4(RC4_PlainText_use,RC4_key_use);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_RC4);
            str_RC4="";
        }else if(which==8)//DES
        {
            char DES_PlainText_8[9],DES_key_8[9];
            QString DES_key=ui->key_DES->text();
            QString DES_Plaintext=ui->Plaintext->toPlainText();
            int DES_long_p=DES_Plaintext.length();
            while(DES_long_p%8!=0)
            {
                DES_Plaintext+='0';
                DES_long_p=DES_Plaintext.length();
            }
            QByteArray ba = DES_key.toLatin1();
            char *a=ba.data();
            for(i=0;i<8;i++)
                DES_key_8[i]=a[i];
            DES_key_8[8]='\0';
            ba = DES_Plaintext.toLatin1();
            char *b=ba.data();
            int z=0;
            while(DES_long_p/8!=0)
            {
                for(i=0;i<8;i++)
                    DES_PlainText_8[i]=b[i+z*8];
                DES_PlainText_8[8]='\0';
                DES_long_p=DES_long_p-8;
                z++;
                DES(DES_key_8,DES_PlainText_8,1);
            }
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_DES);
            str_DES="";
        }else if(which==9)//RSA
        {
            char RSA_PlainText_use[100];
            QString RSA_Plaintext=ui->Plaintext->toPlainText();
            QByteArray RSA_ba = RSA_Plaintext.toLatin1();
            char *b=RSA_ba.data();
            for(i=0;i<RSA_Plaintext.length();i++)
                RSA_PlainText_use[i]=b[i];
            RSA_PlainText_use[RSA_Plaintext.length()]='\0';
            TestRSA(RSA_PlainText_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_RSA);
            ui->e->setText("");
            ui->e->setText(str_RSA_e);
            ui->d->setText("");
            ui->d->setText(str_RSA_d);
            ui->n_1->setText("");
            ui->n_1->setText(str_RSA_n);
            ui->n_2->setText("");
            ui->n_2->setText(str_RSA_n);
            str_RSA="";
            str_RSA_e="";
            str_RSA_d="";
            str_RSA_n="";
        }else if(which==10)//DH
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->DH_2->text();
            QString Caesar_Plaintext=ui->Plaintext->toPlainText();
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,1);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Caesar);
            str_Caesar="";
        }
    }else if(str_file!=""){
        char lu_use[100];
        str_file_lu_1=ui->lu->text();
        QByteArray file_ba = str_file_lu_1.toLatin1();
        char *a=file_ba.data();
        for(i=0;i<str_file_lu_1.length();i++)
            lu_use[i]=a[i];
        lu_use[i]='\0';
//        cout<<lu_use<<endl;
        Read(lu_use);
        if(which==0)//Caesar
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->key_Caesar->text();
            QString Caesar_Plaintext=str_filed;
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Caesar,1);
            str_Caesar="";
        }else if(which==1)//Keyword
        {
            char Keyword_PlainText_use[100],Keyword_key_use[100];
            QString Keyword_key=ui->key_keyword->text();
            QString Keyword_Plaintext=str_filed;
            QByteArray Keyword_ba = Keyword_key.toLatin1();
            char *a=Keyword_ba.data();
            for(i=0;i<Keyword_key.length();i++)
                Keyword_key_use[i]=a[i];
            Keyword_key_use[Keyword_key.length()]='\0';
            Keyword_ba = Keyword_Plaintext.toLatin1();
            char *b=Keyword_ba.data();
            for(i=0;i<Keyword_Plaintext.length();i++)
                Keyword_PlainText_use[i]=b[i];
            Keyword_PlainText_use[Keyword_Plaintext.length()]='\0';
            keyword(Keyword_PlainText_use,Keyword_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_keyword,1);
            str_keyword="";
        }else if(which==2)//Vigenere
        {
            char Vigenere_PlainText_use[100],Vigenere_key_use[100];
            QString Vigenere_key=ui->key_Vigenere->text();
            QString Vigenere_Plaintext=str_filed;
            QByteArray Vigenere_ba = Vigenere_key.toLatin1();
            char *a=Vigenere_ba.data();
            for(i=0;i<Vigenere_key.length();i++)
                Vigenere_key_use[i]=a[i];
            Vigenere_key_use[Vigenere_key.length()]='\0';
            Vigenere_ba = Vigenere_Plaintext.toLatin1();
            char *b=Vigenere_ba.data();
            for(i=0;i<Vigenere_Plaintext.length();i++)
                Vigenere_PlainText_use[i]=b[i];
            Vigenere_PlainText_use[Vigenere_Plaintext.length()]='\0';
            Vigenere(Vigenere_PlainText_use,Vigenere_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Vigenere,1);
            str_Vigenere="";
        }else if(which==3)//Autokey
        {
            char Autokey_PlainText_use[100],Autokey_key_use[100];
            QString Autokey_key=ui->key_Autokey->text();
            QString Autokey_Plaintext=str_filed;
            QByteArray Autokey_ba = Autokey_key.toLatin1();
            char *a=Autokey_ba.data();
            for(i=0;i<Autokey_key.length();i++)
                Autokey_key_use[i]=a[i];
            Autokey_key_use[Autokey_key.length()]='\0';
            Autokey_ba = Autokey_Plaintext.toLatin1();
            char *b=Autokey_ba.data();
            for(i=0;i<Autokey_Plaintext.length();i++)
                Autokey_PlainText_use[i]=b[i];
            Autokey_PlainText_use[Autokey_Plaintext.length()]='\0';
            Autokey(Autokey_PlainText_use,Autokey_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Autokey,1);
            str_Autokey="";
        }else if(which==4)//Playfair
        {
            char Playfair_PlainText_use[100],Playfair_key_use[100];
            QString Playfair_key=ui->key_Playfair->text();
            QString Playfair_Plaintext=str_filed;
            QByteArray Playfair_ba = Playfair_key.toLatin1();
            char *a=Playfair_ba.data();
            for(i=0;i<Playfair_key.length();i++)
                Playfair_key_use[i]=a[i];
            Playfair_key_use[Playfair_key.length()]='\0';
            Playfair_ba = Playfair_Plaintext.toLatin1();
            char *b=Playfair_ba.data();
            for(i=0;i<Playfair_Plaintext.length();i++)
                Playfair_PlainText_use[i]=b[i];
            Playfair_PlainText_use[Playfair_Plaintext.length()]='\0';
            Playfair(Playfair_PlainText_use,Playfair_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Playfair,1);
            str_Playfair="";
        }else if(which==5)//Permutation
        {
            char Permutation_PlainText_use[100],Permutation_key_use[100];
            QString Permutation_key=ui->key_per->text();
            QString Permutation_Plaintext=str_filed;
            QByteArray Permutation_ba = Permutation_key.toLatin1();
            char *a=Permutation_ba.data();
            for(i=0;i<Permutation_key.length();i++)
                Permutation_key_use[i]=a[i];
            Permutation_key_use[Permutation_key.length()]='\0';
            Permutation_ba = Permutation_Plaintext.toLatin1();
            char *b=Permutation_ba.data();
            for(i=0;i<Permutation_Plaintext.length();i++)
                Permutation_PlainText_use[i]=b[i];
            Permutation_PlainText_use[Permutation_Plaintext.length()]='\0';
            Permutation_en(Permutation_PlainText_use,Permutation_key_use);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Permutation,1);
            str_Permutation="";
        }else if(which==6)//C_Permutation
        {
            char C_Permutation_PlainText_use[100],C_Permutation_key_use[100];
            QString C_Permutation_key=ui->key_C_per->text();
            QString C_Permutation_Plaintext=str_filed;
            QByteArray C_Permutation_ba = C_Permutation_key.toLatin1();
            char *a=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_key.length();i++)
                C_Permutation_key_use[i]=a[i];
            C_Permutation_key_use[C_Permutation_key.length()]='\0';
            C_Permutation_ba = C_Permutation_Plaintext.toLatin1();
            char *b=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_Plaintext.length();i++)
                C_Permutation_PlainText_use[i]=b[i];
            C_Permutation_PlainText_use[C_Permutation_Plaintext.length()]='\0';
            C_Permutation(C_Permutation_PlainText_use,C_Permutation_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_C_Permutation,1);
            str_C_Permutation="";
        }else if(which==7)//RC4
        {
            char RC4_PlainText_use[100],RC4_key_use[]="123456";
            QString RC4_Plaintext=str_filed;
            QByteArray RC4_ba = RC4_Plaintext.toLatin1();
            char *b=RC4_ba.data();
            for(i=0;i<RC4_Plaintext.length();i++)
                RC4_PlainText_use[i]=b[i];
            RC4_PlainText_use[RC4_Plaintext.length()]='\0';
            RC4(RC4_PlainText_use,RC4_key_use);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_RC4,1);
            str_RC4="";
        }else if(which==8)//DES
        {
            char DES_PlainText_8[9],DES_key_8[9];
            QString DES_key=ui->key_DES->text();
            QString DES_Plaintext=str_filed;
            int DES_long_p=DES_Plaintext.length();
            while(DES_long_p%8!=0)
            {
                DES_Plaintext+='0';
                DES_long_p=DES_Plaintext.length();
            }
            QByteArray ba = DES_key.toLatin1();
            char *a=ba.data();
            for(i=0;i<8;i++)
                DES_key_8[i]=a[i];
            DES_key_8[8]='\0';
            ba = DES_Plaintext.toLatin1();
            char *b=ba.data();
            int z=0;
            while(DES_long_p/8!=0)
            {
                for(i=0;i<8;i++)
                    DES_PlainText_8[i]=b[i+z*8];
                DES_PlainText_8[8]='\0';
                DES_long_p=DES_long_p-8;
                z++;
                DES(DES_key_8,DES_PlainText_8,1);
            }
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_DES,1);
            str_DES="";
        }else if(which==9)//RSA
        {
            char RSA_PlainText_use[100];
            QString RSA_Plaintext=str_filed;
            QByteArray RSA_ba = RSA_Plaintext.toLatin1();
            char *b=RSA_ba.data();
            for(i=0;i<RSA_Plaintext.length();i++)
                RSA_PlainText_use[i]=b[i];
            RSA_PlainText_use[RSA_Plaintext.length()]='\0';
            TestRSA(RSA_PlainText_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_RSA,1);
            ui->e->setText("");
            ui->e->setText(str_RSA_e);
            ui->d->setText("");
            ui->d->setText(str_RSA_d);
            ui->n_1->setText("");
            ui->n_1->setText(str_RSA_n);
            ui->n_2->setText("");
            ui->n_2->setText(str_RSA_n);
            str_RSA="";
            str_RSA_e="";
            str_RSA_d="";
            str_RSA_n="";
        }else if(which==10)//DH
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->DH_2->text();
            QString Caesar_Plaintext=str_filed;
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,1);
            ui->Ciphertext->setText("Success!C:/Ciphertext.txt");
            file_write(str_Caesar,1);
            str_Caesar="";
        }
    }

}
//-----------------------------------------------------
//解密按键
//-----------------------------------------------------
void MainWindow::on_Decrypt_clicked()
{
    int i;
    QString str_file=ui->lu->text();
    if(str_file==""){
        if(which==0)//Caesar
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->key_Caesar->text();
            QString Caesar_Plaintext=ui->Plaintext->toPlainText();
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Caesar);
            str_Caesar="";
        }else if(which==1)//Keyword
        {
            char Keyword_PlainText_use[100],Keyword_key_use[100];
            QString Keyword_key=ui->key_keyword->text();
            QString Keyword_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Keyword_ba = Keyword_key.toLatin1();
            char *a=Keyword_ba.data();
            for(i=0;i<Keyword_key.length();i++)
                Keyword_key_use[i]=a[i];
            Keyword_key_use[Keyword_key.length()]='\0';
            Keyword_ba = Keyword_Plaintext.toLatin1();
            char *b=Keyword_ba.data();
            for(i=0;i<Keyword_Plaintext.length();i++)
                Keyword_PlainText_use[i]=b[i];
            Keyword_PlainText_use[Keyword_Plaintext.length()]='\0';
            keyword(Keyword_PlainText_use,Keyword_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_keyword);
            str_keyword="";
        }else if(which==2)//Vigenere
        {
            char Vigenere_PlainText_use[100],Vigenere_key_use[100];
            QString Vigenere_key=ui->key_Vigenere->text();
            QString Vigenere_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Vigenere_ba = Vigenere_key.toLatin1();
            char *a=Vigenere_ba.data();
            for(i=0;i<Vigenere_key.length();i++)
                Vigenere_key_use[i]=a[i];
            Vigenere_key_use[Vigenere_key.length()]='\0';
            Vigenere_ba = Vigenere_Plaintext.toLatin1();
            char *b=Vigenere_ba.data();
            for(i=0;i<Vigenere_Plaintext.length();i++)
                Vigenere_PlainText_use[i]=b[i];
            Vigenere_PlainText_use[Vigenere_Plaintext.length()]='\0';
            Vigenere(Vigenere_PlainText_use,Vigenere_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Vigenere);
            str_Vigenere="";
        }else if(which==3)//Autokey
        {
            char Autokey_PlainText_use[100],Autokey_key_use[100];
            QString Autokey_key=ui->key_Autokey->text();
            QString Autokey_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Autokey_ba = Autokey_key.toLatin1();
            char *a=Autokey_ba.data();
            for(i=0;i<Autokey_key.length();i++)
                Autokey_key_use[i]=a[i];
            Autokey_key_use[Autokey_key.length()]='\0';
            Autokey_ba = Autokey_Plaintext.toLatin1();
            char *b=Autokey_ba.data();
            for(i=0;i<Autokey_Plaintext.length();i++)
                Autokey_PlainText_use[i]=b[i];
            Autokey_PlainText_use[Autokey_Plaintext.length()]='\0';
            Autokey(Autokey_PlainText_use,Autokey_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Autokey);
            str_Autokey="";
        }else if(which==4)//Playfair
        {
            char Playfair_PlainText_use[100],Playfair_key_use[100];
            QString Playfair_key=ui->key_Playfair->text();
            QString Playfair_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Playfair_ba = Playfair_key.toLatin1();
            char *a=Playfair_ba.data();
            for(i=0;i<Playfair_key.length();i++)
                Playfair_key_use[i]=a[i];
            Playfair_key_use[Playfair_key.length()]='\0';
            Playfair_ba = Playfair_Plaintext.toLatin1();
            char *b=Playfair_ba.data();
            for(i=0;i<Playfair_Plaintext.length();i++)
                Playfair_PlainText_use[i]=b[i];
            Playfair_PlainText_use[Playfair_Plaintext.length()]='\0';
            Playfair(Playfair_PlainText_use,Playfair_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Playfair);
            str_Playfair="";
        }else if(which==5)//Permutation
        {
            char Permutation_PlainText_use[100],Permutation_key_use[100];
            QString Permutation_key=ui->key_per->text();
            QString Permutation_Plaintext=ui->Plaintext->toPlainText();
            QByteArray Permutation_ba = Permutation_key.toLatin1();
            char *a=Permutation_ba.data();
            for(i=0;i<Permutation_key.length();i++)
                Permutation_key_use[i]=a[i];
            Permutation_key_use[Permutation_key.length()]='\0';
            Permutation_ba = Permutation_Plaintext.toLatin1();
            char *b=Permutation_ba.data();
            for(i=0;i<Permutation_Plaintext.length();i++)
                Permutation_PlainText_use[i]=b[i];
            Permutation_PlainText_use[Permutation_Plaintext.length()]='\0';
            Permutation_de(Permutation_PlainText_use,Permutation_key_use);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Permutation);
            str_Permutation="";
        }else if(which==6)//C_Permutation
        {
            char C_Permutation_PlainText_use[100],C_Permutation_key_use[100];
            QString C_Permutation_key=ui->key_C_per->text();
            QString C_Permutation_Plaintext=ui->Plaintext->toPlainText();
            QByteArray C_Permutation_ba = C_Permutation_key.toLatin1();
            char *a=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_key.length();i++)
                C_Permutation_key_use[i]=a[i];
            C_Permutation_key_use[C_Permutation_key.length()]='\0';
            C_Permutation_ba = C_Permutation_Plaintext.toLatin1();
            char *b=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_Plaintext.length();i++)
                C_Permutation_PlainText_use[i]=b[i];
            C_Permutation_PlainText_use[C_Permutation_Plaintext.length()]='\0';
            C_Permutation(C_Permutation_PlainText_use,C_Permutation_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_C_Permutation);
            str_C_Permutation="";
        }else if(which==7)//RC4
        {
            char RC4_PlainText_use[100],RC4_key_use[]="123456";
            QString RC4_Plaintext=ui->Plaintext->toPlainText();
            QByteArray RC4_ba = RC4_Plaintext.toLatin1();
            char *b=RC4_ba.data();
            for(i=0;i<RC4_Plaintext.length();i++)
                RC4_PlainText_use[i]=b[i];
            RC4_PlainText_use[RC4_Plaintext.length()]='\0';
            RC4(RC4_PlainText_use,RC4_key_use);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_RC4);
            str_RC4="";
        }else if(which==8)//DES
        {
            char DES_PlainText_8[9],DES_key_8[9];
            QString DES_key=ui->key_DES->text();
            QString DES_Plaintext=ui->Plaintext->toPlainText();
            int DES_long_p=DES_Plaintext.length();
            while(DES_long_p%8!=0)
            {
                DES_Plaintext+='0';
                DES_long_p=DES_Plaintext.length();
            }
            QByteArray ba = DES_key.toLatin1();
            char *a=ba.data();
            for(i=0;i<8;i++)
                DES_key_8[i]=a[i];
            DES_key_8[8]='\0';
            ba = DES_Plaintext.toLatin1();
            char *b=ba.data();
            int z=0;
            while(DES_long_p/8!=0)
            {
                for(i=0;i<8;i++)
                    DES_PlainText_8[i]=b[i+z*8];
                DES_PlainText_8[8]='\0';
                DES_long_p=DES_long_p-8;
                z++;
                DES(DES_key_8,DES_PlainText_8,2);
            }
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_DES);
            str_DES="";
        }else if(which==9)//RSA
        {
            char RSA_PlainText_use[100];
            str_RSA_d=ui->d->text();
            str_RSA_n=ui->n_2->text();
            QString RSA_Plaintext=ui->Plaintext->toPlainText();
            QByteArray RSA_ba = RSA_Plaintext.toLatin1();
            char *b=RSA_ba.data();
            for(i=0;i<RSA_Plaintext.length();i++)
                RSA_PlainText_use[i]=b[i];
            RSA_PlainText_use[RSA_Plaintext.length()-1]='\0';
            resultt=Chartoint(RSA_PlainText_use);
            TestRSA(RSA_PlainText_use,2);
            ui->Ciphertext->setText(str_RSA);
            str_RSA="";
        }else if(which==10)//DH
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->DH_2->text();
            QString Caesar_Plaintext=ui->Plaintext->toPlainText();
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,2);
            ui->Ciphertext->setText("");
            ui->Ciphertext->append(str_Caesar);
            str_Caesar="";
        }
    }else if(str_file!=""){
        char lu_use[100];
        str_file_lu_1=ui->lu->text();
        QByteArray file_ba = str_file_lu_1.toLatin1();
        char *a=file_ba.data();
        for(i=0;i<str_file_lu_1.length();i++)
            lu_use[i]=a[i];
        lu_use[i]='\0';
//        cout<<lu_use<<endl;
        Read(lu_use);
        if(which==0)//Caesar
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->key_Caesar->text();
            QString Caesar_Plaintext=str_filed;
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Caesar,2);
            str_Caesar="";
        }else if(which==1)//Keyword
        {
            char Keyword_PlainText_use[100],Keyword_key_use[100];
            QString Keyword_key=ui->key_keyword->text();
            QString Keyword_Plaintext=str_filed;
            QByteArray Keyword_ba = Keyword_key.toLatin1();
            char *a=Keyword_ba.data();
            for(i=0;i<Keyword_key.length();i++)
                Keyword_key_use[i]=a[i];
            Keyword_key_use[Keyword_key.length()]='\0';
            Keyword_ba = Keyword_Plaintext.toLatin1();
            char *b=Keyword_ba.data();
            for(i=0;i<Keyword_Plaintext.length();i++)
                Keyword_PlainText_use[i]=b[i];
            Keyword_PlainText_use[Keyword_Plaintext.length()]='\0';
            keyword(Keyword_PlainText_use,Keyword_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_keyword,2);
            str_keyword="";
        }else if(which==2)//Vigenere
        {
            char Vigenere_PlainText_use[100],Vigenere_key_use[100];
            QString Vigenere_key=ui->key_Vigenere->text();
            QString Vigenere_Plaintext=str_filed;
            QByteArray Vigenere_ba = Vigenere_key.toLatin1();
            char *a=Vigenere_ba.data();
            for(i=0;i<Vigenere_key.length();i++)
                Vigenere_key_use[i]=a[i];
            Vigenere_key_use[Vigenere_key.length()]='\0';
            Vigenere_ba = Vigenere_Plaintext.toLatin1();
            char *b=Vigenere_ba.data();
            for(i=0;i<Vigenere_Plaintext.length();i++)
                Vigenere_PlainText_use[i]=b[i];
            Vigenere_PlainText_use[Vigenere_Plaintext.length()]='\0';
            Vigenere(Vigenere_PlainText_use,Vigenere_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Vigenere,2);
            str_Vigenere="";
        }else if(which==3)//Autokey
        {
            char Autokey_PlainText_use[100],Autokey_key_use[100];
            QString Autokey_key=ui->key_Autokey->text();
            QString Autokey_Plaintext=str_filed;
            QByteArray Autokey_ba = Autokey_key.toLatin1();
            char *a=Autokey_ba.data();
            for(i=0;i<Autokey_key.length();i++)
                Autokey_key_use[i]=a[i];
            Autokey_key_use[Autokey_key.length()]='\0';
            Autokey_ba = Autokey_Plaintext.toLatin1();
            char *b=Autokey_ba.data();
            for(i=0;i<Autokey_Plaintext.length();i++)
                Autokey_PlainText_use[i]=b[i];
            Autokey_PlainText_use[Autokey_Plaintext.length()]='\0';
            Autokey(Autokey_PlainText_use,Autokey_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Autokey,2);
            str_Autokey="";
        }else if(which==4)//Playfair
        {
            char Playfair_PlainText_use[100],Playfair_key_use[100];
            QString Playfair_key=ui->key_Playfair->text();
            QString Playfair_Plaintext=str_filed;
            QByteArray Playfair_ba = Playfair_key.toLatin1();
            char *a=Playfair_ba.data();
            for(i=0;i<Playfair_key.length();i++)
                Playfair_key_use[i]=a[i];
            Playfair_key_use[Playfair_key.length()]='\0';
            Playfair_ba = Playfair_Plaintext.toLatin1();
            char *b=Playfair_ba.data();
            for(i=0;i<Playfair_Plaintext.length();i++)
                Playfair_PlainText_use[i]=b[i];
            Playfair_PlainText_use[Playfair_Plaintext.length()]='\0';
            Playfair(Playfair_PlainText_use,Playfair_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Playfair,2);
            str_Playfair="";
        }else if(which==5)//Permutation
        {
            char Permutation_PlainText_use[100],Permutation_key_use[100];
            QString Permutation_key=ui->key_per->text();
            QString Permutation_Plaintext=str_filed;
            QByteArray Permutation_ba = Permutation_key.toLatin1();
            char *a=Permutation_ba.data();
            for(i=0;i<Permutation_key.length();i++)
                Permutation_key_use[i]=a[i];
            Permutation_key_use[Permutation_key.length()]='\0';
            Permutation_ba = Permutation_Plaintext.toLatin1();
            char *b=Permutation_ba.data();
            for(i=0;i<Permutation_Plaintext.length();i++)
                Permutation_PlainText_use[i]=b[i];
            Permutation_PlainText_use[Permutation_Plaintext.length()]='\0';
            Permutation_de(Permutation_PlainText_use,Permutation_key_use);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Permutation,2);
            str_Permutation="";
        }else if(which==6)//C_Permutation
        {
            char C_Permutation_PlainText_use[100],C_Permutation_key_use[100];
            QString C_Permutation_key=ui->key_C_per->text();
            QString C_Permutation_Plaintext=str_filed;
            QByteArray C_Permutation_ba = C_Permutation_key.toLatin1();
            char *a=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_key.length();i++)
                C_Permutation_key_use[i]=a[i];
            C_Permutation_key_use[C_Permutation_key.length()]='\0';
            C_Permutation_ba = C_Permutation_Plaintext.toLatin1();
            char *b=C_Permutation_ba.data();
            for(i=0;i<C_Permutation_Plaintext.length();i++)
                C_Permutation_PlainText_use[i]=b[i];
            C_Permutation_PlainText_use[C_Permutation_Plaintext.length()]='\0';
            C_Permutation(C_Permutation_PlainText_use,C_Permutation_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_C_Permutation,2);
            str_C_Permutation="";
        }else if(which==7)//RC4
        {
            char RC4_PlainText_use[100],RC4_key_use[]="123456";
            QString RC4_Plaintext=str_filed;
            QByteArray RC4_ba = RC4_Plaintext.toLatin1();
            char *b=RC4_ba.data();
            for(i=0;i<RC4_Plaintext.length();i++)
                RC4_PlainText_use[i]=b[i];
            RC4_PlainText_use[RC4_Plaintext.length()]='\0';
            RC4(RC4_PlainText_use,RC4_key_use);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_RC4,2);
            str_RC4="";
        }else if(which==8)//DES
        {
            char DES_PlainText_8[9],DES_key_8[9];
            QString DES_key=ui->key_DES->text();
            QString DES_Plaintext=str_filed;
            int DES_long_p=DES_Plaintext.length();
            while(DES_long_p%8!=0)
            {
                DES_Plaintext+='0';
                DES_long_p=DES_Plaintext.length();
            }
            QByteArray ba = DES_key.toLatin1();
            char *a=ba.data();
            for(i=0;i<8;i++)
                DES_key_8[i]=a[i];
            DES_key_8[8]='\0';
            ba = DES_Plaintext.toLatin1();
            char *b=ba.data();
            int z=0;
            while(DES_long_p/8!=0)
            {
                for(i=0;i<8;i++)
                    DES_PlainText_8[i]=b[i+z*8];
                DES_PlainText_8[8]='\0';
                DES_long_p=DES_long_p-8;
                z++;
                DES(DES_key_8,DES_PlainText_8,2);
            }
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_DES,2);
            str_DES="";
        }else if(which==9)//RSA
        {
            char RSA_PlainText_use[100];
            str_RSA_d=ui->d->text();
            str_RSA_n=ui->n_2->text();
            QString RSA_Plaintext=str_filed;
            QByteArray RSA_ba = RSA_Plaintext.toLatin1();
            char *b=RSA_ba.data();
            for(i=0;i<RSA_Plaintext.length();i++)
                RSA_PlainText_use[i]=b[i];
            RSA_PlainText_use[RSA_Plaintext.length()-1]='\0';
            resultt=Chartoint(RSA_PlainText_use);
            TestRSA(RSA_PlainText_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_RSA,2);
            str_RSA="";
        }else if(which==10)//DH
        {
            char Caesar_PlainText_use[100];
            int Caesar_key_use;
            QString Caesar_key=ui->DH_2->text();
            QString Caesar_Plaintext=str_filed;
            Caesar_key_use=Caesar_key.toInt();
            QByteArray Caesar_ba = Caesar_Plaintext.toLatin1();
            char *b=Caesar_ba.data();
            for(i=0;i<Caesar_Plaintext.length();i++)
                Caesar_PlainText_use[i]=b[i];
            Caesar_PlainText_use[Caesar_Plaintext.length()]='\0';
            Caesar(Caesar_PlainText_use,Caesar_key_use,2);
            ui->Ciphertext->setText("Success!C:/Plaintext.txt");
            file_write(str_Caesar,2);
            str_Caesar="";
        }
    }
}

void MainWindow::on_Clean_clicked()
{
    ui->Plaintext->setText("");
    ui->Ciphertext->setText("");
    ui->key_Autokey->setText("");
    ui->key_Caesar->setText("");
    ui->key_C_per->setText("");
    ui->key_DES->setText("");
    ui->key_keyword->setText("");
    ui->key_per->setText("");
    ui->key_Playfair->setText("");
    ui->key_Vigenere->setText("");
    ui->lu->setText("");
    ui->e->setText("");
    ui->d->setText("");
    ui->n_1->setText("");
    ui->n_2->setText("");
    ui->DH_1->setText("");
    ui->DH_2->setText("");
    ui->DH_g->setText("");
    ui->DH_p->setText("");
}
//-----------------------------------------------------
//Socket按键
//-----------------------------------------------------
void MainWindow::on_Socket_clicked()
{
    if( WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )//初始化
    {
        str_server_temp.sprintf( "initializationing error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    if( ( iServerSock = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
    {
        str_server_temp.sprintf( "error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons( PORT );//监视的端口号
    ServerAddr.sin_addr.s_addr = INADDR_ANY;//本地IP
    memset( & ( ServerAddr.sin_zero ), 0, sizeof( ServerAddr.sin_zero ) );


    if( bind( iServerSock, ( struct sockaddr * )&ServerAddr, sizeof( struct sockaddr ) ) == -1 )
    {
        str_server_temp.sprintf( "bind error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    if( listen( iServerSock, BACKLOG ) == -1 )
    {
        str_server_temp.sprintf( "listen error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    sin_size = sizeof( struct sockaddr_in );
    iClientSock = accept( iServerSock, ( struct sockaddr * )&ClientAddr, &sin_size );

    if( iClientSock == -1 )
    {
        str_server_temp.sprintf( "accept error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }
    str_server_temp.sprintf("%s", inet_ntoa( ClientAddr.sin_addr ) );
    ui->IP->setText(str_server_temp);
    int_socket=1;
}

void MainWindow::on_connect_clicked()
{
    QString str_IP=ui->IP->text();
    QByteArray IP_ba = str_IP.toLatin1();
    char *ip=IP_ba.data();
    if( WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )//初始化
    {
        str_server_temp.sprintf( "initializationing error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    if( ( iClientSock = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
    {
        str_server_temp.sprintf( "error!\n" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons( PORT );
    ServerAddr.sin_addr.s_addr = inet_addr(ip);
    memset( &( ServerAddr.sin_zero ), 0, sizeof( ServerAddr.sin_zero ) );

    if( ::connect( iClientSock, ( struct sockaddr * ) & ServerAddr, sizeof( struct sockaddr ) ) == -1 )
    {
        str_server_temp.sprintf( "connect error!" );
        ui->IP->setText(str_server_temp);
        WSACleanup( );
    }
    int_socket=1;
}

void MainWindow::on_Send_clicked()
{
    QString DH_1=ui->DH_1->text();
    QString str_file=ui->lu->text();
    if(int_socket==0){
        QMessageBox::warning(this,tr("Warnning"),tr("Please connect to the server!"),QMessageBox::Ok);
    }else if(int_socket==1&&str_file==""&&DH_1==""){
        //---------------
        if( send( iClientSock, "n", strlen( "n" ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
        numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
        if( numbytes == -1 )
        {
            str_server_temp.sprintf( "recv error!" );
            ui->IP->setText(str_server_temp);
            WSACleanup( );
        }
        buf[ numbytes ] = '\0';
        //---------------

        int i=0;
        QString Plaintext=ui->Ciphertext->toPlainText();
        QByteArray ba = Plaintext.toLatin1();
        char *b=ba.data();
        for(i=0;i<Plaintext.length();i++)
            buf[i]=b[i];
        buf[Plaintext.length()]='\0';
        if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
    }else if(int_socket==1&&str_file!=""&&DH_1==""){
        //---------------
        if( send( iClientSock, "y", strlen( "y" ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
        numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
        if( numbytes == -1 )
        {
            str_server_temp.sprintf( "recv error!" );
            ui->IP->setText(str_server_temp);
            WSACleanup( );
        }
        buf[ numbytes ] = '\0';
        //---------------
        Read("C:/Ciphertext.txt");
        int i=0;
        QByteArray ba = str_filed.toLatin1();
        char *b=ba.data();
        for(i=0;i<str_filed.length();i++)
            buf[i]=b[i];
        buf[str_filed.length()]='\0';
        if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
    }else if(int_socket==1&&DH_1!=""){
        DH_p=RandomPrime(20);
        DH_g=g_Rnd.Random(100000);
        DH_a=DH_1.toULongLong();
        DH_AtoB=PowMod(DH_g,DH_a,DH_p);
        QString str_DH_p=QString::number(DH_p);
        QString str_DH_g=QString::number(DH_g);
        QString str_DH_a=QString::number(DH_AtoB);
        int i=0;
        QByteArray ba;
        //---------------------
        ba = str_DH_a.toLatin1();
        char *a=ba.data();
        for(i=0;i<str_DH_a.length();i++)
            buf[i]=a[i];
        buf[str_DH_a.length()]='\0';
        if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
        //---------------------
        numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
        if( numbytes == -1 )
        {
            str_server_temp.sprintf( "recv error!" );
            ui->IP->setText(str_server_temp);
            WSACleanup( );
        }
        buf[ numbytes ] = '\0';
        //---------------------
        ba = str_DH_p.toLatin1();
        char *p=ba.data();
        for(i=0;i<str_DH_p.length();i++)
            buf[i]=p[i];
        buf[str_DH_p.length()]='\0';
        if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
        //---------------------
        numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
        if( numbytes == -1 )
        {
            str_server_temp.sprintf( "recv error!" );
            ui->IP->setText(str_server_temp);
            WSACleanup( );
        }
        buf[ numbytes ] = '\0';
        //---------------------
        ba = str_DH_g.toLatin1();
        char *g=ba.data();
        for(i=0;i<str_DH_g.length();i++)
            buf[i]=g[i];
        buf[str_DH_g.length()]='\0';
        if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
        {
            str_server_temp.sprintf( "send error!" );
            ui->IP->setText(str_server_temp);
            closesocket( iClientSock );
            WSACleanup( );
        }
        //---------------------
        numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
        if( numbytes == -1 )
        {
            str_server_temp.sprintf( "recv error!" );
            ui->IP->setText(str_server_temp);
            WSACleanup( );
        }
        buf[ numbytes ] = '\0';
        QString str_DH_b=QString(buf);
        DH_BtoA=str_DH_b.toInt();
        //---------------------
        DH_result_A=PowMod(DH_BtoA,DH_a,DH_p);
        QString str_DH_result=QString::number(DH_result_A);
        ui->DH_2->setText(str_DH_result);
        ui->DH_p->setText(str_DH_p);
        ui->DH_g->setText(str_DH_g);
        ui->DH_1->setText("");
    }
}

void MainWindow::on_Receive_clicked()
{
    QString DH_1=ui->DH_1->text();
    if(int_socket==0){
        QMessageBox::warning(this,tr("Warnning"),tr("Please connect to the server!"),QMessageBox::Ok);
    }else if(int_socket==1){
        if(DH_1==""){
            //--------------
            numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
            if( numbytes == -1 )
            {
                str_server_temp.sprintf( "recv error!" );
                ui->IP->setText(str_server_temp);
                WSACleanup( );
            }
            buf[ numbytes ] = '\0';
            if( send( iClientSock, "wait", strlen( "wait" ), 0 ) == -1 )
            {
                str_server_temp.sprintf( "send error!" );
                ui->IP->setText(str_server_temp);
                closesocket( iClientSock );
                WSACleanup( );
            }
            //--------------
            if(buf[0]=='n'){
                numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
                if( numbytes == -1 )
                {
                    str_server_temp.sprintf( "recv error!" );
                    ui->IP->setText(str_server_temp);
                    WSACleanup( );
                }
                buf[ numbytes ] = '\0';
                str_server.sprintf("%s",buf);
                ui->Plaintext->setText(str_server);
            }else if(buf[0]=='y'){
                numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
                if( numbytes == -1 )
                {
                    str_server_temp.sprintf( "recv error!" );
                    ui->IP->setText(str_server_temp);
                    WSACleanup( );
                }
                buf[ numbytes ] = '\0';
                file_write(buf,1);
                ui->lu->setText("C:/Ciphertext.txt");
                ui->Plaintext->setText("");
            }
        }else if(DH_1!=""){
            numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
            if( numbytes == -1 )
            {
                str_server_temp.sprintf( "recv error!" );
                ui->IP->setText(str_server_temp);
                WSACleanup( );
            }
            buf[ numbytes ] = '\0';
            QString str_DH_a=QString(buf);
            DH_AtoB=str_DH_a.toInt();
            //---------------------
            if( send( iClientSock, "wait", strlen( "wait" ), 0 ) == -1 )
            {
                str_server_temp.sprintf( "send error!" );
                ui->IP->setText(str_server_temp);
                closesocket( iClientSock );
                WSACleanup( );
            }
            //---------------------
            numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
            if( numbytes == -1 )
            {
                str_server_temp.sprintf( "recv error!" );
                ui->IP->setText(str_server_temp);
                WSACleanup( );
            }
            buf[ numbytes ] = '\0';
            QString str_DH_p=QString(buf);
            DH_p=str_DH_p.toInt();
            //---------------------
            if( send( iClientSock, "wait", strlen( "wait" ), 0 ) == -1 )
            {
                str_server_temp.sprintf( "send error!" );
                ui->IP->setText(str_server_temp);
                closesocket( iClientSock );
                WSACleanup( );
            }
            //---------------------
            numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );
            if( numbytes == -1 )
            {
                str_server_temp.sprintf( "recv error!" );
                ui->IP->setText(str_server_temp);
                WSACleanup( );
            }
            buf[ numbytes ] = '\0';
            QString str_DH_g=QString(buf);
            DH_g=str_DH_g.toInt();
            //---------------------
            DH_b=DH_1.toULongLong();
            DH_BtoA=PowMod(DH_g,DH_b,DH_p);
            QString str_DH_b=QString::number(DH_BtoA);
            int i=0;
            QByteArray ba;
            //---------------------
            ba = str_DH_b.toLatin1();
            char *p=ba.data();
            for(i=0;i<str_DH_b.length();i++)
                buf[i]=p[i];
            buf[str_DH_b.length()]='\0';
            if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
            {
                str_server_temp.sprintf( "send error!" );
                ui->IP->setText(str_server_temp);
                closesocket( iClientSock );
                WSACleanup( );
            }
            DH_result_B=PowMod(DH_AtoB,DH_b,DH_p);
            QString str_DH_result=QString::number(DH_result_B);
            ui->DH_2->setText(str_DH_result);
            ui->DH_p->setText(str_DH_p);
            ui->DH_g->setText(str_DH_g);
            ui->DH_1->setText("");
        }
    }
}

void MainWindow::on_disconnect_clicked()
{
    if(int_socket==0){
        QMessageBox::warning(this,tr("Warnning"),tr("Please connect to the server!"),QMessageBox::Ok);
    }else if(int_socket==1){
        closesocket( iClientSock );
        WSACleanup( );
        ui->IP->setText("Disconnected!");
        int_socket=0;
    }
}


