#include "encoder.hpp"
#include <cmath>
#include <sstream>
#include <limits>
#include <NTL/LLL.h>

int gcd(int32_t a, int32_t b)
{
    if (b == 0)
        return abs(a);
    return gcd(b, a % b);
}

int modInverse(int32_t a, int32_t m)
{
    a = a % m;
    for (int32_t x = 1; x < m; x++)
        if ((a * x) % m == 1)
            return x;
    return 1;
}

Encoder::Encoder(const Params &params) : p(params.p), precision(params.r), q(pow(p, precision)), Nq(std::floor(std::sqrt((q - 1) / 2.0)))
{
    meta(params);
}

bool Encoder::canEncodeFarey(std::pair<int32_t, int32_t> rational)
{
    int32_t numerator = rational.first;
    int32_t denominator = rational.second;
    return gcd(numerator, denominator) == 1 && gcd(denominator, q) == 1 && abs(numerator) < Nq && abs(denominator) < Nq;
}
bool Encoder::canEncode(std::pair<int32_t, int32_t> rational)
{
    int32_t numerator = rational.first;
    int32_t denominator = rational.second;
    return gcd(numerator, denominator) == 1 && denominator % q != 0;
}

void Encoder::PrintParameters()
{
    std::stringstream ss;
    ss << "Prime: " << p << std::endl;
    ss << "Precision: " << precision << std::endl;
    ss << "Q: " << q << std::endl;
    ss << "Nq: " << Nq << std::endl;

    ss << "Context: " << std::endl;
    ss << meta.data->context << std::endl;

    std::cout << ss.str();
}

int32_t Encoder::encodeInteger(int32_t integer)
{
    return encodeRational(std::make_pair(integer, 1));
}

int32_t Encoder::encodeRational(std::pair<int32_t, int32_t> rational)
{
    if (!canEncode(rational))
    {
        throw std::invalid_argument("Cannot encode rational number " + std::to_string(rational.first) + "/" + std::to_string(rational.second) + " with precision " + std::to_string(precision) + " and prime " + std::to_string(p) + " using Farey rationals of order " + std::to_string(Nq) + ".");
    }
    // a / b -> h = a (b^{-1} mod q) mod q
    int32_t inverse = modInverse(rational.second, q);
    int32_t h = (rational.first * inverse) % q;

    return h;
}

std::pair<int32_t, int32_t> SVP(NTL::mat_ZZ &B)
{
    // LLL reduction
    long _ = NTL::LLL_XD(B);

    // Return the shortest vector
    int32_t a = NTL::conv<int>(B[0][0]);
    int32_t b = NTL::conv<int>(B[0][1]);

    if (a < 0 && b < 0)
    {
        a = -a;
        b = -b;
    }

    return std::make_pair(a, b);
}

std::pair<int32_t, int32_t> Encoder::decode(int32_t h)
{
    NTL::mat_ZZ B;
    B.SetDims(2, 2);
    B[0][0] = q;
    B[0][1] = 0;
    B[1][0] = h;
    B[1][1] = 1;

    return SVP(B);
}

helib::Ptxt<helib::BGV> Encoder::encodeRationals(std::vector<std::pair<int32_t, int32_t>> rationals)
{
    if (rationals.size() > meta.data->context.getEA().size())
    {
        throw std::invalid_argument("Too many rationals to encode. The number of rationals must be less than or equal to " + std::to_string(meta.data->context.getEA().size()) + ".");
    }
    helib::Ptxt<helib::BGV> ptxt(meta.data->context);
    for (size_t i = 0; i < rationals.size(); i++)
    {
        ptxt[i] = encodeRational(rationals[i]);
    }
    return ptxt;
}

helib::Ctxt Encoder::pkEncrypt(helib::Ptxt<helib::BGV> ptxt)
{
    helib::Ctxt ctxt(meta.data->publicKey);
    meta.data->publicKey.Encrypt(ctxt, ptxt);
    return ctxt;
}

std::vector<std::pair<int32_t, int32_t>> Encoder::decodeRationals(helib::Ptxt<helib::BGV> ptxt)
{
    std::vector<helib::PolyMod> poly_mod_result = ptxt.getSlotRepr();

    std::vector<std::pair<int32_t, int32_t>> rationals;
    for (size_t i = 0; i < ptxt.size(); i++)
    {
        rationals.push_back(decode((int32_t)(long)poly_mod_result[i]));
    }
    return rationals;
}

helib::Ptxt<helib::BGV> Encoder::decrypt(helib::Ctxt ctxt)
{
    helib::Ptxt<helib::BGV> ptxt(meta.data->context);
    meta.data->secretKey.Decrypt(ptxt, ctxt);
    return ptxt;
}

std::string Encoder::padicRepresentation(std::pair<int32_t, int32_t> rational)
{
    int32_t h = encodeRational(rational);

    if (h < 0) // h can be negative even after modulus
    {
        h += q;
    }

    std::string rep = ".";
    for (size_t i = 0; i < precision; i++)
    {
        rep += std::to_string(h % p);
        h = h / p;
    }
    return rep;
}

double Encoder::padicNorm(std::pair<int32_t, int32_t> rational1, std::pair<int32_t, int32_t> rational2)
{
    std::string rep1 = padicRepresentation(rational1);
    std::string rep2 = padicRepresentation(rational2);

    for (size_t i = 1; i < std::min(rep1.size(), rep2.size()); i++)
    {
        if (rep1[i] != rep2[i])
        {
            return 1.0 / std::pow((double)p, (double)(i));
        }
    }
    return 0.0;
}