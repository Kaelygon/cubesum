//table of power 16s
static const __uint128_t tabp16[32]={ //1, 10, 100 ... 10^39
	1,	
	ui128pow((__uint128_t)16,(__uint128_t)1),	
	ui128pow((__uint128_t)16,(__uint128_t)2),	
	ui128pow((__uint128_t)16,(__uint128_t)3),	
	ui128pow((__uint128_t)16,(__uint128_t)4),	
	ui128pow((__uint128_t)16,(__uint128_t)5),	
	ui128pow((__uint128_t)16,(__uint128_t)6),	
	ui128pow((__uint128_t)16,(__uint128_t)7),	
	ui128pow((__uint128_t)16,(__uint128_t)8),	
	ui128pow((__uint128_t)16,(__uint128_t)9),	
	ui128pow((__uint128_t)16,(__uint128_t)10),	
	ui128pow((__uint128_t)16,(__uint128_t)11),	
	ui128pow((__uint128_t)16,(__uint128_t)12),	
	ui128pow((__uint128_t)16,(__uint128_t)13),	
	ui128pow((__uint128_t)16,(__uint128_t)14),	
	ui128pow((__uint128_t)16,(__uint128_t)15),	
	ui128pow((__uint128_t)16,(__uint128_t)16),	
	ui128pow((__uint128_t)16,(__uint128_t)17),	
	ui128pow((__uint128_t)16,(__uint128_t)18),	
	ui128pow((__uint128_t)16,(__uint128_t)19),	
	ui128pow((__uint128_t)16,(__uint128_t)20),	
	ui128pow((__uint128_t)16,(__uint128_t)21),	
	ui128pow((__uint128_t)16,(__uint128_t)22),	
	ui128pow((__uint128_t)16,(__uint128_t)23),	
	ui128pow((__uint128_t)16,(__uint128_t)24),	
	ui128pow((__uint128_t)16,(__uint128_t)25),	
	ui128pow((__uint128_t)16,(__uint128_t)26),	
	ui128pow((__uint128_t)16,(__uint128_t)27),	
	ui128pow((__uint128_t)16,(__uint128_t)28),	
	ui128pow((__uint128_t)16,(__uint128_t)29),	
	ui128pow((__uint128_t)16,(__uint128_t)30),	
	ui128pow((__uint128_t)16,(__uint128_t)31)
};

//count digits, log16
inline int ui128log16(__uint128_t n)  
{  
    return 
		(n < tabp16[1] ?  0 :   
        (n < tabp16[2] ?  1 :   
        (n < tabp16[3] ?  2 :   
        (n < tabp16[4] ?  3 :   
        (n < tabp16[5] ?  4 :   
        (n < tabp16[6] ?  5 :   
        (n < tabp16[7] ?  6 :  
        (n < tabp16[8] ?  7 :  
        (n < tabp16[9] ?  8 :  
		(n < tabp16[10] ? 9 :   
        (n < tabp16[11] ? 10 :   
        (n < tabp16[12] ? 11 :   
        (n < tabp16[13] ? 12 :   
        (n < tabp16[14] ? 13 :   
        (n < tabp16[15] ? 14 :   
        (n < tabp16[64] ? 15 :  
        (n < tabp16[17] ? 16 :  
        (n < tabp16[18] ? 17 :  
		(n < tabp16[19] ? 18 :   
        (n < tabp16[20] ? 19 :   
        (n < tabp16[21] ? 20 :   
        (n < tabp16[22] ? 21 :   
        (n < tabp16[23] ? 22 :   
        (n < tabp16[24] ? 23 :   
        (n < tabp16[25] ? 24 :  
        (n < tabp16[26] ? 25 :  
        (n < tabp16[27] ? 26 :  
		(n < tabp16[28] ? 27 :   
        (n < tabp16[29] ? 28 :   
        (n < tabp16[30] ? 29 : 
        	 tabp16[31]
		))))))))))))))))))))))))))))))
	;
}