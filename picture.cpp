#include "picture.h"

PictureLoader *gPictureLoader = NULL;

PictureLoader::PictureLoader()
{
    if(gPictureLoader)
        printf("This class should not be redeclared. Although, memory leak.\n");

    gPictureLoader = this;
}

PictureLoader::~PictureLoader()
{
    gPictureLoader = NULL;
}

bool PictureLoader::load(const std::string& filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if(!fp) {
        printf("Could not open file %s.\n", filename.c_str());
        return false;
    }

    fread(&mSignature, 4, 1, fp);

    uint16 imagesNum;
    fread(&imagesNum, 2, 1, fp);

    mPictures = new Texture*[imagesNum];

    for(uint16 image = 0; image < imagesNum; ++image) {

        uint8 spritesX;
        fread(&spritesX, 1, 1, fp);

        uint8 spritesY;
        fread(&spritesY, 1, 1, fp);

        uint8 *newImage = new uint8[spritesX * 32 * spritesY * 32 * 4];
        memset(newImage, 0, spritesX * 32 * spritesY * 32 * 4);

        uint8 colorKey[3];
        fread(&colorKey, 1, 3, fp);

        uint8 a, b;
        for(a = 0; a < spritesY; ++a) {
            for(b = 0; b < spritesX; ++b) {
                uint32 spriteAddress;
                fread(&spriteAddress, 4, 1, fp);

                uint32 currentAddress = ftell(fp); // Get current file pos to go back
                fseek(fp, spriteAddress, SEEK_SET);  // Goto the sprite data

                uint16 spriteSize;
                fread(&spriteSize, 2, 1, fp);

                uint32 read = 0, alpha = 0, pixelNum = 0, pixelCount = 0;
                while(read < spriteSize) {
                    fread(&alpha, 2, 1, fp);
                    pixelCount += alpha;
                    fread(&pixelNum, 2, 1, fp);
                    read += 4;

                    alpha = pixelCount + pixelNum;
                    while(pixelCount < alpha) {
                        fread(&newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 0], 1, 1, fp);
                        fread(&newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 1], 1, 1, fp);
                        fread(&newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 2], 1, 1, fp);

                        if(newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 0] == 255 &&
                           newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 1] == 0 &&
                           newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 2] == 255) // Set Pink as Alpha
                            newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 3] = 0x00;
                        else
                            newImage[(a * spritesX * 4096) + (b * 128) + ((pixelCount / 32) * spritesX * 128) + ((pixelCount % 32) * 4) + 3] = 0xFF;

                        read += 3;
                        ++pixelCount;
                    }
                }
                fseek(fp, currentAddress, SEEK_SET);
            }
        }

        mPictures[image] = new Texture(spritesX * 32, spritesY * 32, 4, newImage);
    }

    fclose(fp);
    return true;
}

Texture* PictureLoader::getPicture(uint16 picture)
{
    return mPictures[picture];
}

uint32 PictureLoader::getSignature()
{
    return mSignature;
}
