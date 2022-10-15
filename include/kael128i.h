#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>

#define MAX__UINT128 (__uint128_t)(-1)
#define MAX__UINT64 (__uint64_t)(-1)

using namespace std;

template <class T>
inline T ui128pow( T m, T n) //128-bit pow
{
	T o=m;

	while(n>1){
		n--;
		o*=m;
	}
	return o;
}

//table of 10s
static const __uint128_t tabp10[39]={ //10, 100 ... 10^39
	ui128pow((__uint128_t)10,(__uint128_t)1),	
	ui128pow((__uint128_t)10,(__uint128_t)2),	
	ui128pow((__uint128_t)10,(__uint128_t)3),	
	ui128pow((__uint128_t)10,(__uint128_t)4),	
	ui128pow((__uint128_t)10,(__uint128_t)5),	
	ui128pow((__uint128_t)10,(__uint128_t)6),	
	ui128pow((__uint128_t)10,(__uint128_t)7),	
	ui128pow((__uint128_t)10,(__uint128_t)8),	
	ui128pow((__uint128_t)10,(__uint128_t)9),	
	ui128pow((__uint128_t)10,(__uint128_t)10),	
	ui128pow((__uint128_t)10,(__uint128_t)11),	
	ui128pow((__uint128_t)10,(__uint128_t)12),	
	ui128pow((__uint128_t)10,(__uint128_t)13),	
	ui128pow((__uint128_t)10,(__uint128_t)14),	
	ui128pow((__uint128_t)10,(__uint128_t)15),	
	ui128pow((__uint128_t)10,(__uint128_t)16),	
	ui128pow((__uint128_t)10,(__uint128_t)17),	
	ui128pow((__uint128_t)10,(__uint128_t)18),	
	ui128pow((__uint128_t)10,(__uint128_t)19),	
	ui128pow((__uint128_t)10,(__uint128_t)20),	
	ui128pow((__uint128_t)10,(__uint128_t)21),	
	ui128pow((__uint128_t)10,(__uint128_t)22),	
	ui128pow((__uint128_t)10,(__uint128_t)23),	
	ui128pow((__uint128_t)10,(__uint128_t)24),	
	ui128pow((__uint128_t)10,(__uint128_t)25),	
	ui128pow((__uint128_t)10,(__uint128_t)26),	
	ui128pow((__uint128_t)10,(__uint128_t)27),	
	ui128pow((__uint128_t)10,(__uint128_t)28),	
	ui128pow((__uint128_t)10,(__uint128_t)29),	
	ui128pow((__uint128_t)10,(__uint128_t)30),	
	ui128pow((__uint128_t)10,(__uint128_t)31),	
	ui128pow((__uint128_t)10,(__uint128_t)32),	
	ui128pow((__uint128_t)10,(__uint128_t)33),	
	ui128pow((__uint128_t)10,(__uint128_t)34),	
	ui128pow((__uint128_t)10,(__uint128_t)35),	
	ui128pow((__uint128_t)10,(__uint128_t)36),	
	ui128pow((__uint128_t)10,(__uint128_t)37),	
	ui128pow((__uint128_t)10,(__uint128_t)38),	
	ui128pow((__uint128_t)10,(__uint128_t)39)
};

//count digits, log10
inline int ui128log10(__uint128_t n)  
{  
    return 
		(n < tabp10[0] ? 0 :   
        (n < tabp10[1] ? 1 :   
        (n < tabp10[2] ? 2 :   
        (n < tabp10[3] ? 3 :   
        (n < tabp10[4] ? 4 :   
        (n < tabp10[5] ? 5 :   
        (n < tabp10[6] ? 6 :  
        (n < tabp10[7] ? 7 :  
        (n < tabp10[8] ? 8 :  
		(n < tabp10[9] ? 9 :   
        (n < tabp10[10] ? 10 :   
        (n < tabp10[11] ? 11 :   
        (n < tabp10[12] ? 12 :   
        (n < tabp10[13] ? 13 :   
        (n < tabp10[14] ? 14 :   
        (n < tabp10[15] ? 15 :  
        (n < tabp10[16] ? 16 :  
        (n < tabp10[17] ? 17 :  
		(n < tabp10[18] ? 18 :   
        (n < tabp10[19] ? 19 :   
        (n < tabp10[20] ? 20 :   
        (n < tabp10[21] ? 21 :   
        (n < tabp10[22] ? 22 :   
        (n < tabp10[23] ? 23 :   
        (n < tabp10[24] ? 24 :  
        (n < tabp10[25] ? 25 :  
        (n < tabp10[26] ? 26 :  
		(n < tabp10[27] ? 27 :   
        (n < tabp10[28] ? 28 :   
        (n < tabp10[29] ? 29 :   
        (n < tabp10[30] ? 30 :   
        (n < tabp10[31] ? 31 :   
        (n < tabp10[32] ? 32 :   
        (n < tabp10[33] ? 33 :  
        (n < tabp10[34] ? 34 :  
        (n < tabp10[35] ? 35 :  
		(n < tabp10[36] ? 36 :   
        (n < tabp10[37] ? 37 :
        	 tabp10[38]
		))))))))))))))))))))))))))))))))))))))
	;
}

template <class T>
inline T ui128pow10(T n) 
{
	return tabp10[n];
}

template <class T>
T fsqrt128(T N) //return floor(sqrt(N))
{ 
    T square = 1;
	T s=0;
	//find s that is s^3<N 
	for(__int64_t i=ui128log10(N)-2; i>=0; i--){ //test increment 10^i , 10^(i-1) ... 10^2, 10^1
		T inc = ui128pow10(i);
		while(s*s<N){
			s+=inc;
		}
		s-=inc;
	}
	while(square<N){ //1 increments
		s+=1;
		square=s*s;
	}
	if(s*s!=N){
		 s-=1;
	}
	return s;
	
}

inline int fastmod(const int input, const int ceil) {
    // apply the modulo operator only when needed
    // (i.e. when the input is greater than the ceiling)
    return input >= ceil ? input % ceil : input;
    // NB: the assumption here is that the numbers are positive
}

static inline int isodd(int x) { return x & 1; }

template <class T>
string ui128tos(T n){//128 uint to string
	if(n==0){return (string)"0";}
	
	string str = "";

	while(n){
		str+=to_string( (__uint8_t)(n%10) );
		n/=10;
	}
	
	string inv = "";
	for( int i=str.size()-1;i>=0;i-- ){
		inv+=str[i];
	}

	return inv;
}

//brute logarithmic search 128-bit cube (slower than binary search)
template <class T>
T bruteCube(T n)
{
    T cube = 1;
	T s=0;
	//find s that is s^3<N 
	for(__int64_t i=ui128log10(n)-2; i>=0; i--){ //test increment 10^i , 10^(i-1) ... 10^2, 10^1
		T inc = ui128pow10(i);
		while(s*s*s<n){
			s+=inc;
		}
		s-=inc;
	}
	while(cube<n){ //1 increments
		s+=1;
		cube=s*s*s;
	}
	if(s*s*s!=n){
		 return 0;
	}
	return cube;
}