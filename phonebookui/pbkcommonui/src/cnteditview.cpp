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

#include "cnteditview.h"
#include "cntviewparameters.h"
#include "cntviewmanager.h"
#include "cntdetailpopup.h"
#include "cnteditviewdetailitem.h"
#include "cnteditviewheadingitem.h"

#include <QGraphicsItem>
#include <QGraphicsLinearLayout>
#include <hbscrollarea.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>
#include <hbmenu.h>
#include <thumbnailmanager_qt.h>

const char *CNT_UI_XML = ":/xml/contacts_ev.docml";

CntEditView::CntEditView(CntViewManager *aViewManager, QGraphicsItem *aParent) : 
    CntBaseView(aViewManager, aParent),
    mScrollArea(0),
    mContainerWidget(0),
    mContainerLayout(0),
    mContact(0),
    mDetailItem(0),
    mThumbnailManager(0),
    mHeadingItem(0),
    mDetailItemIndex(-1),
    mAddressItemIndex(-1)
{   
    bool ok = false;
    ok=loadDocument(CNT_UI_XML);

    if (ok)
    {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_ev.docml");
    }
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
	
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
        this, SLOT(thumbnailReady(QPixmap, void*, int, int)) );
}

CntEditView::~CntEditView()
{
    delete mContact;
}

void CntEditView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    QIcon qicon(pixmap);
    HbIcon icon(qicon);
    mHeadingItem->setIcon(icon);
}

/*!
Activates a previous view
*/
void CntEditView::aboutToCloseView()
{   
    if (contact() && contact()->localId() == contactManager()->selfContactId()
        && contactManager()->selfContactId() != 0 && contact()->details().count() <= 4)
    {
        //delete empty mycard
        contactManager()->removeContact(contact()->localId());
        CntViewParameters viewParameters(CntViewParameters::namesView);
        viewManager()->onActivateView(viewParameters);
    }

    // save contact if there is one and it's not empty
    else if (contact() && !contact()->isEmpty())
    {
        bool isSaved = contactManager()->saveContact(contact());
		
        CntViewParameters viewParameters(CntViewParameters::namesView);
        if (isSaved)
        {
            viewParameters.setSelectedContact(*contact());
            viewParameters.setSelectedAction("save");
        }
        else
        {
            viewParameters.setSelectedAction("failed");
        }
        viewManager()->onActivateView(viewParameters);
    }
    else
    {
        CntViewParameters viewParameters(CntViewParameters::namesView);
        viewManager()->onActivateView(viewParameters);
    }
}

void CntEditView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width() - 50, 0);
    }
    CntBaseView::resizeEvent(event);
}

/*
Activates a default view
*/
void CntEditView::activateView(const CntViewParameters &aViewParameters)
{
    QContact xcontact = aViewParameters.selectedContact();
    mContact = new QContact(xcontact);

    mHeadingItem = static_cast<CntEditViewHeadingItem*>(findWidget(QString("cnt_editview_heading")));
    mHeadingItem->setDetails(mContact);
    connect(mHeadingItem, SIGNAL(iconClicked()), this, SLOT(openImageEditor()));
    connect(mHeadingItem, SIGNAL(textClicked()), this, SLOT(openNameEditor()));
    
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
    if (details.count() > 0)
    {
        for (int i = 0;i < details.count();i++)
        {
            if (details.at(i).subType() == QContactAvatar::SubTypeImage)
            {
                mThumbnailManager->getThumbnail(details.at(i).avatar());
                break;
            }
        }
    }

    //construct listview & connect signals
    mScrollArea = static_cast<HbScrollArea*>(findWidget(QString("cnt_listview_labels")));
    mScrollArea->setScrollDirections(Qt::Vertical);

    mContainerWidget = new QGraphicsWidget(mScrollArea);
    mContainerWidget->setPreferredWidth(mScrollArea->size().width() - 50);
    mScrollArea->setContentWidget(mContainerWidget);

    mContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mContainerLayout->setContentsMargins(0, 0, 0, 0);
    mContainerLayout->setSpacing(0);
    mContainerWidget->setLayout(mContainerLayout);
    prepareItems();

    // command handling (for delete menu option)
    connect(commands(), SIGNAL(commandExecuted(QString, QContact)), 
            this, SLOT(handleExecutedCommand(QString, QContact)));

    // You can't delete a contact which hasn't been saved yet, now can you?
    if (mContact->localId() == 0)
    {
        menu()->removeAction(actions()->baseAction("cnt:deletecontact"));
    }

    // Save and Discard all changes options only available once changes have been made
    if (xcontact == contactManager()->contact(mContact->localId()))
    {
        actions()->baseAction("cnt:discardallchanges")->setEnabled(false);
        actions()->baseAction("cnt:save")->setEnabled(false);
    }
}

void CntEditView::prepareItems()
{
    QList<QContactPhoneNumber> numberDetails = mContact->details<QContactPhoneNumber>();
    QList<QContactEmailAddress> emailDetails = mContact->details<QContactEmailAddress>();
    QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
    QList<QContactOnlineAccount> chatDetails = mContact->details<QContactOnlineAccount>();
    QList<QContactUrl> urlDetails = mContact->details<QContactUrl>();

    // phone numbers
    if (numberDetails.count())
    {
        for (int i = 0;i < numberDetails.count();i++)
        {
            createItem(numberDetails.at(i));
        }
    }
    else
    {
        QContactPhoneNumber number;
        createItem(number);
    }

    // email addresses
    if (emailDetails.count())
    {
        for (int i = 0;i < emailDetails.count();i++)
        {
            createItem(emailDetails.at(i));
        }
    }
    else
    {
        QContactEmailAddress email;
        createItem(email);
    }

    // postal addresses
    if (!addressDetails.count())
    {
        QContactAddress address;
        createItem(address);
    }
    else
    {
        mAddressItemIndex = mContainerLayout->count();
    }

    // IM accounts
    if (chatDetails.count())
    {
        for (int i = 0;i < chatDetails.count();i++)
        {
            createItem(chatDetails.at(i));
        }
    }
    else
    {
        QContactOnlineAccount account;
        createItem(account);
    }

    // URL addresses
    if (urlDetails.count())
    {
        for (int i = 0;i < urlDetails.count();i++)
        {
            createItem(urlDetails.at(i));
        }
    }
    else
    {
        QContactUrl url;
        createItem(url);
    }

    QList<QContactOrganization> companyDetails = mContact->details<QContactOrganization>();
    QList<QContactBirthday> birthdayDetails = mContact->details<QContactBirthday>();
    QList<QContactAnniversary> anniversaryDetails = mContact->details<QContactAnniversary>();
    QList<QContactNote> noteDetails = mContact->details<QContactNote>();
    QList<QContactFamily> familyDetails = mContact->details<QContactFamily>();

    QList<QContactAvatar> ringingtoneDetails = mContact->details<QContactAvatar>();
    bool hasRingingtone = false;
    for (int i = 0;i < ringingtoneDetails.count();i++)
    {
        if (ringingtoneDetails.at(i).subType() == QContactAvatar::SubTypeAudioRingtone)
        {
            hasRingingtone = true;
            break;
        }
    }

    if (addressDetails.count() || companyDetails.count() || birthdayDetails.count() ||
        anniversaryDetails.count() || noteDetails.count() || familyDetails.count() || hasRingingtone)
    {
        mDetailItem = new HbGroupBox(this);
        mDetailItem->setHeading(hbTrId("Details"));
        mContainerLayout->addItem(mDetailItem);
        mDetailItemIndex = mContainerLayout->count();

        // address details
        if (addressDetails.count())
        {
            for (int i = 0;i < addressDetails.count();i++)
            {
                createItem(addressDetails.at(i));
            }
        }

        // company details
        if (companyDetails.count())
        {
            for (int i = 0;i < companyDetails.count();i++)
            {
                if (!companyDetails.at(i).title().isEmpty() ||
                        !companyDetails.at(i).name().isEmpty() ||
                        !companyDetails.at(i).department().isEmpty())
                {
                    mExcludeList.append(QContactOrganization::DefinitionName);
                    createItem(companyDetails.at(i), QContactOrganization::DefinitionName);
                }
                if (!companyDetails.at(i).assistantName().isEmpty())
                {
                    mExcludeList.append(QContactOrganization::FieldAssistantName);
                    createItem(companyDetails.at(i), QContactOrganization::FieldAssistantName);
                }
            }
        }

        // birthday
        if (birthdayDetails.count())
        {
            mExcludeList.append(QContactBirthday::DefinitionName);
            for (int i = 0;i < birthdayDetails.count();i++)
            {
                createItem(birthdayDetails.at(i));
            }
        }

        // anniversary
        if (anniversaryDetails.count())
        {
            mExcludeList.append(QContactAnniversary::DefinitionName);
            for (int i = 0;i < anniversaryDetails.count();i++)
            {
                createItem(anniversaryDetails.at(i));
            }
        }

        // ringing tone
        if (ringingtoneDetails.count())
        {
            for (int i = 0;i < ringingtoneDetails.count();i++)
            {
                if (ringingtoneDetails.at(i).subType() == QContactAvatar::SubTypeAudioRingtone)
                {
                    mExcludeList.append(QContactAvatar::SubTypeAudioRingtone);
                    createItem(ringingtoneDetails.at(i));
                }
            }
        }

        // notes
        if (noteDetails.count())
        {
            mExcludeList.append(QContactNote::DefinitionName);
            for (int i = 0;i < noteDetails.count();i++)
            {
                createItem(noteDetails.at(i));
            }
        }

        // family details
        if (familyDetails.count())
        {
            for (int i = 0;i < familyDetails.count();i++)
            {
                if (!familyDetails.at(i).spouse().isEmpty())
                {
                    mExcludeList.append(QContactFamily::FieldSpouse);
                    createItem(familyDetails.at(i), QContactFamily::FieldSpouse);
                }
                if (familyDetails.at(i).children().count())
                {
                    mExcludeList.append(QContactFamily::FieldChildren);
                    createItem(familyDetails.at(i), QContactFamily::FieldChildren);
                }
            }
        }
    }
}

void CntEditView::onLongPressed(const QPointF &point)
{
    CntEditViewDetailItem *item = qobject_cast<CntEditViewDetailItem*>(sender());
    QContactDetail detail = item->detail();

    HbMenu *menu = new HbMenu();
    HbAction *addDetailAction = 0;
    HbAction *editAction = 0;
    HbAction *deleteAction = 0;

    // not possible for some of the details or when detail type doesn't exist yet
    if (detail.definitionName() != QContactAddress::DefinitionName &&
        detail.definitionName() != QContactFamily::DefinitionName &&
        detail.definitionName() != QContactOrganization::DefinitionName &&
        detail.definitionName() != QContactBirthday::DefinitionName &&
        detail.definitionName() != QContactAnniversary::DefinitionName &&
        detail.definitionName() != QContactAvatar::DefinitionName &&
        !detail.isEmpty())
    {
        addDetailAction = menu->addAction(hbTrId("Add detail"));
    }

    editAction = menu->addAction(hbTrId("Edit"));

    // only add this if details contains something to delete
    if (!detail.isEmpty())
    {
        deleteAction = menu->addAction(hbTrId("Delete detail"));
    }
    
    HbAction *selectedAction = menu->exec(point);

    if (selectedAction)
    {
        if (selectedAction == editAction)
        {
            onItemActivated();
        }
        else if (selectedAction == deleteAction)
        {
            deleteDetail();
        }
        else if (selectedAction == addDetailAction)
        {
            addDetail();
        }
    }
    menu->deleteLater();
}

void CntEditView::onItemActivated()
{
    CntEditViewDetailItem *item = qobject_cast<CntEditViewDetailItem*>(sender());
    QContactDetail detail = item->detail();

    QString field = detail.definitionName();

    // check the context (needed for editing postal address)
    // or the index of the detail (needed for other editors for correct VKB handling)
    QString context;
    if (detail.contexts().count() && field == QContactAddress::DefinitionName)
    {
        context = detail.contexts().at(0);
    }
    // Phone number
    else if (field == QContactPhoneNumber::DefinitionName)
    {
        int index = mContact->details<QContactPhoneNumber>().indexOf(detail);
        context.setNum(index);
    }
    // Email address
    else if (field == QContactEmailAddress::DefinitionName)
    {
        int index = mContact->details<QContactEmailAddress>().indexOf(detail);
        context.setNum(index);
    }
    // Online account (IM)
    else if (field == QContactOnlineAccount::DefinitionName)
    {
        int index = mContact->details<QContactOnlineAccount>().indexOf(detail);
        context.setNum(index);
    }
    // URL
    else if (field == QContactUrl::DefinitionName)
    {
        int index = mContact->details<QContactUrl>().indexOf(detail);
        context.setNum(index);
    }
    // Note
    else if (field == QContactNote::DefinitionName)
    {
        int index = mContact->details<QContactNote>().indexOf(detail);
        context.setNum(index);
    }
    // Family details
    else if (field == QContactFamily::DefinitionName)
    {
        if (item->fieldType() == QContactFamily::FieldSpouse)
        {
            context = "spouse";
        }
    }
    // Company details
    else if (field == QContactOrganization::DefinitionName)
    {
        if (item->fieldType() == field)
        {
            context = "company";
        }
    }
    // Ringing tone
    else if (field == QContactAvatar::DefinitionName)
    {
        // launch ringtone picker service
    }

    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters = prepareToEditContact(field, context);
    viewParameters.setSelectedContact(*contact());
    if (viewParameters.nextViewId() != CntViewParameters::noView)
    {
        viewManager()->onActivateView(viewParameters);
    }
}

/*!
Open the editor for the selected detail and add a new empty field with default subtype & context
*/
void CntEditView::addDetail()
{
    CntEditViewDetailItem *item = qobject_cast<CntEditViewDetailItem*>(sender());
    QContactDetail detail = item->detail();
    QString field = detail.definitionName();

    CntViewParameters viewParameters(CntViewParameters::noView);
    QString action("add");
    viewParameters = prepareToEditContact(field, action);
    viewParameters.setSelectedContact(*contact());
    if (viewParameters.nextViewId() != CntViewParameters::noView)
    {
        viewManager()->onActivateView(viewParameters);
    }
}

/*!
Delete the selected (via longtap menu) detail
*/
void CntEditView::deleteDetail()
{
    CntEditViewDetailItem *item = qobject_cast<CntEditViewDetailItem*>(sender());
    QContactDetail detail = item->detail();

    // family detail needs special checking as it is split in multiple items in the list
    if (detail.definitionName() == QContactFamily::DefinitionName)
    {
        QContactFamily family(detail);

        if (item->fieldType() == QContactFamily::FieldSpouse)
        {
            family.setSpouse(QString());
            mExcludeList.removeOne(QContactFamily::FieldSpouse);
        }
        else
        {
            family.setChildren(QStringList());
            mExcludeList.removeOne(QContactFamily::FieldChildren);
        }
        contact()->saveDetail(&family);
    }
    // same goes for company details
    else if (detail.definitionName() == QContactOrganization::DefinitionName)
    {
        QContactOrganization company(detail);

        if (item->fieldType() == QContactOrganization::DefinitionName)
        {
            company.setTitle(QString());
            company.setDepartment(QStringList());
            company.setName(QString());
            mExcludeList.removeOne(QContactOrganization::DefinitionName);
        }
        else
        {
            company.setAssistantName(QString());
            mExcludeList.removeOne(QContactOrganization::FieldAssistantName);
        }
        contact()->saveDetail(&company);
    }
    else if (detail.definitionName() == QContactBirthday::DefinitionName)
    {
        mExcludeList.removeOne(QContactBirthday::DefinitionName);
        contact()->removeDetail(&detail);
    }
    else if (detail.definitionName() == QContactAnniversary::DefinitionName)
    {
        mExcludeList.removeOne(QContactAnniversary::DefinitionName);
        contact()->removeDetail(&detail);
    }
    else if (detail.definitionName() == QContactAvatar::DefinitionName)
    {
        mExcludeList.removeOne(QContactAvatar::SubTypeAudioRingtone);
        contact()->removeDetail(&detail);
    }
    else if (detail.definitionName() == QContactNote::DefinitionName)
    {
        contact()->removeDetail(&detail);
        if (!contact()->details<QContactNote>().count())
        {
            mExcludeList.removeOne(QContactNote::DefinitionName);
        }
    }
    else
    {
        contact()->removeDetail(&detail);
    }

    // if removed detail was the last of its type, create an empty one as a replacement (else decrease detail item index)
    if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
    {
        if (contact()->details<QContactPhoneNumber>().count())
        {
            mDetailItemIndex--;
            mAddressItemIndex--;
            mContainerLayout->removeItem(item);
            item->deleteLater();
        }
        else
        {
            QContactPhoneNumber number;
            item->setDetail(number);
        }
    }
    else if (detail.definitionName() == QContactEmailAddress::DefinitionName)
    {
        if (contact()->details<QContactEmailAddress>().count())
        {
            mDetailItemIndex--;
            mAddressItemIndex--;
            mContainerLayout->removeItem(item);
            item->deleteLater();
        }
        else
        {
            QContactEmailAddress email;
            item->setDetail(email);
        }
    }
    else if (detail.definitionName() == QContactAddress::DefinitionName)
    {
        mContainerLayout->removeItem(item);
        item->deleteLater();

        if (!contact()->details<QContactAddress>().count())
        {
            QContactAddress address;
            createItem(address, QString(), mAddressItemIndex);
            mDetailItemIndex++;
        }
    }
    else if (detail.definitionName() == QContactOnlineAccount::DefinitionName)
    {
        if (contact()->details<QContactOnlineAccount>().count())
        {
            mDetailItemIndex--;
            mContainerLayout->removeItem(item);
            item->deleteLater();
        }
        else
        {
            QContactOnlineAccount account;
            item->setDetail(account);
        }
    }
    else if (detail.definitionName() == QContactUrl::DefinitionName)
    {
        if (contact()->details<QContactUrl>().count())
        {
            mDetailItemIndex--;
            mContainerLayout->removeItem(item);
            item->deleteLater();
        }
        else
        {
            QContactUrl url;
            item->setDetail(url);
        }
    }
    else
    {
        mContainerLayout->removeItem(item);
        item->deleteLater();
    }

    if (mContainerLayout->count() == mDetailItemIndex)
    {
        mContainerLayout->removeItem(mDetailItem);
        mDetailItem->deleteLater();
        mDetailItemIndex = -1;
    }

    actions()->baseAction("cnt:discardallchanges")->setEnabled(true);
    actions()->baseAction("cnt:save")->setEnabled(true);
}

/*!
Add actions also to toolbar
*/
void CntEditView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:addfield");
    actions()->addActionsToToolBar(toolBar());
    
    connect(actions()->baseAction("cnt:addfield"), SIGNAL(triggered()),
            this, SLOT(addField()));
}

/*!
Add actions to options menu
*/
void CntEditView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:save") << actions()->baseAction("cnt:discardallchanges") << 
        actions()->baseAction("cnt:deletecontact");
    actions()->addActionsToMenu(menu());
    
    connect(actions()->baseAction("cnt:discardallchanges"), SIGNAL(triggered()),
            this, SLOT(discardAllChanges()));

    connect(actions()->baseAction("cnt:deletecontact"), SIGNAL(triggered()),
            this, SLOT(deleteContact()));
    
    connect(actions()->baseAction("cnt:save"), SIGNAL(triggered()),
            this, SLOT(aboutToCloseView()));
}

/*!
Cancel all changes made and return to names view
*/
void CntEditView::discardAllChanges()
{
    CntViewParameters viewParameters(CntViewParameters::namesView);
    viewManager()->onActivateView(viewParameters);
}

/*!
Launch delete query
*/
void CntEditView::deleteContact()
{
    commands()->deleteContact(*contact());
}

/*!
Open the "Add field" popup and launch a specific detail editor view if needed
*/
void CntEditView::addField()
{
    QString detail = CntDetailPopup::selectDetail(mExcludeList);
    
    if (detail == hbTrId("txt_phob_formlabel_note"))
    {
        CntViewParameters viewParameters(CntViewParameters::noteEditorView);
        viewParameters.setSelectedContact(*contact());
        viewManager()->onActivateView(viewParameters);
    }
    else if (detail == hbTrId("txt_phob_formlabel_personal_ringing_tone"))
    {
        // launch ringtone selection service
    }
    else if (detail == hbTrId("txt_phob_formlabel_date"))
    {
        CntViewParameters viewParameters(CntViewParameters::dateEditorView);
        viewParameters.setSelectedContact(*contact());
        viewManager()->onActivateView(viewParameters);
    }
    else if (detail == hbTrId("txt_phob_formlabel_company_details"))
    {
        CntViewParameters viewParameters(CntViewParameters::companyEditorView);
        viewParameters.setSelectedContact(*contact());
        viewParameters.setSelectedAction("company");
        viewManager()->onActivateView(viewParameters);
    }
    else if (detail == hbTrId("txt_phob_formlabel_family"))
    {
        CntViewParameters viewParameters(CntViewParameters::familyDetailEditorView);
        viewParameters.setSelectedContact(*contact());
        viewParameters.setSelectedAction("spouse");
        viewManager()->onActivateView(viewParameters);
    }
    else if (detail == hbTrId("Synchronization"))
    {
        // Synchronization detail editor view to be done (lacks engine support)
    }
}

/*!
Opens the name detail editor view
*/
void CntEditView::openNameEditor()
{
    CntViewParameters viewParameters(CntViewParameters::namesEditorView);
    viewParameters.setSelectedContact(*contact());
    viewManager()->onActivateView(viewParameters);
}

/*!
Opens the image detail editor view
*/
void CntEditView::openImageEditor()
{
    CntViewParameters viewParameters(CntViewParameters::imageEditorView);
    viewParameters.setSelectedContact(*contact());
    viewManager()->onActivateView(viewParameters);
}

/*
Handle signals emitted from CntCommands, only used for delete command for now.
*/
int CntEditView::handleExecutedCommand(QString aCommand, const QContact &aContact)
{
    Q_UNUSED(aContact);
    int result=-1;
    if (aCommand == "delete")
    {
        CntViewParameters viewParameters(CntViewParameters::namesView);
        viewParameters.setSelectedContact(*contact());
        viewParameters.setSelectedAction("delete");
        viewManager()->onActivateView(viewParameters);
        result=0;
    }
    return result;
}

/*!
Set correct next view id and possible action string (used for "Add detail" longtap menu command
and when opening postal address editor view)
*/
CntViewParameters CntEditView::prepareToEditContact(const QString &aViewType, const QString &aAction)
{
    CntViewParameters viewParameters(CntViewParameters::noView);
    
    // open name editor
    if (aViewType == QContactName::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::namesEditorView);
    }
    // open number editor
    else if (aViewType == QContactPhoneNumber::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::phoneNumberEditorView);
    }
    // open email editor
    else if (aViewType == QContactEmailAddress::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::emailEditorView);
    }
    // open address editor
    else if (aViewType == QContactAddress::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::addressEditorView);
    }
    // open online account editor
    else if (aViewType == QContactOnlineAccount::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::onlineAccountEditorView);
    }
    // open URL editor
    else if (aViewType == QContactUrl::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::urlEditorView);
    }
    // open note editor
    else if (aViewType == QContactNote::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::noteEditorView);
    }
    // open family detail editor
    else if (aViewType == QContactFamily::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::familyDetailEditorView);
    }
    // open company detail editor
    else if (aViewType == QContactOrganization::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::companyEditorView);
    }
    // open date detail editor
    else if (aViewType == QContactBirthday::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::dateEditorView);
    }
    // open date detail editor
    else if (aViewType == QContactAnniversary::DefinitionName)
    {
        viewParameters.setNextViewId(CntViewParameters::dateEditorView);
    }
    else 
    {
        viewParameters.setNextViewId(CntViewParameters::noView);
    }
    
    viewParameters.setSelectedAction(aAction);
    
    return viewParameters;
}

void CntEditView::createItem(const QContactDetail &detail, const QString &type, int position)
{
    CntEditViewDetailItem *item = new CntEditViewDetailItem(mContainerWidget);
    item->setDetail(detail, type);
    mContainerLayout->insertItem(position, item);
    connect(item, SIGNAL(longPressed(const QPointF&)), this, SLOT(onLongPressed(const QPointF&)));
    connect(item, SIGNAL(clicked()), this, SLOT(onItemActivated()));
}

QContact *CntEditView::contact()
{
    return mContact;
}

void CntEditView::setContact(QContact* aContact)
{
    if (contact())
    {
        delete mContact;
    }
    mContact=aContact;
}

// EOF
