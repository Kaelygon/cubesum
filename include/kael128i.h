#define MAX__UINT128 (__uint128_t)(-1)
#define MAX__UINT64 (__uint64_t)(-1)

using namespace std;

//128 - (count leading zeros ui128) + 2 
inline int iclz_ui128 (__uint128_t u) {
	uint64_t hi = u>>64;
	uint64_t lo = u;

	return  
		hi ? 
		 	lo&hi ?  66-__builtin_clzll(lo) : 2								//hi true  -> lo&hi ? 1 : 0
			:
			lo&hi ? 130-__builtin_clzll(hi) : 66-__builtin_clzll(lo)		//hi false -> lo&hi ? 2 : 1
	;
}

//unsigned integer quadruple cube root
__uint128_t uiqcbrt(__uint128_t n){

	__uint128_t r;
	int lz=iclz_ui128(n)/3;
	__uint128_t r0=1<<lz;

	do{ //quadratic iterations c = ctarg-(c^3-ctarg)/derivative(c^3-ctarg,c)
		r = r0;
		r0 = (2*r + n/(r*r))/3 ;
	}
	while (r0 < r);

	return (r*r*r==n) ? r : 0;
		
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
