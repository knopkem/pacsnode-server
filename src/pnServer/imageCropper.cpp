#include "ImageCropper.h"

#include "CommonHelper.h"

#include <QImage>
#include <QDebug>
#include <QString>

namespace pacsnode {

ImageCropper::ImageCropper()
{

}

ImageCropper::~ImageCropper()
{

}

bool ImageCropper::createTiles( const QString& sourceFile, const QString& outputFolder, QString appendix, int tileSize )
{
    QImage sourceImage;
    if (!sourceImage.load(sourceFile, "png")) {
        qWarning() << "Could not load file" << sourceFile;
        return false;
    }
    bool good = true;
    int level = 0;
    while(good) {
        good = tilesForLevel(sourceImage, outputFolder + "/level" + QString::number(level), appendix, tileSize);
        // attention we need to scale using fast transformation otherwise we smooth the values
        sourceImage = sourceImage.scaled(sourceImage.width() / 2, sourceImage.height() / 2, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        level++;
    }
    return true;
}

bool ImageCropper::tilesForLevel( const QImage& sourceImage, const QString& outputFolder, QString appendix, int tileSize )
{
    QSize imageSize = sourceImage.size();

    int xOffset = 0;
    int yOffset = 0;
    int xRest = imageSize.width();
    int yRest = imageSize.height();
    int xTarget = 0;
    int yTarget = 0;
    int fileCounter = 0;
    QString fileName;
    if (yRest < tileSize) {
        return false;
    }
    CommonHelper::mkpath(outputFolder);
    bool result = true;
    while (yRest > 0) {
        int yExtent = tileSize;
        if (yRest < tileSize) {
            yExtent = yRest;
        }
        yTarget = yOffset + yExtent;
        xRest = imageSize.width();
        xOffset = 0;
        while(xRest > 0) {
            fileName = QString::number(fileCounter);
            QImage copy;
            QString outFile = outputFolder + "/" + fileName + appendix;
            int xExtent = tileSize;
            if (xRest < tileSize) {
                xExtent = xRest;
            }
            xTarget = xOffset + xExtent;
            //qDebug() << "creating" << xOffset << yOffset;
            copy = sourceImage.copy( xOffset, yOffset, xExtent, yExtent); 
            if( !copy.save(outFile) ) {
                qWarning() << "failed to store image" << outFile; 
            }
            xRest -= (xTarget - xOffset);
            xOffset = xTarget;
            fileCounter++;
            result = true;
        }
        yRest -= (yTarget - yOffset);
        yOffset = yTarget;
    }

    return result;
}

}
