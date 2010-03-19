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

#ifndef CNTFAMILYDETAILEDITORVIEW_H
#define CNTFAMILYDETAILEDITORVIEW_H

#include "cntbasedetaileditorview.h"

class CntFamilyDetailEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntFamilyDetailEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntFamilyDetailEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::familyDetailEditorView; }
    void initializeForm();
    
public slots:
    void aboutToCloseView();

};

#endif // CNTFAMILYDETAILEDITORVIEW_H
