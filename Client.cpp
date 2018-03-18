#include<iostream>
#include"Client.h"

#include<sstream>
using namespace std;
#include<iomanip>
#include <string>
#include<cstring>
#include<iostream>
#include<fstream>
//#include <openssl/rand.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <assert.h>

//for test only
Client::Client(){}
//

Client::Client(Server*server, bigint *elemenets, int el_size){

	serv=server;
	//set elements
	elem=(mpz_t*)malloc(el_size*sizeof(mpz_t));
	for(int i=0;i<el_size;i++){
		mpz_init_set(elem[i],elemenets[i]);
	}
	//keep size;
	elem_size=el_size;
	//Paillier keys and a seed are generated
	gen_keys();
	int size;
	get_xpoints(size);
	get_pubModuli();
}

void Client::gen_keys(){
	Random rd;
	//Generate secret & public keys
	paillier_pubkey_t* pu;//The public key
	paillier_prvkey_t* pr;//The private key 
	paillier_keygen(512, &pu,&pr, paillier_get_rand_devurandom ); 
	//set the keys
	pk=pu;
	sk=pr;
	//Gen seed for PRF
	gmp_randstate_t rand;
	bigint ran;
	//ran=(paillier_random_seed*)malloc(sizeof(paillier_random_seed));
	//init_rand2(rand, paillier_get_rand_devurandom ,ran,8);// the last parameter is in byte
	rd.init_rand3(rand, ran, 8);

	mpz_init_set(seed, ran);
}

void Client::get_xpoints(int&size){
	xpoints=serv->get_xpoints (size); 
	xpoint_size=size;
}

void Client::get_pubModuli(){
	bigint *ptr=(mpz_t*)malloc(1*sizeof(mpz_t));
	ptr=serv->send_pubModuli();
	mpz_init_set(pubmoduli, ptr[0]);
}


void Client::outsource_poly(string & poly_ID){
	outpoly_ID=poly_ID;
	Polynomial poly(elem, poly_ID, xpoints,elem_size, xpoint_size,pubmoduli);
	poly.blind_poly(seed,pubmoduli);
	serv->store_poly(poly); 
}

CompPerm_Request * Client::gen_compPerm_req(){

	CompPerm_Request* ptr;
	ptr=new CompPerm_Request;
	ptr->pub_k=pk;
	// regen seed then encrypt them
	bigint *pr_val;
	paillier_plaintext_t  *pl;
	paillier_ciphertext_t* ca,*ar;

	////for test only
	//paillier_ciphertext_t*temp;
	//temp=new paillier_ciphertext_t;
	//paillier_plaintext_t  *pl2;
	//pl2=new paillier_plaintext_t;
	//------------------
	pr_val=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	ar=new paillier_ciphertext_t[xpoint_size];

	pl=new paillier_plaintext_t;
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	for(int i=0;i<xpoint_size;i++){
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,100);// PR vals size:128 bits
	// here each pr_val[i] must be encrypted
		mpz_init_set(pl->m,pr_val[i]);
		ca=paillier_enc(0, pk, pl, paillier_get_rand_devurandom);
		mpz_init_set(ar[i].c,ca->c);
	}
	CompPerm_Request * req;
	req=new CompPerm_Request;
	req->enc_rand_vals=ar;
	req->pub_k=pk;
	req->id=outpoly_ID;
	return req;	
}

GrantComp_Info * Client::grant_comp(CompPerm_Request * com_req, bool accept){
//void Client::grant_comp(CompPerm_Request * com_req, bool accept){

	GrantComp_Info * ptr;
	bigint temp;
	mpz_init(temp);
	paillier_plaintext_t  *pl;
	pl=new paillier_plaintext_t;

	paillier_ciphertext_t* res,*ca,*temp2;
	temp2=new paillier_ciphertext_t;
	ca=new paillier_ciphertext_t;
	res=new paillier_ciphertext_t[xpoint_size];
	ptr=new GrantComp_Info;
	ptr->pub_k=com_req->pub_k;
	if(!accept){
		ptr=NULL;
		return ptr;
	}
		gmp_randstate_t rand;
		gmp_randinit_default(rand);
		gmp_randseed(rand,seed);
	for(int i=0;i<xpoint_size;i++){
		mpz_urandomb(temp,rand,100);
		mpz_invert(temp,temp,pubmoduli);
		mpz_init_set(pl->m,temp);
		temp2=&com_req->enc_rand_vals[i];
		mpz_init(ca->c);
		paillier_exp(com_req->pub_k,ca,temp2,pl);
		mpz_init_set(res[i].c,ca->c);
	}
	ptr->converter=res;
	ptr->id=new string[2];
	ptr->id[0]=com_req->id;
	ptr->id[1]=outpoly_ID;
	ptr->set_size=elem_size;
	return ptr;
}


void Client::find_intersection(Server_Result * res,int& size){
	cout<<"\n----------\nFind intersection"<<endl;
	bigint * temp,*pr_val;
	temp=new bigint[xpoint_size];
	pr_val=new bigint[xpoint_size];
		 ofstream wr("roots.txt");

	paillier_plaintext_t  *pl;
	paillier_ciphertext_t* ca;
	pl= new paillier_plaintext_t;
	ca=new paillier_ciphertext_t;
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	double start00=omp_get_wtime();

	for(int i=0;i<xpoint_size;i++){
		ca=& res->result[i];
		mpz_init(pl->m);
		//Decrypt
		paillier_dec(pl,pk,sk,ca);
		mpz_init_set(temp[i],pl->m);
		//Regenete PR_val
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,100);// PR vals size:128 bits
		//Compute mult_inverse of PR_Val
		mpz_invert(pr_val[i], pr_val[i],pubmoduli);
		// Remove PR_val from decrypted val
		mpz_mul(temp[i],pr_val[i],temp[i]);
		mpz_mod(temp[i],temp[i],pubmoduli);	
	}

	double end00=omp_get_wtime();
	cout<<"\nDecryption and unblinding:"<<end00-start00<<endl;

		double start0=omp_get_wtime();

	bigint *values=interpolate(xpoint_size,xpoints,temp,pubmoduli);
		double end0=omp_get_wtime();
		cout<<"Interpolation:"<<end0-start0<<endl;

	bigint zero;
	mpz_init_set_str(zero,"0",10);
	int count=0;
	for(int i=xpoint_size-1;i>=0;i--){
	if(mpz_cmp(values[i],zero)==0)
		count++;
	exit;
	}

	bigint one;
	mpz_init_set_str(one,"1",10);
	if(mpz_cmp(values[xpoint_size-count-1],one)>0){
	bigint temp2;
	mpz_init(temp2);
	mpz_invert(temp2,values[xpoint_size-count-1],pubmoduli);
	//cout<<"\ninv:"<<temp<<endl;
	for(int i=0;i<xpoint_size-count;i++){
		mpz_mul(values[i],values[i],temp2);
		mpz_mod(values[i],values[i],pubmoduli);
	}
	}

	char * tmp_mod = mpz_get_str(NULL,10,pubmoduli);
	string s_mod=tmp_mod;
	char *cc;
	cc=new char[s_mod.length()];
	strcpy(cc,s_mod.c_str());
	ZZ p=to_ZZ(cc);
	ZZ_p::init(p);
	ZZ_pX P;


	for(int i=0;i<xpoint_size;i++){
	char * tmp = mpz_get_str(NULL,10,values[i]);
	string s=tmp;
	char *c;
	c=new char[s.length()];
	strcpy(c,s.c_str());
	ZZ_p dd=to_ZZ_p(conv<ZZ> (c));
	SetCoeff(P,i,dd);
	}
	//cout<<"ppppP:"<<P<<endl;
	Vec< Pair< ZZ_pX, long > > factors;
	double start=omp_get_wtime();
	CanZass(factors, P); 
	double end1=omp_get_wtime();
	cout<<" Factoring:"<<end1-start<<endl;
	vec_ZZ_p root;

	//string* sd;
	//sd=new string[factors.length()];
	int cn=0;
	double start2=omp_get_wtime();

	for(int i=0;i<factors.length() ;i++){
	//stringstream buf;
	if(factors[i].a.rep.length()==2){
		root=FindRoots(factors[i].a);
			for(int j=0;j<root.length();j++){
				wr<<root[j];
				wr<<"\r"<<endl;
			//	buf<<root[j];
			//	sd[cn++]=buf.str();
			//	cout <<"root["<<j<<"]:"<< root[j]<< "\n";
			}
	}
}
		wr.close();

	double end2=omp_get_wtime();
	cout<<"find root:"<<end2-start2<<endl;
	//size=cn;
	
	// return sd;
}


bigint* Client::interpolate(int size, bigint* a, bigint* b,bigint N)
{
   long m = size;

   bigint* prod;
   prod=(mpz_t*)malloc(size*sizeof(mpz_t));
   prod = a;
   bigint t1, t2;
   mpz_init(t1);
   mpz_init(t2);
   int k, i;

   bigint* res;
   res=(mpz_t*)malloc(size*sizeof(mpz_t));
   bigint aa;
   for (k = 0; k < m; k++) {
	   
      mpz_init_set(aa ,a[k]);
      mpz_init_set_str(t1,"1",10);
      for (i = k-1; i >= 0; i--) {
         mpz_mul(t1, t1, aa);
		 mpz_mod(t1, t1,N);//xxx
         mpz_add(t1, t1, prod[i]);
      }

      mpz_init_set_str(t2,"0",10);
      for (i = k-1; i >= 0; i--) {
         mpz_mul(t2, t2, aa);
		 mpz_mod(t2, t2,N);//xxx
         mpz_add(t2, t2, res[i]);
      }

      mpz_invert(t1, t1,N);
      mpz_sub(t2, b[k], t2);
      mpz_mul(t1, t1, t2);

      for (i = 0; i < k; i++) {
         mpz_mul(t2, prod[i], t1);
		 mpz_mod(t2, t2,N);//xxx

         mpz_add(res[i], res[i], t2);
		 mpz_mod(res[i], res[i],N);

      }

      mpz_init_set(res[k], t1);
	  mpz_mod(res[k], res[k],N);
      if (k < m-1) {
         if (k == 0)
            mpz_neg(prod[0], prod[0]);
         else {
            mpz_neg(t1, a[k]);
            mpz_add(prod[k], t1, prod[k-1]);
            for (i = k-1; i >= 1; i--) {
               mpz_mul(t2, prod[i], t1);
			   mpz_mod(t2, t2,N);//xxx

               mpz_add(prod[i], t2, prod[i-1]);
            }
            mpz_mul(prod[0], prod[0], t1);
			mpz_mod(prod[0], prod[0],N);//xxx

         }
      }
   }

   while (m > 0 && (res[m-1]==0)) m--;
   return res;
}





// int main(){
//// test for gen_compPerm_req();
//	Server serv(3,2,5);
//	Server * serv_ptr (& serv);
//	bigint *el,*xp;
//	el= (mpz_t*)malloc(1*sizeof(mpz_t));
//	xp= (mpz_t*)malloc(2*sizeof(mpz_t));
//	/*mpz_init_set_str(xp[0],"44",10);
//
//	mpz_init_set_str(xp[1],"5",10);*/
//
//	mpz_init_set_str(el[0],"22",10);
//	Client A(serv_ptr,el,1);
//	string id="A_id";
//	A.outsource_poly(id);
//	cout<<"ID:"<<serv.poly[1].poly_ID<<endl;
//	cout<<"pubmod:"<<A.pubmoduli<<endl;
//	cout<<"va[0]:"<<serv.poly[1].values[0]<<endl;
//	cout<<"va[1]:"<<serv.poly[1].values[1]<<endl;
//	cout<<"va[2]:"<<serv.poly[1].values[2]<<endl;
//
//	bigint* bl_vals;
//	bl_vals=(mpz_t*)malloc(3*sizeof(mpz_t));
//	bl_vals=serv.poly[1].values;
//	
//	bigint *pr_val,*un_blinded, temp,*values;
//	//bl_vals=get_values(); 
//	pr_val=(mpz_t*)malloc(3*sizeof(mpz_t));
//	un_blinded=(mpz_t*)malloc(3*sizeof(mpz_t));
//	values=(mpz_t*)malloc(3*sizeof(mpz_t));
//	gmp_randstate_t rand;
//	gmp_randinit_default(rand);
//	gmp_randseed(rand,A.seed);
//	for(int i=0;i<3;i++){
//		mpz_init(pr_val[i]);
//		mpz_urandomb(pr_val[i],rand,128);// PR vals size:128 bits
//		mpz_init(temp);
//		mpz_init(un_blinded[i]);
//		mpz_invert(temp, pr_val[i],A.pubmoduli);
//		mpz_mul(un_blinded[i],temp,bl_vals[i]);
//		mpz_mod(un_blinded[i],un_blinded[i],A.pubmoduli);
//	}
//	//values= un_blinded;
//	bigint te;
//	mpz_init_set_str(te,"444",10);
//	mpz_mul(un_blinded[0],te,un_blinded[0]);
//		mpz_mul(un_blinded[1],te,un_blinded[1]);
//	mpz_mul(un_blinded[2],te,un_blinded[2]);
//
//	values= A.interpolate(3, A.xpoints, un_blinded,A.pubmoduli);
//	cout<<"\nRESULT................"<<endl;
//	cout<<"val[0]:"<<values[0]<<endl;
//	cout<<"val[1]:"<<values[1]<<endl;
//	cout<<"val[2]:"<<values[2]<<endl;

	//cout<<"A.xpoint size:"<<A.xpoint_size<<endl;
	//CompPerm_Request *ptr=A.gen_compPerm_req();
	//cout<<"enc_rand_vals:"<<ptr->enc_rand_vals[0].c<<endl;


// test for grant_comp(); // it checks whether what client A sends to server contains 
	// clien B's PR_vals
//	Server serv(2,1);
//	Server * serv_ptr (& serv);
//	bigint *el,*xp;
//	el= (mpz_t*)malloc(1*sizeof(mpz_t));
//	xp= (mpz_t*)malloc(1*sizeof(mpz_t));
//	mpz_init_set_str(xp[0],"44",10);
//	mpz_init_set_str(el[0],"22",10);
//	Client A(serv_ptr,el,1);
//	Client B(serv_ptr,el,1);
//	CompPerm_Request *ptr;
//	ptr=B.gen_compPerm_req();
//	GrantComp_Info*ptr1=A.grant_comp(ptr,true);
//	paillier_prvkey_t *prv;
//	paillier_pubkey_t * pub;
//	paillier_ciphertext_t* res,*ca;
//	paillier_plaintext_t  *pl;
//	pl=new paillier_plaintext_t;
//	mpz_init(pl->m);
//	ca=new paillier_ciphertext_t;
//	mpz_init_set(ca->c,ptr1->converter[0].c);
//	prv=B.sk;
//	pub=B.pk;
//	paillier_dec(pl,pub,prv,ca);
//	bigint temp4;
//	gmp_randstate_t rand;
//	gmp_randinit_default(rand);
//	gmp_randseed(rand,A.seed);
//	mpz_init(temp4);
//	mpz_urandomb(temp4,rand,128);
//	mpz_invert(temp4, temp4,A.pubmoduli);
//	mpz_mul(temp4,temp4,pl->m);
//	mpz_mod(temp4,temp4,A.pubmoduli);
//	cout<<endl;
//	cout<<"temp4 must equal b.pr_val[0]:"<<temp4<<endl;
//	return 0;
//
//}
