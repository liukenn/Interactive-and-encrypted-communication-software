#include <iostream>
using namespace std;
void keyword(char p[],char key[],int v);
void key_str(char str[],char key_ABC[]);
int main()
{
	char p[100],key[20];
	cin>>p;
	cin>>key;
	keyword(p,key,2);
	return 0;
}
void keyword(char p[],char key[],int v)
{
	int i=0,j=0,n=strlen(p);
	char ABC[27]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char key_ABC[27]={0};
	key_str(key,key_ABC);//key_str作用：把key里有并且key_ABC里没有的写进key_ABC
	key_str(ABC,key_ABC);//key_str作用：把ABC里有并且key_ABC里没有的写进key_ABC
	if(v==1)
	{
		for(i=0;i<n;i++)
			for(j=0;j<26;j++)
				if(p[i]==ABC[j])
					cout<<key_ABC[j];
	}
	else if(v==2)
	{
		for(i=0;i<n;i++)
		{
			for(j=0;j<26;j++)
			{
				if(p[i]==key_ABC[j])
				{
					cout<<ABC[j];
				}
			}
		}
	}

}
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