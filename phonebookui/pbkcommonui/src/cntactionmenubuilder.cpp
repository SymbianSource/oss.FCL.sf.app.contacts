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
#include "cntactionmenubuilder.h"
#include <qcontact.h>
#include "cntstringmapper.h"
#include <hbaction.h>

CntActionMenuBuilder::CntActionMenuBuilder( QContactLocalId aMyCardId ) : 
QObject(),
iMyCardId( aMyCardId ),
mContact( 0 ),
mMap( 0 )
    {
    mMap = new CntStringMapper();
    }

CntActionMenuBuilder::~CntActionMenuBuilder()
    {
    delete mMap;
    mMap = 0;
    
    delete mContact;
    mContact = 0;
    }
    
HbMenu* CntActionMenuBuilder::buildActionMenu( QContact& aContact )
    {
    HbMenu* menu = new HbMenu();
    
    // Regular contact, NOT MyCard
    if ( aContact.localId() != iMyCardId )
        {
        QList<QContactActionDescriptor> actionDescriptors = aContact.availableActions();
        QStringList actions;
        foreach ( QContactActionDescriptor d, aContact.availableActions() )
            {
            actions << d.actionName();
            }
        
        if ( actions.contains("call", Qt::CaseInsensitive) )
            {
            createCallAction( *menu, aContact );
            }
        
        if ( actions.contains("message", Qt::CaseInsensitive) )
            {
            createSmsAction( *menu, aContact );
            }
        
        if ( actions.contains("email", Qt::CaseInsensitive) )
            {
            createEmailAction( *menu, aContact );
            }
        
        if ( menu->actions().size() > 0 )
            menu->addSeparator();
        }
    
    // If contact is NOT MyCard OR MyCard is not empty (detail count is more than 4)
    if ( aContact.localId() != iMyCardId || aContact.details().size() > 4 )
        {
        menu->addAction(hbTrId("txt_common_menu_open"), this, SLOT(emitOpenContact()) );
        menu->addAction(hbTrId("txt_common_menu_edit"), this, SLOT(emitEditContact()) );
        menu->addAction(hbTrId("txt_common_menu_delete_contact"), this, SLOT(emitDeleteContact()));
        }
    return menu;
    }

void CntActionMenuBuilder::execActionMenu( QContact& aContact, QPointF aPoint )
{
    mContact = new QContact( aContact );
    HbMenu* menu = buildActionMenu( aContact );
    menu->exec( aPoint );
    delete menu;
}

void CntActionMenuBuilder::emitOpenContact()
{
    emit openContact( *mContact );
}

void CntActionMenuBuilder::emitEditContact()
{
    emit editContact( *mContact );
}

void CntActionMenuBuilder::emitDeleteContact()
{
    emit deleteContact( *mContact );
}

void CntActionMenuBuilder::emitCallContact()
{
    emit performContactAction( *mContact, "call" );
}

void CntActionMenuBuilder::emitSmsContact()
{
    emit performContactAction( *mContact, "message" );
}

void CntActionMenuBuilder::emitMailContact()
{
    emit performContactAction( *mContact, "email" );
}

void CntActionMenuBuilder::createCallAction( HbMenu& aMenu, QContact& aContact )
    {
    // Create call action
    QContactDetail detail = aContact.preferredDetail("call");
    QContactPhoneNumber number = detail.isEmpty() ? aContact.detail<QContactPhoneNumber>() : detail;
    
    QString detailName = mMap->getMappedDetail( number.subTypes().first() );
    if (!number.contexts().isEmpty())
        {
        detailName += hbTrId(" %1").arg(mMap->getMappedDetail( number.contexts().first()) );
        }
    aMenu.addAction( hbTrId("Call %1").arg(detailName), this, SLOT(emitCallContact()) );
    }

void CntActionMenuBuilder::createEmailAction( HbMenu& aMenu, QContact& aContact )
    {
    QContactDetail detail = aContact.preferredDetail("email");
    QContactEmailAddress email = detail.isEmpty() ? aContact.detail<QContactEmailAddress>() : detail;

    QString detailName = hbTrId("txt_phob_menu_email");
    if (!email.contexts().isEmpty())
        {
        detailName += hbTrId(" %1").arg(mMap->getMappedDetail(email.contexts().first()));
        }
     aMenu.addAction(hbTrId("Mail %1").arg(detailName), this, SLOT(emitMailContact()));
    }

void CntActionMenuBuilder::createSmsAction( HbMenu& aMenu, QContact& aContact )
    {
    Q_UNUSED( aContact )
    aMenu.addAction(hbTrId("txt_phob_menu_send_message"), this, SLOT(emitSmsContact()));
    }
    
// End of File
