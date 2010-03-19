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
#ifndef CNTEDITORDATAMODELITEM_H
#define CNTEDITORDATAMODELITEM_H

#include <hbdataformmodelitem.h>
#include <QStandardItemModel>
#include <qmobilityglobal.h>

class HbDataFormViewItem;
class HbInputFilter;

QTM_BEGIN_NAMESPACE
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntEditorDataModelItem : public HbDataFormModelItem
{

public:
    // ctor for fields that use HbComboBox
    CntEditorDataModelItem(QContactDetail &detail, const QString &key, QStandardItemModel *fieldModel,
            HbDataFormModelItem::DataItemType type, const QString &label = QString(), const HbDataFormModelItem *parent = 0);
    // ctor for fields that use HbLabel
    CntEditorDataModelItem(QContactDetail &detail, const QString &key, int maxLength, HbDataFormModelItem::DataItemType type,
            const QString &label = QString(), const HbDataFormModelItem *parent = 0);
    // ctor for special pushbutton items
    CntEditorDataModelItem(QContactDetail &detail, const QString &buttonLabel, HbDataFormModelItem::DataItemType type,
            const QString &label = QString(), const HbDataFormModelItem *parent = 0);
    ~CntEditorDataModelItem();

public:
    QContactDetail &contactDetail();
    QString key();
    QString buttonLabel();
    QStandardItemModel *fieldModel();
    int maxLength();
    HbInputFilter *inputFilter();
    void setTextFilter(HbInputFilter* inputFilter);
    
private:
    QContactDetail      &mContactDetail;
    QString              mKey;
    QString              mButtonLabel;
    QStandardItemModel  *mFieldModel;
    int                  mMaxLength;
    HbInputFilter       *mInputFilter;

};
#endif //CNTEDITORDATAMODELITEM_H
