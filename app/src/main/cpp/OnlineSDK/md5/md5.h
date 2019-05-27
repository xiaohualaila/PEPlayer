#ifndef MD5_H_
#define MD5_H_

typedef struct tag_MD5_CTX
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

static void MD5Init(MD5_CTX *context);
static void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
static void MD5Final(MD5_CTX *context,unsigned char digest[16]);
static void MD5Transform(unsigned int state[4],unsigned char block[64]);
static void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
static void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

#endif
