#include<iostream>
#include"Server.h"
#include<sstream>
using namespace std;
#include<iostream>
#include<fstream>
//#include <openssl/rand.h>

#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
//#include <cryptopp/rsa.h>
//#include <cryptopp/dh.h>
//#include <cryptopp/sha.h>
//#include <cryptopp/aes.h>
//#include <cryptopp/cryptlib.h>
//#include <cryptopp/hex.h>
//#include <cryptopp/filters.h>
//#include <cryptopp/des.h>
//#include<cryptopp/integer.h>
//#include <cryptopp/files.h>
//#include <cryptopp/osrng.h>
//#include <cryptopp/randpool.h>
//#include <cryptopp/integer.h>
//#include <cryptopp/aes.h>
//#include <cryptopp/ccm.h>
#include <assert.h>

// pu_moduli size=512 bits

//for test only
Server::Server(){}
//

Server::Server(int num_xpoints, int pol_size,int pu_mod_bitsize)
{
	pu_moduli_bitsize=pu_mod_bitsize;
	xpoint_size=num_xpoints;
	xpoints= gen_randSet(num_xpoints,112);// 200 We can increase the x points bit-size
	/*for(int i=0;i<num_xpoints;i++){
		cout<<"num_xpoints["<<i<<"]"<<xpoints[i]<<endl;
	}*/
	pu_moduli=gen_randSet(1,pu_moduli_bitsize);// 512 the size of public moduli=max|element's domain|
	mpz_nextprime(pu_moduli[0],pu_moduli[0]);
	poly_size=pol_size;
	count=0;// no polynomial is stored
	poly=new Polynomial[pol_size];
}


bigint* Server::gen_randSet (int size, int max_bitsize){ // the 2nd argument allows us 1-to set xpoints less than public moduli 2-to set public moduli smaller than N for each clients so they do not need to compare it with N. 
	Random rd;
	mpz_t *pr_val;
	pr_val=(mpz_t*)malloc(size*sizeof(mpz_t));
	int max_bytesize=max_bitsize;
	gmp_randstate_t rand;
	bigint ran;
	//ran=(paillier_random_seed*)malloc(sizeof(paillier_random_seed));
	//init_rand2(rand, paillier_get_rand_devurandom ,ran, max_bytesize);
	rd.init_rand3(rand, ran, max_bytesize);

	for(int i=0;i<size;i++){
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,max_bitsize);// The last argument is in bit
	}
	return pr_val;
}


bigint* Server::get_xpoints(int&size){
	size=xpoint_size;
	bigint *ptr=xpoints;
	return ptr;
}


bigint*  Server::send_pubModuli(){
	bigint *ptr;
	ptr=(mpz_t*)malloc(1*sizeof(mpz_t));
	ptr=pu_moduli;
	return ptr;
}


void Server::set_poly(int index, Polynomial &p){
	poly[index]=p;
}




void Server::store_poly(Polynomial& p){
	if(count<=poly_size){
		if(p.get_poly_ID()=="B_ID")
			set_poly(0,p);
		else {++count;set_poly(count,p);}	
	}
	else {
		cout<<"No space to store anymore poly."<<endl;
		return;
	}
}



 Server_Result * Server::compute_result (GrantComp_Info * grantComp_info){
	
	// check whether all polynomial speified in grantComp_info have been already outsourced
	Server_Result*ptr;
	ptr=new Server_Result;
	int b_index=find_polyindex(grantComp_info->id[0]);
	int a_index=find_polyindex(grantComp_info->id[1]);

	if(b_index==1000000 || a_index==1000000){
		return ptr=NULL;

		//cout<<"in server: It's null"<<endl;
	}
	int size=grantComp_info->set_size;
	
	bigint *a,*b,*temp_a,*temp_b;
	a=(mpz_t*)malloc(size*sizeof(mpz_t));
	b=(mpz_t*)malloc(size*sizeof(mpz_t));
	temp_a=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	temp_b=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	//Generate random sets
	a=gen_randSet (size, 112); //XXXXXX to prevent overflow we pu_moduli_bitsize/5.
	b=gen_randSet (size, 112); //xxxx turn back to 50
	// Get two random polys.
	string s1="ID1";
	string s2="ID2";

	Polynomial pa(a,s1, xpoints, size,xpoint_size,pu_moduli[0]);
	Polynomial pb(b,s2, xpoints, size,xpoint_size,pu_moduli[0]);

	paillier_plaintext_t  *pl_b,*pl_a;
	paillier_ciphertext_t* cb,*ca,*ar,*t,*res;
	ar=new paillier_ciphertext_t[xpoint_size];
	t=new paillier_ciphertext_t;
	pl_b=new paillier_plaintext_t;
	pl_a=new paillier_plaintext_t;
	ca=new paillier_ciphertext_t;
	res=new paillier_ciphertext_t;
	//

	mpz_init(ca->c);
	for(int i=0; i< xpoint_size;i++){
		mpz_init(temp_b[i]);
		// multiply B's poly by a random poly
		Polynomial p1=get_poly(b_index);
		//mpz_mul(temp_b[i],pb.values[i],poly[b_index].values[i]);
		



		bigint *temp_p1=p1.get_values();
		bigint *temp_pb=pb.get_values();
		

		mpz_mul(temp_b[i],temp_pb[i],temp_p1[i]);
//mpz_mul(temp_b[i],pb.values[i],p1.values[i]);

		//mpz_mod(temp_b[i],temp_b[i],grantComp_info->pub_k->n); 
		mpz_mod(temp_b[i],temp_b[i],pu_moduli[0]);//xxxxxxxxxx

		// encrypt the element in temp_b[i] (encrypt B's poly)

		mpz_init_set(pl_b->m,temp_b[i]);
		cb=paillier_enc(0, grantComp_info->pub_k, pl_b, paillier_get_rand_devurandom);
		// multiply A's poly by a random poly
		mpz_init(temp_a[i]);
		Polynomial p2=get_poly(a_index);
		

		bigint *temp_p2=p2.get_values();
		bigint *temp_pa=pa.get_values();
		mpz_mul(temp_a[i],temp_pa[i],temp_p2[i]);
//mpz_mul(temp_a[i],pa.values[i],p2.values[i]);


		//mpz_mod(temp_a[i],temp_a[i],grantComp_info->pub_k->n);
		mpz_mod(temp_a[i],temp_a[i],pu_moduli[0]);//

		//Switch 
		mpz_init_set(pl_a->m,temp_a[i]);
		t=& grantComp_info->converter[i];
		paillier_exp(grantComp_info->pub_k,ca,t,pl_a);
		mpz_init(res->c);
		// Sum the plaintextes together or multiply the ciphertexts
	
		paillier_mul( grantComp_info->pub_k,res, ca,cb);

		mpz_init(ar[i].c);
		mpz_init_set(ar[i].c,res->c);	
	}

	ptr->result= ar;
	return ptr;
}
 int Server::find_polyindex(string id){
	 int i;
	 string s;
	 for(i=0;i<poly_size;i++){
		 Polynomial p=get_poly(i);
		 if(p.get_poly_ID()==id){
			// cout<<i<<endl;
			 return i;
			 }
	 }
	 if(i==poly_size){
		 cout<<"There is exist no poly. in server with ID:"<<id<<endl; 
		 
		 return i=1000000;// the value only is used to indicate NULL
	 }
}


// tester
//int main(){
//Server s(3,1,400);
//
//Server_Result*sr=new Server_Result;











	//bigint *el,*xp;
	//el= (mpz_t*)malloc(2*sizeof(mpz_t));
	//xp= (mpz_t*)malloc(5*sizeof(mpz_t));

	//mpz_init_set_str(el[0],"22",10);
	//mpz_init_set_str(el[1],"11",10);

	//mpz_init_set_str(xp[0],"44",10);
	//mpz_init_set_str(xp[1],"66",10);
	//mpz_init_set_str(xp[2],"58",10);
	//mpz_init_set_str(xp[3],"110",10);
	//mpz_init_set_str(xp[4],"112",10);
	//

	//string id="ID";
	//int size=2;
	//int x_size=5;
	//bigint mod;
	//mpz_init_set_str(mod,"251",10);
	//Polynomial pol(el,id,xp,size,x_size,mod);
	//s.store_poly(pol);
	//bigint* pl=s.poly[0].get_values();

//
//	return 0;
//
//}
