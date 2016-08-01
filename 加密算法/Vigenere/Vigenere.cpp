#include<iostream>
#include<String>
using namespace std;

int main()
{
	int i,j,v=-1,P_len;
	char Plaintext[100];
	char Ciphertext[100];
	string key;
	//---------------------Vigenere 表---------------------
	char a[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char table[26][26];
	for(i=0;i<26;i++)
	{
		for(j=0;j<26;j++)
			table[i][j]=a[(i+j)%26];
	}
//	cout<<endl<<endl<<"	---------------欢迎使用Vigenere加解密系统---------------"<<endl<<endl;
	while(v!=0)
	{
		if(v!=-1)
			cout<<"按任意键继续:）"<<endl;
		fflush(stdin);
		getchar();
		system ("cls");
		cout<<endl<<endl<<"	---------------使用Vigenere加密输入1---------------"<<endl;
		cout<<"	---------------使用Vigenere解密输入2---------------"<<endl;
		cout<<"	---------------退出程序输入0        ---------------"<<endl<<"	";
		cin>>v;
		while(v!=0&&v!=1&&v!=2)
		{
			cout<<"输入错误，重新输入：";
			cin>>v;
		}
		if(v==1)
		{
			cout<<"输入plaintext："<<endl;
			cin>>Plaintext;
			P_len=strlen(Plaintext);
			cout<<"输入key："<<endl;
			cin>>key;
			cout<<endl<<"加密完ciphertext："<<endl;
			i=0;
			//---------------------加密---------------------
			while(i!=P_len)
			{
				cout<<table[(int)key[i%key.size()]-97][(int)Plaintext[i]-97];					//索引Vigenere 表
				Ciphertext[i]=table[(int)key[i%key.size()]-97][(int)Plaintext[i]-97];
				i++;
			}
			cout<<endl;
		}
		else if(v==2)
		{
			cout<<"输入Ciphertext："<<endl;
			cin>>Ciphertext;
			P_len=strlen(Ciphertext);
			cout<<"输入key："<<endl;
			cin>>key;
			cout<<endl<<"解密完plaintext："<<endl;
			i=0;
			j=0;
			//---------------------Vigenere 表---------------------
			while(j!=P_len)
			{
				if(Ciphertext[j]==table[(int)key[j%key.size()]-97][i])							//索引Vigenere 表逆操作
				{
					cout<<table[0][i];
					j++;
					i=0;
				}
				else
					i++;
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<endl;
	}
	cout<<"欢迎使用:）"<<endl;
	return 0;
}