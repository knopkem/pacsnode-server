#ifndef ImageCropper_h__
#define ImageCropper_h__

class QImage;
class QString;

namespace pacsnode {

//! helper for tile based viewer, experimental
class ImageCropper
{
public:
    ImageCropper();
    ~ImageCropper();

    bool createTiles(const QString& sourceFile, const QString& outputFolder, QString appendix, int tileSize = 256);

protected:
    bool tilesForLevel(const QImage& sourceImage, const QString& outputFolder, QString appendix, int tileSize);
};

}
#endif // ImageCropper_h__

