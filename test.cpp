#include <stdint.h>
#include <algorithm>
#include <gtest/gtest.h>
#include "BDFParser.h"

//#define FONT_PATH "./MigMix-1M-Reglar-12pt.bdf"
#define FONT_PATH_F "./mplus_f12r.bdf"
#define FONT_PATH_J "./mplus_j12r_u.bdf"

namespace {

TEST(Construct, OK) {
    BDFParser parser(FONT_PATH_F);
    EXPECT_TRUE(parser.parse());
}

TEST(Construct, NG) {
    BDFParser parser("./hige");
    EXPECT_FALSE(parser.parse());
}

TEST(getFont, AasString) {
    BDFParser parser(FONT_PATH_F);
    ASSERT_TRUE(parser.parse());
    std::string text = parser.getFontString(L'A');
    EXPECT_STREQ(
"------\n"
"------\n"
"--*---\n"
"--*---\n"
"-*-*--\n"
"-*-*--\n"
"-*-*--\n"
"*****-\n"
"*---*-\n"
"*---*-\n"
"*---*-\n"
"------\n"
"------\n"
 , text.c_str());
}

TEST(getFont, UnicodeAsString) {
    BDFParser parser(FONT_PATH_J);
    ASSERT_TRUE(parser.parse());
    std::string text = parser.getFontString(L'あ');
    EXPECT_STREQ(
"------------\n"
"-----*------\n"
"-----*--**--\n"
"-*******----\n"
"----*-------\n"
"----*****---\n"
"---**--*-*--\n"
"--*-*--*--*-\n"
"-*--*-*---*-\n"
"-*--**----*-\n"
"--**-*---*--\n"
"-------**---\n"
"------------\n"
 , text.c_str());
}


}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
