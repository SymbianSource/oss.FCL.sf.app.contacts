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


#include <QObject>
#include <hbdocumentloader.h>
#include "cntdocumentloader.h"
#include "cnteditviewheadingitem.h"

CntDocumentLoader::CntDocumentLoader()
{
}
    
CntDocumentLoader::~CntDocumentLoader()
{
}
    
QObject *CntDocumentLoader::createObject(const QString &type, const QString &name)
{
    QObject *result = 0;

    if (type == "CntEditViewHeadingItem")
    {
        result = new CntEditViewHeadingItem();
        if (result) 
        {
            result->setObjectName(name);
        }
    }
    else
    {
        result = HbDocumentLoader::createObject(type,name);    
    }    
    
    return result;
}
    

// EOF