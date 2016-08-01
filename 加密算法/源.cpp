#include <iostream>
#include <stdlib.h>

using namespace std;


unsigned _int64* Chartoint()
{
	char example[]="123 4568 123456 332222";
	int i=0,temp_int=0,j=0,k=0;
	int length_int=0;
	for(i=0;example[i]!='\0';i++)
	{	
		if(example[i]==' ')
			length_int++;
	}
	length_int++;
	unsigned _int64 *result=new unsigned _int64[length_int];
	
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
	
	
//	for(i=0;i<length_int;i++)		²âÊÔ
//		cout<<result[i]<<' ';

	return result;
}

int main()
{
	unsigned _int64 *result;
	
	result=Chartoint();


	system("pause");
	return 0;

}
