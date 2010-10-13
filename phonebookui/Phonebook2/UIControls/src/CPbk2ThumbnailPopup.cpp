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
* Description:  Phonebook 2 thumbnail popup.
*
*/


// INCLUDE FILES

#include "CPbk2ThumbnailPopup.h"

// Phonebook 2
#include "CPbk2ThumbnailPopupControl.h"
#include "CPbk2ThumbnailScaler.h"
#include <CPbk2ImageManager.h>
#include <CPbk2IconFactory.h>
#include <CPbk2IconInfoContainer.h>
#include <TPbk2AppIconId.h>
#include <TPbk2ImageManagerParams.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <MVPbkBaseContact.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <TVPbkFieldTypeMapping.h>

// System includes
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsUtils.h>
#include <barsread.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>


/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicInvariant_InvalidTimerState,
    EPanicPreCond_Pbk2ThumbnailGetComplete,
    EPanicPreCond_Pbk2ThumbnailGetFailed,
    EPanicPreCond_LoadContactL,
    EPanicPreCond_ShowPopupBitmap
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ThumbnailPopup");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace

/**
 * Observer for the thumbnail scaling process.
 */
NONSHARABLE_CLASS(CPbk2ThumbnailScalerObserver) :
            public CBase,
            public MPbk2ThumbnailScalerObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aPopup    Popup that is used to show the bitmap.
         * @return  A new instance of this class.
         */
        static CPbk2ThumbnailScalerObserver* NewL(
                CPbk2ThumbnailPopup& aPopup );

        /**
         * Standard C++ destructor.
         */
        ~CPbk2ThumbnailScalerObserver();

    private: // From MPbk2ThumbnailScalerObserver
        void ThumbnailScalingComplete(
                TInt aError,
                CFbsBitmap* aBitmap );

    private: // Implementation
        CPbk2ThumbnailScalerObserver(
                CPbk2ThumbnailPopup& aPopup );

    private: // Data
        /// Ref: Popup that is used to show the bitmap
        CPbk2ThumbnailPopup& iPopup;
    };

// --------------------------------------------------------------------------
// CPbk2ThumbnailScalerObserver::CPbk2ThumbnailScalerObserver
// --------------------------------------------------------------------------
//
CPbk2ThumbnailScalerObserver::CPbk2ThumbnailScalerObserver
        ( CPbk2ThumbnailPopup& aPopup ) :
            iPopup(aPopup)
    {
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailScalerObserver::~CPbk2ThumbnailScalerObserver
// --------------------------------------------------------------------------
//
CPbk2ThumbnailScalerObserver::~CPbk2ThumbnailScalerObserver()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailScalerObserver::NewL
// --------------------------------------------------------------------------
//
CPbk2ThumbnailScalerObserver* CPbk2ThumbnailScalerObserver::NewL
        ( CPbk2ThumbnailPopup& aPopup )
    {
    return new ( ELeave ) CPbk2ThumbnailScalerObserver( aPopup );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailScalerObserver::ThumbnailScalingComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailScalerObserver::ThumbnailScalingComplete
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

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::CPbk2ThumbnailPopup
// --------------------------------------------------------------------------
//
inline CPbk2ThumbnailPopup::CPbk2ThumbnailPopup
    (CVPbkContactManager& aContactManager)
    : iContactManager(aContactManager)
    {
    __ASSERT_DEBUG( !iThumbnailPopupControl,
        Panic(EPanicPostCond_Constructor));

    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::~CPbk2ThumbnailPopup
// --------------------------------------------------------------------------
//
CPbk2ThumbnailPopup::~CPbk2ThumbnailPopup()
    {
    delete iThumbnailScaler;
    delete iScalerObserver;
    delete iThumbOperation;
    delete iThumbnailPopupControl;
    delete iThumbnailBackg;
    delete iThumbnailBackgMask;
    delete iManager;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ThumbnailPopup::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::NewL
// --------------------------------------------------------------------------
//
CPbk2ThumbnailPopup* CPbk2ThumbnailPopup::NewL
        ( CVPbkContactManager& aContactManager )
    {
    CPbk2ThumbnailPopup* self =
        new (ELeave) CPbk2ThumbnailPopup(aContactManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::Load
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::Load
        ( MVPbkBaseContact& aContact, MPbk2ThumbnailPopupObserver& aObserver,
        const CEikListBox* aListBox, const MVPbkFieldType* aFieldType )
    {
    iListBox = aListBox;
    iLoadObserver = &aObserver;
    TRAPD(err, DoLoadL(aContact, aFieldType));
    if ( err != KErrNone)
        {
        iLoadObserver->PopupLoadFailed( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::HideAndCancel
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::HideAndCancel()
    {
    HideControl();

    // Cancel thumbnail loading
    delete iThumbOperation;
    iThumbOperation = NULL;
    // Cancel thumbnail scaling
    delete iThumbnailScaler;
    iThumbnailScaler = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::ShowPopupBitmap
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::ShowPopupBitmap(CFbsBitmap* aBitmap)
    {
    iBitmap=aBitmap;
    const TInt orientation( Layout_Meta_Data::IsLandscapeOrientation() ? 0 : 1 );

    /**
    * The thumbnail window requires size equal to the
    * thumbnail size + the space needed for bg graphics (see LAF documentation)
    */
    TAknLayoutRect thumbWindowLayoutRect, thumbBgLayoutRect, thumbLayoutRect;

    thumbWindowLayoutRect.LayoutRect( iListBox->View()->ViewRect(),
                                      AknLayoutScalable_Apps::popup_phob_thumbnail2_window( orientation ) );

    thumbBgLayoutRect.LayoutRect( thumbWindowLayoutRect.Rect(),
                                  AknLayoutScalable_Apps::bg_popup_preview_window_pane_cp01( orientation ) );

    thumbLayoutRect.LayoutRect( thumbWindowLayoutRect.Rect(),
                                AknLayoutScalable_Apps::popup_phob_thumbnail2_window_g1( orientation ) );

    const TSize bgSpace( thumbBgLayoutRect.Rect().Size() - thumbLayoutRect.Rect().Size() );
    const TSize bgSize( aBitmap->SizeInPixels() + bgSpace );

    // scale the background bitmaps
    AknIconUtils::SetSize( iThumbnailBackg,
                           bgSize,
                           EAspectRatioNotPreserved );

    AknIconUtils::SetSize( iThumbnailBackgMask,
                           bgSize,
                           EAspectRatioNotPreserved );

    iThumbnailPopupControl->SetThumbnailBackg( iThumbnailBackg, iThumbnailBackgMask );
    iThumbnailPopupControl->SetBitmap( aBitmap );
    iThumbnailPopupControl->MakeControlVisible( ETrue );

    iLoadObserver->PopupLoadComplete();
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::Refresh
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::Refresh()
    {
    if (( iThumbnailPopupControl ) && (iBitmap))
        {
        iThumbnailPopupControl->MakeControlVisible( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::SetThumbnailLocation
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::SetThumbnailLocation
        ( TPbk2ThumbnailLocation aThumbnailLocation )
    {
    iThumbnailLocation = aThumbnailLocation;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::Pbk2ImageGetComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::Pbk2ImageGetComplete
        (MPbk2ImageOperation& aOperation,
        CFbsBitmap* aBitmap)
    {
    __ASSERT_DEBUG(&aOperation==iThumbOperation && aBitmap
        && iThumbnailPopupControl && iThumbnailScaler,
        Panic(EPanicPreCond_Pbk2ThumbnailGetComplete));

    // Suppress the unreferenced parameter warning
    (void) aOperation;

    iThumbnailScaler->CreateThumbnail(aBitmap);

    delete iThumbOperation;
    iThumbOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::Pbk2ImageGetFailed
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::Pbk2ImageGetFailed
        (MPbk2ImageOperation& aOperation,
        TInt aError)
    {
    __ASSERT_DEBUG(iThumbOperation == &aOperation &&
                    aError != KErrNone &&
                    iLoadObserver,
            Panic(EPanicPreCond_Pbk2ThumbnailGetFailed));

    // Suppress the unreferenced parameter warnings
    (void) aError;
    (void) aOperation;

    HideControl();
    delete iThumbOperation;
    iThumbOperation = NULL;

    iLoadObserver->PopupLoadFailed(aError);
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::DoLoadL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::DoLoadL
        (MVPbkBaseContact& aContact,
        const MVPbkFieldType* aFieldType)
    {
    HideAndCancel();

    if ( !aFieldType )
        {
        ReadFieldTypeL();
        }
    else
        {
        iFieldType = aFieldType;
        }

    LoadThumbnailBackgL();
    GetThumbnailL( aContact, *iFieldType );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::HideControl
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::HideControl()
    {
    if (iThumbnailPopupControl)
        {
        // hide the control
        iThumbnailPopupControl->MakeControlVisible(EFalse);
        iThumbnailPopupControl->SetBitmap(NULL);
        iBitmap = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::GetThumbnailL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::GetThumbnailL
            ( MVPbkBaseContact& aContact, const MVPbkFieldType& aFieldType )
    {
    if (!iManager)
        {
        iManager = CPbk2ImageManager::NewL(iContactManager);
        }

    if (iManager->HasImage(aContact, aFieldType))
        {
        if (!iThumbnailPopupControl )
            {
            iThumbnailPopupControl =
                CPbk2ThumbnailPopupControl::NewL( iThumbnailLocation, iListBox );
            }
        if (!iScalerObserver)
            {
            iScalerObserver = CPbk2ThumbnailScalerObserver::NewL(*this);
            }
        if (!iThumbnailScaler)
            {
            iThumbnailScaler = CPbk2ThumbnailScaler::NewL(*iScalerObserver);
            }

        // start loading picture
        delete iThumbOperation;
        iThumbOperation = NULL;

        TPbk2ImageManagerParams params;
        SetupImageParams( params );

        iThumbOperation =
            iManager->GetImageAsyncL( &params, aContact, aFieldType, *this);
        }
    else if (iThumbnailPopupControl)
    	{
    	iThumbnailPopupControl->SetBitmap(NULL);
    	}

    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::ReadFieldTypeL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::ReadFieldTypeL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( reader, R_PBK2_THUMBNAIL_FIELD_TYPE );
    const TInt count = reader.ReadInt16();

    TVPbkFieldTypeMapping mapping(reader);
    iFieldType = mapping.FindMatch(iContactManager.FieldTypes());
    if (!iFieldType)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PopAndDestroy(); // CreateResourceReaderLC
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::LoadThumbnailBackgL
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::LoadThumbnailBackgL()
    {
    delete iThumbnailBackg;
    iThumbnailBackg = NULL;
    delete iThumbnailBackgMask;
    iThumbnailBackgMask = NULL;

    CPbk2IconInfoContainer* iconInfoContainer =
        CPbk2IconInfoContainer::NewL( R_PBK2_GRAF_BG_THUMBNAIL );
    CleanupStack::PushL( iconInfoContainer );
    CPbk2IconFactory* factory =
        CPbk2IconFactory::NewLC( *iconInfoContainer );

    factory->CreateIconL(
        TPbk2AppIconId( EPbk2qgn_graf_phob_thumbnail_backg ),
        *AknsUtils::SkinInstance(),
        iThumbnailBackg,
        iThumbnailBackgMask );

    CleanupStack::PopAndDestroy(2); // factory, iconInfoContainer
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailPopup::SetupImageParams
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailPopup::SetupImageParams
        ( TPbk2ImageManagerParams& aParams )
    {
    const TInt orientation( Layout_Meta_Data::IsLandscapeOrientation() ? 0 : 1 );
    TAknLayoutRect thumbWindowLayoutRect, thumbLayoutRect;

    thumbWindowLayoutRect.LayoutRect(
        iListBox->View()->ViewRect(),
        AknLayoutScalable_Apps::popup_phob_thumbnail2_window( orientation ) );

    thumbLayoutRect.LayoutRect(
        thumbWindowLayoutRect.Rect(),
        AknLayoutScalable_Apps::popup_phob_thumbnail2_window_g1( orientation ) );

    aParams.iSize = thumbLayoutRect.Rect().Size();
    aParams.iFlags = TPbk2ImageManagerParams::EScaleImage;
    }


// End of File
