//功能是处理文件：将文件里的内容读取到char A[]中；将char B[]中的内容写入到文件中
//两个函数：Write(char B[],File *f);Read(char A[],File *f);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void Read(char A[]);
int main()
{
    char B[100];
    char A[100];
    FILE *f1;

 
    //写入
    int i;

    printf("请输入B数组的内容：");
    scanf("%s",B);

    f1=fopen("I:/a.txt","w");
    for(i=0;B[i]!=NULL;i++)
    {
        fprintf(f1,"%c",B[i]);
    }
    fclose(f1);
    printf("信息成功写入文件！\n");


    Read(A);
    return 0;
}

//将文件f中的内容读到A中
void Read(char A[])
{
	FILE *f;
    int i=0;
    char c;
    char name[100];
    printf("请输入存有要显示内容的文件的名字:");
    scanf("%s",name);
    f=fopen(name,"r");
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
        printf("%c",A[i]);
    }
    fclose(f);

}
