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

#include "cnteditordatamodelitem.h"
#include <hbdataformviewitem.h>
#include <qtcontacts.h>

CntEditorDataModelItem::CntEditorDataModelItem(QContactDetail &detail, const QString &key, QStandardItemModel *fieldModel,
        HbDataFormModelItem::DataItemType type, const QString &label, const HbDataFormModelItem *parent) : 
            HbDataFormModelItem(type, label, parent),
            mContactDetail(detail),
            mKey(key),
            mFieldModel(fieldModel),
            mInputFilter(0)
{

}

CntEditorDataModelItem::CntEditorDataModelItem(QContactDetail &detail, const QString &key, int maxLength, 
        HbDataFormModelItem::DataItemType type, const QString &label, const HbDataFormModelItem *parent) :
            HbDataFormModelItem(type, label, parent),
            mContactDetail(detail),
            mKey(key),
            mMaxLength(maxLength),
            mInputFilter(0)
{

}

CntEditorDataModelItem::CntEditorDataModelItem(QContactDetail &detail, const QString &buttonLabel,
        HbDataFormModelItem::DataItemType type, const QString &label, const HbDataFormModelItem *parent) :
            HbDataFormModelItem(type, label, parent),
            mContactDetail(detail),
            mButtonLabel(buttonLabel)
{

}

CntEditorDataModelItem::~CntEditorDataModelItem()
{

}

QContactDetail &CntEditorDataModelItem::contactDetail()
{
    return mContactDetail;
}

QString CntEditorDataModelItem::key()
{
    return mKey;
}

QString CntEditorDataModelItem::buttonLabel()
{
    return mButtonLabel;
}

QStandardItemModel *CntEditorDataModelItem::fieldModel()
{
    return mFieldModel;
}

int CntEditorDataModelItem::maxLength()
{
    return mMaxLength;
}

HbInputFilter *CntEditorDataModelItem::inputFilter()
{
    return mInputFilter;
}

void CntEditorDataModelItem::setTextFilter(HbInputFilter *inputFilter)
{
    mInputFilter = inputFilter;
}
