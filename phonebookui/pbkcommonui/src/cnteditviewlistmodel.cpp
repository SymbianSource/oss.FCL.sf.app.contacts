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

#include "cnteditviewlistmodel.h"
#include "cntextensionmanager.h"
#include "cntuiextensionfactory.h"
#include "cnteditviewitemsupplier.h"
#include "cnteditviewdetailitem.h"
#include "cnteditviewitembuilder.h"
#include <cntviewparams.h>

CntEditViewListModel::CntEditViewListModel( QContact* aContact ) :
mContact( aContact )
{
    mLookupTable.insert( EPhonenumber,    -1 );
    mLookupTable.insert( EEmailAddress,   -1 );
    mLookupTable.insert( EAddressTemplate,-1 );
    mLookupTable.insert( EPluginItem,     -1 );
    mLookupTable.insert( EUrl,            -1 );
    mLookupTable.insert( ESeparator,      -1 );
    mLookupTable.insert( EAddressDetail,  -1 );
    mLookupTable.insert( ECompany,        -1 );
    mLookupTable.insert( EDate,           -1 );
    mLookupTable.insert( ERingingTone,    -1 );
    mLookupTable.insert( ENote,           -1 );
    mLookupTable.insert( EFamily,         -1 );
    mLookupTable.insert( ESynchronization,-1 );
    
    mLookupMap.insert( QContactPhoneNumber::DefinitionName,     EPhonenumber );
    mLookupMap.insert( QContactOnlineAccount::DefinitionName,   EPhonenumber );
    mLookupMap.insert( QContactEmailAddress::DefinitionName,    EEmailAddress );
    mLookupMap.insert( QContactAddress::DefinitionName,         EAddressDetail);
    mLookupMap.insert( QContactUrl::DefinitionName,             EUrl );
    mLookupMap.insert( QContactOrganization::DefinitionName,    ECompany);
    mLookupMap.insert( QContactBirthday::DefinitionName,        EDate );
    mLookupMap.insert( QContactAnniversary::DefinitionName,     EDate);
    mLookupMap.insert( QContactNote::DefinitionName,            ENote);
    mLookupMap.insert( QContactFamily::DefinitionName,          EFamily);
    
    mManager = new CntExtensionManager();
    mBuilder = new CntEditViewItemBuilder();
    mSeparator = new CntEditViewSeparator();
    
    refresh();
}

CntEditViewListModel::~CntEditViewListModel()
{
    qDeleteAll( mItemList );
    
    delete mBuilder;
    delete mManager;
}
 
int CntEditViewListModel::rowCount( const QModelIndex& aParent ) const
{
    Q_UNUSED( aParent );
    return mItemList.size();
}

QVariant CntEditViewListModel::data( const QModelIndex& aIndex, int aRole ) const
{
   if ( aIndex.row() >= 0 && aIndex.row() < mItemList.size() )
   {
       CntEditViewItem* item = mItemList.at( aIndex.row() );
       return item->data( aRole );
   }
    return QVariant();
}

Qt::ItemFlags CntEditViewListModel::flags(const QModelIndex& aIndex) const
{
    CntEditViewItem* item = mItemList.at( aIndex.row() );
    QVariant data = item->data( Hb::ItemTypeRole );
    if ( data.toInt() == Hb::SeparatorItem )
    {
        return Qt::NoItemFlags;
    }
    
    return QAbstractListModel::flags( aIndex );
}

CntEditViewItem* CntEditViewListModel::itemAt( const QModelIndex& aIndex ) const
{
    return mItemList.at( aIndex.row() );
}

void CntEditViewListModel::removeItem( CntEditViewItem* aItem, const QModelIndex& aIndex )
{
    int index = mItemList.indexOf( aItem );
    if ( index >= 0 )
    {
        beginRemoveRows( aIndex.parent(), index, index );
        // remove item from item list
        int count = mItemList.count();
        CntEditViewItem* item = mItemList.takeAt( index );
        count = mItemList.count();
        
        // get detailed information
        QContactDetail detail = item->data(ERoleContactDetail).value<QContactDetail>();
        QStringList fields = item->data(ERoleContactDetailFields).toStringList();
        
        // remove the detail from QContact
        mBuilder->removeDetail( *mContact, detail, fields );
        
        // Update lookup table. Note, in case of QContactAddress,
        // we can't remove address template, so the mapping for address always points to address detail
        KLookupKey lookupKey = mLookupMap.value( detail.definitionName() );
        removeItem( lookupKey );
        endRemoveRows();
        
        // Remove separator item if needed
        if (mItemList.last()->data( Hb::ItemTypeRole ) == QVariant(Hb::SeparatorItem))
        {
            int separatorIndex = mItemList.indexOf( mSeparator );
            beginRemoveRows( aIndex.parent(), separatorIndex, separatorIndex );
            mItemList.removeAt( mItemList.indexOf(mSeparator) );
            removeItem( ESeparator  );
            endRemoveRows();
        }
        
        
        // Check if the removed item is -1 in lookuptable and if it needs a template
        int lookupValue = mLookupTable.value( lookupKey );
        if ( lookupValue == -1 )
        {
            beginResetModel();
            
            if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
                insertItem( EPhonenumber, mBuilder->phoneNumberItems(*mContact) );
            
            else if ( detail.definitionName() == QContactEmailAddress::DefinitionName )
                insertItem( EEmailAddress, mBuilder->emailAddressItems(*mContact) );
            
            else if ( detail.definitionName() == QContactAddress::DefinitionName )
                insertItem( EAddressTemplate, mBuilder->addressItems(*mContact) );
            
            else if ( detail.definitionName() == QContactUrl::DefinitionName )
                insertItem( EUrl, mBuilder->urlItems(*mContact) );
            
            endResetModel();
        }
        delete item;
    }
}

void CntEditViewListModel::refreshExtensionItems( const QModelIndex& aIndex )
{
    beginResetModel();
    // remove all extension items
    for( int i(mItemList.count()-1); i >= 0; i-- )
    {
        CntEditViewItem* item = mItemList.at( i );
        if ( item->data(ERoleItemType) == QVariant(ETypeUiExtension) )
        {
            mItemList.removeAt(i);
            removeItem( EPluginItem );
        }
    }
    // query extension items again
    int count = mManager->pluginCount();
    for ( int i(0); i < count; i++ )
    {
        CntUiExtensionFactory* factory = mManager->pluginAt(i);
        CntEditViewItemSupplier* supplier = factory->editViewItemSupplier( *mContact );
        if (supplier)
        {
            loadPluginItems( supplier );
        }
    }
    endResetModel();
}

void CntEditViewListModel::refresh()
{
    beginResetModel();
    
    insertItem( EPhonenumber, mBuilder->phoneNumberItems(*mContact) );
    insertItem( EEmailAddress, mBuilder->emailAddressItems(*mContact) );
    insertItem( EAddressTemplate, mBuilder->addressItems(*mContact) );
    insertItem( EUrl, mBuilder->urlItems(*mContact) );
        
    int count = mManager->pluginCount();
    for ( int i(0); i < count; i++ )
    {
        CntUiExtensionFactory* factory = mManager->pluginAt(i);
        CntEditViewItemSupplier* supplier = factory->editViewItemSupplier( *mContact );
        if (supplier)
        {
            loadPluginItems( supplier );
        }
    }
        
    insertDetailItem( EAddressDetail, mBuilder->addressDetails(*mContact) );
    insertDetailItem( ECompany, mBuilder->companyDetails(*mContact) );
    insertDetailItem( EDate, mBuilder->dateDetails(*mContact) );
    insertDetailItem( ENote, mBuilder->noteDetails(*mContact) );
    insertDetailItem( EFamily, mBuilder->familyDetails(*mContact) );
    
    endResetModel();
}

bool CntEditViewListModel::isEmptyItem( CntEditViewItem* aItem )
{
    QContactDetail d = aItem->data( ERoleContactDetail ).value<QContactDetail>();
    QStringList fields = aItem->data( ERoleContactDetailFields ).toStringList();
    
    foreach ( QString field, fields )
    {
        if ( !d.value(field).isEmpty() )
        {
            return false;
        }
    }
    return true;
}

void CntEditViewListModel::loadPluginItems( CntEditViewItemSupplier* aSupplier )
{
    QList<CntEditViewItem*> list;
    int count = aSupplier->itemCount();
    for ( int i(0); i < count; i++ )
    {
        CntEditViewItem* item = aSupplier->itemAt( i );
        if ( item )
        {
            list << item;
        }
    }
    
    if ( !list.isEmpty() )
        insertItem( EPluginItem, list );
}

void CntEditViewListModel::insertDetailItem( KLookupKey aKey, QList<CntEditViewItem*> aList )
{
    if ( !aList.isEmpty() )
    {
        insertSeparator();
        insertItem( aKey, aList );
    }
}

void CntEditViewListModel::insertItem( KLookupKey aLookupKey, QList<CntEditViewItem*> aList )
{
    if ( !aList.isEmpty() )
    {
        QList<KLookupKey> keys = mLookupTable.keys();
        int ind = keys.indexOf( aLookupKey );
        for ( int i=ind; i >= 0; i-- )
        {
            KLookupKey key = keys.at( i );
            int lastIndexValue = mLookupTable.value( key );
            // search next suitable "lastindex" where to insert the items
            if ( i != 0 && lastIndexValue == -1 )
                continue;
                   
            // insert items to current index
            for ( int j(0); j < aList.count(); j++ )
            {
                mItemList.insert( lastIndexValue + j + 1, aList[j] );
            }
                   
            // lookup keys value "lastindex"
            int listCount = aList.count();
            lastIndexValue = lastIndexValue + listCount;
            mLookupTable.insert( aLookupKey, lastIndexValue );
            // update all indexes in lookuptable 
            for ( int k(ind+1); k < keys.size(); k++ )
            {
                int value = mLookupTable.value(keys[k]);
                if ( value != -1 )
                {
                    mLookupTable.insert( keys[k], value + aList.count() );
                }
            }
            break;
        }
    }
}

void CntEditViewListModel::removeItem( KLookupKey aKey )
{
    QList<KLookupKey> keys = mLookupTable.keys();
    int ind = keys.indexOf( aKey );
    for ( int i(ind); i < keys.count(); i++ )
    {
        KLookupKey key = keys.at( i );
        int lastIndexValue = mLookupTable.value( key ) - 1;
        
        // climb the keys (bottom to top) and see if somebody has the same value (then set value to -1)
        for ( int j(ind); j >= 0; j-- )
        {
            if ( mLookupTable.value(keys[j]) == lastIndexValue )
            {
                lastIndexValue = -1;
                break;
            }
        }
        
        mLookupTable.insert( key, lastIndexValue );
        // update rest of the keys by reducing one (top to bottom)
        for ( int k(ind+1); k < keys.count(); k++ )
        {
            KLookupKey tmp = keys.at( k );
            int value = mLookupTable.value( tmp );
            if ( value != -1 )
            {
                mLookupTable.insert( tmp, value - 1 );
            }
        }
        
        break;
    }
}

void CntEditViewListModel::insertSeparator()
{
    if ( mItemList.indexOf(mSeparator) == -1 )
    {
        QList<CntEditViewItem*> list;
        list << mSeparator;
        insertItem( ESeparator, list );
    }
}

// End of File
