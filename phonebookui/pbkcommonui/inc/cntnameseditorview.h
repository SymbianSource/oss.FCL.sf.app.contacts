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

#ifndef CNTNAMESEDITORVIEW_H
#define CNTNAMESEDITORVIEW_H

#include "cntbasedetaileditorview.h"

class CntNamesEditorView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntNamesEditorView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntNamesEditorView();

public: 
    CntViewParameters::ViewId viewId() const { return CntViewParameters::namesEditorView; }
    void initializeForm();
    
public slots:
    void aboutToCloseView();

};

#endif // CNTNAMESEDITORVIEW_H
