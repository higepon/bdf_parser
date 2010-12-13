#include <stdint.h>
#include <algorithm>
#include <gtest/gtest.h>
#include "BDFParser.h"

#define FONT_PATH "./MigMix-1M-Reglar-12pt.bdf"

namespace {

TEST(Construct, OK) {
    BDFParser parser(FONT_PATH);
    EXPECT_TRUE(parser.parse());
}

TEST(Construct, NG) {
    BDFParser parser("./hige");
    EXPECT_FALSE(parser.parse());
}

TEST(getFont, A) {
    BDFParser parser(FONT_PATH);
    ASSERT_TRUE(parser.parse());
    FontImage image = parser.getFont('A');
    EXPECT_EQ(17, image.height);
    EXPECT_EQ(9, image.width);
    ASSERT_EQ(12, image.bits.size());

    EXPECT_EQ(0x18, image.bits[0]);
    EXPECT_EQ(0x1C, image.bits[1]);
    EXPECT_EQ(0x14, image.bits[2]);
    EXPECT_EQ(0x34, image.bits[3]);
    EXPECT_EQ(0x24, image.bits[4]);
    EXPECT_EQ(0x26, image.bits[5]);
    EXPECT_EQ(0x26, image.bits[6]);
    EXPECT_EQ(0x7E, image.bits[7]);
    EXPECT_EQ(0x62, image.bits[8]);
    EXPECT_EQ(0x43, image.bits[9]);
    EXPECT_EQ(0x43, image.bits[10]);
    EXPECT_EQ(0xC1, image.bits[11]);
}

TEST(getFont, AasString) {
    BDFParser parser(FONT_PATH);
    ASSERT_TRUE(parser.parse());
    std::string text = parser.getFontString(L'A');
    EXPECT_STREQ(
"     **  \n"
"     *** \n"
"     * * \n"
"    ** * \n"
"    *  * \n"
"    *  **\n"
"    *  **\n"
"   ******\n"
"   **   *\n"
"   *    *\n"
"   *    *\n"
"  **     \n"
""
 , text.c_str());
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
