/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for fetching ringing tones into phonebook.
*
*/


// INCLUDE FILES
#include    "CPbkRingingToneFetch.h"
#include    <cntfldst.h>
#include    <coemain.h>
#include    <CFLDFileListContainer.h>
#include    <PbkView.rsg>
#include    <CPbkContactEngine.h>
#include    <CPbkFieldsInfo.h>
#include    <CPbkFieldInfo.h>
#include    <CPbkContactItem.h>
#include    <TPbkContactItemField.h>
#include    <CPbkConstants.h>
#include    <featmgr.h>
#include    <DRMHelper.h>
#include    <centralrepository.h>
#include    <ProfileEngineDomainCRKeys.h>


// ================= MEMBER FUNCTIONS =======================
inline CPbkRingingToneFetch::CPbkRingingToneFetch
        (CPbkContactEngine& aEngine) :
        iEngine(aEngine)
    {
    }

inline void CPbkRingingToneFetch::ConstructL()
    {
    FeatureManager::InitializeLibL();
    CCoeEnv* coeEnv = CCoeEnv::Static();
    iTitle = coeEnv->AllocReadResourceL(R_QTN_TC_POPUP_HEADING);
    iNoSound = coeEnv->AllocReadResourceL(R_QTN_PHOP_SELI_DEFAULT_RTONE);
    }

EXPORT_C CPbkRingingToneFetch* CPbkRingingToneFetch::NewL(CPbkContactEngine& aEngine)
    {
    CPbkRingingToneFetch* self = new(ELeave) CPbkRingingToneFetch(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkRingingToneFetch::~CPbkRingingToneFetch()
    {
    // delete data members
    delete iNoSound;
    delete iTitle;
    FeatureManager::UnInitializeLib();
    }

EXPORT_C TBool CPbkRingingToneFetch::FetchRingingToneL
        (TFileName& aRingingToneFile)
    {
    CFLDFileListContainer* fileListDlg = CFLDFileListContainer::NewLC();
    fileListDlg->InsertNullItemL(*iNoSound);
    SetMaxToneFileSizeL( fileListDlg );
    fileListDlg->SetAutomatedType(CDRMHelper::EAutomatedTypeRingingTone);
    TBool result = fileListDlg->LaunchL(aRingingToneFile, *iTitle);
    CleanupStack::PopAndDestroy(fileListDlg);

    return result;
    }

EXPORT_C void CPbkRingingToneFetch::SetRingingToneL
        (CPbkContactItem& aItem,
        TFileName& aRingingToneFile)
    {
    TPbkContactItemField* field = aItem.FindField(EPbkFieldIdPersonalRingingToneIndication);

    if (aRingingToneFile.Compare(KNullDesC) == 0)
        {
        // remove ringing tone field
        if (field)
            {
            const TDesC& oldRingingToneFile = field->Text();
            HandleRingingToneRemovalL(oldRingingToneFile);
            aItem.RemoveField(aItem.FindFieldIndex(*field));
            }
        }
    else
        {
        // ringing tone selected
        if (field)
            {
            // there was an old ringing tone stored => handle removal
            const TDesC& oldRingingToneFile = field->Text();
            HandleRingingToneRemovalL(oldRingingToneFile);
            field->TextStorage()->SetTextL(aRingingToneFile);
            }
        else
            {
            // create ringing tone field
            CPbkFieldInfo* fieldInfo = iEngine.FieldsInfo().Find(EPbkFieldIdPersonalRingingToneIndication);
            TPbkContactItemField& newField = aItem.AddFieldL(*fieldInfo);
            newField.TextStorage()->SetTextL(aRingingToneFile);
            }
        }
    }

void CPbkRingingToneFetch::HandleRingingToneRemovalL
        (const TDesC& aOldTone)
    {
    if (!FeatureManager::FeatureSupported(KFeatureIdDrmFull))
        {
        return;
        }

    if (aOldTone.Compare(KNullDesC) != 0)
        {
        CDRMHelper* drmHelper = CDRMHelper::NewL();
        drmHelper->RemoveAutomatedPassive(aOldTone);
        delete drmHelper;
        }
    }

// -----------------------------------------------------------------------------
// CPbkRingingToneFetch::SetMaxToneFileSizeL
// -----------------------------------------------------------------------------
//
void CPbkRingingToneFetch::SetMaxToneFileSizeL( CFLDFileListContainer* aFl )
    {
    // Set file size limit if configured ON.
    TInt sizeLimitB( 0 );
    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );

    User::LeaveIfError( cenrep->Get( KProEngRingingToneMaxSize,
                                     sizeLimitB ) );
    CleanupStack::PopAndDestroy(); // cenrep
    if ( sizeLimitB < 0 )
        {
        sizeLimitB = 0;
        }
    sizeLimitB *= 1024;
    if ( sizeLimitB )
        {
        aFl->SetMaxFileSize( sizeLimitB );
        }
    }

//  End of File
