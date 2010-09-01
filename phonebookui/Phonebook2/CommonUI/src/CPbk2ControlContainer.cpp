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
*       Common control container class for Phonebook 2 application.
*
*/


// INCLUDE FILES
#include "CPbk2ControlContainer.h"

// Phonebook 2
#include <MPbk2KeyEventHandler.h>
#include <MPbk2PointerEventHandler.h>

// SYSTEM INCLUDES
#include <AknUtils.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPostCond_DestroyControl,
    EPanicPreCond_SetCoeControlL,
    EPanicPostCond_SetCoeControlL,
    EPanicPreCond_ComponentControl,
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2ControlContainerImpl" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} // namespace


// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::CPbk2ControlContainerImpl
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ControlContainerImpl::CPbk2ControlContainerImpl
        ( MPbk2KeyEventHandler* aKeyEventHandler,
          MPbk2PointerEventHandler* aPointerHandler ) :
            iKeyEventHandler( aKeyEventHandler ),
            iPointerEventHandler( aPointerHandler )
    {
    SetComponentsToInheritVisibility( ETrue );
    __ASSERT_DEBUG(!iControl, Panic(EPanicPostCond_Constructor));
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::~CPbk2ControlContainerImpl
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ControlContainerImpl::~CPbk2ControlContainerImpl()
    {
    if (iOwnsControl)
        {
        delete iControl;
        }
    delete iLongTapDetector;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::ConstructL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::ConstructL
        ( MObjectProvider& aProvider )
    {
    // PreCond
    __ASSERT_DEBUG(!iControl, Panic(EPanicPreCond_ConstructL));

    SetMopParent(&aProvider);

    // Create a window, this is the top-level container control
    CreateWindowL();

    // PostCond
    __ASSERT_DEBUG(!iControl && OwnsWindow(), Panic(EPanicPostCond_ConstructL));
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::CoeControl
// --------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CPbk2ControlContainerImpl::CoeControl() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::DestroyControl
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::DestroyControl()
    {
    if (iOwnsControl)
        {
        delete iControl;
        iControl = NULL;
        __ASSERT_DEBUG(!CoeControl(), Panic(EPanicPostCond_DestroyControl));
        }
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::KeyEventHandler
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2KeyEventHandler*
        CPbk2ControlContainerImpl::KeyEventHandler() const
    {
    return iKeyEventHandler;
    }
    
// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::PointerEventHandler
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2PointerEventHandler*
        CPbk2ControlContainerImpl::PointerEventHandler() const
    {
    return iPointerEventHandler;
    }    

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::SetKeyEventHandler
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::SetKeyEventHandler
        (MPbk2KeyEventHandler* aKeyEventHandler)
    {
    iKeyEventHandler = aKeyEventHandler;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::SetHelpContext
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::SetHelpContext
        (const TCoeHelpContext& aContext)
    {
    iHelpContext = aContext;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::GetHelpContext
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext = iHelpContext;
    }
    
// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::LongTapDetectorL
// --------------------------------------------------------------------------
//
EXPORT_C CAknLongTapDetector& CPbk2ControlContainerImpl::LongTapDetectorL()
    {
    if ( !iLongTapDetector )
        {
        iLongTapDetector = CAknLongTapDetector::NewL( this );
        }
    return *iLongTapDetector;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::SetCoeControl
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::SetCoeControl(
        CCoeControl* aControl,
        const TRect& aRect,
        TBool aOwnsControl /* = ETrue */ )
    {
    __ASSERT_DEBUG(OwnsWindow(), Panic(EPanicPreCond_SetCoeControlL));

    // Set the control
    if (aControl != iControl)
        {
        if (iOwnsControl)
            {
            delete iControl;
            }
        iControl = aControl;
        }

    // Store ownership request
    iOwnsControl = aOwnsControl;
    RemoveForegroundObserver();

    // Set rectangle
    SetRect(aRect);

    __ASSERT_DEBUG(CoeControl()==aControl, Panic(EPanicPostCond_SetCoeControlL));
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::OfferKeyEventL
// --------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CPbk2ControlContainerImpl::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
    ("CPbk2ControlContainerImpl::OfferKeyEventL(0x%x),Code=%d,Mod=%x,Type=%d"), 
        this, aKeyEvent.iCode, aKeyEvent.iModifiers, aType );
        
    TKeyResponse ret = EKeyWasNotConsumed;

    // Offer key event first to the key event handler
    if ( iKeyEventHandler && iKeyEventHandler->Pbk2ProcessKeyEventL
            ( aKeyEvent, aType ) )
        {
        ret = EKeyWasConsumed;
        }
    // Not consumed by the key handler, offer to the control next
    if ( ( ret == EKeyWasNotConsumed ) && iControl )
        {
        ret = iControl->OfferKeyEventL( aKeyEvent, aType );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::CountComponentControls
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2ControlContainerImpl::CountComponentControls() const
    {
    // iControl is the only subcontrol
    return iControl ? 1 : 0;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::ComponentControl
// --------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CPbk2ControlContainerImpl::ComponentControl
        ( TInt PBK2_DEBUG_ONLY( aIndex ) ) const
    {
    // PreCond: Assert that the control is assigned and aIndex is sensible
    __ASSERT_DEBUG( iControl && aIndex == 0,
        Panic( EPanicPreCond_ComponentControl ) );

    // iControl is the only subcontrol
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::SizeChanged
// This control is only a container; sets iControl's rectangle
// to this control's rectangle.
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::SizeChanged()
    {
    if ( iControl )
        {
        // Set the control's rectangle to fill this container's rectangle
        iControl->SetRect( Rect() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::FocusChanged
// Forwards focus changed events.
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ControlContainerImpl::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iControl && !IsNonFocusing() )
        {
        iControl->SetFocus( IsFocused(), aDrawNow );
        }
    
    if ( iObserver && iControl )
        {
        TRAP_IGNORE( iObserver->HandleForegroundEventL( IsFocused() ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::HandlePointerEventL
// --------------------------------------------------------------------------
//    
EXPORT_C void CPbk2ControlContainerImpl::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {    
    if ( AknLayoutUtils::PenEnabled() )
        { 
        // Offer pointer event first to the control
        if (  iControl )
            {
            iControl->HandlePointerEventL( aPointerEvent );
            }           
            
        // Offer pointer event next to the pointer event handler        
        if ( iPointerEventHandler )    
            {
            iPointerEventHandler->Pbk2ProcessPointerEventL( aPointerEvent );
            }             
        }
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::HandleLongTapEventL
// --------------------------------------------------------------------------
//    
EXPORT_C void CPbk2ControlContainerImpl::HandleLongTapEventL(
        const TPoint& aPenEventLocation, 
        const TPoint& aPenEventScreenLocation )
    {
    if ( AknLayoutUtils::PenEnabled() )
        { 
        // Offer long tap event to the pointer event handler
        if ( iPointerEventHandler )    
            {
            iPointerEventHandler->Pbk2HandleLongTapEventL(
                aPenEventLocation, aPenEventScreenLocation );
            }             
        }
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::AddForegroundObserver
// --------------------------------------------------------------------------
// 
EXPORT_C void CPbk2ControlContainerImpl::AddForegroundObserver
        ( MPbk2ControlContainerForegroundEventObserver* aObserver )
    {
    iObserver = aObserver;
    }


// --------------------------------------------------------------------------
// CPbk2ControlContainerImpl::RemoveForegroundObserver
// --------------------------------------------------------------------------
// 
EXPORT_C void CPbk2ControlContainerImpl::RemoveForegroundObserver()
    {
    iObserver = NULL;
    }

//  End of File
