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
#include <fbs.h>
#include <s32mem.h> 
#include <bitdev.h> 
#include <mbsaccess.h>
#include <cbsbitmap.h>
#include <cbsfactory.h>
#include "cmssession.h"
#include <AknIconUtils.h>
#include "cmspresencedata.h"
#include "cmscommondefines.h"

#include <presencetrafficlights.mbg>
#include "presencetrafficlighticons.h"

#include <contactpresencebrandids.h> 

// This must be in line with contactpresence.dll implementation
_LIT( KPresenceTrafficLightsMif, "\\resource\\apps\\presencetrafficlights.mif" );

// ----------------------------------------------------
// CCmsPresenceData::CCmsPresenceData
// 
// ----------------------------------------------------
//
CCmsPresenceData::CCmsPresenceData( RCmsContact& aContact ) 
    : iElementIdText(NULL), iContact( aContact )
    {
    
    }

// ----------------------------------------------------
// CCmsPresenceData::NewL
// 
// ----------------------------------------------------
//
CCmsPresenceData* CCmsPresenceData::NewL( RCmsContact& aContact )
    {
    CCmsPresenceData* self = new ( ELeave ) CCmsPresenceData( aContact );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsPresenceData::ConstructL
// 
// ----------------------------------------------------
//
void CCmsPresenceData::ConstructL()
    {
    }

// ----------------------------------------------------
// CCmsPresenceData::~CCmsPresenceData
// 
// ----------------------------------------------------
//
EXPORT_C CCmsPresenceData::~CCmsPresenceData()
    {
    //The client owns these two guys
    //delete iMask;
    //delete iBitmap;
    delete iBrandId;
    delete iBrandedText;
    delete iUserIdentity;
    delete iElementIdText;
    delete iElementIdImage;
    }
        
// ----------------------------------------------------
// CCmsPresenceData::Mask
// 
// ----------------------------------------------------
//
EXPORT_C CFbsBitmap* CCmsPresenceData::Mask() const
    {
    return iMask;
    }
        
// ----------------------------------------------------
// CCmsPresenceData::~CCmsPresenceData
// 
// ----------------------------------------------------
//
EXPORT_C CFbsBitmap* CCmsPresenceData::Bitmap() const
    {
    return iBitmap;
    }

// ----------------------------------------------------
// CCmsPresenceData::CompilePresenceDataL
// 
// ----------------------------------------------------
//
EXPORT_C TInt CCmsPresenceData::PreparePresenceDataL( TSize aBitmapSize )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsPresenceData::PreparePresenceDataL()" ) );
    #endif
    TInt error = KErrNotFound;
    CBSFactory* factory = CreateBrandingFactoryL();
    if( factory )
        {
        CleanupStack::PushL( factory ); 
        
        //reset brand text here
        delete iBrandedText;        
        iBrandedText = NULL;
        
        if ( !iBrandId->Des().Length() )
            {
            // presence cache is reset
            iMask = new ( ELeave ) CFbsBitmap();
            iBitmap = new ( ELeave ) CFbsBitmap();              
            }
        else if ( iBrandId->Des().Compare( KTrafficLightService ) )
            {
            error = DoGetIconFromBrandSrvL( *factory, aBitmapSize );            
            }
        else
            {
            error = DoGetIconFromFileL( aBitmapSize );
            }
        CleanupStack::PopAndDestroy(factory);  //factory
        }
    return error;
    }

           
// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::ElementIdImage
//
// ---------------------------------------------------------------------------
//
TPtrC8 CCmsPresenceData::ElementIdImage()
    {
    return iElementIdImage ? iElementIdImage->Des() : TPtrC8();
    }

// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::CreateBrandingFactoryL
//
// ---------------------------------------------------------------------------
//
CBSFactory* CCmsPresenceData::CreateBrandingFactoryL()
    {
    CBSFactory* factory = NULL;
    if( iBrandId )
        {
        factory = CBSFactory::NewL( KCPBrandDefaultId, KCPBrandAppId );
        }
    else
        {
        #ifdef _DEBUG
            RCmsSession::WriteToLog( _L8( " Brand ID is NULL, cannot create branding factory" ) );
        #endif
        }
    return factory;
    }
    
// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::CopyBitmapL
//
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCmsPresenceData::CopyBitmapL( CFbsBitmap& aBitmap )
    {
    // Create a temporary bitmap
    CFbsBitmap* newBitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( newBitmap );
    User::LeaveIfError( newBitmap->Create( aBitmap.SizeInPixels(), aBitmap.DisplayMode() ) );
    // Create a bitmap device
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( newBitmap );
    CleanupStack::PushL( device );
    // Create a bitmap context
    CBitmapContext* context = NULL;
    User::LeaveIfError( device->CreateBitmapContext( context ) );
    CleanupStack::PushL( context );
    // Copy contents of aBitmap into the new bitmap
    context->BitBlt( TPoint( 0,0 ), &aBitmap );
    CleanupStack::PopAndDestroy( 2 ); // context, device
    CleanupStack::Pop(); //newBitmap
    return newBitmap;
    }

// ----------------------------------------------------
// CCmsPresenceData::BrandedText
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CCmsPresenceData::BrandedText() const
    {
    return iBrandedText ? iBrandedText->Des() : TPtrC();
    }

// ----------------------------------------------------
// CCmsPresenceData::UserIdentity
// 
// ----------------------------------------------------
//        
EXPORT_C TPtrC8 CCmsPresenceData::UserIdentity() const
    {
    return iUserIdentity ? iUserIdentity->Des() : TPtrC8();
    }

// ----------------------------------------------------
// CCmsPresenceData::ServiceType
// 
// ----------------------------------------------------
//
EXPORT_C TUint32 CCmsPresenceData::ServiceType() const
    {
    return ( TUint32 )iServiceType;
    }

// ----------------------------------------------------
// CCmsPresenceData::AssignDataL
// 
// ----------------------------------------------------
//
void CCmsPresenceData::AssignDataL( TInt aFieldType, const TDesC8& aFieldData )
    {
    #ifdef _DEBUG
        RCmsSession::WriteToLog( _L8( "CCmsPresenceData::AssignDataL()" ) );
    #endif
    switch( aFieldType )
        {
        case KPresenceDataBrandId:
            delete iBrandId;
            iBrandId = NULL;
            iBrandId = aFieldData.AllocL();
            break;
        case KPresenceDataElementTextId:
            delete iElementIdText;
            iElementIdText = NULL;
            iElementIdText = aFieldData.AllocL();
            break;
        case KPresenceDataElementImageId:
            delete iElementIdImage;
            iElementIdImage = NULL;
            iElementIdImage = aFieldData.AllocL();
            break;
        case KPresenceDataLanguageId:
            {
            TLex8 lex( aFieldData );
            lex.Val( iLanguageId );
            }
            break;
        case KPresenceDataServiceType:
            {
            TLex8 lex( aFieldData );
            lex.Val( iServiceType );
            }
            break;            
        case KPresenceDataUserIdentity:
            delete iUserIdentity;
            iUserIdentity = NULL;
            iUserIdentity = aFieldData.AllocL();
            break; 
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::DoGetIconFromBrandSrvL
//
// ---------------------------------------------------------------------------
//
TInt CCmsPresenceData::DoGetIconFromBrandSrvL( CBSFactory& aFactory, TSize aBitmapSize )
    {
    TLanguage userLang = User::Language();
    TLanguage defLang = ELangInternationalEnglish;
    
    //Search for BrandPackage using PhoneLanguage
    //PhoneLanguage gets the Highest Priority
    TRAPD(err, DoGetIconFromBrandSrvL( aFactory, aBitmapSize, ( TLanguage ) userLang ));    
          
    if ( ( err ) && ( iLanguageId != KErrNotFound ) && ( iLanguageId != userLang ) )
        {
        //The next priority goes to BrandLanguage set in the SpSettings/service table
        //during provisioning
        //Search for BrandPackage using this BrandLanguage
        TRAP( err, DoGetIconFromBrandSrvL( aFactory, aBitmapSize, ( TLanguage ) iLanguageId ) );       
        }
    
    
    if ( ( err ) && ( userLang != defLang ) && ( iLanguageId != defLang ) )
        {
        //Fetching of brand with UserLang & with the one mentioned in the Servicetable
        //was not successfull. 
        //Finally try with Default Language ID and even if this fails
        //proceed without any brand icons/texts
        TRAP( err, DoGetIconFromBrandSrvL( aFactory, aBitmapSize, ( TLanguage ) defLang ) );        
        }   
    
    return err;
    }
            
           
// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::DoGetIconFromBrandSrvL
//
// ---------------------------------------------------------------------------
//
TInt CCmsPresenceData::DoGetIconFromBrandSrvL( CBSFactory& aFactory, TSize aBitmapSize, TLanguage aLanguage )
    {
    TInt error = KErrNone;
    
    MBSAccess* access = aFactory.CreateAccessLC( iBrandId->Des(), ( TLanguage )aLanguage );
    
    CFbsBitmap* mask = NULL;
    CFbsBitmap* bitmap = NULL;    

    TRAP( error, access->GetBitmapL( ElementIdImage(), bitmap, mask ) );
    if( KErrNone == error )
        {
        CleanupStack::PushL( mask );
        CleanupStack::PushL( bitmap );
        AknIconUtils::SetSize( mask, aBitmapSize );
        AknIconUtils::SetSize( bitmap, aBitmapSize );
        iMask = CopyBitmapL( *mask );
        iBitmap = CopyBitmapL( *bitmap );
        CleanupStack::PopAndDestroy( 2 );  //bitmap, mask
        }
    else
        {
        //instantiate bitmaps
        iMask = new ( ELeave ) CFbsBitmap();
        iBitmap = new ( ELeave ) CFbsBitmap();
        }
    
    delete iBrandedText;
    iBrandedText = NULL;
    if ( iElementIdText && iElementIdText->Length() )
        {
        iBrandedText = access->GetTextL( iElementIdText->Des() );
        }
    
    CleanupStack::PopAndDestroy();  //access    
    return error;
    }


// ---------------------------------------------------------------------------
// CCmsPresenceIconHandler::DoGetIconFromFileL
//
// ---------------------------------------------------------------------------
//
TInt CCmsPresenceData::DoGetIconFromFileL( TSize aBitmapSize )
    {
    TInt error = KErrNone;
    
    CFbsBitmap *bitmap = NULL;
    CFbsBitmap *mask = NULL;
    
    TInt bitmapId(0);
    TInt maskId(0);
    
    if ( !(ElementIdImage().CompareF( KTrafficOnline )))
        {
        bitmapId = EMbmPresencetrafficlightsQgn_prop_im_friend_on;
        maskId = EMbmPresencetrafficlightsQgn_prop_im_friend_on_mask;
        }
    else if ( !(ElementIdImage().CompareF( KTrafficOffline )))
        {
        bitmapId = EMbmPresencetrafficlightsQgn_prop_im_friend_off;
        maskId = EMbmPresencetrafficlightsQgn_prop_im_friend_off_mask;
        }
    else
        {
        }
        
    TRAP(error, AknIconUtils::CreateIconL(
         bitmap,
         mask,
         KPresenceTrafficLightsMif,
         bitmapId,
         maskId));
    
    if ( error )
        {
        //instantiate bitmaps
        iMask = new ( ELeave ) CFbsBitmap();
        iBitmap = new ( ELeave ) CFbsBitmap();     
        }
    else
        {   
        AknIconUtils::SetSize(  bitmap, aBitmapSize );  
        AknIconUtils::SetSize(  mask, aBitmapSize );          
        iBitmap = bitmap;
        iMask = mask;
        }  
    
    return error;    
    
    }

// End of File
