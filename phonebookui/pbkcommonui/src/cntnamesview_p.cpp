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
#include "cntaction.h"
#include "cntfetchcontactsview.h"
#include "cntextensionmanager.h"
#include "qtpbkglobal.h"

#include <cntuiextensionfactory.h>
#include <cntuisocialextension.h>

#include <qapplication.h>
#include <hbabstractviewitem.h>
#include <hbaction.h>
#include <hbnotificationdialog.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbtextitem.h>
#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbgroupbox.h>
#include <hbsearchpanel.h>
#include <hbtoolbar.h>
#include <hbframebackground.h>
#include <hbstaticvkbhost.h>
#include <hbmessagebox.h>
#include <hbparameterlengthlimiter.h>

#include <QGraphicsLinearLayout>
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
    mLayout(NULL),
    mMenuBuilder(NULL),
    mHandledContactId(0),
    mFetchView(NULL),
    mIsDefault(true),
    mId( namesView )
{
    bool ok;
    document()->load( CNT_CONTACTLIST_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_CONTACTLIST_XML);
    }
    mView = static_cast<HbView*> (document()->findWidget("view"));
    
    mVirtualKeyboard = new HbStaticVkbHost(mView);
    connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
    connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClose()));
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
       
    mNewContact = static_cast<HbAction*> (document()->findObject("cnt:newcontact"));
    mMultipleDeleter = static_cast<HbAction*> (document()->findObject("cnt:delete"));
    HbAction* findContacts = static_cast<HbAction*> (document()->findObject("cnt:find"));
    HbAction* groups = static_cast<HbAction*> (document()->findObject("cnt:groups"));
    mImportSim = static_cast<HbAction*> (document()->findObject("cnt:importsim"));
    
    HbAction* extension = static_cast<HbAction*> (document()->findObject("cnt:activity"));
       
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    connect(mNewContact, SIGNAL(triggered()), this, SLOT(createNewContact()));
    connect(mMultipleDeleter, SIGNAL(triggered()), this, SLOT(deleteMultipleContacts()));
    connect(findContacts, SIGNAL(triggered()), this, SLOT(showFinder()));
    connect(groups, SIGNAL(triggered()), this, SLOT(showCollectionView()));
    connect(mImportSim, SIGNAL(triggered()), this, SLOT(importSim()));
    connect(extension, SIGNAL(triggered()), this, SLOT(handleExtensionAction()));
    connect(list(), SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
           SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(list(), SIGNAL(activated (const QModelIndex&)), this,
           SLOT(showContactView(const QModelIndex&)));
}

CntNamesViewPrivate::~CntNamesViewPrivate()
{
    mView->deleteLater();

    delete mListModel;
    mListModel = NULL;

    delete mListView;
    mListView = NULL;

    delete mSearchPanel;
    mSearchPanel = NULL;

    delete mEmptyList;
    mEmptyList = NULL;

    delete mBanner;
    mBanner = NULL;

    delete mLoader;
    mLoader = NULL;

    delete mVirtualKeyboard;
    mVirtualKeyboard = NULL;
    
    delete mMenuBuilder;
    mMenuBuilder = NULL;
}

void CntNamesViewPrivate::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    mViewManager = aMgr;
    if (!mListModel) {
        QContactSortOrder sortOrderFirstName;
        sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldFirst);
        sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

        QContactSortOrder sortOrderLastName;
        sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldLast);
        sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

        QList<QContactSortOrder> sortOrders;
        sortOrders.append(sortOrderFirstName);
        sortOrders.append(sortOrderLastName);

        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);

        mListModel = new MobCntModel(mViewManager->contactManager(SYMBIAN_BACKEND), filter, sortOrders);
        list()->setModel(mListModel);
        
    }

    mMenuBuilder = new CntActionMenuBuilder( mListModel->myCardId() );
    connect( mMenuBuilder, SIGNAL(deleteContact(QContact&)), this, SLOT(deleteContact(QContact&)) );
    connect( mMenuBuilder, SIGNAL(editContact(QContact&)), this, SLOT(showContactEditorView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(openContact(QContact&)), this, SLOT(showContactView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(performContactAction(QContact&,QString)), this, SLOT(executeAction(QContact&,QString)));
    
    if ( mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
    
    QContactManager* contactManager = aMgr->contactManager( SYMBIAN_BACKEND );
    if ( aArgs.contains(ESelectedContact) )
    {
        QContact selectedContact = aArgs.value(ESelectedContact).value<QContact>();
        QString action = aArgs.value(ESelectedAction).toString();
        
        if ( action == "save" )
        {
            QString name = contactManager->synthesizedDisplayLabel( selectedContact );
            HbNotificationDialog::launchDialog(qtTrId("Contact \"%1\" saved").arg(name));
        }
               
        else if ( action == "delete" )
        {
            QString name = contactManager->synthesizedDisplayLabel( selectedContact );
            HbNotificationDialog::launchDialog(qtTrId("%1 deleted").arg(name));
        }    
        else if ( action == "failed")
        {
            HbNotificationDialog::launchDialog(qtTrId("SAVING FAILED!"));
        }
    }
    
    bool multipleContacts = mListModel->rowCount() >=  CNT_MIN_ROW_COUNT;
    mMultipleDeleter->setEnabled(multipleContacts);
}

void CntNamesViewPrivate::deactivate()
{
    hideFinder();

    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    // delete the hbsearch since we can not empty text from outside.
    delete mSearchPanel;
    mSearchPanel = NULL;
}

void CntNamesViewPrivate::setFilter(const QString &filterString)
{
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName,
        QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(searchList);

    mListModel->setFilterAndSortOrder(filter);

    if (mListModel->rowCount() == 0) {
        layout()->removeItem(list());
        layout()->insertItem(1, emptyLabel());
        list()->setVisible(false);
        emptyLabel()->setVisible(true);
    }
    else {
        layout()->removeItem(emptyLabel());
        layout()->insertItem(1, list());
        emptyLabel()->setVisible(false);
        list()->setVisible(true);
    }
    mListModel->showMyCard(false);
}

void CntNamesViewPrivate::handleKeypadOpen()
{
    qreal searchHeight = search()->size().height();
    qreal bannerHeight = groupBox()->size().height();
    qreal heightToSet = mView->size().height() - mVirtualKeyboard->keyboardArea().height()
        - searchHeight - bannerHeight;

    list()->setMaximumHeight(heightToSet);
    emptyLabel()->setMaximumHeight(heightToSet);
}

void CntNamesViewPrivate::handleKeypadClose()
{
    list()->setMaximumHeight(mView->size().height());
    emptyLabel()->setMaximumHeight(mView->size().height());
}

void CntNamesViewPrivate::showBanner(const QString aText)
{
    layout()->insertItem(0, groupBox());
    groupBox()->setHeading(aText);
    groupBox()->setVisible(true);
}

void CntNamesViewPrivate::hideBanner()
{
    layout()->removeItem(groupBox());
    groupBox()->setVisible(false);
}

void CntNamesViewPrivate::showFinder()
{
    showBanner(hbTrId("txt_phob_subtitle_find_all_contacts"));

    mView->toolBar()->hide();
    mImportSim->setVisible(false);
    mNewContact->setVisible(false);
    mMultipleDeleter->setVisible(false);
    
    setFilter(QString());

    list()->setVisible(true);
    layout()->addItem(search());
    search()->setVisible(true);

    mListModel->showMyCard(false);
}

void CntNamesViewPrivate::hideFinder()
{
    layout()->removeItem(emptyLabel());
    layout()->removeItem(search());
    emptyLabel()->setVisible(false);
    search()->setVisible(false);

    hideBanner();

    layout()->addItem(list());
    list()->setVisible(true);

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);

    mListModel->setFilterAndSortOrder(filter);
    mListModel->showMyCard(true);

    mNewContact->setVisible(true);
    mImportSim->setVisible(true);
    mMultipleDeleter->setVisible(true);
    mView->toolBar()->show();
}

bool CntNamesViewPrivate::isFinderVisible()
{
    return search()->isVisible();
}

void CntNamesViewPrivate::showPreviousView()
{
    if ( !isFinderVisible() )
    {
        qApp->quit();
    }
    hideFinder();
}

void CntNamesViewPrivate::handleExtensionAction()
{
    for(int i = 0;i < mExtensionManager.pluginCount();i++)
    {
        CntUiSocialExtension* socialExtension = mExtensionManager.pluginAt(i)->socialExtension();
        if (socialExtension)
        {
            CntViewParameters params;
            socialExtension->handleToolbarAction(params);
            if (params.count())
            {
                mViewManager->changeView(params);
                break;
            }
        }
    }
}

void CntNamesViewPrivate::createNewContact()
{
    QContact newContact;
    newContact.setType( QContactType::TypeContact );
    
    showContactEditorView(newContact);
}

void CntNamesViewPrivate::deleteContact(QContact& aContact)
{
    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );
    QString name = manager->synthesizedDisplayLabel(aContact);
    
    mHandledContactId = aContact.localId();
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(HbAction*)),
            hbTrId("txt_phob_button_delete"), hbTrId("txt_common_button_cancel"));
}

void CntNamesViewPrivate::deleteMultipleContacts()
{
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(mViewManager->contactManager( SYMBIAN_BACKEND ));
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleDeleteMultipleContacts()));
    }

    mFetchView->setDetails(hbTrId("txt_phob_opt_delete_contact"),hbTrId("txt_phob_button_delete"));
    QSet<QContactLocalId> emptyContactsSet;

    // Pop up a list of contacts for deletion
    mFetchView->displayContacts(CntFetchContacts::popup,
                                HbAbstractItemView::MultiSelection,
                                emptyContactsSet);
}

void CntNamesViewPrivate::handleDeleteMultipleContacts()
{
    QSet<QContactLocalId> selectedContacts = mFetchView->getSelectedContacts();

    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );
    if ( !mFetchView->wasCanceled() && !selectedContacts.isEmpty() ) {
        QList<QContactLocalId> selectedContactsList = selectedContacts.values();
        foreach (QContactLocalId id, selectedContactsList) {
            QContact contact = manager->contact(id);

            manager->removeContact(contact.localId());
        }
    }

    if (mListModel->rowCount() < CNT_MIN_ROW_COUNT) {
        mMultipleDeleter->setEnabled(false);
    }

    delete mFetchView;
    mFetchView = NULL;
}

void CntNamesViewPrivate::showContactView( const QModelIndex& aIndex )
{
    QContact c = mListModel->contact(aIndex);
    showContactView( c );
}

void CntNamesViewPrivate::showContactView( QContact& aContact )
{
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
}

void CntNamesViewPrivate::showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint)
{
    QContact contact = mListModel->contact(aItem->modelIndex());
    HbMenu* menu = mMenuBuilder->actionMenu( contact );
    menu->setPreferredPos( aPoint );
    menu->setAttribute( Qt::WA_DeleteOnClose, true );
    menu->open();
}

void CntNamesViewPrivate::executeAction( QContact& aContact, QString aAction )
{
    CntAction* other = new CntAction( aAction );
    connect(other, SIGNAL(actionExecuted(CntAction*)), this, SLOT(actionExecuted(CntAction*)));
    other->execute(aContact, QContactDetail());
}

void CntNamesViewPrivate::actionExecuted(CntAction* aAction)
{
    aAction->deleteLater();
}

void CntNamesViewPrivate::handleDeleteContact( HbAction* aAction )
{
    HbMessageBox *note = static_cast<HbMessageBox*>(sender());
    
    if (note && aAction == note->actions().first())
    {
        mViewManager->contactManager( SYMBIAN_BACKEND )->removeContact(mHandledContactId);
    }
    
    mHandledContactId = 0;
}

void CntNamesViewPrivate::showContactEditorView(QContact& aContact)
{
    CntViewParameters args;
    args.insert(EViewId, editView);
    
    QVariant contact;
    contact.setValue(aContact);
    args.insert(ESelectedContact, contact);

    mViewManager->changeView(args);
}

void CntNamesViewPrivate::showCollectionView()
{
    CntViewParameters args;
    args.insert(EViewId, collectionView);
    mViewManager->changeView(args);
}

void CntNamesViewPrivate::importSim()
{
   CntViewParameters args;        
   args.insert(EViewId, importsView);
   mViewManager->changeView(args);
}


//// lazy accessors
HbListView* CntNamesViewPrivate::list()
{
    if (!mListView) {
        mListView = static_cast<HbListView*> (mLoader->findWidget("listView"));
        HbListViewItem *prototype = mListView->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::Thumbnail);

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

        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
        indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mListView);
        
        connect( mListView, SIGNAL(scrollPositionChanged(const QPointF&)), this, SLOT(handleScroll(const QPointF&)) );
    }
    return mListView;
}

HbTextItem* CntNamesViewPrivate::emptyLabel()
{
    if (!mEmptyList) {
        mEmptyList = new HbTextItem(hbTrId("(no matching contacts)"));
        mEmptyList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        mEmptyList->setFontSpec(HbFontSpec(HbFontSpec::Primary));
        mEmptyList->setAlignment(Qt::AlignCenter);
    }
    return mEmptyList;
}
HbGroupBox* CntNamesViewPrivate::groupBox()
{
    if (!mBanner)
        mBanner = new HbGroupBox();
    return mBanner;
}

HbSearchPanel* CntNamesViewPrivate::search()
{
    if (!mSearchPanel) {
        mSearchPanel = new HbSearchPanel();
        mSearchPanel->setVisible( false );
        connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(hideFinder()));
        connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    }
    return mSearchPanel;
}
QGraphicsLinearLayout* CntNamesViewPrivate::layout()
{
    if (!mLayout) {
        QGraphicsWidget *w = mLoader->findWidget(QString("content"));
        mLayout = static_cast<QGraphicsLinearLayout*> (w->layout());
    }
    return mLayout;
}

HbDocumentLoader* CntNamesViewPrivate::document()
{
    if (!mLoader) {
        mLoader = new HbDocumentLoader();
    }
    return mLoader;
}

// End of File
