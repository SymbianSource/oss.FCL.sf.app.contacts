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

#ifndef CNTURLEDITORVIEW_H
#define CNTURLEDITORVIEW_H

#include "cntbasedetaileditorview.h"

#include <QStandardItemModel>

class CntUrlEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntUrlEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntUrlEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::urlEditorView; }
    void initializeForm();
    QStandardItemModel *itemModel();
    
public slots:
    void addDetail();
    void aboutToCloseView();

};

#endif // CNTURLEDITORVIEW_H
