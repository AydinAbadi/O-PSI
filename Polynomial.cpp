#include<iostream>
#include"Polynomial.h"
#include<gmp.h>
#include <gmpxx.h>
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


Polynomial::Polynomial(){}// needed for dynamic array.

Polynomial::Polynomial(bigint* elem,string ID, bigint * xpoints,int elem_size,int xpoints_size,bigint pubmoduli){
	val_size=xpoints_size;
	values= evaluate (elem, xpoints,elem_size, xpoints_size, pubmoduli);
	poly_ID=ID;
}


bigint* Polynomial:: evaluate(bigint* elem, bigint* xp,int ele_size, int xp_size,bigint pubmod){
	bigint mult2;
	bigint*val;
	val= (mpz_t*)malloc(xp_size*sizeof(mpz_t));
	for (int i=0;i<xp_size;i++){
		mpz_init_set_str(mult2,"1",10);
		for (int j=0;j<ele_size;j++){
			bigint temp;
			mpz_init_set_str(temp,"0",10);
			mpz_sub(temp,xp[i],elem[j]);  
			mpz_mul(mult2,mult2,temp);
			mpz_mod(mult2,mult2,pubmod);//***
			mpz_clear(temp);
		}
		mpz_init_set(val[i],mult2);
		mpz_clear(mult2);
	}
	return val;
}


void Polynomial::blind_poly(bigint seed, bigint pubmod){

	bigint *pr_val,*ptr;
	ptr=get_values(); 

	//int i=0;
	pr_val=(mpz_t*)malloc(val_size*sizeof(mpz_t));
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	for(int i=0;i<val_size;i++){
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,100);// PR vals size:128 bits
				//cout<<"PR_vals["<<i<<"]: "<<pr_val[i]<<endl;

		mpz_mul(pr_val[i],ptr[i],pr_val[i]);
		mpz_mod(pr_val[i],pr_val[i],pubmod);

	}
	values=pr_val;
}

void Polynomial::unblind_poly( bigint seed, bigint pubmod){

	bigint *pr_val,*bl_vals,*un_blinded, temp;
	bl_vals=get_values(); 
	pr_val=(mpz_t*)malloc(val_size*sizeof(mpz_t));
	un_blinded=(mpz_t*)malloc(val_size*sizeof(mpz_t));
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	for(int i=0;i<val_size;i++){
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,100);// PR vals size:128 bits
		mpz_init(temp);
		mpz_init(un_blinded[i]);
		mpz_invert(temp, pr_val[i],pubmod);
		mpz_mul(un_blinded[i],temp,bl_vals[i]);
		mpz_mod(un_blinded[i],un_blinded[i],pubmod);
	}
	values= un_blinded;
	/*for(int i=0;i<val_size;i++){
		mpz_init_set(values[i],un_blinded[i]);
	}*/
}



bigint* Polynomial::get_values(){
	return values;
}




//bigint* shamir_reconstruct (int n, bigint *x, bigint *y,bigint N) {
//    int d,i;
//	mpz_t temp,temp2,temp3;
//    for (d = 1; d < n; d++) {
//        for (i = 0; i < n - d; i++) {
//            int j = i + d;
//			mpz_init(temp);
//			mpz_init(temp2);
//			mpz_init(temp3);
//			mpz_mul( temp,x[j], y[i]);
//			mpz_mul( temp2,x[i], y[i+1] );
//            mpz_sub( temp2,temp2,temp );
//            mpz_sub( temp3, x[j], x[i] );
//			mpz_invert(temp3,temp3,N);
//            mpz_mul(y[i] ,temp2, temp3 );
//        }
//    }
//    return y;
//}

//
//bigint* interpolate(int size, bigint* a, bigint* b,bigint N)
//{
//   long m = size;
//
//   bigint* prod;
//   prod=(mpz_t*)malloc(size*sizeof(mpz_t));
//   prod = a;
//   bigint t1, t2;
//   mpz_init(t1);
//   mpz_init(t2);
//   int k, i;
//
//   bigint* res;
//   res=(mpz_t*)malloc(size*sizeof(mpz_t));
//   bigint aa;
//   for (k = 0; k < m; k++) {
//	   
//      mpz_init_set(aa ,a[k]);
//      mpz_init_set_str(t1,"1",10);
//      for (i = k-1; i >= 0; i--) {
//         mpz_mul(t1, t1, aa);
//         mpz_add(t1, t1, prod[i]);
//      }
//
//      mpz_init_set_str(t2,"0",10);
//      for (i = k-1; i >= 0; i--) {
//         mpz_mul(t2, t2, aa);
//         mpz_add(t2, t2, res[i]);
//      }
//
//      mpz_invert(t1, t1,N);
//      mpz_sub(t2, b[k], t2);
//      mpz_mul(t1, t1, t2);
//
//      for (i = 0; i < k; i++) {
//         mpz_mul(t2, prod[i], t1);
//         mpz_add(res[i], res[i], t2);
//		 mpz_mod(res[i], res[i],N);
//
//      }
//
//      mpz_init_set(res[k], t1);
//	  mpz_mod(res[k], res[k],N);
//      if (k < m-1) {
//         if (k == 0)
//            mpz_neg(prod[0], prod[0]);
//         else {
//            mpz_neg(t1, a[k]);
//            mpz_add(prod[k], t1, prod[k-1]);
//            for (i = k-1; i >= 1; i--) {
//               mpz_mul(t2, prod[i], t1);
//               mpz_add(prod[i], t2, prod[i-1]);
//            }
//            mpz_mul(prod[0], prod[0], t1);
//         }
//      }
//   }
//
//   while (m > 0 && (res[m-1]==0)) m--;
//   return res;
//}



//int main(){
//	
//	bigint*x,*y,N,*z,a,b;
//	int size=14;
//	x=	(mpz_t*)malloc(size*sizeof(mpz_t));
//	y=	(mpz_t*)malloc(size*sizeof(mpz_t));
//	z=	(mpz_t*)malloc(size*sizeof(mpz_t));
//
//	mpz_init_set_str(x[0],"1456888888888888888846545645677777777777777776666666666666222222222222222264656456456456456456465456456456445654564654645645645645646545645645645456414565645645646",10);
//	mpz_init_set_str(x[1],"156466666666666654456456456565656565656565656565656565656666666666666666645645645645646545645646546456456456456455645654654645645645645646545645646564564564564046654564",10);
//	mpz_init_set_str(x[2],"156486666666666697897811199999999999999999911111111111111222222222222222255555555555555975455654654564564564678797987978978976546456465465454564566454564564254654564564564564",10);
//	mpz_init_set_str(x[3],"1556596444444444444448789755444444444444444444488888888888888888886666666666666668978979878978978976456465464645645564654646456454545656456456456465454564564",10);
//	mpz_init_set_str(x[4],"16545467747777777777777777777799999999999999997777222222222222222222225454456564654656469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[5],"456546456456456456466774777777778888888888888888887777777777777777222222222222222222225454456564654656469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[6],"65645645645645647777777777722222228999999999999999922222222222225454456564654656469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[7],"65477777777777777222222222222222228888888888888888882225454456564654656469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[8],"8822254544565646546564698789789754699999999999999999999995456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[9],"569754654564545465465445454564897897877777777777777777779789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[10],"642222545445656465465646987897897546545888888888888888888888886454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[11],"16545467747777777777777777777777772222266856469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[12],"1654546774999999999999999994654656469878978975465456454546546544545456489789789789798797987978956564654645645645646",10);
//	mpz_init_set_str(x[13],"99822222222222222545445656465465646987897897546545646545646666666666666666666666666666668888888854546546544545456489789789789798797987978956564654645645645646",10);
//
//	
//
//	mpz_init_set_str(b,"55",10);
//	mpz_init(y[0]);	mpz_init(y[1]);	mpz_init(y[2]);	mpz_init(y[3]);	mpz_init(y[4]);mpz_init(y[5]);mpz_init(y[6]);
//	mpz_init(y[7]);mpz_init(y[8]);mpz_init(y[9]);mpz_init(y[10]);mpz_init(y[11]);mpz_init(y[12]);mpz_init(y[13]);
//
//	mpz_sub(y[0],x[0],b);
//
//	mpz_sub(y[1],x[1],b);
//	mpz_sub(y[2],x[2],b);
//	mpz_sub(y[3],x[3],b);
//	mpz_sub(y[4],x[4],b);
//	mpz_sub(y[5],x[5],b);
//	mpz_sub(y[6],x[6],b);
//	mpz_sub(y[7],x[7],b);
//	mpz_sub(y[8],x[8],b);
//	mpz_sub(y[9],x[9],b);
//	mpz_sub(y[10],x[10],b);
//	mpz_sub(y[11],x[11],b);
//	mpz_sub(y[12],x[12],b);
//	mpz_sub(y[13],x[13],b);
//
////	//mpz_init_set_str(y[0],"14564654564564564545637",10);
////	//mpz_init_set_str(y[1],"1564544564564565645645645636",10);
////	//mpz_init_set_str(y[2],"1564545545645664545645638",10);
////	//mpz_init_set_str(y[3],"1556564564545456564564564560",10);
////	//mpz_init_set_str(y[4],"165454654544564545456452",10);
////
////
//	mpz_init_set_str(N,"253",10);
////
//	bigint*res=interpolate(size,x,y,N);
//	cout<<res[0]<<endl;
//	cout<<res[1]<<endl;
//	cout<<res[2]<<endl;
//	cout<<res[3]<<endl;
//	cout<<res[4]<<endl;
//
//cout<<res[5]<<endl;cout<<res[6]<<endl;cout<<res[7]<<endl;cout<<res[8]<<endl;cout<<res[9]<<endl;
//	cout<<res[10]<<endl;cout<<res[11]<<endl;cout<<res[12]<<endl;cout<<res[13]<<endl;
	/*z=shamir_reconstruct ( 2, x, y,N);

	mpz_init_set_str(a,"4",10);
	cout<<z[0]<<endl;
	cout<<z[1]<<endl;*/
	
//	bigint *el,*xp;
//	el= (mpz_t*)malloc(2*sizeof(mpz_t));
//	xp= (mpz_t*)malloc(5*sizeof(mpz_t));
//
//	mpz_init_set_str(el[0],"22",10);
//	mpz_init_set_str(el[1],"11",10);
//
//	mpz_init_set_str(xp[0],"44",10);
//	mpz_init_set_str(xp[1],"66",10);
//	mpz_init_set_str(xp[2],"55648",10);
//	mpz_init_set_str(xp[3],"1564546546510",10);
//	mpz_init_set_str(xp[4],"112",10);
//	
//
//	string id="ID";
//	int size=2;
//	int x_size=5;
//	bigint mod;
//	mpz_init_set_str(mod,"256545654644654564561",10);
//	Polynomial pol(el,id,xp,size,x_size,mod);
//	cout<< "id:"<<pol.poly_ID<<endl;
//	cout<<"Val[0]:"<<pol.values[0]<<endl;
//	cout<<"Val[1]:"<<pol.values[1]<<endl;
//	cout<<"Val[2]:"<<pol.values[2]<<endl;
//	cout<<"Val[3]:"<<pol.values[3]<<endl;
//	cout<<"Val[4]:"<<pol.values[4]<<endl;
//
//	cout<<endl;
//
//	pol.blind_poly( xp[3], mod);
//	bigint *ptr=pol.get_values();
//	cout<<"blinded ptr[0]:"<<ptr[0]<<endl;
//	cout<<"blinded ptr[1]:"<<ptr[1]<<endl;
//	cout<<"blinded ptr[2]:"<<ptr[2]<<endl;
//	cout<<"blinded ptr[3]:"<<ptr[3]<<endl;
//	cout<<"blinded ptr[4]:"<<ptr[4]<<endl;
//
//	
//	pol.unblind_poly (xp[3],mod);
//	bigint*ptr2=pol.get_values();
//	cout<<endl;
//
//	cout<<"unblind ptr2[0]:"<<ptr2[0]<<endl;
//	cout<<"unblind ptr2[1]:"<<ptr2[1]<<endl;
//	cout<<"unblind ptr2[2]:"<<ptr2[2]<<endl;
//	cout<<"unblind ptr2[3]:"<<ptr2[3]<<endl;
//	cout<<"unblind ptr2[4]:"<<ptr2[4]<<endl;
//
//	Polynomial *ppt;
//	ppt=new Polynomial[10];
//
//	ppt[0]=pol;
//	cout<<"val_size:"<<ppt[0].val_size<<endl;
//	bigint* point=ppt[0].get_values();
//	cout<<"vaaals:"<<point[0]<<endl;
//	return 0;
//
//
//
//}
