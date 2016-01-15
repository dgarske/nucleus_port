/* dh.c
 *
 * Copyright (C) 2006-2015 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef NO_DH

#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifndef USER_MATH_LIB
    #include <math.h>
    #define XPOW(x,y) pow((x),(y))
    #define XLOG(x)   log((x))
#else
    /* user's own math lib */
#endif


#ifndef min

    static INLINE word32 min(word32 a, word32 b)
    {
        return a > b ? b : a;
    }

#endif /* min */


void wc_InitDhKey(DhKey* key)
{
    (void)key;
/* TomsFastMath doesn't use memory allocation */
#ifndef USE_FAST_MATH
    key->p.dp = 0;
    key->g.dp = 0;
#endif
}


void wc_FreeDhKey(DhKey* key)
{
    (void)key;
/* TomsFastMath doesn't use memory allocation */
#ifndef USE_FAST_MATH
    mp_clear(&key->p);
    mp_clear(&key->g);
#endif
}


static word32 DiscreteLogWorkFactor(word32 n)
{
    /* assuming discrete log takes about the same time as factoring */
    if (n<5)
        return 0;
    else
        return (word32)(2.4 * XPOW((double)n, 1.0/3.0) *
                XPOW(XLOG((double)n), 2.0/3.0) - 5);
}


static int GeneratePrivate(DhKey* key, RNG* rng, byte* priv, word32* privSz)
{
    int ret;
    word32 sz = mp_unsigned_bin_size(&key->p);
    sz = min(sz, 2 * DiscreteLogWorkFactor(sz * WOLFSSL_BIT_SIZE) /
                                           WOLFSSL_BIT_SIZE + 1);

    ret = wc_RNG_GenerateBlock(rng, priv, sz);
    if (ret != 0)
        return ret;

    priv[0] |= 0x0C;

    *privSz = sz;

    return 0;
}


static int GeneratePublic(DhKey* key, const byte* priv, word32 privSz,
                          byte* pub, word32* pubSz)
{
    int ret = 0;

    mp_int x; 
    mp_int y;

    if (mp_init_multi(&x, &y, 0, 0, 0, 0) != MP_OKAY)
        return MP_INIT_E;

    if (mp_read_unsigned_bin(&x, priv, privSz) != MP_OKAY)
        ret = MP_READ_E;

    if (ret == 0 && mp_exptmod(&key->g, &x, &key->p, &y) != MP_OKAY)
        ret = MP_EXPTMOD_E;

    if (ret == 0 && mp_to_unsigned_bin(&y, pub) != MP_OKAY)
        ret = MP_TO_E;

    if (ret == 0)
        *pubSz = mp_unsigned_bin_size(&y);

    mp_clear(&y);
    mp_clear(&x);

    return ret;
}


int wc_DhGenerateKeyPair(DhKey* key, RNG* rng, byte* priv, word32* privSz,
                      byte* pub, word32* pubSz)
{
    int ret = GeneratePrivate(key, rng, priv, privSz);

    return (ret != 0) ? ret : GeneratePublic(key, priv, *privSz, pub, pubSz);
}

int wc_DhAgree(DhKey* key, byte* agree, word32* agreeSz, const byte* priv,
            word32 privSz, const byte* otherPub, word32 pubSz)
{
    int ret = 0;

    mp_int x; 
    mp_int y;
    mp_int z;

    if (mp_init_multi(&x, &y, &z, 0, 0, 0) != MP_OKAY)
        return MP_INIT_E;

    if (mp_read_unsigned_bin(&x, priv, privSz) != MP_OKAY)
        ret = MP_READ_E;

    if (ret == 0 && mp_read_unsigned_bin(&y, otherPub, pubSz) != MP_OKAY)
        ret = MP_READ_E;

    if (ret == 0 && mp_exptmod(&y, &x, &key->p, &z) != MP_OKAY)
        ret = MP_EXPTMOD_E;

    if (ret == 0 && mp_to_unsigned_bin(&z, agree) != MP_OKAY)
        ret = MP_TO_E;

    if (ret == 0)
        *agreeSz = mp_unsigned_bin_size(&z);

    mp_clear(&z);
    mp_clear(&y);
    mp_clear(&x);

    return ret;
}


#endif /* NO_DH */
