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
*       Provides methods for the thumbnail popup control on top of the context pane.
*
*/


// INCLUDE FILES
#include    "CPbkThumbnailPopup.h"
#include    "CPbkThumbnailPopupControl.h"
#include    "CPbkThumbnailPopupControlSlim.h"
#include    "CPbkThumbnailManager.h"
#include    "CPbkThumbnailScaler.h"
#include    "CPbkSwapContextPaneIcon.h"
#include    "PbkIconInfo.h"

#include    <CPbkContactEngine.h>
#include    <CPbkContactItem.h>
#include    <CPbkConstants.h>
#include    <PbkView.rsg>
#include    <AknsUtils.h>
#include <layoutmetadata.cdl.h>

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
	{
    EPanicPostCond_Constructor = 1,
    EPanicPostCond_CancelLoading,
    EPanicInvariant_InvalidTimerState,
    EPanicPreCond_PbkThumbnailGetComplete,
    EPanicPreCond_PbkThumbnailGetFailed,
    EPanicPreCond_LoadContactL
	};

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkThumbnailPopup");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

/**
 * Observer for the thumbnail scaling process.
 */
NONSHARABLE_CLASS(CPbkThumbnailScalerObserver) :
        public CBase,
        public MPbkThumbnailScalerObserver
    {
    public: // Construction
        /**
         * Static 2-phased constructor.
         * @param aPopup Popup that is used to show the bitmap.
         * @return Newly created CPbkThumbnailScalerObserver.
         */
        static CPbkThumbnailScalerObserver* NewL(CPbkThumbnailPopup& aPopup);

        /**
         * Standard C++ destructor.
         */
        ~CPbkThumbnailScalerObserver();
    private: // Implementation
        /**
         * C++ constructor.
         * @param aPopup Popup that is used to show the bitmap.
         */
        CPbkThumbnailScalerObserver(CPbkThumbnailPopup& aPopup);

    private: // From MPbkThumbnailScalerObserver
        void ThumbnailScalingComplete(TInt aError, CFbsBitmap* aBitmap);

    private: // Data
        /// Ref: Popup that is used to show the bitmap.
        CPbkThumbnailPopup& iPopup;
    };

CPbkThumbnailScalerObserver* CPbkThumbnailScalerObserver::NewL
        (CPbkThumbnailPopup& aPopup)
    {
    return new(ELeave) CPbkThumbnailScalerObserver(aPopup);
    }

CPbkThumbnailScalerObserver::CPbkThumbnailScalerObserver
        (CPbkThumbnailPopup& aPopup) :
    iPopup(aPopup)
    {
    }

CPbkThumbnailScalerObserver::~CPbkThumbnailScalerObserver()
    {
    }

void CPbkThumbnailScalerObserver::ThumbnailScalingComplete
        (TInt aError, CFbsBitmap* aBitmap)
    {
    if (aError == KErrNone)
        {
        iPopup.ShowPopupBitmap(aBitmap);
        }
    else
        {
        delete aBitmap;
        }
    }

// CPbkThumbnailPopup
inline CPbkThumbnailPopup::CPbkThumbnailPopup
        (CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    __ASSERT_DEBUG( !iThumbnailPopupControl,
        Panic(EPanicPostCond_Constructor));        
    
    }

EXPORT_C CPbkThumbnailPopup* CPbkThumbnailPopup::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkThumbnailPopup* self = new (ELeave) CPbkThumbnailPopup(aEngine);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); //self
    return self;
    }

void CPbkThumbnailPopup::ConstructL()
    {
    iSwapContextIcon = CPbkSwapContextPaneIcon::NewL( *CEikonEnv::Static() );
    LoadThumbnailBackgL();
    }

CPbkThumbnailPopup::~CPbkThumbnailPopup()
    {    
    delete iSwapContextIcon;
    delete iThumbnailScaler;
    delete iScalerObserver;    
    delete iThumbOperation;
    delete iThumbnailPopupControl;
    delete iThumbnailBackg;
    delete iThumbnailBackgMask;
    delete iManager;
    delete iLoadedItem;
    }

EXPORT_C void CPbkThumbnailPopup::Load
        (TContactItemId aContactId,
        TBool /*aDelay*/)
    {
    TRAP_IGNORE(DoLoadL(aContactId));
    }

EXPORT_C void CPbkThumbnailPopup::Load
        (const CPbkContactItem& aContactItem,
        TBool /*aDelay*/)
    {
    TRAP_IGNORE(DoLoadL(aContactItem));
    }
    
void CPbkThumbnailPopup::Load
        (TContactItemId aContactId,
        CEikListBox* aListBox )
    {
    iListBox = aListBox;
    TRAP_IGNORE(DoLoadL(aContactId));
    }

void CPbkThumbnailPopup::Load
        (const CPbkContactItem& aContactItem,
        CEikListBox* aListBox )
    {
    iListBox = aListBox;
    TRAP_IGNORE(DoLoadL(aContactItem));
    }


EXPORT_C void CPbkThumbnailPopup::CancelLoading()
    {    
    HideControl();

    // Cancel thumbnail loading
    delete iThumbOperation;
    iThumbOperation = NULL;
    // Cancel thumbnail scaling
    delete iThumbnailScaler;
    iThumbnailScaler = NULL;
    // Delete any loaded contact
    delete iLoadedItem;
    iLoadedItem = NULL;
    }

TContactItemId CPbkThumbnailPopup::ContactId() const
    {
    if (iContactItem)
        {
        return iContactItem->Id();
        }
    else
        {
        return iLoadContactId;
        }
    }

void CPbkThumbnailPopup::DoLoadL
        (TContactItemId aContactId)
    {    
    CancelLoading();

    iLoadContactId = aContactId;
    iContactItem = NULL;

    LoadContactL();    
    
    }

void CPbkThumbnailPopup::DoLoadL
        (const CPbkContactItem& aContactItem)
    {    
    CancelLoading();

    iContactItem = &aContactItem;
    iLoadContactId = KNullContactId;
    GetThumbnailL(*iContactItem);    
    }

void CPbkThumbnailPopup::__DbgTestInvariant() const
    {    
    }

void CPbkThumbnailPopup::PbkThumbnailGetComplete
        (MPbkThumbnailOperation& aOperation,
        CFbsBitmap* aBitmap)
    {    
    __ASSERT_DEBUG(&aOperation==iThumbOperation && aBitmap
		&& iThumbnailPopupControl && iThumbnailScaler,
        Panic(EPanicPreCond_PbkThumbnailGetComplete));

	// Suppress the unreferenced parameter warning
	(void) aOperation;

    iThumbnailScaler->CreateThumbnail(aBitmap);

    delete iThumbOperation;
    iThumbOperation = NULL;    
    }

void CPbkThumbnailPopup::ShowPopupBitmap(CFbsBitmap* aBitmap)
    {    
    // set the scaled bitmap to the control
    AknIconUtils::SetSize( iThumbnailBackg, aBitmap->SizeInPixels(), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iThumbnailBackgMask, aBitmap->SizeInPixels(), EAspectRatioNotPreserved );
    
    iThumbnailPopupControl->SetThumbnailBackg( iThumbnailBackg, iThumbnailBackgMask );
    iThumbnailPopupControl->ChangeBitmap( aBitmap, iListBox );
    ShowControl();    
    }

void CPbkThumbnailPopup::PbkThumbnailGetFailed
        (MPbkThumbnailOperation& aOperation,
        TInt aError)
    {    
    __ASSERT_DEBUG(iThumbOperation == &aOperation && aError != KErrNone,
            Panic(EPanicPreCond_PbkThumbnailGetFailed));

	// Suppress the unreferenced parameter warnings
	(void) aError;
	(void) aOperation;

    HideControl();
    delete iThumbOperation;
    iThumbOperation = NULL;    
    }

void CPbkThumbnailPopup::ShowControl()
    {
    if ( iThumbnailPopupControl )
        {
        iSwapContextIcon->ShowContextPaneIcon( CPbkSwapContextPaneIcon::EEmptyIcon );
        iThumbnailPopupControl->MakeControlVisible( ETrue );
        }
    }

void CPbkThumbnailPopup::HideControl()
    {
    if (iThumbnailPopupControl)
        {
        // hide the control
        iSwapContextIcon->ShowContextPaneIcon( CPbkSwapContextPaneIcon::EPhonebookIcon );
        iThumbnailPopupControl->MakeControlVisible(EFalse);
        }    
    }

void CPbkThumbnailPopup::LoadContactL()
    {    
    __ASSERT_DEBUG(iLoadContactId!=KNullContactId,
        Panic(EPanicPreCond_LoadContactL));

    // Load contact item
    CPbkContactItem* loadedItem = iEngine.ReadContactL(iLoadContactId);
    delete iLoadedItem;
    iLoadedItem = loadedItem;

    // start loading picture
    GetThumbnailL(*iLoadedItem);   
    }

void CPbkThumbnailPopup::GetThumbnailL(const CPbkContactItem& aItem)
    {   
    if (!iManager)
        {
        iManager = CPbkThumbnailManager::NewL(iEngine);
        }

    if (iManager->HasThumbnail(aItem))
        {
        TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();        
        if (!iThumbnailPopupControl || iLandscape != landscape )
            {
            delete iThumbnailPopupControl;
            iThumbnailPopupControl = NULL;
            iLandscape = landscape;
            if ( landscape )
                {
                iThumbnailPopupControl = CPbkThumbnailPopupControlSlim::NewL();
                }
            else
                {
                iThumbnailPopupControl = CPbkThumbnailPopupControl::NewL();                
                }
            }
        if (!iScalerObserver)
            {
            iScalerObserver = CPbkThumbnailScalerObserver::NewL(*this);
            }
        if (!iThumbnailScaler)
            {
            iThumbnailScaler = CPbkThumbnailScaler::NewL(*iScalerObserver);
            }

        // start loading picture
        delete iThumbOperation;
        iThumbOperation = NULL;
        iThumbOperation = iManager->GetThumbnailAsyncL(*this, aItem);
        }    
    }

void CPbkThumbnailPopup::LoadThumbnailBackgL()
    {
    CPbkIconInfoContainer* iconInfoContainer =
		CPbkIconInfoContainer::NewL(R_PBK_GRAF_BG_THUMBNAIL, CCoeEnv::Static());
	CleanupStack::PushL( iconInfoContainer );
    const TPbkIconInfo* iconInfo = 
        iconInfoContainer->Find( EPbkqgn_graf_phob_thumbnail_backg );

    AknsUtils::CreateIconL(
        AknsUtils::SkinInstance(),
        iconInfo->SkinId(),
        iThumbnailBackg,
        iThumbnailBackgMask,
        iconInfo->MbmFileName(),
        iconInfo->IconId(),
        iconInfo->MaskId() );
        
    CleanupStack::PopAndDestroy(); //iconInfoContainer
    }
    
void CPbkThumbnailPopup::Refresh()
    {
    // This method can be called when iThumbnailPopupControl is NULL
    if ( iThumbnailPopupControl )
        {
        iThumbnailPopupControl->ChangeBitmap( NULL, iListBox );    
        }    
    }


//  End of File
