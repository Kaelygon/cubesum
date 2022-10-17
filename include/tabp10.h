//table of power 10s
static const __uint128_t tabp10[40]={ //1, 10, 100 ... 10^39
	1,	
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
		(n < tabp10[1] ?  0 :   
        (n < tabp10[2] ?  1 :   
        (n < tabp10[3] ?  2 :   
        (n < tabp10[4] ?  3 :   
        (n < tabp10[5] ?  4 :   
        (n < tabp10[6] ?  5 :   
        (n < tabp10[7] ?  6 :  
        (n < tabp10[8] ?  7 :  
        (n < tabp10[9] ?  8 :  
		(n < tabp10[10] ?  9 :   
        (n < tabp10[11] ? 10 :   
        (n < tabp10[12] ? 11 :   
        (n < tabp10[13] ? 12 :   
        (n < tabp10[14] ? 13 :   
        (n < tabp10[15] ? 14 :   
        (n < tabp10[16] ? 15 :  
        (n < tabp10[17] ? 16 :  
        (n < tabp10[18] ? 17 :  
		(n < tabp10[19] ? 18 :   
        (n < tabp10[20] ? 19 :   
        (n < tabp10[21] ? 20 :   
        (n < tabp10[22] ? 21 :   
        (n < tabp10[23] ? 22 :   
        (n < tabp10[24] ? 23 :   
        (n < tabp10[25] ? 24 :  
        (n < tabp10[26] ? 25 :  
        (n < tabp10[27] ? 26 :  
		(n < tabp10[28] ? 27 :   
        (n < tabp10[29] ? 28 :   
        (n < tabp10[30] ? 29 :   
        (n < tabp10[31] ? 30 :   
        (n < tabp10[32] ? 31 :   
        (n < tabp10[33] ? 32 :   
        (n < tabp10[34] ? 33 :  
        (n < tabp10[35] ? 34 :  
        (n < tabp10[36] ? 35 :  
		(n < tabp10[37] ? 36 :   
        (n < tabp10[38] ? 37 :
        	 tabp10[39]
		))))))))))))))))))))))))))))))))))))))
	;
}

template <class T>
inline T ui128pow10(T n) 
{
	return tabp10[n];
}