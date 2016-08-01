#include<iostream>
#include<String>
using namespace std;
char Plaintext[100];
char Ciphertext[100];
int main()
{
	int i=0,j=0,key_numble[20]={0},len,len_P,m,n,x=0,k,v=-1;
	string key;
	cout<<endl<<endl<<"	---------------欢迎使用Column permutation加解密系统---------------"<<endl<<endl;
	while(v!=0)
	{
		if(v!=-1)
			cout<<endl<<endl<<"按任意键继续:）"<<endl;
		fflush(stdin);
		getchar();
		system ("cls");
		cout<<endl<<endl<<"	---------------使用Column permutation加密输入1---------------"<<endl;
		cout<<"	---------------使用Column permutation解密输入2---------------"<<endl;
		cout<<"	---------------       退出程序输入0           ---------------"<<endl<<"	";
		cin>>v;
		while(v!=0&&v!=1&&v!=2)
		{
			cout<<"输入错误，重新输入：";
			cin>>v;
		}
		if(v==1||v==2)
		{
			cout<<"输入keyword："<<endl;
			cin>>key;
			len=key.size();
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
		}
		if(v==1)
		{
			cout<<"输入Plaintext："<<endl;
			cin>>Plaintext;
			len_P=strlen(Plaintext);
			m=len_P/len;
			n=len_P%len;
			if(n!=0)														//明文字符不够数用"x"填充
			{
				m++;
				for(i=0;i<len-n;i++)
					Plaintext[len_P++]='x';
			}
			//---------Column permutation encryption核心代码---------
			for(i=0;i<len;i++)
			{
				k=key_numble[i]-1;
				for(j=0;j<m;j++)
					Ciphertext[x++]=Plaintext[j*len+k];
			}
			cout<<endl<<"keyword顺序为：";
			for(i=0;i<len;i++)
				cout<<key_numble[i]<<" ";
			cout<<endl<<endl<<"加密成功！密文为："<<endl<<Ciphertext<<endl;
		}
		else if(v==2)
		{
			cout<<"输入Ciphertext："<<endl;
			cin>>Ciphertext;
			len_P=strlen(Ciphertext);
			m=len_P/len;
			n=len_P%len;
			if(n!=0)														//明文字符不够数用"x"填充
			{
				m++;
				for(i=0;i<len-n;i++)
					Ciphertext[len_P++]='x';
			}
			//---------解密---------
			x=0;
			for(i=0;i<len;i++)
			{
				k=key_numble[i]-1;
				for(j=0;j<m;j++)
					Plaintext[j*len+k]=Ciphertext[x++];
			}
			cout<<endl<<"keyword顺序为：";
			for(i=0;i<len;i++)
				cout<<key_numble[i]<<" ";
			cout<<endl<<endl<<"解密成功！明文为："<<endl<<Plaintext<<endl;
		}
	}
	cout<<endl<<endl<<"欢迎使用:）"<<endl;
	return 0;
}
