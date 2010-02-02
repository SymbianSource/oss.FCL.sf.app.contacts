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
* Description:  contact presence
*
*/


// INCLUDE FILES

#include <e32std.h>
#include <cbsfactory.h>
#include <mbsaccess.h>
#include <fbs.h>
#include <s32buf.h>
#include <s32mem.h>
#include <AknIconUtils.h>

#include <presencetrafficlights.mbg>

#include "presencetrafficlighticons.h"
#include "contactpresence.h"
#include <mcontactpresenceobs.h>
#include "presencebrandedicon.h"


#include <spsettings.h>
#include <spproperty.h>

// This must be in line wirth presencetrafficlight.dll implementation
_LIT( KPresenceTrafficLightsMif, "\\resource\\apps\\presencetrafficlights.mif" );

//constants
const TInt KMaxBufLength = 512; 
 

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CPresenceBrandedIcon::CPresenceBrandedIcon
// ----------------------------------------------------------
//
CPresenceBrandedIcon::CPresenceBrandedIcon(CBSFactory* aBrandingFactory , 
                                                    MContactPresenceObs& aObs )
    : CActive( CActive::EPriorityStandard ), iDestroyedPtr( NULL ), iObs( aObs )
    {
    iBrandingFactory = aBrandingFactory;
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------
// CPresenceBrandedIcon::~CPresenceBrandedIcon
// ----------------------------------------------------------
//
CPresenceBrandedIcon::~CPresenceBrandedIcon()
    {
    Cancel();
            
    if(iBrandId)
        delete iBrandId;
        
    if(iElementId)
        delete iElementId;
    
    if ( iDestroyedPtr )
        {
        // We are called inside callback
        *iDestroyedPtr = ETrue;
        iDestroyedPtr = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CPresenceBrandedIcon::Destroy
// -----------------------------------------------------------------------------
void CPresenceBrandedIcon::Destroy()
    {
    iQueLink.Deque();
    delete this;
    }    

// ----------------------------------------------------------
// CPresenceBrandedIcon::NewL
// ----------------------------------------------------------
//
CPresenceBrandedIcon* CPresenceBrandedIcon::NewL(CBSFactory* const aBrandingFactory,
                                                 MContactPresenceObs& aObs)
    {
    CPresenceBrandedIcon* self = new (ELeave) CPresenceBrandedIcon( aBrandingFactory, aObs );
    CleanupStack::PushL( self );
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------
// CPresenceBrandedIcon::StartGetIconInfoL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::StartGetIconInfoL( TInt aOpId, const TDesC8& aBrandId, 
                                  const TDesC8& aElementId, const TSize aBitmapSize)
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iBitmapSize = aBitmapSize; 
    
    iBrandId = aBrandId.AllocL();
    iElementId = aElementId.AllocL();    
    iBrandLanguage = FindBrandLanguageIdL( aBrandId ) ; 
    
    // complete request in RunL
    iStatus = KRequestPending;    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
    SetActive();
    
    iOpId = aOpId;
    }     
    
// ----------------------------------------------------------
// CPresenceBrandedIcon::RunL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::RunL()
    {
    // Set the member to point to stack variable
    // to make leak on destruction proof while in callback
    TBool   destroyed( EFalse );
    iDestroyedPtr = &destroyed;
    
    if ( iBrandId->Des().Compare( KTrafficLightService ) )
        {
        TRAP_IGNORE( ProcessServiceL() );
        }
    else
        {
        TRAP_IGNORE( ProcessCommonL() );
        }
    
    
    // work is done, destroy self, but check if we still exist
    iDestroyedPtr = NULL;    
    if (!destroyed)
        Destroy(); 
    }  
    
// ----------------------------------------------------------
// CPresenceBrandedIcon::DoCancel
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::DoCancel()
    {
    // not needed
    }
    
// ----------------------------------------------------------
// CPresenceBrandedIcon::ProcessServiceL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::ProcessServiceL()
    {
    
    CFbsBitmap* brandedBitmap(NULL);
    CFbsBitmap* brandedMask(NULL);
    TInt err(KErrNone);
    
    TRAP ( err, GetBitmapL( brandedBitmap, brandedMask ) );    
    
    if((KErrNone == err) && brandedBitmap && brandedMask)
        {
        CleanupStack::PushL(brandedBitmap);
        CleanupStack::PushL(brandedMask);

        AknIconUtils::SetSize( brandedBitmap, iBitmapSize );
        AknIconUtils::SetSize( brandedMask, iBitmapSize );
        
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        
        bitmap = new (ELeave) CFbsBitmap;
        CleanupStack::PushL( bitmap );
        bitmap->Duplicate( brandedBitmap->Handle() );
                                    
        mask = new (ELeave) CFbsBitmap;
        CleanupStack::PushL( mask );
        mask->Duplicate( brandedMask->Handle() );
        
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);      
                    
        CleanupStack::PopAndDestroy(brandedMask);
        CleanupStack::PopAndDestroy(brandedBitmap);
                        
        iObs.ReceiveIconFileL(*iBrandId, *iElementId, bitmap, mask);
        }
                
    else // report an error to observer
        {
        if(brandedBitmap)
            {
            delete brandedBitmap;
            brandedBitmap = NULL;
            }
        if(brandedMask)
            {
            delete brandedMask;
            brandedMask = NULL;
            }
            
        iObs.ErrorOccured(iOpId,err);
        }
    }

// ----------------------------------------------------------
// CPresenceBrandedIcon::ProcessCommonL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::ProcessCommonL()
    {
    
    CFbsBitmap *bitmap = NULL;
    CFbsBitmap *mask = NULL;
    
    TInt bitmapId(0);
    TInt maskId(0);
    
    if ( !(iElementId->Des().CompareF( KTrafficOnline )))
        {
        bitmapId = EMbmPresencetrafficlightsQgn_prop_im_friend_on;
        maskId = EMbmPresencetrafficlightsQgn_prop_im_friend_on_mask;
        }
    else if ( !(iElementId->Des().CompareF( KTrafficOffline )))
        {
        bitmapId = EMbmPresencetrafficlightsQgn_prop_im_friend_off;
        maskId = EMbmPresencetrafficlightsQgn_prop_im_friend_off_mask;
        }
    else
        {
        iObs.ErrorOccured( iOpId, KErrNotFound );        
        User::Leave( KErrNotFound );
        }
        
    TRAPD(err, AknIconUtils::CreateIconL(
         bitmap,
         mask,
         KPresenceTrafficLightsMif,
         bitmapId,
         maskId));
    
    if ( err )
        {
        iObs.ErrorOccured( iOpId, err );
        }
    else
        {   
        AknIconUtils::SetSize(  bitmap, iBitmapSize );  
        AknIconUtils::SetSize(  mask, iBitmapSize );          
        iObs.ReceiveIconFileL( *iBrandId, *iElementId, bitmap, mask );
        }               
    }
    
// ----------------------------------------------------------
// CPresenceBrandedIcon::GetOpId
// ----------------------------------------------------------
//
TInt CPresenceBrandedIcon::GetOpId() const
    {
    return iOpId;     
    }

// ----------------------------------------------------------
// CPresenceBrandedIcon::FindBrandLanguageIdL
// ----------------------------------------------------------
//
TLanguage CPresenceBrandedIcon::FindBrandLanguageIdL( const TDesC8& aBrandId )
    {
    CSPSettings* spSettings = CSPSettings::NewLC();
    RIdArray idArray;
    CleanupClosePushL(idArray);
    User::LeaveIfError(spSettings->FindServiceIdsL(idArray));
    CSPProperty* prop = CSPProperty::NewLC();    
    HBufC* brandID = HBufC::NewLC(KMaxBufLength);
    TLanguage servLang = ELangInternationalEnglish;
    TInt serviceCount = idArray.Count();
    
    //The challenge here is to find the ServiceTable entry using BrandId.
    //Iterate thro each service id and find the BrandId
    //for each service and check whether it matches to out Input BrandId.
    //if it matches we have found the service, and now
    //get the brandlanguage.
    //Note : The problem with this logic is that it works only
    //       if all the services brandid is unique. 
    //       If not GOD needs to help us. 
    //       This logic is a temporary one untill 
    //       one of our CR for Changing the Interface, so that you
    //       get the BrandLanguage as part of the StartGetIconInfoL
    //       is approved. 
    //
    for (TInt index=0; index<serviceCount; index++)
        {    
        if ( KErrNone == spSettings->FindPropertyL( idArray[index], EPropertyBrandId, *prop ) )
            {
            TPtr des = brandID->Des();            
            if ( KErrNone == prop->GetValue( des ) )
                {                
                TBuf8 <KMaxBufLength> brandIdFromSpSettings;
                brandIdFromSpSettings.Copy(des);
                if ( KErrNone == brandIdFromSpSettings.Compare( aBrandId ) )
                    {
                    if( KErrNone == spSettings->FindPropertyL( 
                                    idArray[index], EPropertyBrandLanguage, *prop) )
                        {                        
                        prop->GetValue( ( TInt& )servLang );
                        }                           
                    break;
                    }
                }
            }
        }        
    
    CleanupStack::PopAndDestroy( 4 ); //brandID, prop, idArray, spSettings
    
    return servLang;
    }

// ----------------------------------------------------------
// CPresenceBrandedIcon::GetBitmapL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::GetBitmapL( 
                    CFbsBitmap*& aBrandedBitmap, 
                    CFbsBitmap*& aBrandedMask )
    {   
    TLanguage userLang = User::Language();
    TLanguage defaultLang = ELangInternationalEnglish;
    
    if( !iBrandingFactory )
        {
        User::Leave ( KErrGeneral );
        }  
    
    //Search for BrandPackage using PhoneLanguage
    //PhoneLanguage gets the Highest Priority
    TRAPD( err, GetBitmapFromBrandingServerL( userLang, aBrandedBitmap, aBrandedMask ) );
    
    if ( err && ( userLang != iBrandLanguage ) ) 
        {
        //The next priority goes to BrandLanguage set in the SpSettings/service table
        //during provisioning
        //Search for BrandPackage using this BrandLanguage  
        TRAP( err, GetBitmapFromBrandingServerL( iBrandLanguage, aBrandedBitmap, aBrandedMask ) );
        }
    
    if ( err && ( iBrandLanguage != defaultLang ) && ( userLang != defaultLang ) ) 
        {
        //The least priority goes to the default language which is ELangInternationalEnglish        
        //Search for BrandPackage using this ELangInternationalEnglish
        GetBitmapFromBrandingServerL( defaultLang, aBrandedBitmap, aBrandedMask ) ;        
        }                
    }

// ----------------------------------------------------------
// CPresenceBrandedIcon::GetBitmapFromBrandingServerL
// ----------------------------------------------------------
//
void CPresenceBrandedIcon::GetBitmapFromBrandingServerL(
                    TLanguage aLanguageId, 
                    CFbsBitmap*& aBrandedBitmap, 
                    CFbsBitmap*& aBrandedMask )
    {   
    MBSAccess*  brandingAccess = iBrandingFactory->CreateAccessLC( 
                        *iBrandId, aLanguageId );    
    
    if ( ( iElementId ) && ( iElementId->Length() ) )
        {
        brandingAccess->GetBitmapL( *iElementId, aBrandedBitmap, aBrandedMask );        
        }
    
    CleanupStack::PopAndDestroy();  //access    
    }

//eof
