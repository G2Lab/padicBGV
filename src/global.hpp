#pragma once

#include <helib/helib.h>
#include <helib/Ctxt.h>
#include <helib/polyEval.h>

struct Params
{
  const long m, p, r, qbits;
  const std::vector<long> gens;
  const std::vector<long> ords;
  const std::vector<long> mvec;
  Params(long _m,
         long _p,
         long _r,
         long _qbits,
         const std::vector<long>& _gens = {},
         const std::vector<long>& _ords = {},
         const std::vector<long>& _mvec = {}) :
      m(_m), p(_p), r(_r), qbits(_qbits), gens(_gens), ords(_ords), mvec(_mvec)
  {}
  Params(const Params& other) :
      Params(other.m,
             other.p,
             other.r,
             other.qbits,
             other.gens,
             other.ords,
             other.mvec)
  {}
  bool operator!=(Params& other) const { return !(*this == other); }
  bool operator==(Params& other) const
  {
    return m == other.m && p == other.p && r == other.r &&
           qbits == other.qbits && gens == other.gens && ords == other.ords &&
           mvec == other.mvec;
  }
};

struct ContextAndKeys
{
  const Params params;

  helib::Context context;
  helib::SecKey secretKey;
  const helib::PubKey publicKey;
  const helib::EncryptedArray& ea;

  ContextAndKeys(const Params& _params) :
      params(_params),
      context(helib::ContextBuilder<helib::BGV>()
                  .m(params.m)
                  .p(params.p)
                  .r(params.r)
                  .bits(params.qbits)
                  .gens(params.gens)
                  .ords(params.ords)
                  .bootstrappable(!params.mvec.empty())
                  .mvec(params.mvec)
                  .build()),
      secretKey(context),
      publicKey((secretKey.GenSecKey(),
                 helib::addSome1DMatrices(secretKey),
                 secretKey)),
      ea(context.getEA())
  {
  }
};

struct Meta
{
  std::unique_ptr<ContextAndKeys> data;
  Meta& operator()(const Params& params)
  {
    data = std::make_unique<ContextAndKeys>(params);
    return *this;
  }
};

namespace constants
{
    // DEBUG PARAMETERS

    const int DEBUG = 0;
    
    // BGV PARAMETERS
    const Params N15QP881(32768,65537, 1, 881);
    const Params N14QP438(16384,65537,  1, 438);
    const Params N13QP218(8192, 65537, 1, 218);
    const Params N12QP109(/*m=*/4096, /*p=*/65537, /*r=*/1, /*qbits=*/109);
    const Params P131(17293, 131, 1, 431);
    const Params BenchParams(65536, 131071, 1, 880);
    const Params BenchParams2(50001, 100003, 1, 880);
}