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
#include "mobcntaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>
#include <QTimer>
#include <QDebug>

MobCntAction::MobCntAction(const QString &actionName) :
        m_actionName(actionName),
        m_vendorName("symbian"),
        m_implementationVersion(1),
        m_result(), 
        m_contact(),
        m_detail()
{
}
        
MobCntAction::~MobCntAction()
{
}

QVariantMap MobCntAction::metaData() const
{
    return QVariantMap();
}

QContactFilter MobCntAction::contactFilter(const QVariant& value) const
{
    Q_UNUSED(value);
    
    QContactUnionFilter unionFilter;
   
    QContactDetailFilter landlineFilter;
    landlineFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    landlineFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    unionFilter << landlineFilter;
    
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    unionFilter << mobileFilter;
        
    QContactDetailFilter pagerFilter;
    pagerFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    pagerFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypePager));
    unionFilter << pagerFilter;
        
    QContactDetailFilter voiceFilter;
    voiceFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    voiceFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeVoice));
    unionFilter << voiceFilter;
        
    QContactDetailFilter modemFilter;
    modemFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    modemFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeModem));
    unionFilter << modemFilter;
        
    QContactDetailFilter videoFilter;
    videoFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    videoFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeVideo));
    unionFilter << videoFilter;
        
    QContactDetailFilter carFilter;
    carFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    carFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeCar));
    unionFilter << carFilter;
        
    QContactDetailFilter bulletinBoardFilter;
    bulletinBoardFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    bulletinBoardFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    unionFilter << bulletinBoardFilter;
	
    return unionFilter;
}

//virtual function, common code for call, videocall and message actions
bool MobCntAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactPhoneNumber::DefinitionName);
}

QList<QContactDetail> MobCntAction::supportedDetails(const QContact& /*contact*/) const
{
    return QList<QContactDetail>();
}

QContactActionDescriptor MobCntAction::actionDescriptor() const
{
    QContactActionDescriptor ret;
    ret.setActionName(m_actionName);
    ret.setVendorName(m_vendorName);
    ret.setImplementationVersion(m_implementationVersion);
    return ret;
}


bool MobCntAction::invokeAction(const QContact& contact, const QContactDetail& detail, const QVariantMap& /*parameters*/)
{
	m_contact = contact;
	m_detail  = detail;
	
	QTimer::singleShot(1, this, SLOT(performAction()));
	m_state = QContactAction::ActiveState;
	return true;
}

QContactAction::State MobCntAction::state() const
{
    return m_state;
}

QVariantMap MobCntAction::results() const
{
    return m_result;
}

//Clears the action data, is called after the result has been emitted to contact (emitResult function)
void MobCntAction::resetAction()
{
	m_contact = QContact(); 
	m_detail  = QContactDetail();
}

//returns whether the actionDescription is supported by this action
bool MobCntAction::actionDescriptionSupported(const QContactActionDescriptor& descriptor) const
{
    bool supported(false);
    
    if ((descriptor.actionName() == m_actionName) && (descriptor.vendorName() == m_vendorName) && (descriptor.implementationVersion() == m_implementationVersion))
        supported = true;
    else
        supported = false; 
    
    return supported;       
}

//common code to perform a call, videocall and message action
void MobCntAction::performNumberAction(const QString &service, const QString &type)
{
	XQServiceRequest snd(service, type, false);
	
	QVariant retValue;
	
	//detail exist use it
	if (m_detail.definitionName() == QContactPhoneNumber::DefinitionName)
	{
		const QContactPhoneNumber &phoneNumber = static_cast<const QContactPhoneNumber &>(m_detail);
		
		snd << phoneNumber.number() << m_contact.localId() << m_contact.displayLabel();
	
		emitResult(snd.send(retValue), retValue);
	}
	
	//if no detail, pick preferred
	else if(m_detail.isEmpty())
	{
		QContactDetail detail = m_contact.preferredDetail(m_actionName);
		QContactPhoneNumber phoneNumber;
	
		//if preferred is empty pick first phonenumber
		if(detail.isEmpty())
		{
			phoneNumber = m_contact.detail<QContactPhoneNumber>();
		}
		
		//if not empty, cast detail to phonenumber
		else
		{
			phoneNumber = static_cast<QContactPhoneNumber>(detail);
		}
		
		snd << phoneNumber.number() << m_contact.localId() << m_contact.displayLabel();
				 
		emitResult(snd.send(retValue), retValue);
	}
	
	//else return an error
	else
	{
		emitResult(GeneralError, retValue);
	}
}

//emit the result to the client
void MobCntAction::emitResult(int errorCode, const QVariant &retValue)
{
	m_result.clear();
	m_result.insert("Error", QVariant(errorCode));
	m_result.insert("ReturnValue", retValue);
	
	if (errorCode == 0){
		m_state = QContactAction::FinishedState;
	}
	
	else{
		m_state = QContactAction::FinishedWithErrorState;
	}
	
	//emit progress(state, m_result);
	emit stateChanged(m_state);
	emit resultsAvailable();

	resetAction(); //reset values in the action
}
