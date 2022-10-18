#include <string>

#define MAX__UINT128 (__uint128_t)(-1)
#define MAX__UINT64 (__uint64_t)(-1)

using namespace std;

//128 minus count leading zeros ui128 
inline int iclz_ui128 (__uint128_t u) {
	uint64_t hi = u>>64;
	uint64_t lo = u;

	return  
		hi ? 
		 	lo&hi ?  64-__builtin_clzll(lo) : 0 							//hi true  -> lo&hi ? 1 : 0
			:
			lo&hi ? 128-__builtin_clzll(hi) : 64-__builtin_clzll(lo)		//hi false -> lo&hi ? 2 : 1
	;
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
