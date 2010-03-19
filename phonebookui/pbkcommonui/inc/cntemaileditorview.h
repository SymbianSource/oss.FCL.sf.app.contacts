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

#ifndef CNTEMAILEDITORVIEW_H
#define CNTEMAILEDITORVIEW_H

#include "cntbasedetaileditorview.h"

#include <QStandardItemModel>

class CntEmailEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntEmailEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntEmailEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::emailEditorView; }
    void initializeForm();
    QStandardItemModel *itemModel();
    
public slots:
    void addDetail();
    void aboutToCloseView();

};

#endif // CNTEMAILEDITORVIEW_H
