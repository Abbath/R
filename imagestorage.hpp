#ifndef IMAGESTORAGE_HPP
#define IMAGESTORAGE_HPP

#include <memory>
#include <mutex>
#include <QtCore>
#include <QtGui>

class ImageStorage
{
public:
    virtual ~ImageStorage();
    static ImageStorage& getInstance();
    QImage getImage() const { return image; }
    void setImage(const QImage& _image) { image = _image; }
    void loadImage(QString filename){ image.load(filename); }
    int getImageWidth() const { return image.width(); }
    int getImageHeight() const { return image.height(); }
    bool isImageNull(){ return image.isNull(); }
private:
    QImage image;
    static std::unique_ptr<ImageStorage> instance;
    static std::once_flag onceFlag;
    ImageStorage(void);
    ImageStorage(const ImageStorage& src);
    ImageStorage& operator=(const ImageStorage& rhs);
};

#endif // IMAGESTORAGE_HPP
