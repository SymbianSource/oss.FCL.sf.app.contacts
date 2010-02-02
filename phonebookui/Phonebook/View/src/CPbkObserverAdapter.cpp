/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "CPbkObserverAdapter.h"
#include <MPbkImageOperationObservers.h>

// ============================= LOCAL FUNCTIONS ===============================

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
/// Panic codes 
enum TPanicCode
    {    
    EPanicPreCond_PbkThumbnailGetComplete=1,
    EPanicPreCond_PbkThumbnailGetFailed,
    EPanicPreCond_PbkThumbnailSetComplete,
    EPanicPreCond_PbkThumbnailSetFailed,
    EPanicPreCond_PbkThumbnailGetImageComplete,
    EPanicPreCond_PbkThumbnailGetImageFailed,
    EPanicPreCond_PbkThumbnailGetObserver,
    EPanicPreCond_PbkThumbnailSetObserver,
    EPanicPreCond_PbkThumbnailGetImageObserver 

    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkObserverAdapter");
    User::Panic(KPanicText, aReason);
    }

}  // namespace


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbkObserverAdapter::CPbkObserverAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbkObserverAdapter::CPbkObserverAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CPbkObserverAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbkObserverAdapter::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CPbkObserverAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPbkObserverAdapter* CPbkObserverAdapter::NewL()
    {
    CPbkObserverAdapter* self = new( ELeave ) CPbkObserverAdapter;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CPbkObserverAdapter::~CPbkObserverAdapter()
    {    
    }

void CPbkObserverAdapter::DoSetImageGetObserver(MPbkImageGetObserver& 
    aPbkImageGetObserver)
    {
    iPbkImageGetObserver = &aPbkImageGetObserver;
    }

MPbkImageGetObserver* CPbkObserverAdapter::ImageGetObserver()
    {
    return iPbkImageGetObserver;
    }

void CPbkObserverAdapter::PbkThumbnailGetComplete
    (MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap)
    {
    __ASSERT_ALWAYS(iPbkImageGetObserver, Panic(
        EPanicPreCond_PbkThumbnailGetComplete));

    // MPbkImageOperation and MPbkThumbnailOperation are identical. 
    // Both contain only the destructor. More complex implementation avoided 
    // by using reinterpret cast. Otherwise there might be a need to generate 
    // a new thumbnail operation adapter instance when MPbkImageGetObserver's 
    // method is called once.
    
// TODO reinterpret_cast causes a crash 
// TO BE CONTINUED wrapper needed for MPbkImageOperation and MPbkThumbnailOperation conversion
    iPbkImageGetObserver->PbkImageGetComplete(
        reinterpret_cast<MPbkImageOperation&>(aOperation),aBitmap);
    }

void CPbkObserverAdapter::PbkThumbnailGetFailed
    (MPbkThumbnailOperation& aOperation, TInt aError)
    {
    __ASSERT_ALWAYS(iPbkImageGetObserver, Panic(
        EPanicPreCond_PbkThumbnailGetFailed));
    // MPbkImageOperation and MPbkThumbnailOperation are identical. 
    iPbkImageGetObserver->PbkImageGetFailed(
        reinterpret_cast<MPbkImageOperation&>(aOperation), aError);
    }


void CPbkObserverAdapter::DoSetImageSetObserver(MPbkImageSetObserver& 
    aPbkImageSetObserver)
    {
    iPbkImageSetObserver = &aPbkImageSetObserver;
    }

MPbkImageSetObserver* CPbkObserverAdapter::ImageSetObserver()
    {
    return iPbkImageSetObserver;
    }

void CPbkObserverAdapter::PbkThumbnailSetComplete(
    MPbkThumbnailOperation& aOperation)
    {
    __ASSERT_ALWAYS(iPbkImageSetObserver, Panic(
        EPanicPreCond_PbkThumbnailSetComplete));
    // MPbkImageOperation and MPbkThumbnailOperation are identical.
    iPbkImageSetObserver->PbkImageSetComplete(
        reinterpret_cast<MPbkImageOperation&>(aOperation));
    }
void CPbkObserverAdapter::PbkThumbnailSetFailed(
    MPbkThumbnailOperation& aOperation, TInt aError)
    {
    __ASSERT_ALWAYS(iPbkImageGetObserver, Panic(
        EPanicPreCond_PbkThumbnailSetFailed));
    // MPbkImageOperation and MPbkThumbnailOperation are identical.
    iPbkImageSetObserver->PbkImageSetFailed(
        reinterpret_cast<MPbkImageOperation&>(aOperation), aError);
    }

void CPbkObserverAdapter::DoSetImageGetImageObserver(MPbkImageGetImageObserver& 
            aPbkImageGetImageObserver)
    {
    iPbkImageGetImageObserver = &aPbkImageGetImageObserver;
    }

MPbkImageGetImageObserver* CPbkObserverAdapter::ImageGetImageObserver()
    {
    return iPbkImageGetImageObserver;
    }

void CPbkObserverAdapter::PbkThumbnailGetImageComplete
    (MPbkThumbnailOperation& aOperation, CPbkImageDataWithInfo* aImageData)
    {
    __ASSERT_ALWAYS(iPbkImageGetImageObserver, 
        Panic(EPanicPreCond_PbkThumbnailGetImageComplete));
    // MPbkImageOperation and MPbkThumbnailOperation are identical.
    iPbkImageGetImageObserver->PbkImageGetImageComplete(
        reinterpret_cast<MPbkImageOperation&>(aOperation), aImageData);
    }

void CPbkObserverAdapter::PbkThumbnailGetImageFailed
    (MPbkThumbnailOperation& aOperation, TInt aError)
    {
    __ASSERT_ALWAYS(iPbkImageGetImageObserver, 
        Panic(EPanicPreCond_PbkThumbnailGetImageFailed));
    // MPbkImageOperation and MPbkThumbnailOperation are identical.
    iPbkImageGetImageObserver->PbkImageGetImageFailed(
        reinterpret_cast<MPbkImageOperation&>(aOperation), aError);        
    }

void CPbkObserverAdapter::SetPbkImageGetObserver(MPbkImageGetObserver& aObserver)
    {
    // TODO: Multiple observer might need to be supported
    DoSetImageGetObserver(aObserver);
    }

MPbkThumbnailGetObserver& CPbkObserverAdapter::PbkThumbnailGetObserver()
    {
    MPbkImageGetObserver* currentObserver = ImageGetObserver();
    if (!currentObserver)
        {
        // Observer must be set before use of this function
        Panic( EPanicPreCond_PbkThumbnailGetObserver );
        }
    return *this;
    }

void CPbkObserverAdapter::SetPbkImageSetObserver(MPbkImageSetObserver& aObserver)
    {
    // TODO: Multiple observer might need to be supported
    DoSetImageSetObserver(aObserver);
    }

MPbkThumbnailSetObserver& CPbkObserverAdapter::PbkThumbnailSetObserver()
    {
    MPbkImageSetObserver* currentObserver = ImageSetObserver();
    if (!currentObserver)
        {
        // Observer must be set before use of this function
        Panic( EPanicPreCond_PbkThumbnailSetObserver );
        }
    return *this;
    }

void CPbkObserverAdapter::SetPbkImageGetImageObserver(
        MPbkImageGetImageObserver& aObserver)
    {
    // TODO: Multiple observer might need to be supported
    DoSetImageGetImageObserver(aObserver);
    }

MPbkThumbnailGetImageObserver& CPbkObserverAdapter::PbkThumbnailGetImageObserver()
    {
    MPbkImageGetImageObserver* currentObserver = ImageGetImageObserver();

    if (!currentObserver)
        {
        // Observer must be set before use of this function
        Panic( EPanicPreCond_PbkThumbnailGetImageObserver );
        }
    return *this;
    }

//  End of File  
