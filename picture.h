#ifndef PICTURE_H
#define PICTURE_H

#include "headers.h"
#include "texture.h"

class PictureLoader
{
public:
    PictureLoader();
    ~PictureLoader();

    bool load(const std::string& filename);
    Texture* getPicture(uint16 picture);
    uint32 getSignature();
		
private:
    Texture** mPictures;
    uint32 mSignature;
};

extern PictureLoader *gPictureLoader;

#endif // PICTURE_H
