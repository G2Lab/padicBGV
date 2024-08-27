#include "encoder.hpp"
#include "gtest/gtest.h"

const Params testParamsSmall(/*m=*/1024,/*p=*/3,/*r=*/10,/*logq=*/300);
const Params testParamsLarge(/*m=*/4096,/*p=*/3,/*r=*/10,/*logq=*/800);

TEST(EncoderTest, EncodeFraction){
    Encoder encode(testParamsSmall);
    uint32_t expected = 17723;
    EXPECT_EQ(encode.encodeRational(std::make_pair(83, 10)), expected);
}

TEST(EncoderTest, CanEncodeFarey) {
    Encoder encoder(testParamsSmall);
    EXPECT_TRUE(encoder.canEncodeFarey(std::make_pair(83, 10)));
    EXPECT_FALSE(encoder.canEncodeFarey(std::make_pair(1237, 100)));
}

TEST(EncoderTest, Decode) {
    Encoder encoder(testParamsSmall);
    std::pair<int, int> expected = std::make_pair(83, 10);
    EXPECT_EQ(encoder.decode(17723), expected);
}

TEST(EncoderTest, EncodeDecode){
    Encoder encoder(testParamsSmall);
    int nQ = encoder.getNq();
    int randomA = rand() % nQ;
    int randomB = rand() % nQ;
    while (!encoder.canEncode(std::make_pair(randomA, randomB))){
        randomA = rand() % nQ;
        randomB = rand() % nQ;
    }
    uint32_t encoded = encoder.encodeRational(std::make_pair(randomA, randomB));
    std::pair<int, int> decoded = encoder.decode(encoded);
    EXPECT_EQ(decoded, std::make_pair(randomA, randomB));
}

TEST(EncoderTest, padicRepresentation){
    Encoder encoder(testParamsSmall);
    std::string expected = ".1000010220";
    EXPECT_EQ(encoder.padicRepresentation(std::make_pair(253, 10)), expected);
    EXPECT_EQ(encoder.padicRepresentation(std::make_pair(-10, 233)), expected);
}

TEST(EncoderTest, padicNorm){
    Encoder encoder(testParamsSmall);
    double expected = std::pow(3, -3);
    EXPECT_EQ(encoder.padicNorm(std::make_pair(9, 1), std::make_pair(27, 1)), expected);
}

TEST(HETest, AddRationals){
    Encoder encoder(testParamsSmall);
    helib::Ptxt<helib::BGV> ptxt1 = encoder.encodeRationals({std::make_pair(1, 2), std::make_pair(1, 8)});
    helib::Ctxt ctxt1 = encoder.pkEncrypt(ptxt1);

    helib::Ptxt<helib::BGV> ptxt2 = encoder.encodeRationals({std::make_pair(1, 4), std::make_pair(1, 5)});
    helib::Ctxt ctxt2 = encoder.pkEncrypt(ptxt2);

    ctxt1 += ctxt2;

    helib::Ptxt<helib::BGV> result = encoder.decrypt(ctxt1);
    std::vector<std::pair<int, int>> decoded = encoder.decodeRationals(result);
    std::vector<std::pair<int, int>> expected = {std::make_pair(3, 4), std::make_pair(13, 40)}; 

    EXPECT_EQ(decoded, expected);
}

TEST(HETest, MultiplyRationals){
    Encoder encoder(testParamsLarge);
    helib::Ptxt<helib::BGV> ptxt1 = encoder.encodeRationals({std::make_pair(1, 2), std::make_pair(1, 8)});
    helib::Ctxt ctxt1 = encoder.pkEncrypt(ptxt1);

    helib::Ptxt<helib::BGV> ptxt2 = encoder.encodeRationals({std::make_pair(1, 4), std::make_pair(1, 5)});
    helib::Ctxt ctxt2 = encoder.pkEncrypt(ptxt2);

    ctxt1 *= ctxt2;

    helib::Ptxt<helib::BGV> result = encoder.decrypt(ctxt1);
    std::vector<std::pair<int, int>> decoded = encoder.decodeRationals(result);
    std::vector<std::pair<int, int>> expected = {std::make_pair(1, 8), std::make_pair(1, 40)}; 

    EXPECT_EQ(decoded, expected);
}

TEST(HETest, PseudoOverflow){
    Encoder encoder(testParamsLarge);
    helib::Ptxt<helib::BGV> ptxt1 = encoder.encodeRationals({std::make_pair(83, 10)});
    helib::Ctxt ctxt1 = encoder.pkEncrypt(ptxt1);

    helib::Ptxt<helib::BGV> ptxt2 = encoder.encodeRationals({std::make_pair(17, 1)});
    helib::Ctxt ctxt2 = encoder.pkEncrypt(ptxt2);

    helib::Ptxt<helib::BGV> ptxt3 = encoder.encodeRationals({std::make_pair(16, 1)});
    helib::Ctxt ctxt3 = encoder.pkEncrypt(ptxt3);

    ctxt1 += ctxt2;

    helib::Ptxt<helib::BGV> result = encoder.decrypt(ctxt1);
    std::pair<int, int> decoded = encoder.decodeRationals(result)[0];
    std::pair<int, int> expected = std::make_pair(10, -233);
    EXPECT_EQ(decoded, expected);

    ctxt1 -= ctxt3;

    result = encoder.decrypt(ctxt1);
    decoded = encoder.decodeRationals(result)[0];
    expected = std::make_pair(93, 10);

    EXPECT_EQ(decoded, expected);
}