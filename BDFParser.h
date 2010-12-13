/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef BDF_PARSER_
#define BDF_PARSER_

#include <string>
#include <map>
#include <vector>

typedef uint32_t ucs4char;

class FontImage {
public:
    FontImage() : ch(0), width(0), height(0) {}
    ucs4char ch;
    int width;
    int height;
    std::vector<uint32_t> bits;
};

class BDFParser {
    typedef std::map<ucs4char, FontImage> FontImages;

public:
    BDFParser(const std::string& path) : path_(path),
                                         fontHight_(-1),
                                         numChars_(-1)
        {}

    bool parse()
    {
        FILE* fp = fopen(path_.c_str(), "r");
        if (fp == NULL) {
            return false;
        }
        if (!readHeader(fp)) {
            return false;
        }
        assert(numChars_ > 0);
        if (!readAllFonts(fp)) {
            return false;
        }
        fclose(fp);
        return true;
    }

    std::string getFontString(ucs4char ch)
    {
        FontImage font = getFont(ch);
        std::string ret;
        for (size_t j = 0; j < font.bits.size(); j++) {
            for (int i = 0; i < font.width; i++) {
                if ((font.bits[j] >> (font.width - i)) & 0x01) {
                    ret += '*';
                } else {
                    ret += ' ';
                }
            }
            ret += '\n';
        }
        return ret;
    }

    FontImage getFont(ucs4char ch)
    {
        return fontImages_[ch];
    }

private:

    bool readBitmap(FILE* fp, FontImage& fontImage)
    {
        const int BUFFER_SIZE = 512;
        char buf[BUFFER_SIZE];

        for (;;) {
            if (NULL == fgets(buf, BUFFER_SIZE, fp)) {
                return false;
            }
            std::string line(buf);
            if (line.find("ENDCHAR") == 0) {
                break;
            }
            uint32_t bits;
            if (sscanf(line.c_str(), "%x", &(bits)) != 1) {
                return false;
            }
            fontImage.bits.push_back(bits);
        }
        return true;
    }

    bool readOneFont(FILE* fp, FontImage& fontImage)
    {
        const int BUFFER_SIZE = 512;
        char buf[BUFFER_SIZE];
        if (NULL == fgets(buf, BUFFER_SIZE, fp)) {
            // EOF
            return true;
        }
        std::string line(buf);
        if (line.find("STARTCHAR ") != 0) {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return false;
        }

        if (sscanf(line.c_str(), "STARTCHAR %x ", &(fontImage.ch)) != 1) {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return false;
        }

        for (;;) {
            if (NULL == fgets(buf, BUFFER_SIZE, fp)) {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                return false;
            }
            std::string line(buf);
            if (line.find("DWIDTH ") == 0) {
                if (sscanf(line.c_str(), "DWIDTH %d ", &(fontImage.width)) != 1) {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                    return false;
                }
            }
            if (line.find("BITMAP") == 0) {
                if (!readBitmap(fp, fontImage)) {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                    return false;
                } else {
                    break;
                }
            }
        }
        fontImage.height = fontHight_;
        return true;
    }
    bool readAllFonts(FILE* fp)
    {
        for (int i = 0; i < numChars_; i++) {
            FontImage fontImage;
            if (!readOneFont(fp, fontImage)) {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                return false;
            }
            fontImages_[fontImage.ch] = fontImage;
        }
        return true;
    }
    bool readHeader(FILE* fp)
    {
        const int BUFFER_SIZE = 512;
        char buf[BUFFER_SIZE];
        for (;;) {
            if (NULL == fgets(buf, BUFFER_SIZE, fp)) {
                return false;
            }
            std::string line(buf);
            if (line.find("PIXEL_SIZE ") == 0) {
                if (sscanf(line.c_str(), "PIXEL_SIZE %d ", &fontHight_) != 1) {
                    return false;
                }
            }
            if (line.find("CHARS ") == 0) {
                if (sscanf(line.c_str(), "CHARS %d ", &numChars_) != 1) {
                    return false;
                }
            }
            if (fontHight_ != -1 && numChars_ != -1) {
                return true;
            }
        }
        return false;
    }
    std::string path_;
    int fontHight_;
    int numChars_;
    FontImages fontImages_;
};

#endif // BDF_PARSER_
