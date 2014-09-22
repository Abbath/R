#include "imagestorage.hpp"

ImageStorage::ImageStorage()
{
    
}

std::unique_ptr<ImageStorage> ImageStorage::instance;
std::once_flag ImageStorage::onceFlag;

ImageStorage::~ImageStorage()
{
    
}

ImageStorage& ImageStorage::getInstance(){
    std::call_once(onceFlag, []{instance.reset(new ImageStorage);});
    return *instance.get();
}
