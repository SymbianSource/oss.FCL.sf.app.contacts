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
#include <qtcontacts.h>

class HbListView;
class HbView;
class HbAction;
class HbDocumentLoader;
class HbAbstractViewItem;
class XQAiwRequest;
class QModelIndex;
class CntHistoryModel;


QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryView : public QObject, public CntAbstractView
{
    Q_OBJECT
    friend class TestCntHistoryView;
    
public: // From CntAbstractView
    CntHistoryView();
    ~CntHistoryView();
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const{ return false; }
    HbView* view() const { return mView; }
    int viewId() const { return historyView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
public slots:
    void updateScrollingPosition();
    void clearHistory();
    void handleClearHistory(int action);
    void itemActivated(const QModelIndex &index);
    void showPreviousView();
    void showRootView();
    void showClearHistoryMenu();
    void contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds);
    
private:
    HbDocumentLoader* docLoader();
    
private:   
    HbListView              *mHistoryListView; // not own
    CntHistoryModel         *mHistoryModel; // own
    HbView                  *mView; // not own
    HbDocumentLoader        *mDocumentLoader; // own
    CntAbstractViewManager  *mViewMgr; // not own
    HbAction                *mBackKey; // not own
    QContact                *mContact; // own
    HbAction                *mClearHistory;  // not own
    CntViewParameters        mArgs;
    XQAiwRequest            *mRequest;    
    CntAbstractEngine       *mEngine;
};

#endif // CNTHISTORYVIEW_H

// EOF
