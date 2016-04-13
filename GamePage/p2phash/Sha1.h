#pragma once

namespace hash_function
{
	typedef struct {
		unsigned long state[5];
		unsigned long count[2];
		unsigned char buffer[64];
	} SHA1_CTX;


	class CSha1
	{
	public:
		static void		HashBuffer(const void *buf, int buf_len, BYTE *RetDigest);

	private:
		static void		SHA1Final(unsigned char digest[20], SHA1_CTX* context);
		static void		SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int len);
		static void		SHA1Init(SHA1_CTX* context);

	private:
		static void		SHA1Transform(unsigned long state[5], unsigned char buffer[64]);

	};

};

