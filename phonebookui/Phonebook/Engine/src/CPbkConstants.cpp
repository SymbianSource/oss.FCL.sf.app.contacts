/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*        Implementation file for CPbkConstants class
*
*/


// INCLUDE FILES
#include    <CPbkConstants.h>
#include    <f32file.h>
#include    <barsc.h>
#include    <barsread.h>
#include    <pbkeng.rsg>
#include    "CPbkLocalVariationManager.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_UnnamedTitle = 1
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkConstants");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ==================== MEMBER FUNCTIONS ====================

inline CPbkConstants::CPbkConstants()
    {
    // CBase::operator new(TLeave) resets member data
    }

inline void CPbkConstants::ConstructL(RResourceFile& aResourceFile)
    {
    TResourceReader reader;
    reader.SetBuffer(aResourceFile.AllocReadLC(R_QTN_PHOB_UNNAMED));
    iUnnamedTitle = reader.ReadHBufCL();
    iLocalVariationManager = CPbkLocalVariationManager::NewL();
    CleanupStack::PopAndDestroy();  // reader
    }

EXPORT_C CPbkConstants* CPbkConstants::NewL(RResourceFile& aResourceFile)
    {
    CPbkConstants* self = new(ELeave) CPbkConstants;
    CleanupStack::PushL(self);
    self->ConstructL(aResourceFile);
    CleanupStack::Pop(self);
    return self;
    }

CPbkConstants::~CPbkConstants()
    {
    delete iUnnamedTitle;
    delete iLocalVariationManager;
    }

EXPORT_C const TDesC& CPbkConstants::UnnamedTitle() const
    {
    __ASSERT_DEBUG(iUnnamedTitle, Panic(EPanicPreCond_UnnamedTitle));
    return *iUnnamedTitle;
    }

EXPORT_C TInt CPbkConstants::GroupLabelLength()
    {
    return 35;
    }

EXPORT_C TInt CPbkConstants::FormEditorLength()
    {
    return 35;
    }

EXPORT_C TInt CPbkConstants::CompressionInactivityTimerLength()
    {
    return 10;
    }

EXPORT_C TInt CPbkConstants::RingingTonePreviewDelay()
    {   
    return 1000*1000; // 1 second
    }

EXPORT_C TInt CPbkConstants::ThumbnailPopupDelay()
    {
    return 200*1000; // wait time 200 milliseconds.
    }

EXPORT_C TInt CPbkConstants::FieldEditorLength()
    {
    return 8;
    }

EXPORT_C TInt CPbkConstants::SearchFieldLength()
    {
    return 40;
    }

EXPORT_C TBool CPbkConstants::LocallyVariatedFeatureEnabled
        (TPbkLocalVariantFlags aFeature)
    {
    return iLocalVariationManager->LocallyVariatedFeatureEnabled(aFeature);
    }


//  End of File 
