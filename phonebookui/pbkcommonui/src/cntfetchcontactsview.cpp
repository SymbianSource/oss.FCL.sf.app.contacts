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

#include <hbdialog.h>
#include <hbscrollbar.h>
#include <hbindexfeedback.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hblistviewitem.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <hbstaticvkbhost.h>
#include <QGraphicsLinearLayout>
#include <qcontactid.h>
#include <QDebug>
#include <mobcntmodel.h>
#include "cntfetchcontactsview.h"

/*!
Given a contact manager, CntFetchContacts is responsible for 
retrieving a set of contacts, if any were chosen by the user. 
*/
CntFetchContacts::CntFetchContacts(QContactManager *aManager) :
QObject(),
mPopup(NULL),
mCntModel(NULL),
mListView(NULL),
mEmptyListLabel(NULL),
mSelectionMode(HbAbstractItemView::MultiSelection),
mManager(aManager),
mWasCanceled(false),
mLabel(NULL),
mVirtualKeyboard(NULL),
mPrimaryAction(NULL),
mSecondaryAction(NULL),
mIndexFeedback(NULL)
{
    mSearchPanel = new HbSearchPanel();
    mSearchPanel->setVisible(false);
    mSearchPanel->setCancelEnabled(false);
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));

    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    mContainerWidget = new HbWidget();

    // set up the list with all contacts
    QList<QContactSortOrder> sortOrders;
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrders.append(sortOrderFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrders.append(sortOrderLastName);

    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    mCntModel = new MobCntModel(mManager, contactsFilter, sortOrders, false);
}

CntFetchContacts::~CntFetchContacts()
{
    delete mCntModel;
    mCntModel = NULL;
    
    delete mVirtualKeyboard;
    mVirtualKeyboard = NULL;
    
    delete mIndexFeedback;
    mIndexFeedback = NULL;
}

/*!
Query to see if the user decided to press Cancel after selecting 
group members. Must be called to see if results are valid.
*/
bool CntFetchContacts::wasCanceled() const
{
    return mWasCanceled;
}

void CntFetchContacts::setDetails(QString aTitle, QString aButtonText)
{
    mButtonText = aButtonText;
    
    if (!mLabel) {
        mLabel = new HbLabel(aTitle);
    }
}

/*!
Brings up a list of contacts, awaiting user response. This function is asynchronous.
When a response is given, a clicked signal will be sent.
*/
void CntFetchContacts::displayContacts(DisplayType aType, HbAbstractItemView::SelectionMode aMode, QSet<QContactLocalId> aContacts)
{
    switch (aType) {
    case view:
    {
        // TODO Currently only services will need a view 
        break;
    }

    case popup:
    {
        doInitialize(aMode,aContacts);
        markMembersInView();
        connectSignal();
        showPopup();
        
        mLayout->addItem(mSearchPanel);
        mContainerWidget->setLayout(mLayout);
        mContainerWidget->setPreferredHeight(mListView->mainWindow()->size().height());
        mContainerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        break;
    }

    default:
        break;
    }
}

QSet<QContactLocalId> CntFetchContacts::getSelectedContacts() const
{
   return mCurrentlySelected;
}

void CntFetchContacts::setFilter(const QString &filterString)
{
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    QContactDetailFilter detailfilter;
    detailfilter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    detailfilter.setMatchFlags(QContactFilter::MatchStartsWith);
    detailfilter.setValue(searchList);
    
    mCntModel->setFilterAndSortOrder(detailfilter);

    markMembersInView();

    if (mCntModel->rowCount() == 0) {
        mLayout->removeItem(mListView);
        
        if (mEmptyListLabel) {
            qreal searchHeight = mSearchPanel->size().height();
            HbLabel* heading = static_cast<HbLabel*>(mPopup->headingWidget());
            qreal heightToSet =  mPopup->size().height() - mVirtualKeyboard->keyboardArea().height() - searchHeight - heading->size().height();
            mEmptyListLabel->setMaximumHeight(heightToSet);
            mEmptyListLabel->setVisible(true);
            mLayout->insertItem(0, mEmptyListLabel);
        }

        mListView->setVisible(false);
        mSearchPanel->setVisible(true);
    }
    else {
        if (mEmptyListLabel) {
            mEmptyListLabel->setVisible(false);
        }
        mLayout->removeItem(mEmptyListLabel);
        mLayout->insertItem(0, mListView);
        mListView->setVisible(true);
    }
}

void CntFetchContacts::handleKeypadOpen()
{
    qreal searchHeight = mSearchPanel->size().height();
    HbLabel* heading = static_cast<HbLabel*>(mPopup->headingWidget());
    qreal heightToSet =  mPopup->size().height() - mVirtualKeyboard->keyboardArea().height() - searchHeight - heading->size().height();

    if (mEmptyListLabel) {
        mEmptyListLabel->setMaximumHeight( heightToSet - mEmptyListLabel->size().height() );
    }
    
    mListView->setMaximumHeight(heightToSet);
}

void CntFetchContacts::handleKeypadClose()
{
    mListView->setMaximumHeight(mPopup->size().height());

    if (mEmptyListLabel) {
        mEmptyListLabel->setMaximumHeight(mPopup->size().height());
    }
}

void CntFetchContacts::handleUserResponse(HbAction* action)
{
    HbDialog *popup = static_cast<HbDialog*>(sender());
    
    bool userCanceled = (action == mSecondaryAction); 
    if (popup && userCanceled) {
        mCurrentlySelected.clear();
        
        // Notify that the user canceled.
        mWasCanceled = true;
    }
    else {
        mWasCanceled = false;
    }
    
    emit clicked();
}

void CntFetchContacts::memberSelectionChanged(const QModelIndex &index)
{
    if (!index.isValid()) return;

    if (mSelectionMode == HbAbstractItemView::SingleSelection) {
        mCurrentlySelected.clear();
    }

    QContactLocalId contactId = mCntModel->contact(index).localId();

    bool isSelected = mListView->selectionModel()->isSelected(index);
    if (isSelected != mCurrentlySelected.contains(contactId)) {
        if (isSelected) {
            mCurrentlySelected.insert(contactId);
        }
        else {
            mCurrentlySelected.remove(contactId);
        }
    }
    
    // Check for the case where there is a cancel button only. If so, 
    // after selecting any contact, should dismiss the dialog immediately.
    if (mButtonText.isEmpty() && mSelectionMode == HbAbstractItemView::SingleSelection) {
        mPopup->close();
    }
}

void CntFetchContacts::doInitialize(HbAbstractItemView::SelectionMode aMode,
                                                QSet<QContactLocalId> aContacts)
{
    mSelectionMode = aMode;
    mCurrentlySelected = aContacts;

    mSearchPanel->setVisible(true);

    if (!mPopup) {
        mPopup = new HbDialog;
    }
    
    if (!mListView) {
        mListView = new HbListView(mPopup);
        mListView->setModel(mCntModel);
        mListView->setSelectionMode(mSelectionMode);
        mListView->setFrictionEnabled(true);
        mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
        mListView->verticalScrollBar()->setInteractive(true);

        HbListViewItem *prototype = mListView->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::Thumbnail);
        prototype->setStretchingStyle(HbListViewItem::StretchLandscape);

        mIndexFeedback = new HbIndexFeedback(mPopup);
        mIndexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        mIndexFeedback->setItemView(mListView);

        // Note that the layout takes ownership of the item(s) it contains.
        if (!mCntModel->rowCount()) {
            mListView->setVisible(false);
            if (!mEmptyListLabel) {
                mEmptyListLabel = new HbTextItem(hbTrId("txt_phob_info_no_matching_contacts"));
                mEmptyListLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
                mEmptyListLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
                mEmptyListLabel->setAlignment(Qt::AlignCenter);
                mLayout->insertItem(0, mEmptyListLabel);
            }
        }
        else {
            mLayout->addItem(mListView);
        }

        mCntModel->showMyCard(false);
    }
}

void CntFetchContacts::connectSignal()
{
    connect(mListView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(memberSelectionChanged(const QModelIndex&)));
}

void CntFetchContacts::showPopup()
{
    mPopup->setTimeout(HbPopup::NoTimeout);
    mPopup->setDismissPolicy(HbPopup::NoDismiss);
    mPopup->setModal(true);
    mPopup->setContentWidget(mContainerWidget);

    if (!mVirtualKeyboard) {
        mVirtualKeyboard = new HbStaticVkbHost(mPopup);
        connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
        connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClose()));
    }
    
    if (!mLabel) {
        mLabel = new HbLabel(hbTrId("txt_phob_title_contacts"));
    }
    mPopup->setHeadingWidget(mLabel);

    if (!mButtonText.isEmpty() && !mPrimaryAction) {
        mPrimaryAction = new HbAction(hbTrId(mButtonText.toAscii()));
        mPopup->addAction(mPrimaryAction);
    }
    
    if (!mSecondaryAction) {
        mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
        mPopup->addAction(mSecondaryAction);
    }

    mPopup->open(this, SLOT(handleUserResponse(HbAction*)));
}

void CntFetchContacts::markMembersInView()
{
    // if there are no contacts matching the current filter,
    // show "no matching contacts" label
    if (mCntModel->rowCount() == 0) {
        if (!mEmptyListLabel) {
            mEmptyListLabel = new HbTextItem(hbTrId("(no matching contacts)"));
            mEmptyListLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            mEmptyListLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
            mEmptyListLabel->setAlignment(Qt::AlignCenter);
            mLayout->insertItem(1, mEmptyListLabel);
        }
    }
    else {
        mLayout->removeItem(mEmptyListLabel);
        delete mEmptyListLabel;
        mEmptyListLabel = 0;
    }

    // Mark group members in the listview
    foreach (QContactLocalId id, mCurrentlySelected) {
        QContact contact = mManager->contact(id);
        QModelIndex contactIndex = mCntModel->indexOfContact(contact);
        mListView->selectionModel()->select(contactIndex, QItemSelectionModel::Select);
    }
}

// End of file
