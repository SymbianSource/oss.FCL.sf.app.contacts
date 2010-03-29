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
* Description:  Provides methods for fetching ringing tones into Phonebook.
*
*/


// INCLUDE FILES
#include <CPbk2RingingToneFetch.h>

// Phonebook 2
#include <Pbk2CommonUi.rsg>

// System includes
#include <cntfldst.h>
#include <coemain.h>
#include <mediafilelist.h>
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h>
#include <DRMHelper.h>

// ================= MEMBER FUNCTIONS =======================
inline CPbk2RingingToneFetch::CPbk2RingingToneFetch()
    {
    }

CPbk2RingingToneFetch::~CPbk2RingingToneFetch()
    {
    delete iNoSound;
    delete iTitle;
    }

EXPORT_C CPbk2RingingToneFetch* CPbk2RingingToneFetch::NewL()
    {
    CPbk2RingingToneFetch* self = new ( ELeave ) CPbk2RingingToneFetch();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

inline void CPbk2RingingToneFetch::ConstructL()
    {
    CCoeEnv* coeEnv = CCoeEnv::Static();
    iTitle = coeEnv->AllocReadResourceL( R_QTN_TC_POPUP_HEADING );
    iNoSound = coeEnv->AllocReadResourceL( R_QTN_PHOP_SELI_DEFAULT_RTONE );
    }

EXPORT_C TBool CPbk2RingingToneFetch::FetchRingingToneL
        ( TFileName& aRingingToneFile )
    {
    // Fetch the DefaultToneFile from Profile
    TFileName   ringingToneFile;

    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );

    User::LeaveIfError( cenrep->Get( KProEngDefaultRingingTone,
    		                            ringingToneFile ) );
    CleanupStack::PopAndDestroy(); // cenrep
    
    CMediaFileList* list = CMediaFileList::NewL();
    CleanupStack::PushL( list );
    
    list->SetNullItemL( *iNoSound, ringingToneFile,  
            CMediaFileList::EMediaFileTypeAudio,
            CMediaFileList::ENullItemIconDefaultTone );
    list->SetAttrL( CMediaFileList::EAttrTitle, *iTitle );
    SetMaxToneFileSizeL( list );
    TInt nullItem = KErrNotFound;
    list->SetAttrL( CMediaFileList::EAttrAutomatedType,
            CDRMHelper::EAutomatedTypeRingingTone );
    TBool result = list->ShowMediaFileListL( &aRingingToneFile,
            &nullItem, NULL, NULL );
    CleanupStack::PopAndDestroy( list );
        
    // Set result to be ETrue if nullItem (like default tone) 
    // is selected in media file list.
    if ( KErrNotFound != nullItem )
        {
        //if DefaultTone is selected, then empty aRingingToneFile
        if( !ringingToneFile.Compare(aRingingToneFile) )
        	{
        	aRingingToneFile.Zero();
        	}
        result = ETrue;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2RingingToneFetch::SetMaxToneFileSizeL
// Limit the size of ringing tone files shown on the file list
// according to cenrep key.
//
// @param aFL The ringing tone file list to limit
// --------------------------------------------------------------------------
//
void CPbk2RingingToneFetch::SetMaxToneFileSizeL( CMediaFileList* aFl )
    {
    // Set file size limit if configured ON.
    TInt sizeLimitKB( 0 );
    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );

    User::LeaveIfError( cenrep->Get( KProEngRingingToneMaxSize,
            sizeLimitKB ) );
    CleanupStack::PopAndDestroy(); // cenrep
    if ( sizeLimitKB < 0 )
        {
        sizeLimitKB = 0;
        }
    
    if ( sizeLimitKB )
        {
       aFl->SetAttrL( CMediaFileList::EAttrFileSize, sizeLimitKB );
        }
    }

//  End of File
