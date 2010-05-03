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

#ifndef CNTHISTORYVIEW_H
#define CNTHISTORYVIEW_H

#include <QObject>
#include <cntabstractview.h>

class HbListView;
class CntHistoryModel;
class HbView;
class QModelIndex;
class HbAction;
class HbDocumentLoader;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public: // From CntAbstractView
    CntHistoryView();
    ~CntHistoryView();
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const{ return false; }
    HbView* view() const { return mView; }
    int viewId() const { return historyView; }

public slots:
    void updateScrollingPosition();
    void clearHistory();
    void itemActivated(const QModelIndex &index);
    void showPreviousView();
    
    
private:
    HbDocumentLoader* docLoader();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif    
    HbListView*                 mHistoryListView; // not own
    CntHistoryModel*            mHistoryModel; // own
    HbView*                     mView; // not own
    HbDocumentLoader*           mDocumentLoader; // own
    CntAbstractViewManager*     mViewMgr; // not own
    HbAction*                   mBackKey; // own
    QContact*                   mContact; // own
    
};

#endif // CNTHISTORYVIEW_H

// EOF
