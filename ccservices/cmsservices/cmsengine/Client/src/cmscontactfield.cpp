/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDES
#include "cmscontact.h"
#include "cmssession.h"
#include "cmspresencedata.h"
#include "cmscontactfield.h"
#include "cmscontactfielditem.h"
#include "cmscommondefines.h"

// ----------------------------------------------------
// CCmsContactField::CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactField::CCmsContactField( RCmsContact& aContact,
                                    CCmsContactFieldItem::TCmsContactField aFieldType ) :
                                    CCmsContactBase( aContact ),
                                    iFieldType( aFieldType )
    {
    SelectFieldType();
    }

// ----------------------------------------------------
// CCmsContactField::CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactField::CCmsContactField( RCmsContact& aContact, TRequestStatus& aClientStatus,
                                    CCmsContactFieldItem::TCmsContactField aFieldType ) :
                                    CCmsContactBase( aContact, aClientStatus ),
                                    iFieldType( aFieldType )
    {
    SelectFieldType();
    }

// ----------------------------------------------------
// CCmsContactField::CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactField::CCmsContactField( RCmsContact& aContact, TRequestStatus& aClientStatus,
                                    CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup ) :
                                    CCmsContactBase( aContact, aClientStatus ),
                                    iFieldGroup( aFieldGroup )
    {
    SelectFieldType();
    }

// ----------------------------------------------------
// CCmsContactField::NewL
// 
// ----------------------------------------------------
//
CCmsContactField* CCmsContactField::NewL( RCmsContact& aContact,
                                          CCmsContactFieldItem::TCmsContactField aFieldType )
    {
    CCmsContactField* self = new ( ELeave ) CCmsContactField( aContact, aFieldType );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsContactField::NewL
// 
// ----------------------------------------------------
//
CCmsContactField* CCmsContactField::NewL( RCmsContact& aContact, TRequestStatus& aClientStatus,
                                          CCmsContactFieldItem::TCmsContactField aFieldType )
    {
    CCmsContactField* self = new ( ELeave ) CCmsContactField( aContact, aClientStatus, aFieldType );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsContactField::NewL
// 
// ----------------------------------------------------
//
CCmsContactField* CCmsContactField::NewL( RCmsContact& aContact, TRequestStatus& aClientStatus,
                                          CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup )
    {
    CCmsContactField* self = new ( ELeave ) CCmsContactField( aContact, aClientStatus, aFieldGroup );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
		
// ----------------------------------------------------
// CCmsContactField::ConstructL
// 
// ----------------------------------------------------
//		
void CCmsContactField::ConstructL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::ConstructL()" ) );
    #endif
    }

// ----------------------------------------------------
// CCmsContactField::SelectFieldType
// 
// ----------------------------------------------------
//		
void CCmsContactField::SelectFieldType()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::SelectFieldType()" ) );
    #endif
    iBinary = CCmsContactFieldItem::ECmsThumbnailPic == iFieldType ||
              CCmsContactFieldItem::ECmsPresenceData == iFieldType ||
              CCmsContactFieldItem::ECmsBrandedAvailability == iFieldType;
    }
		
// ----------------------------------------------------
// CCmsContactField::~CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactField::~CCmsContactField()
    {
    iFieldItems.ResetAndDestroy();
    iFieldItems.Close();
    }

// ----------------------------------------------------
// CCmsContactField::FieldCount
// 
// ----------------------------------------------------
//
EXPORT_C TInt CCmsContactField::ItemCount() const
    {
    return iFieldItems.Count();
    }

// ----------------------------------------------------
// CCmsContactField::Item
// 
// ----------------------------------------------------
//
EXPORT_C const CCmsContactFieldItem& CCmsContactField::ItemL( TInt aIndex ) const
    {
    CCmsContactFieldItem* item = NULL;
    if( aIndex >= 0 && aIndex < iFieldItems.Count() )
        {
        item = iFieldItems[aIndex];
        }
    else
        {
        User::Leave( KErrTooBig );
        }
    return *item;
    }

// ----------------------------------------------------
// CCmsContactField::Item
// 
// ----------------------------------------------------
//
EXPORT_C const CCmsContactFieldItem& CCmsContactField::ItemL( CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType ) const
    {
    CCmsContactFieldItem* result = NULL;
    
    TInt count = iFieldItems.Count();
    for( TInt a = 0; a < count; a++ )
        {
        CCmsContactFieldItem* item = iFieldItems[a];
        if(item->HasDefaultAttribute(aDefaultType))
            {
            result = item;
            break;                
            }        
        }

    if(NULL == result)
        User::Leave(KErrNotFound);
        
    return *result;
    }



// ----------------------------------------------------
// CCmsContactField::Items
// 
// ----------------------------------------------------
//
EXPORT_C const RPointerArray<CCmsContactFieldItem>& CCmsContactField::Items() const
    {   
    return iFieldItems;
    }

// ----------------------------------------------------
// CCmsContactField::Type
// 
// ----------------------------------------------------
//
EXPORT_C CCmsContactFieldItem::TCmsContactField CCmsContactField::Type() const
    {
    return iFieldType;
    }

// ----------------------------------------------------
// CCmsContactField::GroupType
// 
// ----------------------------------------------------
//
EXPORT_C CCmsContactFieldItem::TCmsContactFieldGroup CCmsContactField::GroupType() const
    {
    return iFieldGroup;
    }


// ----------------------------------------------------
// CCmsContactField::HasDefaultAttribute
// 
// ----------------------------------------------------
//
EXPORT_C TBool CCmsContactField::HasDefaultAttribute( CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType ) const
    {
    TBool found = EFalse;
        
    TInt count = iFieldItems.Count();
    for( TInt a = 0; !found && a < count; a++ )
        {
        CCmsContactFieldItem* item = iFieldItems[a];
        if(item->HasDefaultAttribute(aDefaultType))
            {
            found = ETrue;
            break;                
            }        
        }

    return found;
    }

// ----------------------------------------------------
// CCmsContactField::HasDefaultAttribute
// 
// ----------------------------------------------------
//
EXPORT_C TInt CCmsContactField::HasDefaultAttribute( ) const
    {
    TInt bitmask = 0;        
    TInt count = CCmsContactFieldItem::ECmsDefaultTypeUrl;
    
    while (count > 0)
        {
        if(HasDefaultAttribute((CCmsContactFieldItem::TCmsDefaultAttributeTypes)count))
            {
            bitmask |= count;
            }
        
        count >>= 1;
        }
    
    return bitmask;
    }



// ----------------------------------------------------
// CCmsContactField::RunL
// 
// ----------------------------------------------------
//
void CCmsContactField::RunL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::RunL() - Error: %d" ), iStatus.Int() );
    #endif
    if( KErrNone <= iStatus.Int() )
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Field data fetched OK." ) );
        #endif
        if( iBinary )
            {
            HandleBinaryDataL();
            } 
        else HandleSixteenBitDataL();
        }
    else  //An error
        {
        //Handle errors in the base class
        HandleError();
        }   
    }

// ----------------------------------------------------
// CCmsContactField::HandleSixteenBitDataL
// 
// ----------------------------------------------------
//
void CCmsContactField::HandleSixteenBitDataL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::HandleSixteenBitDataL()" ) );
    #endif
    if( KErrNone == iStatus.Int() )
        {
        for( TInt i = 0;iStreamDesc16.Length() > 0;i++ )
            {
            TInt length = 0;
            TInt infoLength = 0;
            TInt attribute = 0;
            TPtrC lengthDesc( _L( "" ) );
            
            //data length
            TInt index = iStreamDesc16.Locate( KDelimiterSpace );
            lengthDesc.Set( iStreamDesc16.Left( iStreamDesc16.Locate( KDelimiterSpace ) ) );
            TLex luthor( lengthDesc );
            User::LeaveIfError( luthor.Val( length ) );                        
            iStreamDesc16.Delete( 0, index + 1 );

            //info length
            index = iStreamDesc16.Locate( KDelimiterSpace );
            lengthDesc.Set( iStreamDesc16.Left( iStreamDesc16.Locate( KDelimiterSpace ) ) );
            TLex luthorInfo( lengthDesc );
            User::LeaveIfError( luthorInfo.Val( infoLength ) );                        
            iStreamDesc16.Delete( 0, index + 1 );            
            
            //default attributes
            index = iStreamDesc16.Locate( KDelimiterSpace );                                    
            TPtrC attributedata( iStreamDesc16.Left( iStreamDesc16.Locate( KDelimiterSpace )) );            
            TLex attl( attributedata );            
            User::LeaveIfError( attl.Val( attribute ) );
            
            //data                                    
            TPtrC data( iStreamDesc16.Mid( index + 1, length ) );
            TPtrC dataInfo( iStreamDesc16.Mid( index + length + 1, infoLength ) );
            CreateFieldItemL( data, dataInfo, attribute );
                        
            index > 0 ? iStreamDesc16.Delete( 0, index + 1 + length + infoLength ) :
                        iStreamDesc16.Delete( 0, iStreamDesc16.Length() );
                                  
            }
        User::RequestComplete( iClientStatus, KErrNone );   
        }
    else
        {
        ReAllocBufferL( iStatus.Int() );
        TIpcArgs args( iFieldType, StreamDesc16() );
        TInt operation( ( TInt )ECmsFetchContactField );
        iContact.ResendReceive( operation, Activate(), args );
        }   
    }

// ----------------------------------------------------
// CCmsContactField::HandleBinaryDataL
// 
// ----------------------------------------------------
//
void CCmsContactField::HandleBinaryDataL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::HandleBinaryDataL()" ) );
    #endif
    if( KErrNone == iStatus.Int() )
        {
        switch( iFieldType )
            {
            case CCmsContactFieldItem::ECmsThumbnailPic:
            case CCmsContactFieldItem::ECmsBrandedAvailability:
                HandleBasicDataL();
                break;
            case CCmsContactFieldItem::ECmsPresenceData:
                HandlePresenceDataL();
                break;
            }
        }
    else
        {
        ReAllocBufferL( iStatus.Int() );
        TIpcArgs args( iFieldType, StreamDesc8() );
        TInt operation( ( TInt )ECmsFetchContactField );
        iContact.ResendReceive( operation, Activate(), args );
        }   
    }

// ----------------------------------------------------
// CCmsContactField::HandleBasicDataL
// 
// ----------------------------------------------------
//
void CCmsContactField::HandleBasicDataL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::HandleBasicDataL()" ) );
    #endif
    _LIT8( KDelimiter, "\r\n\r\n" );
    TPtr8 package( *StreamDesc8() );
    TInt separator = package.Find( KDelimiter );
    if( KErrNone < separator )
        {
        while( 0 < package.Length() )
            {
            TPtrC8 data( package.Left( separator ) );
            CreateFieldItemL( data );
            package.Delete( 0, separator + 4 );
            separator = package.Find( KDelimiter );
            }
        }
    else
        {
        CreateFieldItemL( package );
        }
    User::RequestComplete( iClientStatus, KErrNone );
    }

// ----------------------------------------------------
// CCmsContactField::HandlePresenceDataL
// 
// ----------------------------------------------------
//
void CCmsContactField::HandlePresenceDataL()
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::HandlePresenceDataL()" ) );
    #endif
    TPtr8 package( *StreamDesc8() );
    while( 0 < package.Length() )
        {
        const TInt index( package.Find( KDelimiterNewLine ) );
        TPtrC8 presenceData( package.Left( index ) );
        HandlePresencePackageL( presenceData );
        package.Delete( 0, index + KDelimiterNewLine().Length() );
        }
    User::RequestComplete( iClientStatus, KErrNone );
    }

// ----------------------------------------------------
// CCmsContactField::HandlePresencePackageL
// 
// ----------------------------------------------------
//
void CCmsContactField::HandlePresencePackageL( const TDesC8& aPresenceData )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::ExtractFieldType()" ) );
    #endif
    const TInt length( aPresenceData.Length() );
    TPtr8 presenceData( CONST_CAST( TUint8*, aPresenceData.Ptr() ), length, length );
    CCmsPresenceData* item = CCmsPresenceData::NewL( iContact );
    CleanupStack::PushL( item );
    while( 0 < presenceData.Length() )
        {
        TInt delIndex = 0;
        TInt type( ExtractFieldType( presenceData ) );
        TPtrC8 data( ExtractData( presenceData, delIndex ) );
        item->AssignDataL( type, data );
        presenceData.Delete( 0, delIndex );
        presenceData.TrimAll();
        }
    User::LeaveIfError( iFieldItems.Append( item ) );
    CleanupStack::Pop();  //item
    }

// ----------------------------------------------------
// CCmsContactField::ExtractFieldType
// 
// ----------------------------------------------------
//
TInt CCmsContactField::ExtractFieldType( TPtr8& aDataDesc )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::ExtractFieldType()" ) );
    #endif
    TInt fieldType = 0;
    TInt index = aDataDesc.Locate( KDelimiterSpace );
    TPtrC8 data( aDataDesc.Left( index ) );
    TLex8 lex( data );
    lex.Val( fieldType );
    aDataDesc.Delete( 0, index + 1 );
    return fieldType;
    }

// ----------------------------------------------------
// CCmsContactField::ExtractData
// 
// ----------------------------------------------------
//
TPtrC8 CCmsContactField::ExtractData( const TDesC8& aDataDesc, TInt& aDelIndex )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsContactField::ExtractData()" ) );
    #endif
    TInt index1( aDataDesc.Locate( KDelimiterQuote ) );
    TPtrC8 temp( aDataDesc.Mid( index1 + 1 ) );
    RDebug::RawPrint( temp );
    TInt index2 = temp.Locate( KDelimiterQuote );
    TPtrC8 data( index2 >= 0 ? temp.Left( index2 ) : temp );
    RDebug::RawPrint( data );
    aDelIndex = index1 + data.Length() + 3;
    return data;
    }

// ----------------------------------------------------
// CCmsContactField::CreateFieldItemL
// 
// ----------------------------------------------------
//
void CCmsContactField::CreateFieldItemL( const TDesC8& aItemData )
    {
    CCmsContactFieldItem* item = CCmsContactFieldItem::NewL( aItemData );
    CleanupStack::PushL( item );
    User::LeaveIfError( iFieldItems.Append( item ) );
    CleanupStack::Pop();  //item
    }

// ----------------------------------------------------
// CCmsContactField::CreateFieldItemL
// 
// ----------------------------------------------------
//
void CCmsContactField::CreateFieldItemL( const TDesC& aItemData, const TDesC& aItemInfo, const TInt aDefaultAttribute )
    {
    CCmsContactFieldItem* item = CCmsContactFieldItem::NewL( aItemData );
    CleanupStack::PushL( item );
    item->SetDefaultAttribute( aDefaultAttribute );
    User::LeaveIfError( iFieldItems.Append( item ) );
    CleanupStack::Pop();  //item
    item->SetInfo( aItemInfo );    
    }

// End of File


