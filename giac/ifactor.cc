// -*- mode:C++ ; compile-command: "g++-3.4 -I.. -g -c ifactor.cc -DHAVE_CONFIG_H -DIN_GIAC" -*-
#include "giacPCH.h"
#define GIAC_MPQS // define if you want to use giac for sieving
#define GIAC_ADDITIONAL_PRIMES // if defined, additional primes are used in sieve
#define GIAC_RECIPROQUES // if defined % is done with multiplication
#define MP_RADIX 4294967296.0

#include "path.h"
/*
 *  Copyright (C) 2003,7 R. De Graeve & B. Parisse, 
 *  Institut Fourier, 38402 St Martin d'Heres
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

using namespace std;
#include <fstream>
//#include <unistd.h> // For reading arguments from file
#include "ifactor.h"
#include "pari.h"
#include "usual.h"
#include "sym2poly.h"
#include "rpn.h"
#include "prog.h"
#include "misc.h"
#include "giacintl.h"

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

#ifdef GIAC_MPQS 
  #undef HAVE_LIBPARI 
#endif

#if 0
  struct int256 {
    longlong a;
    unsigned longlong b,c,d;
  };
  struct int128 {
    longlong a;
    unsigned longlong b;
  };

  void sub(int256 A,int b,int256 & C){
    bool Apos=A.a>=0;
    if (Apos){
      if (b<0){
	add(A,b,C);
	return;
      }
      bool carry=A.d<b;
      A.d -= b;
      if (carry){
	carry=A.c<1;
	A.c--;
	if (carry){
	  carry=A.b<1;
	  A.b--;
	  if (carry)
	    A.a--;
	}
      }
      C=A;
      return;
    }
    A.a = -A.a;
    sub(A,-b,C);
    C.a = -C.a;
  }

  void add(const int192 & A,int b,int192 & C){
    bool Apos=A.a>=0;
    if (Apos){
      if (b<0){
	sub(A,-b,C);
	return;
      }
      A.d += b;
      if (A.d<b){
	A.c++;
	if (A.c==0){
	  A.b++;
	  if (A.n==0)
	    A.a++;
	}
      }
      C=A;
      return;
    }
    A.a = -A.a;
    sub(A,-b,C);
    C.a = -C.a;
  }

  inline void int128tounsigned(const int128 & A,unsigned & Aa,unsigned & Ab,unsigned & Ac,unsigned & Ad){
    Aa = (* ((unsigned *) (&A.a)+1)) & 0x7fff;
    Ab = *(unsigned *) A.a;
    Ac = *( (unsigned *) &A.b)+1;
    Ad = *(unsigned *) A.d;
  }

  void mul(int128 A,int128 B,int256 & C){
    bool neg=(A.a<0) ^ (B.a<0);
    unsigned A3,A2,A1,A0,B3,B2,B1,B0,C6,C5,C4,C3,C2,C1,C0;
    int128tounsigned(A,A3,A2,A1,A0);
    int128tounsigned(B,B3,B2,B1,B0);
    unsigned longlong p1,p2,p3,p;
    p1=A0; p2=B0;
    p3=p1*p2;
    C0=p3;
    C1=p3>>32;
    p1=A1;
    p = p1*p2;
    p += C1;
    unsigned short carry = (p<C1);
    p1=A0; p2=B1;
    p3=p1*p2;
    p += p3;
    carry += (p<p3);
    C1 = p;
    C2 = p>>32;
    C2 += carry;
    carry = (C2<carry);
  }

#endif

  const short int giac_primes[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973};

  static const int giac_last_prime=giac_primes[sizeof(giac_primes)/sizeof(short)-1];

#ifdef GIAC_HAS_STO_38
  const unsigned QS_B_BOUND=1360; // max size of B allowed so that matrix<256K
#else
  const unsigned QS_B_BOUND=3400; // max size of B allowed (this is also used for memory)
#endif
  const unsigned QS_SIZE=64000; // number of slicetype in a sieve slice
  typedef unsigned char slicetype; // define to unsigned char if not enough
  const unsigned char BITSHIFT=40; // shift so that we can safely use unsigned char for slicetype

  static void printbool(ostream & os,const vector<unsigned> & v,int C=1){
    if (C)
      C=min(C,v.size());
    else
      C=v.size();
    for (int c=0;c<C;++c){
      for (int s=0;s<32;++s){
	os << (((v[c] >> s & 1)==1)?1:0) << " ";
      }
    }
    os << endl;
  }

  static void printbool(ostream & os,const vector< vector<unsigned> > & m,int L=32){
    if (L)
      L=min(L,m.size());
    else
      L=m.size();
    for (int l=0;l<L;++l){
      printbool(os,m[l]);
    }
  }

  void rref(vector< vector<unsigned> > & m){
    int i,l=0,c=0,L=m.size(),C32=m.front().size(),C=C32*32;
    for (;l<L && c<C;){
      // printbool(cerr,m);
      int c1=c/32,c2=c%32;
      // find first non-0 pivot in col c starting at row l
      for (i=l;i<L;++i){
	if ((m[i][c1] >> c2) & 1)
	  break;
      }
      if (i==L){ // none found in this column
	++c;
	continue;
      }
      if (i!=l) 
	swap(m[i],m[l]);
      for (i=0;i<L;++i){
	if (i==l || ( (m[i][c1] >> c2) & 1)!=1) 
	  continue;
	// line combination l and i
	vector<unsigned>::iterator pivptr = m[l].begin(),pivend = pivptr+C32,curptr=m[i].begin();
	for (;pivptr!=pivend;){
	  // small optimization (loop unroll), assumes mult of 4(*32) columns
	  *curptr ^= *pivptr;
	  ++curptr,++pivptr;
	  *curptr ^= *pivptr;
	  ++curptr,++pivptr;
	  *curptr ^= *pivptr;
	  ++curptr,++pivptr;
	  *curptr ^= *pivptr;
	  ++curptr,++pivptr;
	}
      }
      ++l;
      ++c;
    }
  }

  // sieve in [sqrtN+shift,sqrtN+shift+slice.size()-1]
  void sieve(const vector<unsigned short> & basis,vector<unsigned short> & additional_primes,vector<bool> & additional_primes_twice,const vector<short int> & sqrt_mod,const vector<unsigned> & reciproques,const vector<unsigned char> & corrections,const gen & N,const gen & isqrtN,const vector<short int> & isqrtNmodp,vector<slicetype> & slice,int shift,vector< vector<unsigned short> > & puissances,vector<int> & shiftvalues,GIAC_CONTEXT){
    // first fill slice with expected number of bits of 
    // (isqrtN+shift)^2-N = 2*shift*isqrtN + negl.
    // -> log(2*isqrtN)+log(shift)
    mpz_t z1,z2,z3;
    mpz_init(z1);  mpz_init(z2);  mpz_init(z3);
    int nbits=int(0.5+std::log(2*evalf_double(isqrtN,1,context0)._DOUBLE_val)/std::log(2.));
    int curbits=0;
    for (int k=absint(shift);k;++curbits){
      k >>= 1;
    }
    int next=1 << (curbits-(shift<0));
    nbits += curbits+BITSHIFT;
    int bs=basis.size();
    unsigned char logB=(unsigned char) int(1.5*std::log(double(basis.back()))/std::log(2.0)+.5)+BITSHIFT;
    int ss=slice.size();
    if (debug_infolevel>6)
      *logptr(contextptr) << "reset " << clock() << " shift " << shift << " nbits " << nbits-BITSHIFT << endl;
    vector<slicetype>::iterator st=slice.begin(),stend=slice.end();
#if 1 // assumes slice type is size 1 byte and multiple of 32
    unsigned * ptr=(unsigned *) &slice[0];
    unsigned * ptrend=ptr+(stend-st)/4;
    unsigned nbits8=(nbits<<24)|(nbits<<16)|(nbits<<8)|nbits;
    if (shift>=0){
      for (int j=shift;ptr!=ptrend;j+=4,++ptr){
	*ptr=nbits8;
	if (j<next)
	  continue;
	++nbits;
	nbits8=(nbits<<24)|(nbits<<16)|(nbits<<8)|nbits;
	next *= 2;
      }
    }
    else {
      for (int j=-shift;ptr!=ptrend;j-=4,++ptr){
	*ptr=nbits8;
	if (j>next)
	  continue;
	--nbits;
	nbits8=(nbits<<24)|(nbits<<16)|(nbits<<8)|nbits;
	next /= 2;
      }
    }
#else
    if (shift>=0){
      for (int j=shift;st!=stend;++j,++st){
	*st=nbits;
	if (j!=next)
	  continue;
	++nbits;
	next *= 2;
      }
    }
    else {
      for (int j=-shift;st!=stend;--j,++st){
	*st=nbits;
	if (j!=next)
	  continue;
	--nbits;
	next /= 2;
      }
    }
#endif
    if (debug_infolevel>6)
      *logptr(contextptr) << "end reset " << clock() << " nbits " << nbits << endl;
    // now for all primes p in basis move in slice from p to p
    // decrease slice[] by number of bits in p
    nbits=4;
    next=16;
    unsigned bstart;
    for (bstart=0;bstart<basis.size();++bstart){
      if (basis[bstart]>8)
	break;
    }
    for (unsigned i=bstart;i<basis.size();++i){
      int p=basis[i];
      if (p>next){
	++nbits;
	next *=2;
      }
      int pos=(int(sqrt_mod[i])-shift-isqrtNmodp[i])%p; 
      // pos+shift+isqrtN = sqrt_mod[i] mod p
      if (pos<0)
	pos+=p;
      st=slice.begin()+pos; stend=slice.begin()+ss;
      for (;st<stend;){
	*st -= nbits;
	st += p;
      }
      // if (p%2==0) continue; // only one sqrt
      pos=(-int(sqrt_mod[i])-shift-isqrtNmodp[i])%p;
      if (pos<0)
	pos+=p;
      st=slice.begin()+pos;
      for (;st<stend;){
	*st -= nbits;
	st += p;
      }
    }
    if (debug_infolevel>6)
      *logptr(contextptr) << clock() << "relations " << endl;
    // now find relations
    vector<unsigned short> curpuissances;
    st=slice.begin(); stend=slice.end();
    for (int pos=0;st!=stend;++pos,++st){
      // compare slice[pos] to boundary
      if (*st>logB)
	continue;
      // factor (isqrtN+shift+pos)^2-N on basis
      curpuissances.clear();
      int shiftpos=shift+pos;
#if 0
      gen tmp=isqrtN+shift+pos;
      tmp=tmp*tmp-N; tmp.uncoerce(); mpz_set(z1,*tmp._ZINTptr);
      *logptr(contextptr) << tmp << endl;
#else
      if (shiftpos>0)
	mpz_add_ui(z2,*isqrtN._ZINTptr,shiftpos); // gen tmp=isqrtN+shift+pos;
      else
	mpz_sub_ui(z2,*isqrtN._ZINTptr,-shiftpos);
      mpz_mul(z3,z2,z2); mpz_sub(z1,z3,*N._ZINTptr); // tmp=tmp*tmp-N; tmp.uncoerce(); mpz_set(z1,*tmp._ZINTptr);
#endif
      if (mpz_cmp_si(z1,0)<0){ // if (is_positive(-tmp,context0))
	curpuissances.push_back(0xffff);
	mpz_neg(z1,z1); // tmp=-tmp;
      }
      bool done=false;
      int debut=0;
#if 0 // #ifndef USE_GMP_REPLACEMENTS
      if (basis.front()==2){
	++debut;
	int j=0;
	for (;mpz_even_p(z1);++j){
	  mpz_tdiv_q_2exp(z1,z1,1);
	}
	for (;j>=256;j-=256)
	  curpuissances.push_back(2<<8);
	if (j)
	  curpuissances.push_back( (2 << 8) | j);
      }
#endif
      for (int i=debut;i<bs;++i){
	if (mpz_cmp_si(z1,1)==0){
	  break;
	}
	int j=0;
	int bi=basis[i];
	// check that shiftpos+isqrtNmodp[i] is +/-sqrt_mod[i] % bi before doing division
#ifdef GIAC_RECIPROQUES
	int offset = shiftpos+isqrtNmodp[i];
	int check;
	if (bi>256 && bi< (1<<14)){
	  if (offset>0){
	    check = (unsigned) (((ulonglong)( offset+ corrections[i]) * 
				 (ulonglong)reciproques[i]) >> 40);
	    check = offset - check * bi;
	  }
	  else {
	    check = (unsigned) (((ulonglong)( corrections[i]-offset) * 
				 (ulonglong)reciproques[i]) >> 40);
	    check = offset + check * bi;
	    if (check<0)
	      check += bi;
	  }
	}
	else { 
	  check=offset%bi;
	  if (check<0)
	    check+=bi;
	}
	int root=sqrt_mod[i];
	if (check!=root && check!=bi-root)
	  continue;
#else
	int offset = shiftpos+isqrtNmodp[i];
	int check=offset%bi;
	if (check<0)
	  check+=bi;
	int root=sqrt_mod[i];
	if (check!=root && check!=bi-root)
	  continue;
#endif
#ifdef USE_GMP_REPLACEMENTS
	mpz_fdiv_qr_ui(z2,z3,z1,bi);
	mpz_set(z1,z2);
#else
	mpz_fdiv_q_ui(z1,z1,bi);
#endif
	for (++j;;++j){
	  mpz_fdiv_qr_ui(z2,z3,z1,bi);
	  if (mpz_cmp_si(z3,0))
	    break;
	  mpz_set(z1,z2);
	}
	/*
	  while (is_zero(smod(tmp,bi))){
	  tmp=tmp/bi;
	  ++j;
	  }
	*/
	if (!done && bi>255){
	  curpuissances.push_back(0);
	  done=true;
	}
	if (done){
	  for (;j;--j)
	    curpuissances.push_back(bi);
	}
	else {
	  for (;j>=256;j-=256)
	    curpuissances.push_back(bi<<8);
	  if (j)
	    curpuissances.push_back( (bi << 8) | j);
	}
      }
      if (mpz_cmp_si(z1,1)==0){ // is_one(tmp)){
	// *logptr(contextptr) << curpuissances << endl;
	shiftvalues.push_back(shift+pos);
	puissances.push_back(curpuissances);
      }
      else {
	if (mpz_cmp_ui(z1,0xffff)==1){
	  if (debug_infolevel>6)
	    *logptr(contextptr) << "Sieve large remainder:" << gen(z1) << endl;
	}
	else {
#ifdef GIAC_ADDITIONAL_PRIMES
	  unsigned short P=mpz_get_ui(z1);
	  // if (int(P)>2*int(basis.back())) continue;
	  // if (debug_infolevel>5)
	  if (debug_infolevel>6)
	    *logptr(contextptr) << P << " remain " << endl;
	  int Ppos=equalposcomp(additional_primes,P); // this is in O(additional^2)=o(B^3)
	  if (Ppos){
	    if (debug_infolevel>6)
	      *logptr(contextptr) << P << " already additional" << endl;
	    --Ppos;
	    additional_primes_twice[Ppos]=true;
	  } else {
	    // add a prime in additional_primes if <=QS_B_BOUND
	    if (bs+additional_primes.size()>QS_B_BOUND)
	      continue;
	    additional_primes.push_back(P);
	    additional_primes_twice.push_back(false);
	    Ppos=additional_primes.size()-1;
	  }
	  // add relation
	  if (!done)
	    curpuissances.push_back(0);
	  curpuissances.push_back(P);
	  shiftvalues.push_back(shift+pos);
	  puissances.push_back(curpuissances);
#endif
	}
      }
    } // end for loop on slice array
    mpz_clear(z1);  mpz_clear(z2);  mpz_clear(z3);  
    if (debug_infolevel>6)
      *logptr(contextptr) << clock() << " end relations " << endl;
  }

  void release_memory(vector<slicetype> & slice){
    // release memory from slice
    vector<slicetype> tmp;
    swap(slice,tmp);
  }

  // find relations around floor(sqrt(n)), quadratic sieve
  bool sieve(const gen & n,gen & pn,GIAC_CONTEXT){
    if (n.type!=_ZINT)
      return false;
    gen N(n);
    /*
    // make Q(x)=x^2-N divisible by 8 if it is even
    gen r=smod(N,8);
    if (r==3) N=5*N;
    if (r==-3) N=3*N;
    if (r==-1) N=7*N;
    */
    double Nd=evalf_double(N,1,context0)._DOUBLE_val;
    double B=std::exp(std::sqrt(2.0)/4*std::sqrt(std::log(Nd)*std::log(std::log(Nd))));
    *logptr(contextptr) << clock() << " QSieve number of primes " << B << endl;
    // double B=std::exp(std::sqrt(std::log(Nd)*std::log(std::log(Nd)))/2);
    if (B>QS_B_BOUND) 
      return false;
    double M=B*B*B;
    // sieve between sqrt(N)-M and sqrt(N)+M
    // first compute the prime basis and sqrt(N) mod p, p in basis
    vector<unsigned short> basis;
    vector<short> sqrtmod;
    basis.reserve(unsigned(B));
    basis.push_back(2);
    sqrtmod.reserve(basis.capacity());
    sqrtmod.push_back(1); // I assume that N is odd... hence has sqrt 1 mod 2
    int i;
    for (i=1;i<sizeof(giac_primes)/sizeof(short);++i){
      if (ctrl_c)
	return false;
      unsigned short j=giac_primes[i];
      if (i%500==99)
	*logptr(contextptr) << clock() << " QSieve current basis size " << basis.size() << endl;
      if (legendre(N,j)==1){
	basis.push_back(j);
	sqrtmod.push_back(sqrt_mod(N,j,context0).val);
      }
      if (basis.size()>B)
	break;
    }
    int jp=nextprime(basis.back()+1).val;
    for (;basis.size()<B;++i){
      if (ctrl_c)
	return false;
      if (jp>65535)
	break;
      if (i%500==99)
	*logptr(contextptr) << clock() << " QSieve current basis size " << basis.size() << endl;
      if (legendre(N,jp)==1){
	basis.push_back(jp);
	sqrtmod.push_back(sqrt_mod(N,jp,context0).val);
      }
      jp=nextprime(jp+1).val;
    }
    *logptr(contextptr) << clock() << " QSieve basis OK, size " << basis.size() << endl;
    int bs=basis.size();
    // 2^32/primes 
    vector<unsigned> reciproques(bs);
    vector<unsigned char> corrections(bs);
    for (int i=0;i<bs;++i){
      int prime=basis[i];
      reciproques[i]=((ulonglong)1 << 40) / (ulonglong)prime;
      if (floor(256*MP_RADIX / (double)prime + 0.5) == (double) reciproques[i]){
	corrections[i] = 1;
      }
      else {
	corrections[i] = 0;
	++reciproques[i];
      }
    }
    gen isqrtN=isqrt(N);
    vector<short int> isqrtNmodp(bs);
    for (int i=0;i<bs;++i){
      isqrtNmodp[i]=smod(isqrtN,basis[i]).val;
    }
    isqrtN.uncoerce(); N.uncoerce();
    int shift;
    vector<slicetype> slice(QS_SIZE);
    // relations will be written in column
    vector<int> shiftvalues;
    vector<unsigned short> additional_primes;
    vector<bool> additional_primes_twice;
    vector< vector<unsigned short> > puissances;
#ifdef GIAC_ADDITIONAL_PRIMES
    shiftvalues.reserve(2*bs);
    additional_primes.reserve(bs);
    additional_primes_twice.reserve(bs);
    puissances.reserve(2*bs);
#else
    shiftvalues.reserve(bs+1);
    puissances.reserve(bs+1);
#endif
    unsigned todo_rel;
    for (i=0;i<M/QS_SIZE;++i){
      if (ctrl_c)
	return false;
      // pass 1, positive
      shift=i*QS_SIZE;
      sieve(basis,additional_primes,additional_primes_twice,sqrtmod,reciproques,corrections,N,isqrtN,isqrtNmodp,slice,shift,puissances,shiftvalues,contextptr);
      todo_rel=bs+15+additional_primes.size();
      if (debug_infolevel)
	*logptr(contextptr) << clock()<< " sieve : " << shiftvalues.size() << " relations of " << todo_rel << endl;
      if (shiftvalues.size()>=todo_rel)
	break;
      // pass 2, negative
      shift=-(i+1)*QS_SIZE;
      sieve(basis,additional_primes,additional_primes_twice,sqrtmod,reciproques,corrections,N,isqrtN,isqrtNmodp,slice,shift,puissances,shiftvalues,contextptr);
      todo_rel=bs+15+additional_primes.size();
      if (debug_infolevel)
	*logptr(contextptr) << clock()<< " sieve : " << shiftvalues.size() << " relations of " << todo_rel << endl;
      if (shiftvalues.size()>=todo_rel)
	break;
    }
    release_memory(slice);
#ifdef GIAC_ADDITIONAL_PRIMES
    *logptr(contextptr) << clock() << " removing additional primes" << endl;
    // remove relations with additional primes which are used only once
    int lastp=puissances.size()-1,lasta=additional_primes.size()-1;
    for (unsigned i=0;i<=lastp;++i){
      vector<unsigned short> & cur=puissances[i];
      if (cur.empty())
	continue;
      unsigned short u=cur.back();
      int pos=equalposcomp(additional_primes,u);
      if (!pos)
	continue;
      --pos;
      if (additional_primes_twice[pos])
	continue;
      swap(cur,puissances[lastp]);
      shiftvalues[i]=shiftvalues[lastp];
      --lastp;
      additional_primes[pos]=additional_primes[lasta];
      additional_primes_twice[pos]=additional_primes_twice[lasta];
      --lasta;
      --i; // recheck at current index
    }
    puissances.resize(lastp+1);
    shiftvalues.resize(lastp+1);
    additional_primes.resize(lasta+1);
    *logptr(contextptr) << clock() << " end removing additional primes" << endl;
#endif
    // Make relations matrix (currently dense, -> improve to sparse and Lanczos algorithm)
    vector< vector<unsigned> > relations(puissances.size(),vector<unsigned>(int(std::ceil(puissances.size()/128.))*4));
    // sort(additional_primes.begin(),additional_primes.end());
    for (unsigned j=0;j<puissances.size();j++){
      vector<unsigned short> & curpui=puissances[j];
      bool done=false;
      int i=0; // position in basis
      unsigned k=0; // position in curpui
      unsigned short p=0; // prime
      for (;k<curpui.size();++k){
	p=curpui[k];
	if (p==0xffff){
	  relations[0][j/32] |= (1 << (j%32));
	  continue;
	}
	if (p==0){
	  done=true;
	  continue;
	}
	if (!done){
	  if (p%2==0) // even exponent?
	    continue;
	  p >>= 8;
	}
	else {
	  int c=1;
	  for (;k+1<curpui.size();c++){
	    if (curpui[k+1]==p)
	      ++k;
	    else
	      break;
	  }
	  if (c%2==0)
	    continue;
	}
	// advance to next i in basis
	for (;i<bs;++i){
	  if (basis[i]==p)
	    break;
	}
	if (i<bs){
	  ++i;
	  relations[i][j/32] |= (1 << (j %32));
	}
	else {
	  // k must be == curpui.size()-1
	  // find p in additional_primes and position
	  int Ppos=equalposcomp(additional_primes,p);
	  // *logptr(contextptr) << p << " " << Ppos+bs << " " << relations.size() << endl;
	  relations[bs+Ppos][j/32] |= (1 << (j %32));	  
	}
      } // end loop on k in curpui
    } // end loop on j in puissances
    // FIXME remove lines with only one 1 in relations and basis or additional_primes
    // now reduce relations
    // printbool(*logptr(contextptr),relations);
    *logptr(contextptr) << clock() << " begin rref size " << relations.size() << "x" << relations.front().size()*32 << " K " << 0.004*relations.size()*relations.front().size() << endl;
    rref(relations);
    *logptr(contextptr) << clock() << " end rref" << endl;
    // printbool(*logptr(contextptr),relations);
    // move pivots on the diagonal by inserting 0 lines
    int l=relations.front().size(),l32=l*32;
    vector< vector<unsigned> > relations2(l32);
    i=0;
    int j=0,rs=relations.size();
    for (;i<rs && j<l32;++j){
      if (relations[i][j/32] & (1 << j%32)){
	swap(relations2[j],relations[i]);
	++i;
      }
    }
    // printbool(*logptr(contextptr),relations2);
    // for each element of the kernel compute x and y / x^2=y^2[N] 
    // then gcd(x-y,N)
    for (i=0;i<l32;++i){
      if (!relations2[i].empty() && (relations2[i][i/32] & (1<<i%32)))
	continue;
      // using column i of relations2 which is in the kernel, build x and y
      // for x, we can compute the product of the isqrtN+shiftvalues mod N
      // for y, compute the product (isqrtN+shiftvalues)^2-N
      // then the isqrt
      gen x=1,y=1,cur;
      vector<short int> p(bs),add_p(additional_primes.size());
      for (unsigned j=0;int(j)<l32;++j){
	if (j<shiftvalues.size() && (i==j || (!relations2[j].empty() && (relations2[j][i/32] & (1<<(i%32)))))){
	  cur=isqrtN+shiftvalues[j];
	  x=smod(x*cur,N);
	  bool done=false;
	  unsigned bi=0;
	  vector<unsigned short>::const_iterator it=puissances[j].begin(),itend=puissances[j].end();
	  for (;it!=itend;++it){
	    if (*it==0xffff)
	      continue;
	    if (!*it){
	      done=true;
	      continue;
	    }
	    if (done){
	      while (bi<basis.size() && basis[bi]!=*it)
		++bi;
	      if (bi<basis.size())
		p[bi]++;
	      else {
		int pos=equalposcomp(additional_primes,*it);
		if (pos) // otherwise ERROR!!!
		  ++add_p[pos-1];
	      }
	    }
	    else {
	      while (basis[bi]!=(*it>>8))
		++bi;
	      p[bi]+=(*it&0xff);
	    }
	  }
	}
      }
      for (int i=0;i<bs;++i){
	if (p[i] % 2)
	  *logptr(contextptr) << "error, odd exponent for prime " << basis[i] << endl;
	if (p[i])
	  y=smod(y*pow(gen(basis[i]),int(p[i]/2)),N);
      }
      for (unsigned i=0;i<additional_primes.size();++i){
	if (add_p[i] % 2)
	  *logptr(contextptr) << "error" << i << endl;
	if (add_p[i])
	  y=smod(y*pow(gen(additional_primes[i]),int(add_p[i]/2)),N);
      }
      cur=gcd(x-y,N);
      if (debug_infolevel>6)
	*logptr(contextptr) << clock() << "checking gcd" << cur << " " << N << endl;
      if ( (cur.type==_INT_ && cur.val>7) || 
	   (cur.type==_ZINT && is_strictly_greater(n,cur,context0))){
	if (n==N)
	  pn=cur;
	else
	  pn=cur/gcd(3*5*7,cur);
	return true;
      }
    }
    return false;
  }

  const int POLLARD_GCD=64;
#ifdef GIAC_MPQS 
#ifdef USE_GMP_REPLACEMENTS
  const int POLLARD_MAXITER=20000;
#else
  const int POLLARD_MAXITER=10000;
#endif
#else
  const int POLLARD_MAXITER=1000000;
#endif  

  static gen pollard(gen n, gen k,GIAC_CONTEXT){
    k.uncoerce();
    n.uncoerce();
    int m,m1,a,a1,j;
    m1=m=2;
    a1=a=1;
    int c=0;
    mpz_t g,x,x1,x2,x2k,y,y1,p,q,tmpq;
    mpz_init_set_si(g,1); // ? mp_init_size to specify size
    mpz_init_set_si(x,2);
    mpz_init_set_si(x1,2);
    mpz_init_set_si(y,2);
    mpz_init(y1);
    mpz_init(x2);
    mpz_init(x2k);
    mpz_init_set_si(p,1);
    mpz_init(q);
    mpz_init(tmpq);
    while (!ctrl_c && mpz_cmp_si(g,1)==0) {
      a=2*a+1;//a=2^(e+1)-1=2*l(m)-1 
      while (!ctrl_c && mpz_cmp_si(g,1)==0 && a>m) { // ok
	// x=f(x,k,n,q);
#ifdef USE_GMP_REPLACEMENTS
	mp_sqr(&x,&x2);
#else 
	mpz_mul(x2,x,x);
#endif
	mpz_add(x2k,x2,*k._ZINTptr);
	mpz_tdiv_qr(tmpq,x,x2k,*n._ZINTptr);
	m += 1;
	if (m > POLLARD_MAXITER )
	  return -1;
	// p=irem(p*(x1-x),n,q);
	mpz_sub(q,x1,x);
	mpz_mul(x2,p,q);
	mpz_tdiv_qr(tmpq,p,x2,*n._ZINTptr);
	c += 1;
	if (c==POLLARD_GCD) {
	  // g=gcd(abs(p,context0),n); 
	  mpz_abs(q,p);
	  mpz_gcd(g,q,*n._ZINTptr);
	  if (mpz_cmp_si(g,1)==0) {
	    mpz_set(y,x); // y=x;
	    mpz_set(y1,x1); // y1=x1;
	    mpz_set_si(p,1); // p=1;
	    a1=a;
	    m1=m;
	    c=0;
	  }
	}
      }//m=a=2^e-1=l(m)
      if (mpz_cmp_si(g,1)==0) {
	mpz_set(x1,x); // x1=x;//x1=x_m=x_l(m)-1
	j=3*(a+1)/2; // j=3*iquo(a+1,2);
	for (long i=m+1;i<=j;i++){
	  // x=f(x,k,n,q);
	  mpz_mul(x2,x,x);
	  mpz_add(x2k,x2,*k._ZINTptr);
	  mpz_tdiv_qr(tmpq,x,x2k,*n._ZINTptr);
	}
	m=j;
      }
    }
    //g<>1 ds le paquet de POLLARD_GCD
    if (debug_infolevel>5)
      cerr << "Pollard nloops " << m << endl;
    mpz_set(x,y); // x=y;
    mpz_set(x1,y1); // x1=y1;
    mpz_set_si(g,1); // g=1;
    a=(a1-1)/2; // a=iquo(a1-1,2);
    m=m1;
    while (!ctrl_c && mpz_cmp_si(g,1)==0) {
      a=2*a+1;
      while (!ctrl_c && mpz_cmp_si(g,1)==0 && a>m) { // ok
	// x=f(x,k,n,q);
	mpz_mul(x2,x,x);
	mpz_add(x2k,x2,*k._ZINTptr);
	mpz_tdiv_qr(tmpq,x,x2k,*n._ZINTptr);
	m += 1;
	if (m > POLLARD_MAXITER )
	  return -1;
	// p=irem(x1-x,n,q);
	mpz_sub(q,x1,x);
	mpz_tdiv_qr(tmpq,p,q,*n._ZINTptr);
	// g=gcd(abs(p,context0),n);  // ok
	mpz_abs(q,p);
	mpz_gcd(g,q,*n._ZINTptr);
      }
      if (mpz_cmp_si(g,1)==0) {
	mpz_set(x1,x); // x1=x;
	j=3*(a+1)/2; // j=3*iquo(a+1,2);
	for (long i=m+1;j>=i;i++){
	  // x=f(x,k,n,q);
	  mpz_mul(x2,x,x);
	  mpz_add(x2k,x2,*k._ZINTptr);
	  mpz_tdiv_qr(tmpq,x,x2k,*n._ZINTptr);
	}
	m=j;
      }
    }
    mpz_clear(tmpq);
    mpz_clear(x);
    mpz_clear(x1);
    mpz_clear(x2);
    mpz_clear(x2k);
    mpz_clear(y);
    mpz_clear(y1);
    mpz_clear(p);
    mpz_clear(q);
    if (ctrl_c){
      mpz_clear(g);
      return 0;
    }
    if (mpz_cmp(g,*n._ZINTptr)==0) {
      if (k==1) {
	mpz_clear(g);
	return(pollard(n,-1,contextptr)); 
      }
      else {
	if (k*k==1){
	  mpz_clear(g);
	  return(pollard(n,3,contextptr));
	}
	else {
	  ref_mpz_t * ptr=new ref_mpz_t;
	  mpz_init_set(ptr->z,g);
	  mpz_clear(g);
	  return ptr;
	} 
      }
    } 
    ref_mpz_t * ptr=new ref_mpz_t;
    mpz_init_set(ptr->z,g);
    mpz_clear(g);
    return ptr;
  }

  // const short int giac_primes[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997};

  static const char _ithprime_s []="ithprime";
  static symbolic symb_ithprime(const gen & args){
    return symbolic(at_ithprime,args);
  }
  static gen ithprime(const gen & g,GIAC_CONTEXT){
    if (g.type!=_INT_)
      return symb_ithprime(g);
    int i=g.val;
    if (i<0)
      return gendimerr(contextptr);
    if (i==0)
      return 1;
    if (i<=sizeof(giac_primes)/sizeof(short int))
      return giac_primes[i-1];
    return symb_ithprime(g);
  }
  gen _ithprime(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_ithprime,contextptr);
    return ithprime(args,contextptr);
  }
  static define_unary_function_eval (__ithprime,&giac::_ithprime,_ithprime_s);
  define_unary_function_ptr5( at_ithprime ,alias_at_ithprime,&__ithprime,0,true);

  bool is_divisible_by(const gen & n,unsigned long a){
    if (n.type==_ZINT){
#ifdef USE_GMP_REPLACEMENTS
      mp_digit c;
      mp_mod_d(n._ZINTptr, a, &c);
      return c==0;
#else
      return mpz_divisible_ui_p(*n._ZINTptr,a);
#endif
    }
    return n.val%a==0;
  }

  // find trivial factors of n, 
  // if add_last is true the remainder is put in the vecteur,
  // otherwise n contains the remainder
  vecteur pfacprem(gen & n,bool add_last,GIAC_CONTEXT){
    gen a;
    gen q;
    int p,i,prime;
    vecteur v(2);
    vecteur u;
    if (is_zero(n))
      return u;
    if (n.type==_ZINT){
      ref_mpz_t * cur = new ref_mpz_t;
      mpz_t div,q,r;
      mpz_set(cur->z,*n._ZINTptr);
      mpz_init_set(q,*n._ZINTptr);
      mpz_init(r);
      mpz_init(div);
      for (i=0;i<sizeof(giac_primes)/sizeof(short int);++i){
	if (mpz_cmp_si(cur->z,1)==0) 
	  break;
	prime=giac_primes[i];
#ifdef USE_GMP_REPLACEMENTS
	mp_digit c;
	mp_mod_d(&cur->z, prime, &c);
	if (c==0){
	  mpz_set_ui(div,prime);
	  for (p=0;;p++){
	    mpz_tdiv_qr(q,r,cur->z,div);
	    if (mpz_cmp_si(r,0))
	      break;
	    mp_exch(&cur->z,&q);
	  }
	  // *logptr(contextptr) << "Factor " << prime << " " << p << endl;
	  u.push_back(prime);
	  u.push_back(p);
	}
#else
	if (mpz_divisible_ui_p(cur->z,prime)){
	  mpz_set_ui(div,prime);
	  for (p=0;;p++){
	    mpz_tdiv_qr(q,r,cur->z,div);
	    if (mpz_cmp_si(r,0))
	      break;
	    mpz_swap(cur->z,q);
	  }
	  // *logptr(contextptr) << "Factor " << prime << " " << p << endl;
	  u.push_back(prime);
	  u.push_back(p);
	}
#endif
      } // end for on smal primes
      mpz_clear(div); mpz_clear(r); mpz_clear(q);
      n=cur;
    }
    else {
      for (i=0;i<sizeof(giac_primes)/sizeof(short int);++i){
	if (n==1) 
	  break;
	a.val=giac_primes[i];
	p=0;
	while (is_divisible_by(n,a.val)){ // while (irem(n,a,q)==0){
	  n=iquo(n,a); 
	  p=p+1;
	}
	if (p!=0){
	  // *logptr(contextptr) << "Factor " << a << " " << p << endl;
	  u.push_back(a);
	  u.push_back(p);
	}
      }
    }
    if (add_last && i==1229 && !is_one(n)){
      u.push_back(n);
      u.push_back(1);
      n=1;
    }
    //v[0]=n;
    //v[1]=u;
    
    return(u);
  }

#ifdef USE_GMP_REPLACEMENTS
  static gen inpollardsieve(const gen &a,gen k,GIAC_CONTEXT){
    gen b=pollard(a,k,contextptr);
#ifdef GIAC_MPQS
    if (b==-1){ 
      if (sieve(a,b,contextptr)) return b; else return -1; }
#endif
    return b;
  }
  static gen pollardsieve(const gen &a,gen k,GIAC_CONTEXT){
    int debug_infolevel_=debug_infolevel;
    debug_infolevel=6;
    gen res=inpollardsieve(a,k,contextptr);
    debug_infolevel=debug_infolevel_;
    return res;
  }
#else // USE_GMP_REPLACEMENTS
  static gen pollardsieve(const gen &a,gen k,GIAC_CONTEXT){
    gen b=pollard(a,k,contextptr);
#ifdef GIAC_MPQS
    if (b==-1){ 
      if (sieve(a,b,contextptr)) return b; else return -1; }
#endif
    return b;
  }
#endif // USE_GMP_REPLACEMENTS


  static vecteur facprem(gen & n,GIAC_CONTEXT){
    gen a,b,q;
    int p;
    vecteur v(2);    
    if (n==1) {return vecteur(0);}
    if ( (n.type==_INT_ && n.val<giac_last_prime*giac_last_prime) || is_probab_prime_p(n)) {
      v[0]=n;v[1]=1;
      n=1;
      return v;
    }
    b=n;
    if (debug_infolevel>5)
      cerr << "Pollard begin " << clock() << endl;
    a=pollardsieve(b,1,contextptr);
    if (a==-1)
      return makevecteur(gensizeerr(gettext("Quadratic sieve failure, perhaps number too large")));
    if (is_zero(a))
      return makevecteur(gensizeerr(gettext("Stopped by user interruption")));
    p=0;
    while ( a!=b && is_greater(a,giac_last_prime*giac_last_prime,context0) && is_probab_prime_p(a)==0 ) {
      b=a;
      a=pollardsieve(b,1,contextptr);
      if (a==-1)
	return makevecteur(gensizeerr(gettext("Quadratic sieve failure, perhaps number too large")));
      if (is_zero(a))
	return makevecteur(gensizeerr(gettext("Stopped by user interruption")));
    }
    if (debug_infolevel>5)
      cerr << "Pollard end " << clock() << endl;
    //n=iquo(n,a);
    
    while (irem(n,a,q)==0){
      n=iquo(n,a); // n=q does not work because irem assumes n and q does not point to the same _ZINT
      p=p+1;
    }
    v[0]=a;
    if (a==b){v[1]=-p;} else {v[1]=p;}
    return v;
  }

  vecteur ifactors(const gen & n0,GIAC_CONTEXT){
    if (is_zero(n0))
      return vecteur(1,gensizeerr(gettext("ifactors")));
    if (is_one(n0))
      return vecteur(0);
#ifdef HAVE_LIBPARI
    gen g(pari_ifactor(n0),context0); // FIXME GIAC_CONTEXT
    if (g.type==_VECT){
      matrice m(mtran(*g._VECTptr));
      vecteur res;
      const_iterateur it=m.begin(),itend=m.end();
      for (;it!=itend;++it){
	if (it->type!=_VECT) return vecteur(1,gensizeerr(gettext("ifactor.cc/ifactors")));
	res.push_back(it->_VECTptr->front());
	res.push_back(it->_VECTptr->back());
      }
      return res;
    }
#else // LIBPARI
    gen n(n0);
    vecteur f;
    vecteur g;
    vecteur u;
    f=pfacprem(n,false,contextptr);
    //cout<<n<<" "<<f<<endl;
    while (n!=1) {
      g=facprem(n,contextptr);
      if (is_undef(g))
	return g;
      //cout<<n<<" "<<g<<endl;
      u=mergevecteur(u,g);
    }
    
    g=mergevecteur(f,u);
    return g;
#endif // LIBPARI
    return 0;
  }

  vecteur ifactors(const gen & r,const gen & i,const gen & ri,GIAC_CONTEXT){
    gen norm=r*r+i*i;
    gen reste(ri);
    const vecteur & facto = ifactors(norm,contextptr);
    if (is_undef(facto))
      return facto;
    int l=facto.size()/2;
    vecteur res;
    for (int i=0;i<l;++i){
      gen prime=facto[2*i];
      int mult=facto[2*i+1].val,multp=0;
      int n=smod(prime,4).val;
      if (n==2){
	res.push_back(1+cst_i);
	res.push_back(mult);
	reste=reste/pow(1+cst_i,mult,contextptr);
	continue;
      }
      if (n==-1){
	res.push_back(prime);
	res.push_back(mult/2);
	reste=reste/pow(prime,mult/2,contextptr);
	continue;
      }
      prime=pa2b2(prime,contextptr);
      prime=gen(prime[0],prime[1]);
      for (;mult>0;--mult,++multp){
	if (!is_zero(reste % prime))
	  break;
	reste=reste/prime;
      }
      if (multp){
	res.push_back(prime);
	res.push_back(multp);
      }
      if (mult){
	prime=conj(prime,contextptr);
	res.push_back(prime);
	res.push_back(mult);
	reste=reste/pow(prime,mult,contextptr);
      }
    }
    if (!is_one(reste)){
      res.insert(res.begin(),1);
      res.insert(res.begin(),reste);
    }
    return res;
  }

  gen ifactors(const gen & args,int maplemode,GIAC_CONTEXT){
    if ( (args.type==_INT_) || (args.type==_ZINT)){
      if (is_zero(args)){
	if (maplemode==1)
	  return makevecteur(args,vecteur(0));
	else
	  return makevecteur(args);
      }
      vecteur v(ifactors(abs(args,contextptr),contextptr)); // ok
      if (!v.empty() && is_undef(v.front()))
	return v.front();
      if (maplemode!=1){
	if (is_positive(args,context0))
	  return v;
	return mergevecteur(makevecteur(minus_one,plus_one),v);
      }
      vecteur res;
      const_iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;it+=2){
	res.push_back(makevecteur(*it,*(it+1)));
      }
      if (is_positive(args,context0))
	return makevecteur(plus_one,res);
      else
	return makevecteur(minus_one,res);	
    }
    if (args.type==_CPLX && is_integer(*args._CPLXptr) && is_integer(*(args._CPLXptr+1)))
      return ifactors(*args._CPLXptr,*(args._CPLXptr+1),args,contextptr);
    return gentypeerr(gettext("ifactors"));
  }
  static symbolic symb_ifactors(const gen & args){
    return symbolic(at_ifactors,args);
  }
  gen _ifactors(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_ifactors,contextptr);
    gen g(args);
    if (!is_integral(g))
      return gensizeerr(contextptr);
    return ifactors(g,0,contextptr);
  }
  static const char _ifactors_s []="ifactors";
  static define_unary_function_eval (__ifactors,&giac::_ifactors,_ifactors_s);
  define_unary_function_ptr5( at_ifactors ,alias_at_ifactors,&__ifactors,0,true);

  static const char _facteurs_premiers_s []="facteurs_premiers";
  static define_unary_function_eval (__facteurs_premiers,&giac::_ifactors,_facteurs_premiers_s);
  define_unary_function_ptr5( at_facteurs_premiers ,alias_at_facteurs_premiers,&__facteurs_premiers,0,true);

  static const char _maple_ifactors_s []="maple_ifactors";
  static symbolic symb_maple_ifactors(const gen & args){
    return symbolic(at_maple_ifactors,args);
  }
  gen _maple_ifactors(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_maple_ifactors,contextptr);
    return ifactors(args,1,contextptr);
  }
  static define_unary_function_eval (__maple_ifactors,&giac::_maple_ifactors,_maple_ifactors_s);
  define_unary_function_ptr5( at_maple_ifactors ,alias_at_maple_ifactors,&__maple_ifactors,0,true);

  static vecteur in_factors(const gen & gf,GIAC_CONTEXT){
    if (gf.type!=_SYMB)
      return makevecteur(gf,plus_one);
    unary_function_ptr & u=gf._SYMBptr->sommet;
    if (u==at_inv){
      vecteur v=in_factors(gf._SYMBptr->feuille,contextptr);
      iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;it+=2)
	*(it+1)=-*(it+1);
      return v;
    }
    if (u==at_neg){
      vecteur v=in_factors(gf._SYMBptr->feuille,contextptr);
      v.push_back(minus_one);
      v.push_back(plus_one);
      return v;
    }
    if ( (u==at_pow) && (gf._SYMBptr->feuille._VECTptr->back().type==_INT_) ){
      vecteur v=in_factors(gf._SYMBptr->feuille._VECTptr->front(),contextptr);
      gen k=gf._SYMBptr->feuille._VECTptr->back();
      iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;it+=2)
	*(it+1)=k* *(it+1);
      return v;
    }
    if (u!=at_prod)
      return makevecteur(gf,plus_one);
    vecteur res;
    const_iterateur it=gf._SYMBptr->feuille._VECTptr->begin(),itend=gf._SYMBptr->feuille._VECTptr->end();
    for (;it!=itend;++it){
      res=mergevecteur(res,in_factors(*it,contextptr));
    }
    return res;
  }
  vecteur factors(const gen & g,const gen & x,GIAC_CONTEXT){
    gen gf=factor(g,x,false,contextptr);
    vecteur res=in_factors(gf,contextptr);
    if (xcas_mode(contextptr)!=1)
      return res;
    gen coeff(1);
    vecteur v;
    const_iterateur it=res.begin(),itend=res.end();
    for (;it!=itend;it+=2){
      if (lidnt(*it).empty())
	coeff=coeff*(pow(*it,*(it+1),contextptr));
      else
	v.push_back(makevecteur(*it,*(it+1)));
    }
    return makevecteur(coeff,v);
  }
  static const char _factors_s []="factors";
  gen _factors(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_factors,contextptr);
    return factors(args,vx_var,contextptr);
  }
  static define_unary_function_eval (__factors,&giac::_factors,_factors_s);
  define_unary_function_ptr5( at_factors ,alias_at_factors,&__factors,0,true);

  gen ifactors2ifactor(const vecteur & l){
    int s;
    s=l.size();
    gen r;
    vecteur v(s/2);
    for (int j=0;j<s;j=j+2){
      if (!is_one(l[j+1]))
	v[j/2]=symbolic(at_pow,gen(makevecteur(l[j],l[j+1]),_SEQ__VECT));
      else
	v[j/2]=l[j];
    }
    if (v.size()==1){
#if defined(GIAC_HAS_STO_38) && defined(CAS38_DISABLED)
      return symb_quote(v.front());
#else
      return v.front();
#endif
    }
    r=symbolic(at_prod,v);
#if defined(GIAC_HAS_STO_38) && defined(CAS38_DISABLED)
    r=symb_quote(r);
#endif
    return(r);
  }
  gen ifactor(const gen & n,GIAC_CONTEXT){
    vecteur l;
    l=ifactors(n,contextptr);
    if (!l.empty() && is_undef(l.front())) return l.front();
    return ifactors2ifactor(l);
  }
  gen _ifactor(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_CPLX && is_integer(*args._CPLXptr) && is_integer(*(args._CPLXptr+1))){
      const vecteur & v=ifactors(*args._CPLXptr,*(args._CPLXptr+1),args,contextptr);
      return ifactors2ifactor(v);
    }
    gen n=args;
    if (!is_integral(n))
      return gensizeerr(contextptr);
    if (is_strictly_positive(-n,0))
      return -_ifactor(-n,contextptr);
    if (n.type==_INT_ && n.val<=3)
      return n;
    return ifactor(n,contextptr);
  }
  static const char _ifactor_s []="ifactor";
  static define_unary_function_eval (__ifactor,&_ifactor,_ifactor_s);
  define_unary_function_ptr5( at_ifactor ,alias_at_ifactor,&__ifactor,0,true);

  static const char _factoriser_entier_s []="factoriser_entier";
  static define_unary_function_eval (__factoriser_entier,&_ifactor,_factoriser_entier_s);
  define_unary_function_ptr5( at_factoriser_entier ,alias_at_factoriser_entier,&__factoriser_entier,0,true);

  static vecteur divis(const vecteur & l3,GIAC_CONTEXT){
    vecteur l1(1);
    gen d,e;
    int s=l3.size();
    l1[0]=1;//l3.push_back(..);
    for (int k=0;k<s;k=k+2) {
      vecteur l2;
      int s1;
      s1=l1.size();
      vecteur l4(s1);
      d=l3[k];
      e=l3[k+1];
      int ei;
      if (e.type==_INT_){
	ei=e.val;
      }
      else
	return vecteur(1,gensizeerr(gettext("Integer too large")));
      for (int j=1;j<=ei;j++){
	for (int l=0;l<s1;l++){ 
	  l4[l]=l1[l]*pow(d,j);
	}
	l2=mergevecteur(l2,l4);
      }
      l1=mergevecteur(l1,l2);
    }
    return(l1); 
  }
  gen idivis(const gen & n,GIAC_CONTEXT){
    vecteur l3(ifactors(n,contextptr));
    if (!l3.empty() && is_undef(l3.front())) return l3.front();
    return divis(l3,contextptr);
  }
  gen _idivis(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_idivis,contextptr);
    gen n=args;
    if (!is_integral(n) && !is_integer(n)) 
      return gensizeerr(contextptr);
    return idivis(abs(n,contextptr),contextptr);
  }
  static const char _idivis_s []="idivis";
  static define_unary_function_eval (__idivis,&_idivis,_idivis_s);
  define_unary_function_ptr5( at_idivis ,alias_at_idivis,&__idivis,0,true);

  gen _divis(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_divis,contextptr);
    return divis(factors(args,vx_var,contextptr),contextptr);
  }
  static const char _divis_s []="divis";
  static define_unary_function_eval (__divis,&_divis,_divis_s);
  define_unary_function_ptr5( at_divis ,alias_at_divis,&__divis,0,true);

  /*
  gen ichinreme(const vecteur & a,const vecteur & b){
    vecteur r(2);
    gen p=a[1],q=b[1],u,v,d;
    egcd(p,q,u,v,d);
    if (d!=1)  return gensizeerr(contextptr);
    r[0]=(u*p*b[0]+v*q*a[0]%p*q);
    r[1]=p*q;
    return(r);
  }
  gen _ichinreme(const gen & args){
  if ( args){
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=4) )
      return gensizeerr(contextptr);
    vecteur a(2).type==_STRNG && args.subtype==-1{
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=4) )
      return gensizeerr(contextptr);
    vecteur a(2))) return  args){
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=4) )
      return gensizeerr(contextptr);
    vecteur a(2);
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=4) )
      return gensizeerr(contextptr);
    vecteur a(2),b(2);
    a[0]=args[0];
    a[1]=args[1];
    b[0]=args[2];
    b[1]=args[3];
    //gen a=args[0],p=args[1], b=args[2],q=args[3];
    return ichinreme(a,b);
  }
  static const char _ichinreme_s []="ichinreme";
  static define_unary_function_eval (__ichinreme,&_ichinreme,_ichinreme_s);
  define_unary_function_ptr5( at_ichinreme ,alias_at_ichinreme,&__ichinreme,0,true); 
  */

  gen euler(const gen & e,GIAC_CONTEXT){
    if (e==0)
      return e;
    vecteur v(ifactors(e,contextptr));
    if (!v.empty() && is_undef(v.front())) return v.front();
    const_iterateur it=v.begin(),itend=v.end();
    for (gen res(plus_one);;){
      if (it==itend)
	return res;
      gen p=*it;
      ++it;
      int n=it->val;
      res = res * (p-plus_one)*pow(p,n-1);
      ++it;
    }
  }
  static symbolic symb_euler(const gen & args){
    return symbolic(at_euler,args);
  }
  static const char _euler_s []="euler";
  gen _euler(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,_euler,contextptr);
    if ( is_integer(args) && is_positive(args,contextptr))
      return euler(args,contextptr);
    return gentypeerr(contextptr);
  }
  static define_unary_function_eval (__euler,&giac::_euler,_euler_s);
  define_unary_function_ptr5( at_euler ,alias_at_euler,&__euler,0,true);

  gen pa2b2(const gen & p,GIAC_CONTEXT){
    if ((p%4)!=1) return gensizeerr(gettext("pa2b2"));// car p!=1 mod 4
    gen q=(p-1)/4;
    gen a=2;
    gen ra;
    ra=powmod(a,q,p);
    //on cherche ra^2=-1 mod p avec ra!=1 et ra !=p-1
    while ((a!=p-1) && ((ra==1)|| (ra==p-1))){
      a=a+1;
      ra=powmod(a,q,p);
    }
    if ((ra==1)||(ra==p-1))  return gensizeerr(gettext("pa2b2"));//car p n'est pas premier
    gen ux=1,uy=ra,vx=0,vy=p,wx,wy; 
    gen m=1;
    while(m!=0){
      if (is_positive(vx*vx+vy*vy-ux*ux+uy*uy,0)){
	//on echange u et v
	wx=vx;
	wy=vy;
	vx=ux;
	vy=uy;
	ux=wx;
	uy=wy;
      }
      gen alpha=inv(2,contextptr)-(ux*vx+uy*vy)*inv(vx*vx+vy*vy,contextptr);
      //m=partie entiere de alpha (-v.v/2<(u+mv).v<=v.v/2)
      m=_floor(alpha,contextptr);
      ux=ux+m*vx;
      uy=uy+m*vy;
    }
    vecteur v(2);
    //v repond a la question
    v[0]=abs(vx,contextptr); // ok
    v[1]=abs(vy,contextptr); // ok
    if (vx*vx+vy*vy!=p)
      return gensizeerr(gettext("pa2b2"));
    return v;
  }
  gen _pa2b2(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (!is_integer(args)) 
      return gensizeerr(contextptr);
    gen n=args;
    return pa2b2(n,contextptr);
  }
  static const char _pa2b2_s []="pa2b2";
  static define_unary_function_eval (__pa2b2,&_pa2b2,_pa2b2_s);
  define_unary_function_ptr5( at_pa2b2 ,alias_at_pa2b2,&__pa2b2,0,true);

  static gen ipropfrac(const gen & a,const gen & b){
    gen r=a%b;
    gen q=(a-r)/b;
    gen d=gcd(r,b);
    r=r/d;
    gen b1=b/d;
    gen v;
    v=symbolic(at_division,makevecteur(r,b1));
    gen w;
    w=symbolic(at_plus,makevecteur(q,v));    
    return w;
  }
  gen _propfrac(const gen & arg,GIAC_CONTEXT){
    if ( arg.type==_STRNG && arg.subtype==-1) return  arg;
    gen args(arg);
    vecteur v;
    if (arg.type==_VECT && arg._VECTptr->size()==2){
      v=vecteur(1,arg._VECTptr->back());
      args=arg._VECTptr->front();
      lvar(args,v);
    }
    else
      v=lvar(arg);
    gen g=e2r(args,v,contextptr);
    gen a,b;
    fxnd(g,a,b);
    if (v.empty())
      return ipropfrac(a,b);
    else {
      gen d=r2e(b,v,contextptr);
      g=_quorem(makevecteur(r2e(a,v,contextptr),d,v.front()),contextptr);
      if (is_undef(g)) return g;
      vecteur &v=*g._VECTptr;
      return v[0]+rdiv(v[1],d);
    }
  }
  static const char _propfrac_s []="propfrac";
  static define_unary_function_eval (__propfrac,&_propfrac,_propfrac_s);
  define_unary_function_ptr5( at_propfrac ,alias_at_propfrac,&__propfrac,0,true);

  gen iabcuv(const gen & a,const gen & b,const gen & c){
    gen d=gcd(a,b);
    if (c%d!=0)  return gensizeerr(gettext("iabcuv"));
    gen a1=a/d,b1=b/d,c1=c/d;
    gen u,v,w;
    egcd(a1,b1,u,v,w);
    vecteur r(2);
    r[0]=smod(u*c1,b);
    r[1]=iquo(c-r[0]*a,b);
    return r;
  }
  gen _iabcuv(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=3) )
      return gensizeerr(contextptr);
    gen a=args[0],b=args[1],c=args[2];
    return iabcuv(a,b,c);
  }
  static const char _iabcuv_s []="iabcuv";
  static define_unary_function_eval (__iabcuv,&_iabcuv,_iabcuv_s);
  define_unary_function_ptr5( at_iabcuv ,alias_at_iabcuv,&__iabcuv,0,true);

  gen abcuv(const gen & a,const gen & b,const gen & c,const gen & x,GIAC_CONTEXT){
    gen g=_egcd(makevecteur(a,b,x),contextptr);
    if (is_undef(g)) return g;
    vecteur & v=*g._VECTptr;
    gen h=_quorem(makevecteur(c,v[2],x),contextptr);
    if (is_undef(h)) return h;
    vecteur & w=*h._VECTptr;
    if (!is_zero(w[1]))
      return gensizeerr(gettext("No solution in ring"));
    gen U=v[0]*w[0],V=v[1]*w[0];
    if (_degree(makevecteur(c,x),contextptr).val<_degree(makevecteur(a,x),contextptr).val+_degree(makevecteur(b,x),contextptr).val ){
      U=_rem(makevecteur(U,b,x),contextptr);
      V=_rem(makevecteur(V,a,x),contextptr);
    }
    return makevecteur(U,V);
  }
  gen _abcuv(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if ( (args.type!=_VECT) || (args._VECTptr->size()<3) )
      return gensizeerr(contextptr);
    vecteur & v =*args._VECTptr;
    if (v.size()>3)
      return abcuv(v[0],v[1],v[2],v[3],contextptr);
    return abcuv(v[0],v[1],v[2],vx_var,contextptr);
  }
  static const char _abcuv_s []="abcuv";
  static define_unary_function_eval (__abcuv,&_abcuv,_abcuv_s);
  define_unary_function_ptr5( at_abcuv ,alias_at_abcuv,&__abcuv,0,true);

  gen simp2(const gen & a,const gen & b,GIAC_CONTEXT){
    vecteur r(2);
    gen d=gcd(a,b);
    r[0]=normal(a/d,contextptr);
    r[1]=normal(b/d,contextptr);
    return r;
  }
  gen _simp2(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=2) )
      return gensizeerr(contextptr);
    gen a=args[0],b=args[1];
    if ( (a.type==_VECT) || (b.type==_VECT) )
      return gensizeerr(contextptr);
    return simp2(a,b,contextptr);
  }
  static const char _simp2_s []="simp2";
  static define_unary_function_eval (__simp2,&_simp2,_simp2_s);
  define_unary_function_ptr5( at_simp2 ,alias_at_simp2,&__simp2,0,true);
 
  gen fxnd(const gen & a){
    vecteur v(lvar(a));
    gen g=e2r(a,v,context0); // ok
    gen n,d;
    fxnd(g,n,d);
    return makevecteur(r2e(n,v,context0),r2e(d,v,context0)); // ok
  }
  gen _fxnd(const gen & args,GIAC_CONTEXT){
    if ( args.type==_STRNG && args.subtype==-1) return  args;
    if (args.type==_VECT)
      return apply(args,fxnd);
    return fxnd(args);
  }
  static const char _fxnd_s []="fxnd";
  static define_unary_function_eval (__fxnd,&_fxnd,_fxnd_s);
  define_unary_function_ptr5( at_fxnd ,alias_at_fxnd,&__fxnd,0,true);

#if 0
  static inline gen f(gen x,gen k,gen n){
    return (x*x+k)%n;
  }
  
  static gen pollar(gen n, gen k){
    gen g,m,m1,a,a1,c,x,x1,y,y1,p,j;
    g=1;
    m=2;
    a=1;
    x=2;
    y=2;
    x1=2;
    p=1;
    c=0;
    
    // cout<<"k=(1,-1,3)"<<endl;
    //cin>>k;
    while (g==1) {
      a=2*a+1;//a=2^(e+1)-1=2*l(m)-1 
      while (is_strictly_greater(a,m,context0) && (g==1)) { // ok
	x=f(x,k,n);
	m=m+1;
	p=(p*(x1-x))%n;
	//if (is_strictly_positive(-p)) {p=p+n;}
	c=c+1;
	if (c==32) {
	  g=gcd(abs(p,context0),n); // ok
	  if (g==1) {
	    y=x;
	    y1=x1;
	    a1=a;
	    m1=m;
	    c=0;
	    p=1;
	  }
	}
      }//m=a=2^e-1=l(m)
      if (g==1) {
	x1=x;//x1=x_m=x_l(m)-1
	j=3*iquo(a+1,2);
	for (gen i=m+1;is_positive(j-i,0);i=i+1){
	  x=f(x,k,n);//x_i de i=2^e=l(m) jusque 3*l(m)/2
	}
	m=j;
      }
    }
    //g<>1 ds le paquet de 32
    
    g=1;
    a=iquo(a1-1,2);
    m=m1;
    x=y;
    x1=y1;
    while (g==1) {
      a=2*a+1;
      while (is_strictly_greater(a,m,context0) && (g==1)) { // ok
	x=f(x,k,n);
	m=m+1;
	p=(x1-x)%n;
	//if (is_strictly_positive(-p)) {p=p+n;}
	//c=c+1;
	g=gcd(abs(p,context0),n);  // ok
      }
      if (g==1) {
	x1=x;
	j=3*iquo(a+1,2);
	for (gen i=m+1;is_positive(j-i,0);i=i+1){
	  x=f(x,k,n);//x_i
	}
	m=j;
	//x=f(x1,k,n);
	//m=m+1;
      }
    }
    
    
    if (g==n) {
      if (k==1) 
	return(pollar(n,-1)); 
      else {
	if (k*k==1)
	  return(pollar(n,3));
	else
	  return(g); 
      }
    } 
    else {
      //n=iquo(n,g);
      //cout<<g<<endl;
      return (g);
    }  
  }

  static inline gen f(const gen &x,const gen &k,const gen &n,gen &q){
    return irem(x*x+k,n,q);
  }
  
  static gen pollar_old(gen n, gen k){
    gen g,x,x1,y,y1,p,q;
    long m,m1,a,a1,j;
    g=1;
    m=2;
    a=1;
    x=2;
    y=2;
    x1=2;
    p=1;
    int c=0;
    while (g==1) {
      a=2*a+1;//a=2^(e+1)-1=2*l(m)-1 
      while (g==1 && a>m) { // ok
	x=f(x,k,n,q);
	m += 1;
	if (m > long(1)<<30)
	  return undeferr(gettext("Integer too large"));
	p=irem(p*(x1-x),n,q);
	c += 1;
	if (c==32) {
	  g=gcd(abs(p,context0),n); // ok
	  if (g==1) {
	    y=x;
	    y1=x1;
	    p=1;
	    a1=a;
	    m1=m;
	    c=0;
	  }
	}
      }//m=a=2^e-1=l(m)
      if (g==1) {
	x1=x;//x1=x_m=x_l(m)-1
	j=3*(a+1)/2; // j=3*iquo(a+1,2);
	for (long i=m+1;i<=j;i++)
	  x=f(x,k,n,q);
	m=j;
      }
    }
    //g<>1 ds le paquet de 32
    
    g=1;
    a=(a1-1)/2; // a=iquo(a1-1,2);
    m=m1;
    x=y;
    x1=y1;
    while (g==1) {
      a=2*a+1;
      while (g==1 && a>m) { // ok
	x=f(x,k,n,q);
	m += 1;
	if (m > long(1)<<30)
	  return undeferr(gettext("Integer too large"));
	p=irem(x1-x,n,q);
	g=gcd(abs(p,context0),n);  // ok
      }
      if (g==1) {
	x1=x;
	j=3*(a+1)/2; // j=3*iquo(a+1,2);
	for (long i=m+1;j>=i;i++){
	  x=f(x,k,n,q);//x_i
	}
	m=j;
      }
    }
    
    if (g==n) {
      if (k==1) 
	return(pollar_old(n,-1)); 
      else {
	if (k*k==1)
	  return(pollar_old(n,3));
	else
	  return(g); 
      }
    } 
    else 
      return (g);
  }

#endif
 
#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC

