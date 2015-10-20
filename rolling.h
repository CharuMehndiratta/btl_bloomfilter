#ifndef ROLLING_HASH_H
#define ROLLING_HASH_H

#include <stdint.h>

// cpOff is the offset for the complement base in the random seeds table
const int cpOff = -20;

const uint64_t varSeed = 2577914034309095328ul;

static const uint64_t seedTab[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, // 0..7
    0, 0, 0, 0, 0, 0, 0, 0, // 8..15
    0, 0, 0, 0, 0, 0, 0, 0, // 16..23
    0, 0, 0, 0, 0, 0, 0, 0, // 24..31
    0, 0, 0, 0, 0, 0, 0, 0, // 32..39
    0, 0, 0, 0, 0, 2978368046464386134ul, 0, 2319985823310095140ul, // 40..47
    0, 0, 0, 3572411708064410444ul, 0, 0, 0, 0, // 48..55
    0, 0, 0, 0, 0, 0, 0, 0, // 56..63
    4362857412768957556ul, 4362857412768957556ul, 0, 3572411708064410444ul, 0, 0, 0, 2319985823310095140ul, // 64..71
    0, 0, 0, 0, 0, 0, 0, 0, // 72..79
    0, 0, 0, 0, 2978368046464386134ul, 0, 0, 0, // 80..87
    0, 0, 0, 0, 0, 0, 0, 0, // 88..95
    0, 0, 0, 0, 0, 0, 0, 0, // 96..103
    0, 0, 0, 0, 0, 0, 0, 0, // 104..111
    0, 0, 0, 0, 0, 0, 0, 0, // 112..119
    0, 0, 0, 0, 0, 0, 0, 0, // 120..127
    0, 0, 0, 0, 0, 0, 0, 0, // 128..135
    0, 0, 0, 0, 0, 0, 0, 0, // 136..143
    0, 0, 0, 0, 0, 0, 0, 0, // 144..151
    0, 0, 0, 0, 0, 0, 0, 0, // 152..159
    0, 0, 0, 0, 0, 0, 0, 0, // 160..167
    0, 0, 0, 0, 0, 0, 0, 0, // 168..175
    0, 0, 0, 0, 0, 0, 0, 0, // 176..183
    0, 0, 0, 0, 0, 0, 0, 0, // 184..191
    0, 0, 0, 0, 0, 0, 0, 0, // 192..199
    0, 0, 0, 0, 0, 0, 0, 0, // 200..207
    0, 0, 0, 0, 0, 0, 0, 0, // 208..215
    0, 0, 0, 0, 0, 0, 0, 0, // 216..223
    0, 0, 0, 0, 0, 0, 0, 0, // 224..231
    0, 0, 0, 0, 0, 0, 0, 0, // 232..239
    0, 0, 0, 0, 0, 0, 0, 0, // 240..247
    0, 0, 0, 0, 0, 0, 0, 0  // 248..255
};


// Rotate "v" to the left by "s" positions
inline uint64_t rol(const uint64_t v, const int s) {
    return (v << s) | (v >> (64 - s));
}

// Rotate "v" to the right by "s" positions
inline uint64_t ror(const uint64_t v, const int s) {
    return (v >> s) | (v << (64 - s));
}

// Get forward-strand hash v of the base kmer, i.e. fhval(kmer_0)
inline uint64_t getFhval(const char * kmerSeq, const unsigned k) {
    uint64_t hVal=0;
    for(unsigned i=0; i<k; i++)
        hVal ^= rol(seedTab[(unsigned char)kmerSeq[i]], k-1-i);
    return hVal;
}

// Get reverse-strand hash v of the base kmer, i.e. rhval(kmer_0)
inline uint64_t getRhval(const char * kmerSeq, const unsigned k) {
    uint64_t hVal=0;
    for(unsigned i=0; i<k; i++)
        hVal ^= rol(seedTab[(unsigned char)kmerSeq[i]+cpOff], i);
    return hVal;
}

inline uint64_t getChval(const char * kmerSeq, const unsigned k)
{
    uint64_t fhVal = getFhval(kmerSeq, k);
    uint64_t rhVal = getRhval(kmerSeq, k);
    return (rhVal<fhVal)? rhVal : fhVal;
}


inline uint64_t initHashes(const char * kmerSeq, const unsigned k)
{
    return getFhval(kmerSeq, k);
}


inline uint64_t initHashes(const char * kmerSeq, const unsigned k, uint64_t& fhVal, uint64_t& rhVal)
{
    fhVal = getFhval(kmerSeq, k);
    rhVal = getRhval(kmerSeq, k);
    return (rhVal<fhVal)? rhVal : fhVal;
}

inline uint64_t rollHashesRight(const uint64_t fhVal, const char charOut,
                                const char charIn, const unsigned k)
{
    return(rol(fhVal, 1) ^ rol(seedTab[(unsigned char)charOut], k) ^ seedTab[(unsigned char)charIn]);
}


inline uint64_t rollHashesRight(uint64_t& fhVal, uint64_t& rhVal,
                                        const char charOut, const char charIn, const unsigned k)
{
    fhVal = rol(fhVal, 1) ^ rol(seedTab[(unsigned char)charOut], k) ^ seedTab[(unsigned char)charIn];
    rhVal = ror(rhVal, 1) ^ ror(seedTab[charOut+cpOff], 1) ^ rol(seedTab[charIn+cpOff], k-1);
    return (rhVal<fhVal)? rhVal : fhVal;
}

#endif