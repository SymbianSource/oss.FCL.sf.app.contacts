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

#include <QGraphicsLinearLayout>
#include <QList>

const char *CNT_CONTACTLIST_XML = ":/xml/contacts_namelist.docml";

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
       
    HbAction* newContact = static_cast<HbAction*> (document()->findObject("cnt:newcontact"));
    HbAction* findContacts = static_cast<HbAction*> (document()->findObject("cnt:find"));
    HbAction* groups = static_cast<HbAction*> (document()->findObject("cnt:groups"));
    HbAction* importSim = static_cast<HbAction*> (document()->findObject("cnt:importsim"));
    
    HbAction* extension = static_cast<HbAction*> (document()->findObject("cnt:activity"));
       
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    connect(newContact, SIGNAL(triggered()), this, SLOT(createNewContact()));
    connect(findContacts, SIGNAL(triggered()), this, SLOT(showFinder()));
    connect(groups, SIGNAL(triggered()), this, SLOT(showCollectionView()));
    connect(importSim, SIGNAL(triggered()), this, SLOT(importSim()));
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
    }

    list()->setModel(mListModel);

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
    QString action = aArgs.value(ESelectedAction).toString();
    if ( action == "save" )
    {
        QString name = contactManager->synthesizedDisplayLabel( aArgs.value(ESelectedContact).value<QContact>() );
        HbNotificationDialog::launchDialog(qtTrId("Contact \"%1\" saved").arg(name));

        scrollTo( aArgs.value(ESelectedContact).value<QContact>() );
    }
           
    else if ( action == "delete" )
    {
        QString name = contactManager->synthesizedDisplayLabel( aArgs.value(ESelectedContact).value<QContact>() );
        HbNotificationDialog::launchDialog(qtTrId("%1 deleted").arg(name));
    }    
    else if ( action == "failed")
    {
        HbNotificationDialog::launchDialog(qtTrId("SAVING FAILED!"));
    }
}

void CntNamesViewPrivate::deactivate()
{
    hideFinder();

    // Don't delete the model, default views will retain it between
    // activate/deactivate. do it in destructor.
    list()->setModel(NULL);
    
    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    // delete the hbsearch since we can not empty text from outside.
    delete mSearchPanel;
    mSearchPanel = NULL;
}

void CntNamesViewPrivate::scrollTo(const QContact &aContact)
{
    if (mListModel) 
    {
        QModelIndex index = mListModel->indexOfContact(aContact);

        if (index.isValid()) 
        {
            list()->scrollTo(index);
        }
    }
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
    showContactEditorView(newContact);
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
    mMenuBuilder->execActionMenu( contact, aPoint );
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

void CntNamesViewPrivate::deleteContact(QContact& aContact)
{
    QContactManager* manager = mViewManager->contactManager( SYMBIAN_BACKEND );
    QString name = manager->synthesizedDisplayLabel(aContact);

    HbMessageBox *note = new HbMessageBox(
            hbTrId("txt_phob_info_delete_1").arg(name),
            HbMessageBox::MessageTypeQuestion);

    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));

    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        manager->removeContact(aContact.localId());
        HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_1_deleted").arg(name));
    }
    delete note;
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
    int copied(0);
    int failed(0);
    QContactManager* simManager = mViewManager->contactManager( SIM_BACKEND );
    QContactManager* symbianManager = mViewManager->contactManager( SYMBIAN_BACKEND );
    
    QList<QContactLocalId> contactIds = simManager->contactIds();
    if (contactIds.count() == 0) {
        HbMessageBox::information("Nothing to copy: SIM card is empty or not accessible.");
        return;
    }
        
    foreach(QContactLocalId id, contactIds) {
        QContact contact = simManager->contact(id);
        if (contact.localId() > 0) {
            //delete local id before saving to different storage
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(0);
            contactId->setManagerUri(QString());
            contact.setId(*contactId);

            // custom label contains name information, save it to the first name 
            QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
            if (names.count() > 0) {
                QContactName name = static_cast<QContactName>(names.at(0));
                name.setFirstName(name.customLabel());
                name.setCustomLabel(QString());
                contact.saveDetail(&name);
            }
                
            if (symbianManager->saveContact(&contact)) {
                copied++;
            }
            else {
                failed++;
            }
        }
        else {
            failed++;
        }
    }
    
    QString resultMessage;
    resultMessage.setNum(copied);
    resultMessage.append(" contact copied, ");
    resultMessage.append(QString().setNum(failed));
    resultMessage.append(" failed.");
        
    HbMessageBox::information(resultMessage);
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
        mListView->verticalScrollBar()->setInteractive(true);
        
        HbFrameBackground frame;
        frame.setFrameGraphicsName("qtg_fr_list_normal");
        frame.setFrameType(HbFrameDrawer::NinePieces);
        mListView->itemPrototypes().first()->setDefaultFrame(frame);
        
        mListView->setUniformItemSizes(true);

        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
        indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mListView);
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
