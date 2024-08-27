# Private P-adic arithmetic with BGV

This repository contains an encoder class which can encrypt and decrypt p-adic numbers for HE computation using [HElib](https://github.com/homenc/HElib).

## Dependencies

 - [HElib](https://github.com/homenc/HElib), we tested our repository on HElib v2.3.0
 - [GoogleTest](https://github.com/google/googletest), this dependences will be auto installed if it is missing

## Installation
To build our repository, make sure you have the dependencies installed and then running the commands below

```
mkdir -p build
cd build
cmake ..
make
```

## Encoder Class
This class is used for encoding integers and Farey rationals into plaintexts which can be encrypted into ciphertexts. These ciphertexts can be computed homomorphically and then decrypted and decoded back into integers and Farey rationals.

### Class Members
- `p`: Prime number used in the encoding process.
- `precision`: Number of digits in the p-adic number.
- `q`: Value of `p^precision`
- `Nq`: Farey rationals bounded by `floor(sqrt((q - 1) /2))`
- `meta`: Metadata related to the BGV encryption, evaluation, and decryption process.

### Constructor
```
Encoder(const Params& params);
```

Constructor for the Encoder class, initializing the parameters required for encoding.

The params struct has the following fields:

```
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
         const std::vector<long>& _mvec = {}) 
  ...
}
```
where,
- `m` is the m-th cyclotomic polynomial
- `p^r` is the plaintext modulus
- `qbits` is the number of bits for the coefficient modulus `q`

### Encoding Methods
- `canEncodeFarey`: Checks if a given rational number is a farey rational.
- `canEncode`: Checks if a given rational number can be encoded.
- `PrintParameters`: Prints the parameters of the encoder.
- `encodeInteger`: Encodes an integer into a plaintext.
- `encodeRational`: Encodes a rational number into a plaintext. Any p-adic integer can be encoded (integers and rational numbers with non-negative padic valuations - for a rational a/b, gcd(a,b) = 1 and b mod q != 0).
- `encodeRationals`: Encodes multiple rational numbers into a plaintext.
- `pkEncrypt`: Encrypts a plaintext using the public key.

#### Decoding Methods
- `decode`: Decodes an encoded integer from a plaintext.
- `decodeRationals`: Decodes multiple encoded rational numbers from a plaintext.
- `decrypt`: Decrypts a ciphertext using the private key.

#### Utility Methods
- `padicRepresentation`: Calculates the p-adic representation of a rational number.
- `padicNorm`: Calculates the p-adic norm of the difference between two rational numbers.

#### Getters
- `getPrime`: Returns the prime number used in encoding.
- `getPrecision`: Returns the precision of the encoding.
- `getQ`: Returns the value of q
- `getNq`: Returns the value of Nq
- `getMeta`: Returns a reference to the metadata object.

### Example Usage
```
const Params testParamsSmall(/*m=*/1024,/*p=*/3,/*r=*/10,/*logq=*/300);
Encoder encoder(testParamsSmall);

// Encode 1/2 and 1/8 into a plaintext 
helib::Ptxt<helib::BGV> ptxt1 = encoder.encodeRationals({std::make_pair(1, 2), std::make_pair(1, 8)});
// Encrypt the plaintext
helib::Ctxt ctxt1 = encoder.pkEncrypt(ptxt1);

// Encode 1/4 and 1/5 into a plaintext
helib::Ptxt<helib::BGV> ptxt2 = encoder.encodeRationals({std::make_pair(1, 4), std::make_pair(1, 5)});
helib::Ctxt ctxt2 = encoder.pkEncrypt(ptxt2);

// Add the two ciphertexts
ctxt1 += ctxt2;

// Decrypt the result into a rational
helib::Ptxt<helib::BGV> result = encoder.decrypt(ctxt1);
std::vector<std::pair<int, int>> decoded = encoder.decodeRationals(result);
```



