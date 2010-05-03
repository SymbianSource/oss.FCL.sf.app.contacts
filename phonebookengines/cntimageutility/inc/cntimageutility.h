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
#ifndef CNTIMAGEUTILITY_H
#define CNTIMAGEUTILITY_H

#include <qglobal.h>
#include <QObject>
#include "cntimageutilityglobal.h"

class QPixmap;

class CNTIMAGEUTILITYLIB_EXPORT CntImageUtility : public QObject
{
    Q_OBJECT
    
public:
	CntImageUtility(QObject *parent = 0);
	~CntImageUtility();
	
	bool createImage(const QString& filePath, QString& imagePath, int& error);
	bool createImage(const QPixmap& pixmap, QString& imagePath, int& error);
	bool removeImage(const QString& imagePath, int& error);
	
private:
	int selectDrive(int &driveIndex);
	void initPath(QString &path, int drive);
};

#endif
