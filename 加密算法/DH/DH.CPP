#include <iostream>
#include <stdlib.h> 
#include <time.h>

using namespace std;

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

inline unsigned __int64 MulMod(unsigned __int64 a, 
							   unsigned __int64 b, 
							   unsigned __int64 n)
{ 
    return a * b % n;
}

/* 模幂运算，返回值 x=base^pow mod n */ 
unsigned __int64 PowMod(unsigned __int64 &base, 
						unsigned __int64 &pow, 
						unsigned __int64 &n)
{ 
    unsigned __int64    a=base, b=pow, c=1;
	while(b)
	{ 
        while(!(b & 1))
		{ 
            b>>=1;        /*a=a * a % n;    //函数看起来可以处理64位的整数，
							但由于这里a*a在a>=2^32时已经造成了溢出
								因此实际处理范围没有64位*/
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

//	cout<<"i'm in RabinMillerKnl\n";		//测试信息


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
		
		unsigned _int64 squ=2;
		v=PowMod(v, squ, n);		//X：将2改换为多一个变量   
        
		++i;			 //8、循环到5
	}

//	cout<<"i'm going out RabinMillerKnl\n";			//测试信息


	return 1;
}

/* Rabin-Miller素数测试，循环调用核心loop次 全部通过返回1，否则返回0 */ 
long RabinMiller(unsigned __int64 &n, long loop) { 
    //先用小素数筛选一次，提高效率

//	cout<<"i'm in RabinMiller\n";				//测试信息

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

//	cout<<"i'm in RandomPrime\n";				//测试信息


    unsigned __int64    base;
	do
	{ 
        base= (unsigned long)1 << (bits - 1);	   //保证最高位是1 
        
//		cout<<base<<"\t";					//测试信息

		base+=g_Rnd.Random(base);			  //再加上一个随机数
		base|=1;							 //保证最低位是1，即保证是奇数 
    } while(!RabinMiller(base, 30));		//进行拉宾－米勒测试30次
	return base;							//全部通过认为是素数
}

int main()
{
	unsigned _int64 DH_p,DH_g,DH_a,DH_b;

	unsigned _int64 DH_AtoB,DH_BtoA;

	unsigned _int64 DH_result_A,DH_result_B;

	DH_p=RandomPrime(20);
	DH_g=g_Rnd.Random(100000);
	cout<<"输入你自己私有的随机数字a:"<<endl;
	scanf("%u",&DH_a);
	cout<<"输入你自己私有的随机数字b:"<<endl;
	scanf("%u",&DH_b);




	DH_AtoB=PowMod(DH_g,DH_a,DH_p);
	DH_BtoA=PowMod(DH_g,DH_b,DH_p);

	DH_result_A=PowMod(DH_BtoA,DH_a,DH_p);
	DH_result_B=PowMod(DH_AtoB,DH_b,DH_p);

//	cout<<"DH_P="<<DH_p<<endl;
	printf("DH_P=%u\n",DH_p);
//	cout<<"DH_G="<<DH_g<<endl;
	printf("DH_G=%u\n",DH_g);
//	cout<<"DH_A="<<DH_a<<endl;
	printf("DH_A=%u\n",DH_a);
//	cout<<"DH_B="<<DH_b<<endl;
	printf("DH_B=%u\n",DH_b);
//	cout<<"DH_AtoB="<<DH_AtoB<<endl;
	printf("DH_AtoB=%u\n",DH_AtoB);
//	cout<<"DH_BtoA="<<DH_BtoA<<endl;
	printf("DH_BtoA=%u\n",DH_BtoA);
//	cout<<"DH_result_A="<<DH_result_A<<endl;
	printf("DH_result_A=%u\n",DH_result_A);
//	cout<<"DH_result_B="<<DH_result_B<<endl;
	printf("DH_result_B=%u\n",DH_result_B);

	system("pause");
	return 0;
}
