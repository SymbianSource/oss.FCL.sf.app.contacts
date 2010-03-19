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

#ifndef CNTGROUPEDITORDATAVIEWITEM_H
#define CNTGROUPEDITORDATAVIEWITEM_H

#include <hbdataformviewitem.h>
#include <qtcontacts.h>
#include "cntgroupeditorview.h"


class CntEditorDataModelItem;
class QStandardItemModel;
class HbLabel;
class HbLineEdit;
class CntGroupEditorView;


class CntGroupEditorDataViewItem : public HbDataFormViewItem
{
    Q_OBJECT

public:
    CntGroupEditorDataViewItem(CntGroupEditorView *view,QGraphicsItem *parent = 0);
    ~CntGroupEditorDataViewItem();

    virtual HbAbstractViewItem* createItem();
    void setTextFilter(QObject *aEditor, CntEditorDataModelItem *item);
    
public slots:
    void textChanged(QString text);
    void buttonPushed();

protected:
    virtual HbWidget* createCustomWidget();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    int modelPosition(QStandardItemModel *model, QContactDetail &detail);
    CntGroupEditorView      *mView;
    HbLabel                 *mIconLabel;
    HbLineEdit              *mLineEdit;
};


#endif // CNTEDITORDATAVIEWITEM_H
