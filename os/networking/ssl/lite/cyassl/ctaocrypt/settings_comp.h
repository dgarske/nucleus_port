/* settings_comp.h
 *
 * Copyright (C) 2006-2015 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


#ifndef CTAO_CRYPT_SETTINGS_C_H
#define CTAO_CRYPT_SETTINGS_C_H


/* since fips overrides rsa.h map compatibility here */
#if !defined(NO_RSA)
	#ifdef WOLFSSL_KEY_GEN
	    #define RsaKeyToDer wc_RsaKeyToDer
	#endif

	#define RsaPrivateKeyDecode   wc_RsaPrivateKeyDecode
	#define RsaPublicKeyDecode    wc_RsaPublicKeyDecode
	#define RsaPublicKeyDecodeRaw wc_RsaPublicKeyDecodeRaw
#endif /* have rsa and HAVE_FIPS */

/* Macro redefinitions for compatibility */
#ifdef HAVE_NTRU
    #define MakeNtruCert wc_MakeNtruCert
#endif
#if defined(WOLFSSL_SHA512) && !defined(CYASSL_SHA512)
    #define CYASSL_SHA512
#endif
#if defined(WOLFSSL_SHA384) && !defined(CYASSL_SHA384)
    #define CYASSL_SHA384
#endif
#if defined(WOLFSSL_LEANPSK) && !defined(CYASSL_LEANPSK)
    #define CYASSL_LEANPSK
#endif
#if defined(NO_WOLFSSL_MEMORY) && !defined(NO_CYASSL_MEMORY)
    #define NO_CYASSL_MEMORY
#endif
#ifdef WOLFSSL_KEY_GEN
    #define CYASSL_KEY_GEN
#endif

#endif /* CTAO_CRYPT_SETTINGS_C_H */
