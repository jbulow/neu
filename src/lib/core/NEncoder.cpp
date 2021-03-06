/*
 
      ___           ___           ___     
     /\__\         /\  \         /\__\    
    /::|  |       /::\  \       /:/  /    
   /:|:|  |      /:/\:\  \     /:/  /     
  /:/|:|  |__   /::\~\:\  \   /:/  /  ___ 
 /:/ |:| /\__\ /:/\:\ \:\__\ /:/__/  /\__\
 \/__|:|/:/  / \:\~\:\ \/__/ \:\  \ /:/  /
     |:/:/  /   \:\ \:\__\    \:\  /:/  / 
     |::/  /     \:\ \/__/     \:\/:/  /  
     /:/  /       \:\__\        \::/  /   
     \/__/         \/__/         \/__/    
 
 
Neu, Copyright (c) 2013-2014, Andrometa LLC
All rights reserved.

neu@andrometa.net
http://neu.andrometa.net

Neu can be used freely for commercial purposes. We hope you will find
Neu powerful, useful to make money or otherwise, and fun! If so,
please consider making a donation via PayPal to: neu@andrometa.net

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
 
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 
3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/

#include <neu/NEncoder.h>

using namespace std;
using namespace neu;

// The following MD5 and SHA-256 implementations were originally
// written by Brad Conte. Source: https://github.com/B-Con/crypto-algorithms

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <cstring>

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | ~z))

#define FF(a, b, c, d, m, s, t) { a += F(b, c, d) + m + t; \
a = b + ROTLEFT(a, s); }
#define GG(a, b, c, d, m, s, t) { a += G(b, c, d) + m + t; \
a = b + ROTLEFT(a, s); }
#define HH(a, b, c, d, m, s, t) { a += H(b, c, d) + m + t; \
a = b + ROTLEFT(a, s); }
#define II(a, b, c, d, m, s, t) { a += I(b, c, d) + m + t; \
a = b + ROTLEFT(a, s); }

#define SHA256_BLOCK_SIZE 32

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

namespace{
  
  typedef unsigned char BYTE;
  
  typedef unsigned int  WORD;
  
  typedef struct{
    BYTE data[64];
    WORD datalen;
    unsigned long long bitlen;
    WORD state[4];
  }MD5_CTX;
  
  typedef struct{
    BYTE data[64];
    WORD datalen;
    unsigned long long bitlen;
    WORD state[8];
  }SHA256_CTX;
  
  void md5_init(MD5_CTX* ctx);
  
  void md5_update(MD5_CTX* ctx, const BYTE data[], size_t len);
  
  void md5_final(MD5_CTX* ctx, BYTE hash[]);
  
  void sha256_init(SHA256_CTX* ctx);
  
  void sha256_update(SHA256_CTX* ctx, const BYTE data[], size_t len);
  
  void sha256_final(SHA256_CTX* ctx, BYTE hash[]);
  
  static const WORD k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };
  
  void md5_transform(MD5_CTX* ctx, const BYTE data[]){
    WORD a, b, c, d, m[16], i, j;
    
    for(i = 0, j = 0; i < 16; ++i, j += 4){
      m[i] = (data[j]) + (data[j + 1] << 8) + (data[j + 2] << 16) +
      (data[j + 3] << 24);
    }
    
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    
    FF(a, b, c, d, m[0],  7, 0xd76aa478);
    FF(d, a, b, c, m[1], 12, 0xe8c7b756);
    FF(c, d, a, b, m[2], 17, 0x242070db);
    FF(b, c, d, a, m[3], 22, 0xc1bdceee);
    FF(a, b, c, d, m[4],  7, 0xf57c0faf);
    FF(d, a, b, c, m[5], 12, 0x4787c62a);
    FF(c, d, a, b, m[6], 17, 0xa8304613);
    FF(b, c, d, a, m[7], 22, 0xfd469501);
    FF(a, b, c, d, m[8],  7, 0x698098d8);
    FF(d, a, b, c, m[9], 12, 0x8b44f7af);
    FF(c, d, a, b, m[10], 17, 0xffff5bb1);
    FF(b, c, d, a, m[11], 22, 0x895cd7be);
    FF(a, b, c, d, m[12], 7, 0x6b901122);
    FF(d, a, b, c, m[13], 12, 0xfd987193);
    FF(c, d, a, b, m[14], 17, 0xa679438e);
    FF(b, c, d, a, m[15], 22, 0x49b40821);
    
    GG(a, b, c, d, m[1], 5, 0xf61e2562);
    GG(d, a, b, c, m[6], 9, 0xc040b340);
    GG(c, d, a, b, m[11], 14, 0x265e5a51);
    GG(b, c, d, a, m[0], 20, 0xe9b6c7aa);
    GG(a, b, c, d, m[5],  5, 0xd62f105d);
    GG(d, a, b, c, m[10], 9, 0x02441453);
    GG(c, d, a, b, m[15], 14, 0xd8a1e681);
    GG(b, c, d, a, m[4], 20, 0xe7d3fbc8);
    GG(a, b, c, d, m[9],  5, 0x21e1cde6);
    GG(d, a, b, c, m[14], 9, 0xc33707d6);
    GG(c, d, a, b, m[3], 14, 0xf4d50d87);
    GG(b, c, d, a, m[8], 20, 0x455a14ed);
    GG(a, b, c, d, m[13], 5, 0xa9e3e905);
    GG(d, a, b, c, m[2],  9, 0xfcefa3f8);
    GG(c, d, a, b, m[7], 14, 0x676f02d9);
    GG(b, c, d, a, m[12], 20, 0x8d2a4c8a);
    
    HH(a, b, c, d, m[5],  4, 0xfffa3942);
    HH(d, a, b, c, m[8], 11, 0x8771f681);
    HH(c, d, a, b, m[11], 16, 0x6d9d6122);
    HH(b, c, d, a, m[14], 23, 0xfde5380c);
    HH(a, b, c, d, m[1],  4, 0xa4beea44);
    HH(d, a, b, c, m[4], 11, 0x4bdecfa9);
    HH(c, d, a, b, m[7], 16, 0xf6bb4b60);
    HH(b, c, d, a, m[10], 23, 0xbebfbc70);
    HH(a, b, c, d, m[13], 4, 0x289b7ec6);
    HH(d, a, b, c, m[0], 11, 0xeaa127fa);
    HH(c, d, a, b, m[3], 16, 0xd4ef3085);
    HH(b, c, d, a, m[6], 23, 0x04881d05);
    HH(a, b, c, d, m[9],  4, 0xd9d4d039);
    HH(d, a, b, c, m[12], 11, 0xe6db99e5);
    HH(c, d, a, b, m[15], 16, 0x1fa27cf8);
    HH(b, c, d, a, m[2], 23, 0xc4ac5665);
    
    II(a, b, c, d, m[0],  6, 0xf4292244);
    II(d, a, b, c, m[7], 10, 0x432aff97);
    II(c, d, a, b, m[14], 15, 0xab9423a7);
    II(b, c, d, a, m[5], 21, 0xfc93a039);
    II(a, b, c, d, m[12], 6, 0x655b59c3);
    II(d, a, b, c, m[3], 10, 0x8f0ccc92);
    II(c, d, a, b, m[10], 15, 0xffeff47d);
    II(b, c, d, a, m[1], 21, 0x85845dd1);
    II(a, b, c, d, m[8],  6, 0x6fa87e4f);
    II(d, a, b, c, m[15], 10, 0xfe2ce6e0);
    II(c, d, a, b, m[6], 15, 0xa3014314);
    II(b, c, d, a, m[13], 21, 0x4e0811a1);
    II(a, b, c, d, m[4], 6, 0xf7537e82);
    II(d, a, b, c, m[11], 10, 0xbd3af235);
    II(c, d, a, b, m[2], 15, 0x2ad7d2bb);
    II(b, c, d, a, m[9], 21, 0xeb86d391);
    
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
  }
  
  void md5_init(MD5_CTX* ctx){
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
  }
  
  void md5_update(MD5_CTX* ctx, const BYTE data[], size_t len){
    size_t i;
    
    for(i = 0; i < len; ++i){
      ctx->data[ctx->datalen] = data[i];
      ctx->datalen++;
      if(ctx->datalen == 64){
        md5_transform(ctx, ctx->data);
        ctx->bitlen += 512;
        ctx->datalen = 0;
      }
    }
  }
  
  void md5_final(MD5_CTX* ctx, BYTE hash[]){
    size_t i;
    
    i = ctx->datalen;
    
    if(ctx->datalen < 56){
      ctx->data[i++] = 0x80;
      while(i < 56){
        ctx->data[i++] = 0x00;
      }
    }
    else if(ctx->datalen >= 56){
      ctx->data[i++] = 0x80;
      while(i < 64){
        ctx->data[i++] = 0x00;
      }
      md5_transform(ctx, ctx->data);
      memset(ctx->data, 0, 56);
    }
    
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[56] = ctx->bitlen;
    ctx->data[57] = ctx->bitlen >> 8;
    ctx->data[58] = ctx->bitlen >> 16;
    ctx->data[59] = ctx->bitlen >> 24;
    ctx->data[60] = ctx->bitlen >> 32;
    ctx->data[61] = ctx->bitlen >> 40;
    ctx->data[62] = ctx->bitlen >> 48;
    ctx->data[63] = ctx->bitlen >> 56;
    md5_transform(ctx, ctx->data);
    
    for(i = 0; i < 4; ++i){
      hash[i]      = (ctx->state[0] >> (i * 8)) & 0x000000ff;
      hash[i + 4]  = (ctx->state[1] >> (i * 8)) & 0x000000ff;
      hash[i + 8]  = (ctx->state[2] >> (i * 8)) & 0x000000ff;
      hash[i + 12] = (ctx->state[3] >> (i * 8)) & 0x000000ff;
    }
  }
  
  void sha256_transform(SHA256_CTX* ctx, const BYTE data[]){
    WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];
    
    for(i = 0, j = 0; i < 16; ++i, j += 4){
      m[i] = (data[j] << 24) | (data[j + 1] << 16) |
      (data[j + 2] << 8) | (data[j + 3]);
    }
    
    for( ; i < 64; ++i){
      m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }
    
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];
    
    for(i = 0; i < 64; ++i){
      t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
      t2 = EP0(a) + MAJ(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }
    
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
  }
  
  void sha256_init(SHA256_CTX* ctx){
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
  }
  
  void sha256_update(SHA256_CTX* ctx, const BYTE data[], size_t len){
    WORD i;
    
    for(i = 0; i < len; ++i){
      ctx->data[ctx->datalen] = data[i];
      ctx->datalen++;
      if(ctx->datalen == 64){
        sha256_transform(ctx, ctx->data);
        ctx->bitlen += 512;
        ctx->datalen = 0;
      }
    }
  }
  
  void sha256_final(SHA256_CTX* ctx, BYTE hash[]){
    WORD i;
    
    i = ctx->datalen;
    
    if(ctx->datalen < 56){
      ctx->data[i++] = 0x80;
      while(i < 56){
        ctx->data[i++] = 0x00;
      }
    }
    else{
      ctx->data[i++] = 0x80;
      while(i < 64){
        ctx->data[i++] = 0x00;
      }
      sha256_transform(ctx, ctx->data);
      memset(ctx->data, 0, 56);
    }
    
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);
    
    for(i = 0; i < 4; ++i){
      hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
  }
  
} // end namespace

void NEncoder::sha256(const char* buf, nvar& out){
  SHA256_CTX ctx;
  
  sha256_init(&ctx);
  sha256_update(&ctx, (const unsigned char*)buf, strlen(buf));
  
  unsigned char hash[32];
  sha256_final(&ctx, hash);
  
  out.pushBack(*(int64_t*)hash);
  out.pushBack(*(int64_t*)(hash + 8));
  out.pushBack(*(int64_t*)(hash + 16));
  out.pushBack(*(int64_t*)(hash + 24));
}

nstr NEncoder::md5(const char* buf){
  MD5_CTX ctx;
  
  md5_init(&ctx);
  md5_update(&ctx, (const unsigned char*)buf, strlen(buf));
  
  unsigned char hash[16];
  md5_final(&ctx, hash);
  
  nstr ret;
  ret.reserve(32);

  char x[3];
  for(size_t i = 0; i < 16; ++i){
    sprintf(x, "%x", hash[i]);
    ret += x;
  }
  
  return ret;
}
