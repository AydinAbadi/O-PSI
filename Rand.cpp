/*
#include<iostream>
#include<sstream>
using namespace std;
#include<iomanip>
#include <string>
#include<cstring>
#include<iostream>
#include<fstream>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>

*/
#include "Rand.h"




void Random::get_rand_file( char* buf, int len, char* file )
{
	FILE* fp;
	char* p;
	fp = fopen(file, "r");

	p = buf;
	while( len )
	{
		size_t s;
		s = fread(p, 1, len, fp);
		p += s;
		len -= s;
	}


	fclose(fp);
}
void Random::get_rand_devurandom( char* buf, int len )
{
	//cout<<"Running........"<<endl;
	char*cg;
	string sg="/dev/urandom";
	cg=new char[sg.length()];
	strcpy(cg,sg.c_str());
	get_rand_file(buf, len,cg);	
}
void Random::init_rand3( gmp_randstate_t& rand, bigint ran, int bytes ){

	char* buf;
	mpz_t s;
	//ran=(paillier_random_seed*)malloc(sizeof(paillier_random_seed));
	buf = new char[bytes];
	get_rand_devurandom (buf, bytes);
	gmp_randinit_default(rand);
	mpz_init(s);
	mpz_init(ran);

	mpz_import(s, bytes, 1, 1, 0, 0, buf);
	mpz_init_set(ran,s);
	gmp_randseed(rand, s);
	mpz_clear(s);



	free(buf);
}



