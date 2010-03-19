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
#include <qtcontacts.h>
#include "cntbasedetaileditorview.h"
#include "cntviewparameters.h"

class CntEditorDataViewItem;
class CntViewManager;
class CntMainWindow;

class CntDetailEditorTestView : public CntBaseDetailEditorView
{
    Q_OBJECT

public:
    CntDetailEditorTestView(CntViewManager *viewManager, QGraphicsItem *parent = 0):
        CntBaseDetailEditorView(viewManager, parent),
        addDetailSelected(false){ }

    CntViewParameters::ViewId viewId() const { return CntViewParameters::noView; }
    void initializeForm() { }
    void addDetail() { addDetailSelected = true; }
    
public:
    bool addDetailSelected;
};
class TestCntEditorDataViewItem : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void createItem();
    void addDetail();
    void createCustomWidget();
    void modelPosition();
    void textChanged();
    void indexChanged();
    void cleanupTestCase();

private:
    CntEditorDataViewItem       *mViewItem;
    CntDetailEditorTestView     *mTestView;
    CntViewManager              *mViewManager;
    CntMainWindow               *mWindow;

 };


