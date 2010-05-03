/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include "cntimageutility.h"

#include <pathinfo.h>
#include <driveinfo.h>

#include <QFile>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QDateTime>

const char* PBK_FOLDER   = "Private\\20022EF9";
const char* IMAGEFILE_EXT   = ".jpg";
const char* IMAGE_TYPE   = "JPEG";
const char* FOLDER_SEPARATOR   = "\\";

const TInt64 KMinRequiredSpaceImage = 2000000;

CntImageUtility::CntImageUtility(QObject *parent)
    : QObject(parent)
{
}

CntImageUtility::~CntImageUtility()
{
}

bool CntImageUtility::createImage(const QString& filePath, QString& imagePath, int& error)
{
    Q_UNUSED(error);
    
    int drive;
    bool success(false);
    QString fileExt(IMAGEFILE_EXT);
    int err= selectDrive(drive);
    if (KErrNone==err)
        {
        initPath(imagePath, drive);
        // Filename format
        // image_<yyhh_mm_ss_zzz>.jpg
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("yyhh_mm_ss_zzz");
        QString filename = "image_" + dateTimeString + fileExt;
        imagePath += FOLDER_SEPARATOR + filename;
        
        // Copy source image to contacts images
        QFile destFile;
        success= destFile.copy(filePath,imagePath);
        }
    return success;
}

bool CntImageUtility::createImage(const QPixmap& pixmap, QString& imagePath, int& error)
{
    Q_UNUSED(error);
	
    int drive;
    bool success(false);
    QString fileExt(IMAGEFILE_EXT);
    int err= selectDrive(drive);
    if (KErrNone==err)
        {
        initPath(imagePath, drive);
        // Filename format
        // image_<yyhh_mm_ss_zzz>.jpg
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("yyhh_mm_ss_zzz");
        QString filename = "image_" + dateTimeString + fileExt;
        imagePath += FOLDER_SEPARATOR + filename;
        
        if(!pixmap.isNull())
            {
            QPixmap pix(pixmap);
            QImage image(pix.toImage());
            success= image.save(imagePath, IMAGE_TYPE);
            }
        }
    return success;
}

bool CntImageUtility::removeImage(const QString& imagePath, int& error)
{
    Q_UNUSED(error);
    
    QDir dir;
    bool success(false);
    
    QFile file( imagePath );
    if( !file.exists()
      || dir.remove(imagePath))
        success= true;
    
    return success;
}

int CntImageUtility::selectDrive(int &driveIndex)
{
    int err = KErrNone;
    TInt64 minSpaceInBytes = KMinRequiredSpaceImage;
    RFs fsSession;
    
    // Connect to file session
    err= fsSession.Connect();
    if(err!=KErrNone)
        return err;
    
    // Get the drive/volume details
    TVolumeInfo vInfo;
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, driveIndex);
    if (KErrNone==err)
        {
        err = fsSession.Volume(vInfo, driveIndex);
        }

    // Check the availability and disk space
    if ( !err && vInfo.iFree < minSpaceInBytes)
        {
        // All drives are full or inaccessible
        err = KErrDiskFull;
        }
    return err;
}

void CntImageUtility::initPath(QString &path, int drive)
{
     // Get the root path for the given drive.
     TFileName tPath;
     PathInfo::GetRootPath(tPath, drive);
     path = QString::fromUtf16(tPath.Ptr(), tPath.Length());
     
     // Image files saved in Phonebook folder
     // Create folder if not exists
     QDir dir(path);
     if (!dir.cd(PBK_FOLDER)) 
         {
         dir.mkpath(PBK_FOLDER);
         }
     path = dir.path();
}
