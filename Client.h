#include<iostream>
using namespace std;
#include<string>

#include<cstring>

#include<fstream>
#include "Server.h"
#include <stdlib.h>
typedef mpz_t bigint;
#include <NTL/ZZ.h>
#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/ZZ_pEX.h>
#include <NTL/RR.h>
#include <NTL/tools.h>
using namespace NTL;

#include <NTL/vec_ZZVec.h>
#include <NTL/fileio.h>
#include <NTL/FacVec.h>
#include <NTL/new.h>
#include <vector>


class Client{

public:
	Client();
	Client (Server*serv, bigint *, int elem_size);
	void outsource_poly(string & poly_ID);
	CompPerm_Request * gen_compPerm_req();
	GrantComp_Info * grant_comp(CompPerm_Request * , bool);
	//string* find_intersection(Server_Result *,int& size);// size: number of roots found
	void find_intersection(Server_Result *,int& size);// size: number of roots found

private:
	//bigint* gen_PR_vals();
	//Polynomial* gen_poly(string poly_ID, bool blinded);
	void get_pubModuli(); 
	void get_xpoints(int&size);
	bigint seed;
	paillier_prvkey_t *sk;
	paillier_pubkey_t * pk;
	bigint *elem;
	int elem_size;
	string outpoly_ID;
	//bigint PR_vals;
	void gen_keys();
	bigint* interpolate(int size,bigint* a,bigint* b,bigint N);
	Server*serv;
	bigint * xpoints;
	bigint  pubmoduli;
	int xpoint_size;
};




