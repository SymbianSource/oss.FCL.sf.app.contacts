/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Service manager for external service providers.
*
*/


#include <e32base.h>
#include <barsread.h>
#include <Pbk2DataCaging.hrh>
#include <Pbk2Presentation.rsg>
#include <RPbk2LocalizedResourceFile.h>
#include "CPbk2ServiceManager.h"
#include "Pbk2PresentationUtils.h"
#include <spsettings.h>
#include <spproperty.h>
#include <spentry.h>
#include <cbsfactory.h>
#include <mbsaccess.h>
#include <fbs.h>
#include <contactpresencebrandids.h> 
#include <spnotifychange.h>
#include <PbkUID.h>

//constants
const TInt KMaxBufLength = 128; 

void CPbk2ServiceManager::TService::Delete()
    {
    delete iBitmap;
    iBitmap = NULL;
    delete iMask;
    iBitmap = NULL;
    }

EXPORT_C CPbk2ServiceManager* CPbk2ServiceManager::NewL(RFs& aRFs)
    {
    CPbk2ServiceManager* self = new (ELeave) CPbk2ServiceManager(aRFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbk2ServiceManager::~CPbk2ServiceManager()
    {
    if(iSPNotifyChange)
        {
        iSPNotifyChange->NotifyChangeCancel();
        }
    
    delete iSPNotifyChange;
    iSpIdsToObserve.Close(); //Empty, no entries
    delete iSettings;
    DeleteData();
    }

void CPbk2ServiceManager::DeleteData()
    {
    for (TInt i = 0; i < iServices.Count(); i++)
        {
        iServices[i].Delete();
        }
    iServices.Close();

    for (TInt i = 0; i < iStringsOwner.Count(); i++)
        {
        delete iStringsOwner[i];
        }
    iStringsOwner.Close();
    iBitmapIdCounter = 0;
    }

CPbk2ServiceManager::CPbk2ServiceManager(RFs& aRFs) :
    iRFs(aRFs)
    {
    }

void CPbk2ServiceManager::ConstructL()
    {
    iSettings = CSPSettings::NewL();    
    iSPNotifyChange = CSPNotifyChange::NewL(*this);
    iSPNotifyChange->NotifyChangeL( iSpIdsToObserve );
    iRunRefreshData = ETrue;    
    }

EXPORT_C const CPbk2ServiceManager::RServicesArray& CPbk2ServiceManager::Services()
    {
    //If got one or more notifications of changed data run refresh first
     if(iRunRefreshData)
        {
        iRunRefreshData = EFalse;
        TRAP_IGNORE( RunRefreshDataL() );
        }
    
    return iServices;
    }

EXPORT_C void CPbk2ServiceManager::RefreshDataL()
    {
    iRunRefreshData = ETrue;
    }

void CPbk2ServiceManager::RunRefreshDataL()
    {
    DeleteData();    
    ReadWellKnownServicesL();    
    AppendInstalledServicesL();
    UpdateBrandingInfoL();
    }


void CPbk2ServiceManager::ReadWellKnownServicesL()
    {
    RPbk2LocalizedResourceFile resFile( &iRFs );
    resFile.OpenLC(KPbk2RomFileDrive,
                   KDC_RESOURCE_FILES_DIR, 
                   Pbk2PresentationUtils::PresentationResourceFile());
    
    // Read well known services from resource
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( 
            R_PHONEBOOK2_WELL_KNOWN_SERVICES ) );
    TInt count = reader.ReadInt16();
    HBufC* buf = NULL;
    for (TInt i = 0; i < count; i++)
        {
        TService service;
        buf = reader.ReadHBufCL();
        CleanupStack::PushL(buf);
        service.iName.Set(*buf);
        iStringsOwner.AppendL(buf);
        CleanupStack::Pop(buf);
        buf = reader.ReadHBufCL();
        CleanupStack::PushL(buf);
        service.iDisplayName.Set(*buf);
        iStringsOwner.AppendL(buf);
        CleanupStack::Pop(buf);
        service.iFlags = EWellKnown;
        service.iBitmap = service.iMask = NULL;
        service.iBitmapId = 0;
        service.iBrandId.Set(KNullDesC);
        service.iApplicationId.Set(TPtrC8(KCPBrandAppId));
        AppendServiceL(service); 
        }
    CleanupStack::PopAndDestroy(2, &resFile);
    }

void CPbk2ServiceManager::AppendInstalledServicesL()
    {
    RIdArray idArray;
    CleanupClosePushL(idArray);
    User::LeaveIfError(iSettings->FindServiceIdsL(idArray));
    const TInt granularity = 16;
    CDesCArrayFlat* names = new (ELeave) CDesCArrayFlat(granularity);
    CleanupStack::PushL(names);
    User::LeaveIfError(iSettings->FindServiceNamesL(idArray, *names));
    
    //Append if not yet in iServices, otherwise update iServices  
    for (TInt ni = 0; ni < names->Count(); ni++)
        {
        TBool found = EFalse;
        TInt si = 0;
        while(si < iServices.Count()&& !found)
            {
            iServices[si].iName.CompareF(names->operator[](ni)) ? 
                si++ :found=ETrue; 
            }

        //For installed well-known services the service data needs to be updated.
        //For non-well known services some service attributes are first checked
        //to decide whether to add or skip the service.
        if( found )
            {
            //Update existing
            UpdateServiceL(iServices[si], idArray[ni], names->operator[](ni));
            }
        else
            {
            //Append new if ok to add 
            TBool okToAppend(ETrue);
            
            // Check whether the service is VCC.
            // If so, when the VoIP service is becoming available, 
            // the VCC item should be in a same field for UI displaying.
            CSPEntry *entry = CSPEntry::NewLC();
            TInt ret = iSettings->FindEntryL( idArray[ni], *entry );
            if( ret == KErrNone )
                {
                const CSPProperty* prop = NULL;
                ret = entry->GetProperty( prop, ESubPropertyVccVDI);
                if( ret == KErrNone )
                    {
                    okToAppend = EFalse;
                    }
                }
            CleanupStack::PopAndDestroy(); //entry
            // Check whether service supports cs voice call. If so, discard it.            
            CSPProperty* servAttrMask = CSPProperty::NewLC();        
            ret = iSettings->FindPropertyL( 
                idArray[ni], EPropertyServiceAttributeMask, *servAttrMask );
            
            if( ret == KErrNone )
                {
                TInt mask(0);
                servAttrMask->GetValue(mask);
                okToAppend = !(mask & ESupportsCSVoiceCall); 
                }
            CleanupStack::PopAndDestroy(servAttrMask);            
            
            if(okToAppend) 
                {
                TService service;                
                UpdateServiceL(service, idArray[ni], names->operator[](ni));
                AppendServiceL(service);                
                }            
            }    
        }
    CleanupStack::PopAndDestroy(names);
    CleanupStack::PopAndDestroy(); //idArray
    }

void CPbk2ServiceManager::UpdateServiceL(
    TService& aService,
    TServiceId aServiceId, 
    const TDesC& aServiceName )
    {
    //Set service name
    HBufC* name = HBufC::NewLC(aServiceName.Length());
    iStringsOwner.AppendL(name);  
    CleanupStack::Pop(name);        
    name->Des().Copy(aServiceName);

    //iName &  iDisplayName will not be set if the service
    //doesnt belong to the list of well known services
    //Thus it makes sense that We copy the ServiceName provisioned
    //in the Service Table to both iName as well as iDisplayName
    //This iDisplayName will be visible to the end user.
    if ( !aService.iName.Length() )
        {
        aService.iName.Set(*name);
        }
    if ( !aService.iDisplayName.Length() )
        {
        aService.iDisplayName.Set(*name);
        }
    aService.iFlags = EInstalled;
    aService.iBitmap = aService.iMask = NULL;
    aService.iBitmapId = 0;

    //Brand id 
    CSPProperty* prop = CSPProperty::NewLC();
    HBufC* brandID = HBufC::NewLC(KMaxBufLength);

    if (iSettings->FindPropertyL(aServiceId, EPropertyBrandId, *prop) == KErrNone)
        {
        TPtr des = brandID->Des();
        if (prop->GetValue(des) != KErrNone)
            {
            des.Zero();
            }
        }
    
    aService.iBrandId.Set(*brandID);
    aService.iServiceId = aServiceId;
    iStringsOwner.AppendL(brandID);
    
    //Appid
    aService.iApplicationId.Set(TPtrC8(KCPBrandAppId));
    CleanupStack::Pop(brandID);     
    CleanupStack::PopAndDestroy(prop);       
    }

void CPbk2ServiceManager::AppendServiceL(TService aService)
    {
    iServices.AppendL(aService);
    }

void CPbk2ServiceManager::UpdateBrandingInfoL()
    {
    TLanguage userLang = User::Language();
    TLanguage defLang = ELangInternationalEnglish;
    for (TInt i = 0; i < iServices.Count(); i++)    
        {
        TLanguage servLang = ELangTest;  
        //ELangTest CAN BE USED AS DEFAULT VALUE AS THIS DOES NOT BELONG TO ANY LANG
        //SEE THE HEADER FILE e32lang.h
        if(iServices[i].iBrandId.Length())
            {
            //Search for BrandPackage using PhoneLanguage
            //PhoneLanguage gets the Highest Priority
            TRAPD(err, UpdateBrandL(iServices[i], userLang ));
            if (err)
                {
                //The next priority goes to BrandLanguage set in the SpSettings/service table
                //during provisioning
                //Search for BrandPackage using this BrandLanguage      
                CSPProperty* prop = CSPProperty::NewLC();
                if(!iSettings->FindPropertyL(
                    iServices[i].iServiceId, EPropertyBrandLanguage, *prop))
                    {
                    if(!prop->GetValue((TInt&)servLang))
                        {
                        if (userLang != servLang)
                            {
                            TRAP(err, UpdateBrandL(iServices[i], servLang ));
                            }
                        }
                    }
                CleanupStack::PopAndDestroy(prop);                
                }
            
            if ( ( err ) && ( servLang !=  defLang ) && ( userLang != defLang ) )
                {
                //Fetching of brand with UserLang & with the one mentioned in the Servicetable
                //was not successfull. 
                //Finally try with Default Language ID and even if this fails
                //proceed without any brand icons/texts
                TRAP( err, UpdateBrandL( iServices[i], defLang ) );
                }
            }
        }
    }

void CPbk2ServiceManager::UpdateBrandL( TService& aService, TLanguage aLanguage )
    {
    HBufC8* brandID = HBufC8::NewLC(aService.iBrandId.Length());
    brandID->Des().Copy(aService.iBrandId);

    CBSFactory* bsFactory = CBSFactory::NewL( KCPBrandDefaultId, aService.iApplicationId );
    CleanupStack::PushL(bsFactory);
    MBSAccess* access = bsFactory->CreateAccessLC(*brandID, aLanguage); //leaves if none found    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    access->GetBitmapL(KCPBrandElementIdServiceIcon, bitmap, mask);    
    CleanupStack::PushL(bitmap);
    CleanupStack::PushL(mask);
    
    //Update service
    if(bitmap)
        {
        aService.iBitmap = bitmap;
        aService.iMask = mask;
        aService.iBitmapId = ++iBitmapIdCounter;
        }
    CleanupStack::Pop(2);    //bitmap, mask
    //Get Localised Name of Service
    HBufC* localizedName = access->GetTextLC(KCPBrandElementIdLocalizedServiceName);    
    if(localizedName)
        {
        aService.iDisplayName.Set(*localizedName);
        iStringsOwner.AppendL(localizedName); 
        }
    
    CleanupStack::Pop();  //localizedName  
    CleanupStack::PopAndDestroy(3, brandID);
    }

// from MSPNotifyChangeObserver
void CPbk2ServiceManager::HandleNotifyChange( TUint /*aServiceId*/ )
    {
    iRunRefreshData = ETrue;
    }

// from MSPNotifyChangeObserver
void CPbk2ServiceManager::HandleError( TInt /*aError*/ )
    {
    }


// End of File
