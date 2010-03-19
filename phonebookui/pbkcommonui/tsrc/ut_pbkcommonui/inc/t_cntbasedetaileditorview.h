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

#include <QObject>

class CntViewManager;
class CntMainWindow;

#include "cntbasedetaileditorview.h"
#include "cntviewparameters.h"

class CntBaseDetailEditorTestView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntBaseDetailEditorTestView(CntViewManager *viewManager, QGraphicsItem *parent = 0):
        CntBaseDetailEditorView(viewManager, parent), mResult(false) { }

    CntViewParameters::ViewId viewId() const { return CntViewParameters::noView; }
    void initializeForm() { }
    void addDetail() { mResult = true; }
    
    bool mResult;
};

class TestCntBaseDetailEditorView : public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void createClasses();
	
    void addMenuItems();
	
    void formModel();
    void dataForm();
    void setHeader();
    void header();
	
    void activateView();

    void cleanupTestCase();

private:
    CntMainWindow               *mWindow;
    CntViewManager              *mViewManager;
    CntBaseDetailEditorTestView *mBaseDetailEditorView;

};
