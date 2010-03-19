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

#ifndef CNTGROUPEDITORVIEW_H
#define CNTGROUPEDITORVIEW_H

#include "cntbasedetaileditorview.h"

class CntGroupEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntGroupEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntGroupEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::groupEditorView; }
    void initializeForm();
    
public slots:
    void aboutToCloseView();
        
private:
    QList<HbDataFormModelItem*>  mItemList;

};

#endif // CNTGROUPEDITORVIEW_H
