#include <iostream>
using namespace std;
void Caesar(char p[],int key,int v);
int main()
{
	char p[100];
	int key;
	cin>>p;
	cin>>key;
	Caesar(p,key,1);
	return 0;
}
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
		 cout<<p;
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
		 cout<<p;
	}
}
