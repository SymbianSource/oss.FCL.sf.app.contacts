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
* Description:  Class that contains util-functions for logsserviceextension.
*
*/


#include <e32base.h>
#include <f32file.h>
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <cbsfactory.h>
#include <mbsaccess.h>
#include <cbsbitmap.h>
#include <mbselement.h>

#include "tlogsextutil.h"
#include "logsextconsts.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// Retrieves File handle and bitmask/bitmask id and takes service id and 
// element id as input parameters.
// ---------------------------------------------------------------------------
//
void TLogsExtUtil::GetFileHandleL( 
    const TUint32 aServiceId,
    const TDesC8& aElementId,    
    RFile& aBitmapFile,
    TInt& aBitmapId,   
    TInt& aBitmapMaskId )
    {
    _LOG("TLogsExtUtil::GetFileHandleL() begin" ) 
    
    CSPSettings* spSettings = CSPSettings::NewLC();
    _LOG("TLogsExtUtil::GetFileHandleL() CSPSettings created" ) 
    
    // get the service provider settings
    
    // read the brand id from the settings table
    CSPProperty* spProperty = CSPProperty::NewLC();
    
    User::LeaveIfError( 
        spSettings->FindPropertyL( 
            aServiceId,
            EPropertyBrandId,
            *spProperty ) );
    
    if ( !spProperty ) // check if the property was found
        {
        User::Leave( KErrNotFound );   
        }       
    _LOG("TLogsExtUtil::GetFileHandleL() CSPProperty retrieved ok" )
      
    HBufC* brandId16 = HBufC::NewLC( KSPMaxDesLength );        
    TPtr brandIdPtr16( brandId16->Des() );    
    User::LeaveIfError( spProperty->GetValue( brandIdPtr16 ) );
    
    HBufC8* brandId8 = HBufC8::NewLC( brandIdPtr16.Length() );
    TPtr8 brandIdPtr8( brandId8->Des() );
    brandIdPtr8.Copy( *brandId16 );    

    // read the language property from the settings table        
    
    // workaround for missing brand language value from ServiceSettingsTable:
    // use hardcoded value
    TInt err( KErrNone );
    TInt languageId( ELangInternationalEnglish );
    TRAP( err, err = spSettings->FindPropertyL(
        aServiceId,
        EPropertyBrandLanguage,
        *spProperty ) );    
    
    if ( KErrNone == err )
        {
        User::LeaveIfError( spProperty->GetValue( languageId ) );
        _LOG("TLogsExtUtil::GetFileHandleL()..." )
        _LOG("...EPropertyBrandLanguage retrieved ok" )
        }
    else
        {        
        _LOG("TLogsExtUtil::GetFileHandleL() ...")
        _LOG("...retrieval of EPropertyBrandId failed.hardcoded...")
        _LOGPP("...language id(%d) used! error=%d", languageId, err )    
        }    
    
    // connect to branding server
    // KAppId defined in LogsExtConsts.h
    CBSFactory* factory = CBSFactory::NewL( KDefaultBrandId, KAppId );
    CleanupStack::PushL( factory );
    _LOG("TLogsExtUtil::GetFileHandleL() CBSFactory created" )

    // Create access to Branding server
    MBSAccess* access = 
        factory->CreateAccessL( *brandId8, TLanguage( languageId ) );    
    
    _LOG("TLogsExtUtil::GetFileHandleL() MBSAccess created" )
    CleanupClosePushL( *access );    
   
    // fetch the brand logo from the brand server    
    MBSElement* bitmapItem = NULL;    
      
    // get the structure containing the info for the bitmap
    bitmapItem = access->GetStructureL( aElementId );
    _LOG("TLogsExtUtil::GetFileHandleL() bitmapItem retrieved" ) 
    User::LeaveIfNull( bitmapItem );
    
    CleanupClosePushL( *bitmapItem );
    
    if ( EBSBitmap != bitmapItem->ElementType() ) 
        {
        User::Leave( KErrNotFound );
        }
        
    // get the bitmap data from the bitmap element
    const CBSBitmap* bsBitmap = &bitmapItem->BitmapDataL();
    _LOG("TLogsExtUtil::GetFileHandleL() bsBitmap retrieved" ) 
       
    // we need to get the bitmap file from where we can load the bitmap
    access->GetFileL( bsBitmap->BitmapFileId(), aBitmapFile );
    
    // set the return values
    aBitmapId = bsBitmap->BitmapId();
    aBitmapMaskId = bsBitmap->BitmapMaskId();
    
    CleanupStack::PopAndDestroy( bitmapItem );
    bitmapItem = NULL;
    CleanupStack::PopAndDestroy( access );
    access = NULL;
    CleanupStack::PopAndDestroy( factory );    
    factory = NULL;
    CleanupStack::PopAndDestroy( brandId8 ); 
    brandId8 = NULL;
    CleanupStack::PopAndDestroy( brandId16 ); 
    brandId16 = NULL;
    CleanupStack::PopAndDestroy( spProperty );
    spProperty = NULL;
    CleanupStack::PopAndDestroy( spSettings );
    spSettings = NULL;    
    }

