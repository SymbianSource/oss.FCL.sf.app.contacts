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
#include "cntfavourite.h"

QContactLocalId CntFavourite::createFavouriteGroup( QContactManager* aManager )
{
   QContactLocalId favouriteId = CntFavourite::favouriteGroupId(aManager);
    
   if ( favouriteId == 0 )
   {
       QContact favouriteGroup;
       favouriteGroup.setType( QContactType::TypeGroup );

       QContactName favouriteGroupName;
       favouriteGroupName.setCustomLabel( QLatin1String(FavouriteGroupName) );

       favouriteGroup.saveDetail( &favouriteGroupName );
       if ( aManager->saveContact( &favouriteGroup ) )
       {
           favouriteId = favouriteGroup.localId();
       }
   }
   return favouriteId;
}

QContactLocalId CntFavourite::favouriteGroupId( QContactManager* aManager )
{
    QContactLocalId favouriteId( 0 );
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = aManager->contactIds( groupFilter );
        
    for(int i = 0;i < groupContactIds.count();i++)
    {
        QContact contact = aManager->contact(groupContactIds.at(i));
        QContactName contactName = contact.detail<QContactName>();
        QString groupName = contactName.customLabel();
        if ( groupName.compare(QLatin1String(FavouriteGroupName)) == 0 )
        {
            favouriteId = groupContactIds.at(i);
            break;
        }
    }

    return favouriteId;
}

bool CntFavourite::isMemberOfFavouriteGroup( QContactManager* aManager, QContact* aContact )
{
    bool favouriteGroupContact( false );
    QContactLocalId favouriteId = CntFavourite::favouriteGroupId( aManager );
    if ( favouriteId != 0 )
    {
        QContact favoriteGroup = aManager->contact( favouriteId );
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType( QContactRelationship::HasMember );
        rFilter.setRelatedContactRole( QContactRelationship::First );
        rFilter.setRelatedContactId( favoriteGroup.id() );
            
        QList<QContactLocalId> contactsLocalIdList = aManager->contactIds( rFilter );
        favouriteGroupContact = contactsLocalIdList.contains( aContact->localId() );
    }
    return favouriteGroupContact;
}

void CntFavourite::addContactToFavouriteGroup( QContactManager* aManager, QContactId& aId)
{
    QContactLocalId favouriteId = CntFavourite::favouriteGroupId( aManager );
    if ( favouriteId == 0 )
    {
        favouriteId = CntFavourite::createFavouriteGroup( aManager );
    }
    
    QContact favoriteGroup = aManager->contact( favouriteId );
        
    // new contact added to the favorite group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst( favoriteGroup.id() );
    relationship.setSecond( aId );
    
    // save relationship
    aManager->saveRelationship( &relationship );
}

void CntFavourite::removeContactFromFavouriteGroup( QContactManager* aManager, QContactId& aId )
{
    QContactLocalId favouriteId = CntFavourite::favouriteGroupId( aManager );
    if ( favouriteId != 0 )
    {
        QContact favoriteGroup = aManager->contact( favouriteId );
        QContactRelationship relationship;
        relationship.setRelationshipType( QContactRelationship::HasMember );
        relationship.setFirst( favoriteGroup.id() );
        relationship.setSecond( aId );
        aManager->removeRelationship(relationship);
    }
}
// End of File
