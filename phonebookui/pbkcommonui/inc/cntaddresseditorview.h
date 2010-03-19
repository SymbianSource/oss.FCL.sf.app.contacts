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

#ifndef CNTADDRESSEDITORVIEW_H
#define CNTADDRESSEDITORVIEW_H

#include "cntbasedetaileditorview.h"

class CntAddressEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntAddressEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntAddressEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::addressEditorView; }
    void initializeForm();
    
public slots:
    void aboutToCloseView();

};

#endif // CNTADDRESSEDITORVIEW_H
