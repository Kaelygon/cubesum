#include <string>

#define MAX__UINT128 (__uint128_t)(-1)
#define MAX__UINT64 (__uint64_t)(-1)

using namespace std;

template <class T>
inline T ui128pow( T m, T n) //128-bit pow. Does not work for m^0
{
	T o=m;

	while(n>1){
		n--;
		o*=m;
	}
	return o;
}

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
