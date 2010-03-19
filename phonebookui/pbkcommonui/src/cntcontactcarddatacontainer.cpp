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

#include <qtcontacts.h>
#include <hbicon.h>

#include "cntstringmapper.h"

/*!
Constructor
*/
CntContactCardDataContainer::CntContactCardDataContainer(QContact* contact, QObject *parent) : mContact(contact), mSeparatorIndex(-1)
{
    Q_UNUSED(parent);
    mDataPointer = new CntContactCardContainerData;
    //Non standard types needs to be registered before they can be used by QVariant
    qRegisterMetaType<QContactDetail>();
    
    if (contact->type() == QContactType::TypeGroup)
    {
        initializeGroupData();
    }
    else
    {
        initializeData();
        initializeDetailsData();
    }
}

/*!
Destructor
*/
CntContactCardDataContainer::~CntContactCardDataContainer()
{

}

/*!
Initialize contact details which include actions.
*/
void CntContactCardDataContainer::initializeData()
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
        QString context;
        if (details[i].contexts().count())
        {
            context = mStringMapper.getMappedDetail(details[i].contexts().at(0));
        }
        
        if (availableActions.contains("call", Qt::CaseInsensitive) && supportsDetail("call", details[i]) && details[i].definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber number(details.at(i));
            QVariantList dataList;
            //type
            dataList.append("call");
            //name
            QStringList list;
            list << mStringMapper.getMappedDetail(number.subTypes().at(0)) << context;
            QString name = list.join(" ");
            dataList.append(qtTrId("Call %1").arg(name.trimmed()));
            //data
            dataList.append(number.number());
            //icon
            dataList.append(mStringMapper.getMappedLauncherIcon(number.subTypes().at(0)));
            //detail
            QContactDetail detail(number);
            QVariant var;
            var.setValue(detail);
            dataList.append(var);
            mDataPointer->mDataList.insert(rowCount(), dataList);
        }
       
       if (availableActions.contains("message", Qt::CaseInsensitive) && supportsDetail("message", details[i]) && details[i].definitionName() == QContactPhoneNumber::DefinitionName)
       {
           QContactPhoneNumber number(details.at(i));
           QVariantList dataList;
           //type
           dataList.append("message");
           //name
           dataList.append(hbTrId("txt_phob_menu_send_message"));
           //data
           dataList.append(number.number());
           //icon
           dataList.append(":/icons/qtg_large_message.svg");
           //detail
           QContactDetail detail(number);
           QVariant var;
           var.setValue(detail);
           dataList.append(var);
           mDataPointer->mDataList.insert(rowCount(), dataList);
       }    
    }
    
    //email
    if (availableActions.contains("email", Qt::CaseInsensitive))
    {
        QList<QContactDetail> details = actionDetails("email", *mContact);
        for (int i = 0; i < details.count(); i++)
        {
            QString context;
            if (details[i].contexts().count())
            {
                context = mStringMapper.getMappedDetail(details[i].contexts().at(0));
            }
            
            if (details[i].definitionName() == QContactEmailAddress::DefinitionName)
            {
                QContactEmailAddress email(details.at(i));
                QVariantList dataList;
                //action
                dataList.append("email");
                //name
                QStringList list;
                list << mStringMapper.getMappedDetail(email.definitionName()) << context;
                dataList.append(qtTrId("Mail %1").arg(list.join(" ").trimmed()));
                //data
                dataList.append(email.emailAddress());
                //icon
                dataList.append(":/icons/qtg_large_email.svg");
                //detail
                QContactDetail detail(email);
                QVariant var;
                var.setValue(detail);
                dataList.append(var);
                mDataPointer->mDataList.insert(rowCount(), dataList);
            }
        }
    }
}

/*!
Initialize group details which includes actions.
*/
void CntContactCardDataContainer::initializeGroupData()
{
    // get the conference number
    
    QContactPhoneNumber confCallNumber = mContact->detail<QContactPhoneNumber>();
        
    QVariantList callDataList;
    //type
    callDataList.append("call");
    //name
    callDataList.append(hbTrId("txt_phob_dblist_conference_call"));
    //data
    callDataList.append(confCallNumber.number());
    //icon
    callDataList.append(":/icons/qtg_large_call_group.svg");
    //detail
    QContactDetail detail(confCallNumber);
    QVariant var;
    var.setValue(detail);
    callDataList.append(var);
    if(confCallNumber.number() != NULL)
        {
        mDataPointer->mDataList.insert(rowCount(), callDataList);
        }
    
    QVariantList messageDataList;
    //type
    messageDataList.append("message");
    //name
    messageDataList.append(hbTrId("txt_phob_list_send_group_message"));
    //data
    messageDataList.append(confCallNumber.number());
    //icon
    messageDataList.append(":/icons/qtg_large_message.svg");
    //detail
    QContactDetail messageDetail(confCallNumber);
    QVariant messageVar;
    messageVar.setValue(messageDetail);
    messageDataList.append(messageVar);
    mDataPointer->mDataList.insert(rowCount(), messageDataList);
    
    QVariantList emailDataList;
    //type
    emailDataList.append("email");
    //name
    emailDataList.append(hbTrId("txt_phob_list_send_group_mail"));
    //data
    emailDataList.append(confCallNumber.number());
    //icon
    emailDataList.append(":/icons/qtg_large_email.svg");
    //detail
    QContactDetail emailDetail(confCallNumber);
    QVariant emailVar;
    emailVar.setValue(emailDetail);
    emailDataList.append(emailVar);
    mDataPointer->mDataList.insert(rowCount(), emailDataList);
  
}

/*!
Initialize contact details which not include actions.
*/
void CntContactCardDataContainer::initializeDetailsData()
{
    //address
    QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
    for (int i = 0; i < addressDetails.count(); i++)
    {
        QVariantList addressList;
        //no action
        addressList.append(QString());
        if (addressDetails[i].contexts().isEmpty())
        {
            addressList.append(hbTrId("txt_phob_formlabel_address"));
        }
        else
        {
            addressList.append(hbTrId("Address (%1):").arg(addressDetails[i].contexts().at(0)));
        }
        QStringList address;
        
        if (!addressDetails[i].postOfficeBox().isEmpty())
            address.append(addressDetails[i].postOfficeBox());
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
        
        addressList.append(address.join(" "));
        //no icon
        addressList.append(QString());
        //detail
        QContactDetail detail(addressDetails[i]);
        QVariant var;
        var.setValue(detail);
        addressList.append(var);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), addressList);
    } 
    
    //birthday
    QList<QContactBirthday> birthdayDetails = mContact->details<QContactBirthday>();
    for (int i = 0; i < birthdayDetails.count(); i++)
    {
        QVariantList dateList;
        //no action
        dateList.append(QString());
        dateList.append(hbTrId("txt_phob_formlabel_birthday"));
        dateList.append(birthdayDetails[i].date().toString("dd MMMM yyyy"));
        //no icon
        dateList.append(QString());
        //detail
        QContactDetail detail(birthdayDetails[i]);
        QVariant var;
        var.setValue(detail);
        dateList.append(var);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), dateList);
    }

    //anniversary
    QList<QContactAnniversary> anniversaryDetails = mContact->details<QContactAnniversary>();
    for (int i = 0; i < anniversaryDetails.count(); i++)
    {
        QVariantList dateList;
        //no action
        dateList.append(QString());
        dateList.append(hbTrId("txt_phob_formlabel_anniversary"));
        dateList.append(anniversaryDetails[i].originalDate().toString("dd MMMM yyyy"));
        //no icon
        dateList.append(QString());
        //detail
        QContactDetail detail(anniversaryDetails[i]);
        QVariant var;
        var.setValue(detail);
        dateList.append(var);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), dateList);
    }
    
    //ringing tone
    QList<QContactAvatar> ringtoneDetails = mContact->details<QContactAvatar>();
    for (int i = 0; i < ringtoneDetails.count(); i++)
    {
        if (ringtoneDetails.at(i).subType() == QContactAvatar::SubTypeAudioRingtone)
        {
            QVariantList dataList;
            //no action
            dataList.append(QString());
            dataList.append(hbTrId("Ringtone:"));
            dataList.append(ringtoneDetails[i].avatar());
            //no icon
            dataList.append(QString());
            //detail
            QContactDetail detail(ringtoneDetails[i]);
            QVariant var;
            var.setValue(detail);
            dataList.append(var);
            addSeparator(rowCount());
            mDataPointer->mDataList.insert(rowCount(), dataList);
            break;
        }
    }

    //note
    QList<QContactNote> noteDetails = mContact->details<QContactNote>();
    for (int i = 0; i < noteDetails.count(); i++)
    {
        QVariantList noteList;
        //no action
        noteList.append(QString());
        noteList.append(hbTrId("txt_phob_formlabel_note2"));
        noteList.append(noteDetails[i].note());
        //no icon
        noteList.append(QString());
        //detail
        QContactDetail detail(noteDetails[i]);
        QVariant var;
        var.setValue(detail);
        noteList.append(var);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), noteList);
    }

    //family details
    QList<QContactFamily> familyDetails = mContact->details<QContactFamily>();
    for (int i = 0; i < familyDetails.count(); i++)
    {
        // Spouse
        QVariantList spouseList;
        //no action
        spouseList.append(QString());
        spouseList.append(hbTrId("txt_phob_formlabel_spouse"));
        spouseList.append(familyDetails[i].spouse());
        //no icon
        spouseList.append(QString());
        //detail
        QContactDetail spouseDetail(familyDetails[i]);
        QVariant varSpouse;
        varSpouse.setValue(spouseDetail);
        spouseList.append(varSpouse);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), spouseList);

        // Children
        QVariantList childrenList;
        //no action
        childrenList.append(QString());
        childrenList.append(hbTrId("txt_phob_formlabel_children"));
        childrenList.append(familyDetails[i].children().join(", "));
        //no icon
        childrenList.append(QString());
        //detail
        QContactDetail childrenDetail(familyDetails[i]);
        QVariant varChild;
        varChild.setValue(childrenDetail);
        childrenList.append(varChild);
        addSeparator(rowCount());
        mDataPointer->mDataList.insert(rowCount(), childrenList);
    }
}

/*!
Returns true if contactDetails contains spesific action.
*/
bool CntContactCardDataContainer::supportsDetail(const QString &actionName, const QContactDetail &contactDetail)
{    
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    QContactAction* contactAction = QContactAction::action(actionDescriptors.at(0));
    
    bool isSupportDetail = contactAction->supportsDetail(contactDetail);
    
    delete contactAction;
    
    return isSupportDetail;
}

/*!
Returns the list of details which current action support.
*/
QList<QContactDetail> CntContactCardDataContainer::actionDetails(const QString &actionName, const QContact &contact)
{
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    QContactAction* contactAction = QContactAction::action(actionDescriptors.at(0));

    QList<QContactDetail> details = contactAction->supportedDetails(contact);

    delete contactAction;

    return details;
}

/*!
Returns the data for the given index with a role
*/
QVariant CntContactCardDataContainer::data(int index, int role) const
{
    if (index < 0)
        return QVariant();
    
    QVariantList values = mDataPointer->mDataList.at(index);
    
    if (index == mSeparatorIndex)
    {
         if (role == Qt::DisplayRole)
         {
             return QVariant(values[action].toString());
         }
         return QVariant();
    }
    
    if (role == Qt::DisplayRole)
    {
        QStringList list;
        QString field = values[text].toString();
        if(mContact->isPreferredDetail(values[action].toString(), values[detail].value<QContactDetail>()))
        {
            field.prepend("<u>");
            field.append("</u>");
        }
        QString value = values[valueText].toString();
        list << field << value;
        
        return QVariant(list);
    }
     
    else if (role == Qt::DecorationRole)
    {
        if (!values[icon].toString().isEmpty())
        {
            QList<QVariant> icons;
            HbIcon itemIcon(values[icon].toString());
            icons.append(itemIcon);
            return QVariant(icons);
        }
    }
    
    else if (role == Qt::UserRole+1)
    {
        //TODO: Find out better solution!!!
        QMap<QString, QVariant> map;
      
        map.insert("action", values[action]);
        map.insert("name", values[text]);
        map.insert("data", values[valueText]);
        map.insert("icon", values[icon]);
        map.insert("detail", values[detail]);
        return map;
    }

    return QVariant();
}

/*!
Set separator index and add separator item for the given index
*/
void CntContactCardDataContainer::addSeparator(int index)
{
    if (mSeparatorIndex == -1)
    {
        mSeparatorIndex = index;
        QVariantList separatorList;
        separatorList.append(hbTrId("txt_phob_subtitle_details"));
        mDataPointer->mDataList.insert(rowCount(), separatorList);
    }
}

/*!
Returns the amount of rows in the container
*/
int CntContactCardDataContainer::rowCount() const
{
    return mDataPointer->mDataList.count();
}

/*!
Called when a detail is set as preferred, model needs to be refreshed
*/
void CntContactCardDataContainer::preferredUpdated()
{
    //emit layoutChanged();
}
