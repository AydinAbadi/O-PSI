#include<iostream>
using namespace std;
#include<cstring>
#include<iostream>
#include<fstream>
#include <stdlib.h>
#include "Rand.h"
typedef mpz_t bigint;



class Polynomial{

public: 
	Polynomial();
	Polynomial(bigint* elem, string poly_ID, bigint * xpoints,int elem_size, int xpoints_size, bigint pubmoduli); 
	bigint *  evaluate (bigint *,bigint * ,int,int,bigint);
	bigint * get_values (); 
	void   blind_poly (bigint , bigint); 
	void unblind_poly ( bigint , bigint);
	string get_poly_ID(){return poly_ID;};

private:
	bigint * values;  
	string  poly_ID; 

	int val_size;
};






