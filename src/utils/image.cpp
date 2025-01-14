#include "image.h"

#include <QFile>

ImageTexture::ImageTexture(std::string path) { load(path); }

void ImageTexture::load(std::string path) { _image = QImage(path.c_str()).convertToFormat(QImage::Format_RGBA8888); }

uchar *ImageTexture::getData() { return _image.bits(); }

unsigned int ImageTexture::getHeight() const { return _image.height(); }

unsigned int ImageTexture::getWidth() const { return _image.width(); }
