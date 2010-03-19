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

#ifndef CNTEDITORDATAVIEWITEM_H
#define CNTEDITORDATAVIEWITEM_H

#include <hbdataformviewitem.h>
#include <qtcontacts.h>

class CntBaseDetailEditorView;
class CntEditorDataModelItem;
class QStandardItemModel;
class HbLineEdit;

QTM_USE_NAMESPACE

class CntEditorDataViewItem : public HbDataFormViewItem
{
    Q_OBJECT

public:
    CntEditorDataViewItem(CntBaseDetailEditorView *view, QGraphicsItem *parent = 0);
    ~CntEditorDataViewItem();

    virtual HbAbstractViewItem* createItem();
    void setTextFilter(QObject *aEditor, CntEditorDataModelItem *item);
    
public slots:
    void textChanged(QString text);
    void indexChanged(int index);
    void addDetail();
    void editDate();
    void load() {}
    void store() {}
    void focusLineEdit();

protected:
    virtual HbWidget* createCustomWidget();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    int modelPosition(QStandardItemModel *model, QContactDetail &detail);
    
    CntBaseDetailEditorView *mView;
    HbLineEdit              *mLineEdit;

};


#endif // CNTEDITORDATAVIEWITEM_H
