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

#ifndef CNTONLINEACCOUNTEDITORVIEW_H
#define CNTONLINEACCOUNTEDITORVIEW_H

#include "cntbasedetaileditorview.h"

#include <QStandardItemModel>

class CntOnlineAccountEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntOnlineAccountEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntOnlineAccountEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::onlineAccountEditorView; }
    void initializeForm();
    QStandardItemModel *itemModel();
    
public slots:
    void addDetail();
    void aboutToCloseView();

};

#endif // CNTONLINEACCOUNTEDITORVIEW_H
