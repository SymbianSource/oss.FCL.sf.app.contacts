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

#ifndef CNTNAMESVIEWPRIVATE_H
#define CNTNAMESVIEWPRIVATE_H

#include <QObject>

#include "cntnamesview.h"
#include "cntactionlauncher.h"
#include "cntactionmenubuilder.h"

#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <qtcontacts.h>
#include <cntlistmodel.h>

class HbView;
class HbListView;
class HbDocumentLoader;
class HbSearchPanel;
class HbShrinkingVkbHost;
class HbMenu;
class CntExtensionManager;
class CntFetchContacts;

QTM_USE_NAMESPACE

class CntNamesViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntNamesView)
    friend class TestCntNamesView;
    
public:
    CntNamesViewPrivate();
    virtual ~CntNamesViewPrivate();
    
public:
    void activate( const CntViewParameters aArgs );
    void deactivate();
    void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    QString externalize(QDataStream &stream);
    bool internalize(QDataStream &stream, CntViewParameters &viewParameters);

public slots:
    // Search functions
    void showFinder();
    void hideFinder();
    void setFilter(const QString &filterString);
    
    // Extension action (activity stream toolbar button)
    void handleExtensionAction();
    
    // New contact creation
    void createNewContact();
    
    // Single and multiple contact deletion
    void deleteContact( QContact& aContact );
    void handleDeleteContact( int aAction );
    void deleteMultipleContacts();
    void handleDeleteMultipleContacts( QSet<QContactLocalId> aIds );

    // View switches
    void showPreviousView();
    void showCollectionView();
    void showContactView( const QModelIndex& aIndex );
    void showContactView( QContact& aContact );
    void showContactEditorView( QContact& aContact );
    void showSettingsView();
    void showImportsView();
    
    // FTU import dialog
    void handleImportContacts( HbAction *aAction );
    
    // Context menu actions
    void setShowContextMenu( const QModelIndex& aIndex );
    void showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint); 
    void executeAction( QContact& aContact, QContactDetail aDetail, QString aAction );
    void actionExecuted( CntActionLauncher* aAction );
    
    // Contact database notifications
    void handleContactAddition(const QList<QContactLocalId> & aAddedList);
    void handleContactRemoval(const QList<QContactLocalId> & aRemovedList);
    void handleSelfContactIdChange(const QContactLocalId & aOldId, const QContactLocalId & aNewId);
    
public:
    bool isFinderVisible();

private:
    void changeDeleteActionStatus();
    void focusLineEdit();
    void setScrollPosition(int focusedContact);

public:
    CntNamesView *q_ptr;
    
public:  // lazy initializations
    HbListView *list();
    HbDocumentLoader *document();
    
   
private:
    friend class T_NameListTest;
    CntAbstractViewManager  *mViewManager;
    CntListModel            *mListModel;
    HbView                  *mView;
    HbListView              *mListView;
    HbSearchPanel           *mSearchPanel;
    HbDocumentLoader        *mLoader;
    HbShrinkingVkbHost      *mVirtualKeyboard;
    HbAction                *mSoftkey;
    HbAction                *mNamesAction;
    HbAction                *mMultipleDeleter;
    CntActionMenuBuilder    *mMenuBuilder;
    QActionGroup            *mActionGroup;
    HbMenu                  *mMenu;
    HbMenu                  *mViewMenu;
    CntAbstractEngine       *mEngine;
    QContactLocalId          mHandledContactId;
    bool                     mFilterChanged;

    static bool              mIsFirstTimeUse; // FTU flag
};

#endif /* CNTNAMESVIEWPRIVATE_H */
