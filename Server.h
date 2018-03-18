#include<iostream>
using namespace std;
#include<string>
#include<gmp.h>
#include <gmpxx.h>
#include<cstring>
#include<iostream>
#include<fstream>
#include <stdlib.h>
#include"Polynomial.h"
typedef mpz_t bigint;


struct CompPerm_Request{
	paillier_ciphertext_t * enc_rand_vals;
	paillier_pubkey_t * pub_k;
	string id;
};

struct GrantComp_Info{
	paillier_ciphertext_t * converter;
	paillier_pubkey_t * pub_k;
	string *id;// the result reciepent id is id[i].
	int set_size;
};


struct Server_Result{ 
	paillier_ciphertext_t * result;
};


class Server{

public:
	Server();
	Server(int num_xpoints, int poly_size,int pu_moduli_bitsize); 
	bigint * get_xpoints (int& size); 
	 Server_Result * compute_result (GrantComp_Info * grantComp_info);
	bigint*  send_pubModuli(); 
	void store_poly(Polynomial&);
	void set_poly(int index, Polynomial &p);// it adds a new poly to its storage
	Polynomial get_poly(int index){return poly[index];}
	bigint * gen_randSet (int size,int max_bitsize); 
private:
	//Server_Result *gen_serverRes (paillier_ciphertext_t *, string *polyIDs); 
	//bigint*  gen_publicModuli (int bit_size);
	int find_polyindex(string id);

	Polynomial *poly;
	bigint * xpoints;
	int xpoint_size;
	bigint* pu_moduli;// |elemements' domain| <=|pu_moduli|< |N|
	int poly_size;
	int count;
	int pu_moduli_bitsize;
};




