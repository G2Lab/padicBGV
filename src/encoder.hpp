#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <vector>
#include <string>
#include <helib/helib.h>
#include "global.hpp"
#include <limits>

class Encoder
{
private:
    int32_t p;         // prime number
    int32_t precision; // number of digits in p-adic number
    int32_t q;         // p^precision
    int32_t Nq;        // Farey rationals of order N_q
    Meta meta;

public:
    Encoder(const Params &params);

    bool canEncodeFarey(std::pair<int32_t, int32_t> rational);
    bool canEncode(std::pair<int32_t, int32_t> rational);

    void PrintParameters();

    int32_t encodeInteger(int32_t integer);
    int32_t encodeRational(std::pair<int32_t, int32_t> rational);
    helib::Ptxt<helib::BGV> encodeRationals(std::vector<std::pair<int32_t, int32_t>> rationals);
    helib::Ctxt pkEncrypt(helib::Ptxt<helib::BGV> ptxt);

    std::pair<int32_t, int32_t> decode(int32_t h);
    std::vector<std::pair<int32_t, int32_t>> decodeRationals(helib::Ptxt<helib::BGV> ptxt);
    helib::Ptxt<helib::BGV> decrypt(helib::Ctxt ctxt);

    std::string padicRepresentation(std::pair<int32_t, int32_t> rational);
    double padicNorm(std::pair<int32_t, int32_t> rational1, std::pair<int32_t, int32_t> rational2);

    // Getters
    int32_t getPrime() const { return p; }
    int32_t getPrecision() const { return precision; }
    int32_t getQ() const { return q; }
    int32_t getNq() const { return Nq; }
    Meta &getMeta() { return meta; }
};

#endif // ENCODER_HPP