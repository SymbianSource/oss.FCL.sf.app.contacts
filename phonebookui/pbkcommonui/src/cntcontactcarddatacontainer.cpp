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

#include "cntcontactcarddatacontainer.h"
#include "cntcontactcarddataitem.h"
#include <cntmaptileservice.h> //For fetching maptile

#include <QPainter>
#include <QList>
#include <qtcontacts.h>
#include <hbicon.h>
#include <cntviewparams.h>

#include "cntstringmapper.h"

/*!
Compare function for CntContactCardDataItem
*/
namespace
{
    bool compareObjects(const CntContactCardDataItem *d1, const CntContactCardDataItem *d2)
    {   
        return d1->mPosition < d2->mPosition;
    }
}

/*!
Constructor
*/
CntContactCardDataContainer::CntContactCardDataContainer(QContact* contact, QObject *parent) : mContact(contact), mSeparatorIndex(-1)
{
    Q_UNUSED(parent);
    if (contact->type() == QContactType::TypeGroup)
    {
        initializeGroupData();
    }
    else
    {
        initializeActionsData();
        initializeDetailsData();
    }
}

/*!
Destructor
*/
CntContactCardDataContainer::~CntContactCardDataContainer()
{
    while (!mDataItemList.isEmpty())
    {
        delete mDataItemList.takeFirst();
    }
}

/*!
Initialize contact details which include actions.
*/
void CntContactCardDataContainer::initializeActionsData()
{
    QList<QContactActionDescriptor> actionDescriptors = mContact->availableActions();
    QStringList availableActions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        availableActions << actionDescriptors.at(i).actionName();
    }

    QList<QContactDetail> details = mContact->details();
    for (int i = 0; i < details.count(); i++)
    { 
        if (availableActions.contains("call", Qt::CaseInsensitive) && supportsDetail("call", details[i]) && details[i].definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber number(details.at(i));
            
            QString context = number.contexts().isEmpty() ? QString() : number.contexts().first();
            QString subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();
             
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(subtype, context), itemCount(), true);
            dataItem->setAction("call");
            dataItem->setValueText(number.number());
            dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(subtype, context)));
            dataItem->setContactDetail(number);
            mDataItemList.insert(itemCount(), dataItem);
        }
       
        if (availableActions.contains("message", Qt::CaseInsensitive) && supportsDetail("message", details[i]) && details[i].definitionName() == QContactPhoneNumber::DefinitionName)
        {
           QContactPhoneNumber number(details.at(i));
           CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_send_message"), itemCount(), true);
           dataItem->setAction("message");
           dataItem->setValueText(number.number());
           QString icon;
           if (number.contexts().isEmpty())
           {
               icon = "qtg_large_message";
           }
           else if (number.contexts().first() == QContactDetail::ContextHome)
           {
               icon = "qtg_large_message_home";
           }
           else if (number.contexts().first() == QContactDetail::ContextWork)
           {
               icon = "qtg_large_message_work";
           }
           else
           {
               icon = "qtg_large_message";
           }
           dataItem->setIcon(HbIcon(icon));
           dataItem->setContactDetail(number);
           mDataItemList.insert(itemCount(), dataItem);
        }
    }
    //email
    if (availableActions.contains("email", Qt::CaseInsensitive))
    {
        QList<QContactDetail> details = actionDetails("email", *mContact);
        for (int i = 0; i < details.count(); i++)
        {
            QContactEmailAddress email(details.at(i));
            QString context = email.contexts().isEmpty() ? QString() : email.contexts().first();
                                  
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(email.definitionName(), context), itemCount(), true);
            dataItem->setAction("email");
            dataItem->setValueText(email.emailAddress(), Qt::ElideLeft);
            dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(email.definitionName(), context)));
            dataItem->setContactDetail(email);
            mDataItemList.insert(itemCount(), dataItem);  
        }
    }
    //url
    QList<QContactUrl> urlDetails = mContact->details<QContactUrl>();
    for (int i = 0; i < urlDetails.count(); i++)
    {
        QContactUrl url(urlDetails.at(i));
        QString context = url.contexts().isEmpty() ? QString() : url.contexts().first();
        
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(url.definitionName(), context), itemCount(), true);
        dataItem->setAction("url");
        dataItem->setValueText(url.url());
        dataItem->setIcon(HbIcon(mStringMapper.getContactCardIconString(url.definitionName(), context)));
        dataItem->setContactDetail(url);
        mDataItemList.insert(itemCount(), dataItem);     
    }
}

/*!
Initialize group details which includes actions.
*/
void CntContactCardDataContainer::initializeGroupData()
{
    // get the conference number
    QContactPhoneNumber confCallNumber = mContact->detail<QContactPhoneNumber>();
    
    //call
    if (confCallNumber.number() != NULL)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_conference_call"), itemCount(), true);
        dataItem->setAction("call");
        dataItem->setValueText(confCallNumber.number());
        dataItem->setIcon(HbIcon("qtg_large_call_group"));
        dataItem->setContactDetail(confCallNumber);  
        mDataItemList.insert(itemCount(), dataItem);
    }
    
    //message
    CntContactCardDataItem* dataMessageItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_send_val_members"), itemCount(), true);
    dataMessageItem->setAction("message");
    dataMessageItem->setValueText(confCallNumber.number());
    dataMessageItem->setIcon(HbIcon("qtg_large_message"));
    dataMessageItem->setContactDetail(confCallNumber);  
    mDataItemList.insert(itemCount(), dataMessageItem);
    
    //email
    CntContactCardDataItem* dataEmailItem = new CntContactCardDataItem(hbTrId("txt_phob_dblist_email"), itemCount(), true);
    dataEmailItem->setAction("email");
    dataEmailItem->setValueText(confCallNumber.number());
    dataEmailItem->setIcon(HbIcon("qtg_large_email"));
    dataEmailItem->setContactDetail(confCallNumber);  
    mDataItemList.insert(itemCount(), dataEmailItem);
}

/*!
Initialize contact details which not include actions.
*/
void CntContactCardDataContainer::initializeDetailsData()
{
    //sip & internet call
    QList<QContactOnlineAccount> onlinedDetails = mContact->details<QContactOnlineAccount>();
    for (int i = 0; i < onlinedDetails.count(); i++)
    {
        QContactOnlineAccount online(onlinedDetails.at(i));
        QString context = online.contexts().isEmpty() ? QString() : online.contexts().first();
        QString subtype = online.subTypes().isEmpty() ? online.definitionName() : online.subTypes().first();
              
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(mStringMapper.getContactCardListLocString(subtype, context), itemCount(), false);
        dataItem->setValueText(online.accountUri());
        dataItem->setContactDetail(online);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
    }
    
    //address
    QString contextHome(QContactAddress::ContextHome.operator QString());
    QString contextWork(QContactAddress::ContextWork.operator QString());
    CntMapTileService::ContactAddressType sourceAddressType;  
    mLocationFeatureEnabled = CntMapTileService::isLocationFeatureEnabled() ;

    QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
    for (int i = 0; i < addressDetails.count(); i++)
    {
        sourceAddressType = CntMapTileService::AddressPreference;
        QVariantList addressList;
        //no action
        QString title;
        if (addressDetails[i].contexts().isEmpty())
        {
            title = hbTrId("txt_phob_formlabel_address");
        }
        else
        {
            if ( addressDetails[i].contexts().at(0) == contextHome )
            {
                sourceAddressType = CntMapTileService::AddressHome;
                title = hbTrId("txt_phob_formlabel_address_home");
            }
            else if (addressDetails[i].contexts().at(0) == contextWork)
            {
                sourceAddressType = CntMapTileService::AddressWork;
                title = hbTrId("txt_phob_formlabel_address_work");
            }
        }
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(title, itemCount(), false);
        
        QStringList address;
        if (!addressDetails[i].street().isEmpty())
            address.append(addressDetails[i].street());
        if (!addressDetails[i].postcode().isEmpty())
            address.append(addressDetails[i].postcode());
        if (!addressDetails[i].locality().isEmpty())
            address.append(addressDetails[i].locality());
        if (!addressDetails[i].region().isEmpty())
            address.append(addressDetails[i].region());
        if (!addressDetails[i].country().isEmpty())
            address.append(addressDetails[i].country());
                
        dataItem->setValueText(address.join(" "));
        dataItem->setContactDetail(addressDetails[i]);
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
        
        //Check whether location feature enabled
        if (mLocationFeatureEnabled)
        {
            QContactLocalId contactId = mContact->id().localId();
         
            //Get the maptile image path
            QString imageFile = CntMapTileService::getMapTileImage(contactId, sourceAddressType);
        
		    if ( !imageFile.isNull() )
		    {   
		        //Insert the imagepath in data container
		        QVariantList maptileImage;
                maptileImage.append(QString());
                maptileImage.append(QString(" "));
                maptileImage.append(QString(" "));
    
                //Display the maptile image
                HbIcon icon(imageFile);
                QIcon mapTileIcon;
                
                QPainter painter;
                QPixmap baloon( HbIcon("qtg_small_location").pixmap() );                
                QPixmap map(icon.pixmap());

                //Display pin image in the center of maptile image
                painter.begin(&map);
                painter.drawPixmap( ( map.width()/2 ) - ( baloon.width()/ 2 ), 
                               (( map.height()/2 )-( baloon.height())), baloon );
               
                painter.end();
                mapTileIcon.addPixmap(map);
                                
                addSeparator(itemCount());
                
                CntContactCardDataItem* dataItem = new CntContactCardDataItem(QString(), itemCount(), false);
                dataItem->setIcon(HbIcon(mapTileIcon));
                mDataItemList.insert(itemCount(), dataItem);
		    }
        }
    } 
    
    //company
    QList<QContactOrganization> organizationDetails = mContact->details<QContactOrganization>();
    for (int i = 0; i < organizationDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_company_details"), itemCount(), false);
        QStringList companyList;
        companyList << organizationDetails[i].title() << organizationDetails[i].name() << organizationDetails[i].department();
        dataItem->setValueText(companyList.join(" ").trimmed());
        dataItem->setContactDetail(organizationDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
    }
            
    //birthday
    QList<QContactBirthday> birthdayDetails = mContact->details<QContactBirthday>();
    for (int i = 0; i < birthdayDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_birthday"), itemCount(), false);
        dataItem->setValueText(birthdayDetails[i].date().toString("dd MMMM yyyy"));
        dataItem->setContactDetail(birthdayDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
    }

    //anniversary
    QList<QContactAnniversary> anniversaryDetails = mContact->details<QContactAnniversary>();
    for (int i = 0; i < anniversaryDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_anniversary"), itemCount(), false);
        dataItem->setValueText(anniversaryDetails[i].originalDate().toString("dd MMMM yyyy"));
        dataItem->setContactDetail(anniversaryDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
    }
    
    //ringing tone
    QList<QContactRingtone> ringtoneDetails = mContact->details<QContactRingtone>();
    for (int i = 0; i < ringtoneDetails.count(); i++)
    {
        if (!ringtoneDetails.at(i).audioRingtoneUrl().isEmpty())
        {
            CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_ringing_tone"), itemCount(), false);
            dataItem->setValueText(ringtoneDetails[i].audioRingtoneUrl().toString());
            dataItem->setContactDetail(ringtoneDetails[i]);  
            addSeparator(itemCount());
            mDataItemList.insert(itemCount(), dataItem);
            break;
        }
    }

    //note
    QList<QContactNote> noteDetails = mContact->details<QContactNote>();
    for (int i = 0; i < noteDetails.count(); i++)
    {
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_note"), itemCount(), false);
        dataItem->setValueText(noteDetails[i].note());
        dataItem->setContactDetail(noteDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataItem);
    }

    //family details
    QList<QContactFamily> familyDetails = mContact->details<QContactFamily>();
    for (int i = 0; i < familyDetails.count(); i++)
    {
        CntContactCardDataItem* dataSpouseItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_spouse"), itemCount(), false);
        dataSpouseItem->setValueText(familyDetails[i].spouse());
        dataSpouseItem->setContactDetail(familyDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataSpouseItem);
        
        CntContactCardDataItem* dataChildrenItem = new CntContactCardDataItem(hbTrId("txt_phob_formlabel_children"), itemCount(), false);
        dataChildrenItem->setValueText(familyDetails[i].children().join(", "));
        dataChildrenItem->setContactDetail(familyDetails[i]);  
        addSeparator(itemCount());
        mDataItemList.insert(itemCount(), dataChildrenItem);
    }
}

/*!
Returns true if contactDetails contains spesific action.
*/
bool CntContactCardDataContainer::supportsDetail(const QString &actionName, const QContactDetail &contactDetail)
{    
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    if (actionDescriptors.isEmpty())
    {
        return false;
    }
    
    QContactAction* contactAction = QContactAction::action(actionDescriptors.first());
    bool isSupportDetail = contactAction->isDetailSupported(contactDetail);
    
    delete contactAction;
    
    return isSupportDetail;
}

/*!
Returns the list of details which current action support.
*/
QList<QContactDetail> CntContactCardDataContainer::actionDetails(const QString &actionName, const QContact &contact)
{
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    if (actionDescriptors.isEmpty())
    {
       return QList<QContactDetail>();
    }
    
    QContactAction* contactAction = QContactAction::action(actionDescriptors.first()); 
    QList<QContactDetail> details = contactAction->supportedDetails(contact);

    delete contactAction;

    return details;
}

/*!
Returns the data for the given index
*/
CntContactCardDataItem* CntContactCardDataContainer::dataItem(int index) const
{
    if (index < 0)
    {
        return NULL;
    }
    else
    {
        return mDataItemList.at(index);
    }  
}

/*!
Set separator index and add separator item for the given index
*/
void CntContactCardDataContainer::addSeparator(int index)
{
    if (mSeparatorIndex == -1)
    {
        mSeparatorIndex = index;
        CntContactCardDataItem* dataItem = new CntContactCardDataItem(hbTrId("txt_phob_subtitle_details"), itemCount(), false);
        mDataItemList.insert(itemCount(), dataItem);
    }
}

/*!
Returns the amount of items in the container
*/
int CntContactCardDataContainer::itemCount() const
{
    return mDataItemList.count();
}

/*!
Sort data item list by position number
*/
void CntContactCardDataContainer::sortDataItems()
{
    qStableSort(mDataItemList.begin(), mDataItemList.end(), compareObjects);
}



