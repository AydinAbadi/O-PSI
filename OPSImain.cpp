
#include "OPSImain.h"
//#include<gmp.h>
//#include <gmpxx.h>

using namespace std;
typedef mpz_t bigint;
#include <omp.h>
#include <time.h>








/*
bool exits2(bigint a,bigint *b,int size){
	
	for(int i=0;i<size;i++){

		if(mpz_cmp(a,b[i])==0){return true;break;}
	}
	return  false;
}

*/


//#include <cryptopp/sha.h>
//#include <cryptopp/filters.>
//#include <cryptopp/hex.h>

int main(){


int xsize=3;	
int size=1;// number of elements
int bitsize=32;// max bit-length
int pubmod_bitsize=112;
int interSec_size=1;


int count=1;
int l=1; // number of tests

double outsource=0;
double auth=0;
double cloudcomp=0;
double clientresultret=0;
double perm_req=0;


for (int i=0;i<l;i++){
cout<<"\n\n\t ("<<count<<")"<<endl;
cout<<"\n----- xsize:"<<xsize<<endl;
cout<<"-----  set_size (set cardinality):"<<size<<endl;
cout<<"-----  elem bitsize :"<<bitsize<<endl;
cout<<"-----  pubmod_bitsize :"<<pubmod_bitsize<<endl;

cout<<"\n===================="<<endl;



Server serv(xsize,2,pubmod_bitsize);
Server * serv_ptr (& serv);


mpz_t *a,*b;
a=(mpz_t*)malloc(size*sizeof(mpz_t));
b=(mpz_t*)malloc(size*sizeof(mpz_t));

a=serv.gen_randSet (size, bitsize); 
b=serv.gen_randSet (size, bitsize); 

//assign an identical element in the two sets

cout<<"Elements in common include:"<<endl;
for(int i=0;i<interSec_size;i++){
		cout<<i<<"\t: "<<a[i]<<endl;;
	mpz_init_set(b[i],a[i]);}

cout<<"\n===================="<<endl;


double start1=omp_get_wtime();
Client A(serv_ptr,a,size);
string as="A_ID";
A.outsource_poly(as);
double end1=omp_get_wtime();
cout<<"\nTime to run Poly_out:"<<end1-start1<<endl;
outsource+=end1-start1;

Client B(serv_ptr,b,size);
string bs="B_ID";
B.outsource_poly(bs);
cout<<"----------------"<<endl;
//double start6=omp_get_wtime();

double start2=omp_get_wtime();
CompPerm_Request*req=B.gen_compPerm_req();
double end2=omp_get_wtime();
cout<<"\nTime to run B.gen_compPerm_req():"<<end2-start2<<endl;
perm_req+=end2-start2;
cout<<"----------------"<<endl;
double start3=omp_get_wtime();
GrantComp_Info*ptr1=A.grant_comp(req,true);
double end3=omp_get_wtime();
cout<<"\nTime to  run A.grant_comp():"<<end3-start3<<endl;
auth+=end3-start3;
cout<<"----------------"<<endl;
double start4=omp_get_wtime();
Server_Result*res=serv.compute_result(ptr1);
double end4=omp_get_wtime();

cout<<"\nTime to run Serv.comput():"<<end4-start4<<endl;
cloudcomp+=end4-start4;
cout<<"----------------"<<endl;
string *values;

malloc(xsize*sizeof(mpz_t));
int sz=0;
double start5=omp_get_wtime();
//values= B.find_intersection(res,sz);
B.find_intersection(res,sz);

double end5=omp_get_wtime();
cout<<"\nTime to run find_intersection():"<<end5-start5<<endl;
clientresultret+=end5-start5;

count++;

}

cout<<endl;
cout<<"\n====================================\n"<<endl;
cout<<"\n\n\nAverage Outsource Time:"<<outsource/l<<endl;
cout<<"\n\n\nAverage Permision request:"<<perm_req/l<<endl;
cout<<"\n\n\nAverage authorization Time:"<<auth/l<<endl;
cout<<"\n\n\nAverage Cloud-side Computation Time:"<<cloudcomp/l<<endl;
cout<<"\n\n\nClient side result retieval Time:"<<clientresultret/l<<endl;


//double end6=omp_get_wtime();
//cout<<"\nOverall time excluding outsource:"<<end6-start6<<endl;




//For test only
/*
cout<<"\n\t....Correctness Checking..."<<endl;
string line;
int count=0;

ifstream rd("roots.txt");
bigint temp;
char c[1024];
while(rd>>c){
	count++;
}

rd.close();
//cout<<"count:"<<count<<endl;
char c1[1024];
bigint *tm;
tm=(mpz_t*)malloc(count*sizeof(mpz_t));
ifstream rd4("roots.txt");
int h=0;
while(rd4>>c1){
	mpz_init_set_str(tm[h],c1,10);
	h++;
}

rd4.close();
int cj=0;
bool iis=false;

for(int i=0;i<count;i++){
		iis=exits2(tm[i],b,interSec_size);
		if(iis) {
			cj++;
			iis=false;
		}
}
//cout<<"cj:"<<cj<<endl;
//if (cj==0) cout<<"\n********* No intersection ********\n";
 if(cj==interSec_size) cout<<"\n\t ************************* Correct!  *************************"<<endl;
else cout<<"\n********* No intersection ********\n";
*/
return 0;

}

//$ g++ -I$home/win7/include  polynomial.o server.o client.o OPSImain.cpp -lgomp  -L/cygdrive/c/cygwin/home/Win7/libpaillier -l:libpaillier.a  -L$home/win7/lib -lntl -lgmpxx -lgmp -lm
