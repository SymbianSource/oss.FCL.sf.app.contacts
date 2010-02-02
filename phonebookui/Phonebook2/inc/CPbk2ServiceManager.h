/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Service manager for external service providers.
*
*/


#ifndef CPBK2SERVICEMANAGER_H_
#define CPBK2SERVICEMANAGER_H_

#include <spdefinitions.h>
#include <Pbk2InternalUID.h>

#include <mspnotifychangeobserver.h>
#include <spsettings.h>
//We use unique uid to provide private id space for external icons
const TInt32 KPbk2ServManId = 0x2001B2C7;

class RFs;
class CFbsBitmap;
class CSPSettings;
class CSPNotifyChange;

NONSHARABLE_CLASS(CPbk2ServiceManager) : public CBase,
    public MSPNotifyChangeObserver
    {
    public:
        enum TFlags
            {
            EWellKnown = 0x01,
            EInstalled = 0x02
            };
        
        struct TService
            {
            TPtrC iName;
            TPtrC iDisplayName;
            TInt iFlags;
            CFbsBitmap* iBitmap;
            CFbsBitmap* iMask;
            TInt iBitmapId;

            TPtrC iBrandId;
            TPtrC8 iApplicationId;
            TServiceId iServiceId;            
            
            void Delete();
            };
        
        typedef RArray<TService> RServicesArray;

    public:
        IMPORT_C static CPbk2ServiceManager* NewL(RFs& aRFs);
        ~CPbk2ServiceManager();
        
        IMPORT_C const RServicesArray& Services();

        //Call this to refresh the brand data.
        //RefreshDataL is needed because currently branding server
        //does not notify any brand updates (e.g. installation of
        //new brand packages). When notifications in future are
        //provided, this function can be removed from Phonebook.
        IMPORT_C void RefreshDataL();

    private:
        
        // from MSPNotifyChangeObserver
        void HandleNotifyChange( TUint aServiceId );
        void HandleError( TInt aError );        
    private:
        CPbk2ServiceManager(RFs& aRFs);
        void ConstructL();
        void DeleteData();
        void RunRefreshDataL();
        
        void ReadInstalledL();
        void ReadWellKnownServicesL();        
        void AppendInstalledServicesL(); 
        void UpdateServiceL(
            TService& aService,
            TServiceId aServiceId, 
            const TDesC& aServiceName);
        void AppendServiceL(TService aService);
        void UpdateBrandingInfoL();  
        void UpdateBrandL( TService& aService, TLanguage aLanguage ); 

    private:
        CSPSettings* iSettings;
        RServicesArray iServices;
        RPointerArray<HBufC> iStringsOwner; //for retaining ownership here 
        TInt iBitmapIdCounter;
        CSPNotifyChange* iSPNotifyChange;   
        RIdArray iSpIdsToObserve;
        RFs& iRFs;
        TBool iRunRefreshData;
    };


#endif /*CPBK2SERVICEMANAGER_H_*/
