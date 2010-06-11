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

#ifndef CNTABSTRACTLISTVIEW_H_
#define CNTABSTRACTLISTVIEW_H_

#include "cntnamesview.h"
#include "cntactionlauncher.h"
#include "cntactionmenubuilder.h"

#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <QObject>

#include <qcontact.h>
#include <cntlistmodel.h>

class HbView;
class HbListView;
class HbGroupBox;
class HbTextItem;
class HbDocumentLoader;
class HbSearchPanel;
class HbStaticVkbHost;
class QGraphicsLinearLayout;
class CntExtensionManager;
class CntFetchContacts;

class CntNamesViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntNamesView)
    
public:
    CntNamesViewPrivate(CntExtensionManager &extensionManager);
    virtual ~CntNamesViewPrivate();

public slots:
    void showBanner( const QString aText );
    void hideBanner();
    
    void showFinder();
    void hideFinder();
    void setFilter(const QString &filterString);
    
    void handleKeypadOpen();
    void handleKeypadClose();
    
    void handleExtensionAction();
    
    void createNewContact();
    void deleteContact( QContact& aContact );
    void deleteMultipleContacts();
    
    void handleDeleteMultipleContacts();

    void showPreviousView();
    void showCollectionView();
    void showContactView( QContact& aContact );
    void showContactView( const QModelIndex& );
    void showContactEditorView( QContact& aContact );
    void showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint);
    void showSettings();
    
    void executeAction( QContact& aContact, QString aAction );
    void actionExecuted( CntActionLauncher* aAction );
    void handleDeleteContact( HbAction* aAction );
    void importSim();
    
    void handleContactAddition(const QList<QContactLocalId> & aAddedList);
    void handleContactChanged(const QList<QContactLocalId> & aChangedList);
    void handleContactRemoval(const QList<QContactLocalId> & aRemovedList);
    void handleSelfContactIdChange(const QContactLocalId & aOldId, const QContactLocalId & aNewId);
    
public:
    bool isFinderVisible();
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    
private:
    void disableDeleteAction();
    
public:
    CntNamesView *q_ptr;
    
public:  // lazy initializations
    HbListView *list();
    HbTextItem *emptyLabel();
    HbGroupBox *groupBox();
    HbSearchPanel *search();
    QGraphicsLinearLayout *layout();
    HbDocumentLoader *document();
    
private:
    CntExtensionManager&        mExtensionManager;
    CntAbstractViewManager*     mViewManager;
    CntListModel*               mListModel;
    HbView*                     mView;
    HbListView*                 mListView;
    HbTextItem*                 mEmptyList;
    HbGroupBox*                 mBanner;
    HbSearchPanel*              mSearchPanel;
    HbDocumentLoader*           mLoader;
    QGraphicsLinearLayout*      mLayout;
    HbStaticVkbHost*            mVirtualKeyboard;
    HbAction*                   mSoftkey;
    CntActionMenuBuilder*       mMenuBuilder;
    HbAction*                   mImportSim;
    HbAction*                   mNewContact;
    QContactLocalId             mHandledContactId;
    HbAction*                   mMultipleDeleter;
    CntFetchContacts*           mFetchView;
    bool                        mIsDefault;
    int                         mId;
    QActionGroup*               mActionGroup;
};

#endif /* CNTABSTRACTLISTVIEW_H_ */
