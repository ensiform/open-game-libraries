/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#ifndef __OG_RANDOM_INL__
#define __OG_RANDOM_INL__

namespace og {

/*
==============================================================================

  Random

==============================================================================
*/

/*
================
Random::Random
================
*/
OG_INLINE Random::Random( uLong seed ) {
	SetSeed( seed );
}

/*
================
Random::GetSeed
================
*/
OG_INLINE uLong Random::GetSeed( void ) const {
	return storedSeed;
}

/*
================
Random::RandomInt
================
*/
OG_INLINE int Random::RandomInt( void ) {
	return RandomULong();
}
OG_INLINE int Random::RandomInt( int min, int max ) {
	return min+Math::FtoiFast(static_cast<float>(max-min)*RandomFloat());
}

/*
================
Random::RandomBool
================
*/
OG_INLINE bool Random::RandomBool( void ) {
	return Math::FtoiFast(RandomFloat() + 0.5f) != 0;
}

/*
================
Random::RandomFloat
================
*/
OG_INLINE float Random::RandomFloat( float min, float max ) {
	return min+(max-min)*RandomFloat();
}

/*
================
Random::SymmetricRandom
================
*/
OG_INLINE float Random::SymmetricRandom( void ) {
	return (2.0f * RandomFloat() - 1.0f);
}

/*
================
Random::SeedTime
================
*/
OG_INLINE void Random::SeedTime( void ) {
	SetSeed( static_cast<uInt>(SysInfo::GetTime()) );
}

}

#endif
