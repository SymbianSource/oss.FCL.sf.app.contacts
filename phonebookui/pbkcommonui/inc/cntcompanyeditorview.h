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

#ifndef CNTCOMPANYEDITORVIEW_H
#define CNTCOMPANYEDITORVIEW_H

#include "cntbasedetaileditorview.h"

class CntCompanyEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntCompanyEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntCompanyEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::companyEditorView; }
    void initializeForm();
    
public slots:
    void aboutToCloseView();

};

#endif // CNTCOMPANYEDITORVIEW_H
