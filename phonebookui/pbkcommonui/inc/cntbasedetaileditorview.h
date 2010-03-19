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

#ifndef CNTBASEDETAILEDITORVIEW_H
#define CNTBASEDETAILEDITORVIEW_H

#include "cntbaseview.h"
#include "cnteditordatamodelitem.h"
#include <hbdataform.h>
#include <hbdataformmodel.h>

const int CNT_PHONENUMBER_EDITOR_MAXLENGTH = 48;
const int CNT_DTMF_EDITOR_MAXLENGTH = 60;
const int CNT_URL_EDITOR_MAXLENGTH = 1000;
const int CNT_NAMES_EDITOR_MAXLENGTH = 50;
const int CNT_PREFIXSUFFIX_EDITOR_MAXLENGTH = 10;
const int CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH = 100;
const int CNT_EMAIL_EDITOR_MAXLENGTH = 150;
const int CNT_ADDRESSFIELD_EDITOR_MAXLENGTH = 250;
const int CNT_POBOX_EDITOR_MAXLENGTH = 20;
const int CNT_NOTE_EDITOR_MAXLENGTH = 1500;

class HbGroupBox;

QTM_USE_NAMESPACE

class CntBaseDetailEditorView : public CntBaseView
{
    Q_OBJECT

public:
    CntBaseDetailEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntBaseDetailEditorView();

public:
    void addMenuItems();
    HbDataForm *dataForm();
    HbDataFormModel *formModel();
    HbGroupBox *header();
    
    void setHeader(QString headerText);
    void activateView(const CntViewParameters &viewParameters);
    
    virtual void initializeForm() = 0;
    
public slots:
    virtual void aboutToCloseView();
    void cancelChanges();
    virtual void addDetail() {};

#ifdef PBK_UNIT_TEST
public:
#else
protected:    
#endif 
    
    QContact                       *mContact;
    QString                         mParamString;
    HbDataForm                     *mDataForm;
    HbDataFormModel                *mDataFormModel;
    HbGroupBox                     *mHeader;
    QList<QContactDetail*>          mDetailList;

};

#endif // CNTBASEDETAILEDITORVIEW_H
