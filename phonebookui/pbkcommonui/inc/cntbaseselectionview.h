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

#ifndef CNTBASESELECTIONVIEW_H
#define CNTBASESELECTIONVIEW_H

#include "cntabstractviewmanager.h"
#include "cntabstractview.h"
#include "qtpbkglobal.h"

class HbListView;
class HbView;
class HbDocumentLoader;
class HbAction;

class MobCntModel;

class QTPBK_EXPORT CntBaseSelectionView : public QObject, public CntAbstractView
{
    Q_OBJECT

public:
    CntBaseSelectionView();
    ~CntBaseSelectionView();

signals:
    void viewOpened( const CntViewParameters aArgs );
    void viewClosed();
    
public:
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const;
    HbView* view() const;
    
    virtual int viewId() const = 0;
    
private slots:
    void closeView();
    
protected:
    HbDocumentLoader* mDocument;
    HbListView* mListView;
    HbView* mView;
    HbAction* mSoftkey;
    CntAbstractViewManager* mMgr;
    MobCntModel* mListModel;
/*        
public:
    virtual void setupView();
    virtual void activateView(const CntViewParameters viewParameters);
    void addItemsToLayout();
    
public slots:
    virtual void onListViewActivated(const QModelIndex &index) { Q_UNUSED(index); }

public:

    HbListView             *listView();
    QGraphicsLinearLayout  *listLayout();
    QItemSelectionModel    *selectionModel();

private:
    HbListView              *mListView;
    QGraphicsLinearLayout   *mListLayout;

    // needed in subclasses
#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    HbAbstractItemView::SelectionMode   mSelectionMode;
*/
};

#endif /* CNTBASESELECTIONVIEW_H */
