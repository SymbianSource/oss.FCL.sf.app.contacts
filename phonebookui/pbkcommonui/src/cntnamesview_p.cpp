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

#include "cntnamesview_p.h"
#include "cntactionlauncher.h"
#include "cntfetchcontactsview.h"
#include "cntextensionmanager.h"
#include "cntglobal.h"
#include "cntdebug.h"

#include <cntuiextensionfactory.h>
#include <cntuisocialextension.h>

#include <qapplication.h>
#include <hbabstractviewitem.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hblineedit.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbgroupbox.h>
#include <hbsearchpanel.h>
#include <hbtoolbar.h>
#include <hbframebackground.h>
#include <hbstaticvkbhost.h>
#include <hbmessagebox.h>
#include <hbparameterlengthlimiter.h>

#include <QList>

const char *CNT_CONTACTLIST_XML = ":/xml/contacts_namelist.docml";
static const int CNT_MIN_ROW_COUNT = 2;

CntNamesViewPrivate::CntNamesViewPrivate(CntExtensionManager &extensionManager) :
    QObject(),
    mExtensionManager(extensionManager),
    mViewManager(NULL),
    mListModel(NULL),
    mListView(NULL),
	mEmptyList(NULL),
    mBanner(NULL),
    mSearchPanel(NULL),
    mLoader(NULL),
    mVirtualKeyboard(NULL),
    mNamesAction(NULL),
    mMenuBuilder(NULL),
    mHandledContactId(0),
    mFetchView(NULL),
    mIsDefault(true),
    mId( namesView ),
    mActionGroup(NULL),
    mFocusedContact(0)
{
    CNT_ENTRY
    
    bool ok;
    document()->load( CNT_CONTACTLIST_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_CONTACTLIST_XML);
    }
    
    document()->load( CNT_CONTACTLIST_XML, "no_find");
    
    mView = static_cast<HbView*> (document()->findWidget("view"));
    
    mVirtualKeyboard = new HbStaticVkbHost(mView);
    connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
    connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClose()));

    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    mNewContact = static_cast<HbAction*> (document()->findObject("cnt:newcontact"));
    mNewContact->setParent(mView);
    connect(mNewContact, SIGNAL(triggered()), this, SLOT(createNewContact()));

    mMultipleDeleter = static_cast<HbAction*> (document()->findObject("cnt:delete"));
    mMultipleDeleter->setParent(mView);
    connect(mMultipleDeleter, SIGNAL(triggered()), this, SLOT(deleteMultipleContacts()));

    HbAction* findContacts = static_cast<HbAction*> (document()->findObject("cnt:find"));
    findContacts->setParent(mView);
    connect(findContacts, SIGNAL(triggered()), this, SLOT(showFinder()));

    HbAction* groups = static_cast<HbAction*> (document()->findObject("cnt:groups"));
    groups->setParent(mView);
    connect(groups, SIGNAL(triggered()), this, SLOT(showCollectionView()));

    mNamesAction = static_cast<HbAction*> (document()->findObject("cnt:names"));
    mNamesAction->setParent(mView);

    mImportSim = static_cast<HbAction*> (document()->findObject("cnt:importsim"));
    mImportSim->setParent(mView);
    connect(mImportSim, SIGNAL(triggered()), this, SLOT(importSim()));
    
    mActionGroup = new QActionGroup(this);
    groups->setActionGroup(mActionGroup);
    mNamesAction->setActionGroup(mActionGroup);
    
    HbAction* extension = static_cast<HbAction*> (document()->findObject("cnt:activity"));
    extension->setParent(mView);
    connect(extension, SIGNAL(triggered()), this, SLOT(handleExtensionAction()));
       
    HbAction* settings = static_cast<HbAction*>(document()->findObject("cnt:settings") );
    settings->setParent(mView);
    connect( settings, SIGNAL(triggered()), this, SLOT(showSettings()) );

    HbMenu* viewMenu = static_cast<HbMenu*>(document()->findObject("viewMenu") );
    viewMenu->setParent(mView);

    connect(list(), SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
           SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(list(), SIGNAL(activated (const QModelIndex&)), this,
           SLOT(showContactView(const QModelIndex&)));

    mEmptyList = static_cast<HbLabel*> (document()->findWidget("emptyLabel"));
    mBanner = static_cast<HbGroupBox*> (document()->findWidget("banner"));
    mSearchPanel = static_cast<HbSearchPanel*> (document()->findWidget("searchPanel"));
    connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(hideFinder()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    
    CNT_EXIT
}

CntNamesViewPrivate::~CntNamesViewPrivate()
{
    CNT_ENTRY
    
    delete mListModel;
    mListModel = NULL;

    delete mLoader;
    mLoader = NULL;

    delete mVirtualKeyboard;
    mVirtualKeyboard = NULL;
    
    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    delete mView;
    mView = NULL;

    CNT_EXIT
}

void CntNamesViewPrivate::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    CNT_ENTRY
    
    Q_UNUSED( aArgs )
    mViewManager = aMgr;
    
    if (!mListModel) {
        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
        
        mListModel = new CntListModel(mViewManager->contactManager(SYMBIAN_BACKEND), filter);
        list()->setModel(mListModel);
        setFocusedContact();
    }
    
    mNamesAction->setChecked(true);

    mMenuBuilder = new CntActionMenuBuilder( mListModel->myCardId() );
    connect( mMenuBuilder, SIGNAL(deleteContact(QContact&)), this, SLOT(deleteContact(QContact&)) );
    connect( mMenuBuilder, SIGNAL(editContact(QContact&)), this, SLOT(showContactEditorView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(openContact(QContact&)), this, SLOT(showContactView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(performContactAction(QContact&,QString)), this, SLOT(executeAction(QContact&,QString)));
    
    if ( mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }

    disableDeleteAction();
    
    QContactManager* contactManager = aMgr->contactManager( SYMBIAN_BACKEND );
    // make connections unique, that is, duplicate connections are not connected again
    connect(contactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)),
            this, SLOT(handleContactAddition(const QList<QContactLocalId>&)), Qt::UniqueConnection);
    connect(contactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(handleContactRemoval(const QList<QContactLocalId>&)), Qt::UniqueConnection);
    connect(contactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)), 
            this, SLOT(handleSelfContactIdChange(const QContactLocalId&, const QContactLocalId&)), Qt::UniqueConnection);
    
    setScrollPosition();
    
    CNT_EXIT
}

void CntNamesViewPrivate::deactivate()
{
    CNT_ENTRY
    
    hideFinder();
    
    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    CNT_EXIT
}

void CntNamesViewPrivate::disableDeleteAction()
{
    CNT_ENTRY
    
    bool multipleContacts = mListModel->rowCount() >= CNT_MIN_ROW_COUNT;
    mMultipleDeleter->setEnabled(multipleContacts);
    
    CNT_EXIT
}

void CntNamesViewPrivate::focusLineEdit()
{
    CNT_ENTRY
    
    HbLineEdit *editor = static_cast<HbLineEdit*>(mSearchPanel->primitive("lineedit"));
    editor->setInputMethodHints(Qt::ImhNoPredictiveText);
    
    if (editor)
    {
        editor->setText("");
        editor->setFocus();
    }

    CNT_EXIT
}

void CntNamesViewPrivate::setFilter(const QString &filterString)
{
    CNT_ENTRY
    
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName,
        QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(searchList);

    mListModel->setFilter(filter);

    if (mListModel->rowCount() == 0)
    {
        document()->load( CNT_CONTACTLIST_XML, "find_empty" );
    }
    else
    {
        document()->load( CNT_CONTACTLIST_XML, "find_list" );
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleKeypadOpen()
{
    CNT_ENTRY
    
    mView->setMaximumHeight(mVirtualKeyboard->applicationArea().height());
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleKeypadClose()
{
    CNT_ENTRY
    
    mView->setMaximumHeight(-1);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showFinder()
{
    CNT_ENTRY
    
    focusLineEdit();
    
    mView->hideItems(Hb::AllItems);
    
    mListModel->showMyCard(false);

    mImportSim->setVisible(false);
    mNewContact->setVisible(false);
    mMultipleDeleter->setVisible(false);
    
    CNT_EXIT
}

void CntNamesViewPrivate::hideFinder()
{
    CNT_ENTRY
    
    mVirtualKeyboard->closeKeypad();
    mView->setMaximumHeight(-1);
    
    document()->load( CNT_CONTACTLIST_XML, "no_find" );
    mView->showItems(Hb::AllItems);
    
    mListModel->showMyCard(true);

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);

    mListModel->setFilter(filter);

    mNewContact->setVisible(true);
    mImportSim->setVisible(true);
    mMultipleDeleter->setVisible(true);

    CNT_EXIT    
}

bool CntNamesViewPrivate::isFinderVisible()
{
    CNT_ENTRY
    
    bool isVisible = false;
    if ( mSearchPanel )
    {
        isVisible = mSearchPanel->isVisible();
    }
    
    CNT_EXIT
    return isVisible;
}

void CntNamesViewPrivate::showPreviousView()
{
    CNT_ENTRY
    
    if ( !isFinderVisible() )
    {
        mViewManager->back( CntViewParameters() );
    }
    else
    {
        hideFinder();
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleExtensionAction()
{
    CNT_ENTRY
    
    for (int i = 0; i < mExtensionManager.pluginCount(); i++)
    {
        CntUiSocialExtension* socialExtension = mExtensionManager.pluginAt(i)->socialExtension();
        if (socialExtension)
        {
            CntViewParameters params;
            socialExtension->handleToolbarAction(params);
            if (params.count())
            {
                setFocusedContact();
                mViewManager->changeView(params);
                break;
            }
        }
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::createNewContact()
{
    CNT_ENTRY
    
    QContact newContact;
    newContact.setType( QContactType::TypeContact );
    
    showContactEditorView(newContact);
    
    CNT_EXIT
}

void CntNamesViewPrivate::deleteContact(QContact& aContact)
{
    CNT_ENTRY
    
    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );
    QString name = manager->synthesizedDisplayLabel(aContact);
    
    mHandledContactId = aContact.localId();
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(HbAction*)),
            hbTrId("txt_common_button_delete"), hbTrId("txt_common_button_cancel"));
    
    CNT_EXIT
}

void CntNamesViewPrivate::deleteMultipleContacts()
{
    CNT_ENTRY
    
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(mViewManager->contactManager( SYMBIAN_BACKEND ));
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleDeleteMultipleContacts()));
    }

    mFetchView->setDetails(hbTrId("txt_phob_title_delete_contacts"),hbTrId("txt_common_button_delete"));
    QSet<QContactLocalId> emptyContactsSet;

    // Pop up a list of contacts for deletion
    mFetchView->displayContacts(CntFetchContacts::popup,
                                HbAbstractItemView::MultiSelection,
                                emptyContactsSet);
    CNT_EXIT
}

void CntNamesViewPrivate::handleDeleteMultipleContacts()
{
    CNT_ENTRY
    
    QSet<QContactLocalId> selectedContacts = mFetchView->getSelectedContacts();

    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );
    if ( !mFetchView->wasCanceled() && !selectedContacts.isEmpty() ) {
        foreach ( QContactLocalId id, selectedContacts.values() )
        {
            manager->removeContact( id );
        }
    }

    delete mFetchView;
    mFetchView = NULL;
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactView( const QModelIndex& aIndex )
{
    CNT_ENTRY

    QContact c = mListModel->contact(aIndex);
    showContactView( c );
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactView( QContact& aContact )
{
    CNT_ENTRY
    
    setFocusedContact();
    CntViewParameters args;
    args.insert(EViewId, commLauncherView);
    if (aContact.localId() == mListModel->myCardId() && aContact.details().count() <= 4)
    {
        args.insert(EViewId, myCardView);
    }

    QVariant contact;
    contact.setValue(aContact);
    args.insert(ESelectedContact, contact);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint)
{
    CNT_ENTRY
    
    QContact contact = mListModel->contact(aItem->modelIndex());
    
   // In case of an empty MyCard, do not show any ContextMenu
    if (!(contact.localId() == mListModel->myCardId() && contact.details().count() <= 4))
    {
        HbMenu* menu = mMenuBuilder->actionMenu( contact, mListModel->myCardId() );
        menu->setPreferredPos( aPoint );
        menu->setAttribute( Qt::WA_DeleteOnClose, true );
        menu->open();
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::executeAction( QContact& aContact, QString aAction )
{
    CNT_ENTRY
    
    CntActionLauncher* other = new CntActionLauncher( aAction );
    connect(other, SIGNAL(actionExecuted(CntAction*)), this, SLOT(actionExecuted(CntAction*)));
    other->execute(aContact, QContactDetail());
    
    CNT_EXIT
}

void CntNamesViewPrivate::actionExecuted(CntActionLauncher* aAction)
{
    CNT_ENTRY

    aAction->deleteLater();

    CNT_EXIT
}

void CntNamesViewPrivate::handleDeleteContact( HbAction* aAction )
{
    CNT_ENTRY
    
    HbMessageBox *note = static_cast<HbMessageBox*>(sender());
    
    if (note && aAction == note->actions().first())
    {
        mViewManager->contactManager( SYMBIAN_BACKEND )->removeContact(mHandledContactId);
    }
    
    mHandledContactId = 0;
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactEditorView(QContact& aContact)
{
    CNT_ENTRY
    
    setFocusedContact();
    CntViewParameters args;
    args.insert(EViewId, editView);
    
    QVariant contact;
    contact.setValue(aContact);
    args.insert(ESelectedContact, contact);

    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showCollectionView()
{
    CNT_ENTRY
    
    setFocusedContact();
    CntViewParameters args;
    args.insert(EViewId, collectionView);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::importSim()
{
    CNT_ENTRY
    
    setFocusedContact();
    CntViewParameters args;        
    args.insert(EViewId, importsView);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showSettings()
{
    CNT_ENTRY
    
    setFocusedContact();
    CntViewParameters args;
    args.insert( EViewId, settingsView );
    mViewManager->changeView( args );
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleContactAddition(const QList<QContactLocalId>& aAddedList)
{
    CNT_ENTRY

    Q_UNUSED(aAddedList);
    disableDeleteAction();
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleContactRemoval(const QList<QContactLocalId>& aRemovedList)
{
    CNT_ENTRY
  
    Q_UNUSED(aRemovedList);
    disableDeleteAction();
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleSelfContactIdChange(const QContactLocalId& aOldId, const QContactLocalId& aNewId)
{
    CNT_ENTRY
    
    Q_UNUSED(aOldId);
    Q_UNUSED(aNewId);
    disableDeleteAction();
    
    CNT_EXIT
}

void CntNamesViewPrivate::setScrollPosition()
{
    CNT_ENTRY
    
    // Scroll to the focused contact
    if ( mFocusedContact > 0 )
    {
        QContactManager* contactManager = mViewManager->contactManager( SYMBIAN_BACKEND );
        QContact c = contactManager->contact(mFocusedContact);
        list()->scrollTo(mListModel->indexOfContact(c), HbAbstractItemView::PositionAtCenter);
    }
    
    CNT_EXIT
}

//// lazy accessors
HbListView* CntNamesViewPrivate::list()
{
    CNT_ENTRY

    if (!mListView) {
        mListView = static_cast<HbListView*> (mLoader->findWidget("listView"));

        mListView->setFrictionEnabled(true);
        mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
        mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);
        mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
        mListView->verticalScrollBar()->setInteractive(true);
        
        HbFrameBackground frame;
        frame.setFrameGraphicsName("qtg_fr_list_normal");
        frame.setFrameType(HbFrameDrawer::NinePieces);
        mListView->itemPrototypes().first()->setDefaultFrame(frame);
        
        mListView->setUniformItemSizes(true);
        mListView->setItemRecycling(true);

        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
        indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mListView);
    }
    
    CNT_EXIT
    return mListView;
}

HbDocumentLoader* CntNamesViewPrivate::document()
{
    CNT_ENTRY
    
    if (!mLoader) {
        mLoader = new HbDocumentLoader();
    }
    
    CNT_EXIT
    return mLoader;
}

void CntNamesViewPrivate::setFocusedContact()
{
    if ( mListModel )
    {
        mFocusedContact = mListModel->contact(list()->currentIndex()).localId();
    }
}
// End of File
