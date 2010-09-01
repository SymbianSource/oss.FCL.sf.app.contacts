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
*       Methods for Common control container class.
*
*/


// INCLUDE FILES
#include    <CPbkControlContainer.h>
#include    <MPbkKeyEventHandler.h>


/// Unnamed namespace for local definitions
namespace {

// MODULE DATA STRUCTURES

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
    _LIT(KPanicText, "CPbkControlContainerImpl");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkControlContainerImpl::CPbkControlContainerImpl
        (MPbkKeyEventHandler* aKeyEventHandler) :
        iKeyEventHandler(aKeyEventHandler)
    {
    SetComponentsToInheritVisibility(ETrue);
    __ASSERT_DEBUG(!iControl, Panic(EPanicPostCond_Constructor));
    }

EXPORT_C void CPbkControlContainerImpl::ConstructL(MObjectProvider& aProvider)
    {
    // PreCond
    __ASSERT_DEBUG(!iControl, Panic(EPanicPreCond_ConstructL));

    SetMopParent(&aProvider);

    // Create a window, this is the top-level container control
    CreateWindowL();

    // PostCond
    __ASSERT_DEBUG(!iControl && OwnsWindow(), Panic(EPanicPostCond_ConstructL));
    }

EXPORT_C void CPbkControlContainerImpl::DestroyControl()
    {
    if (iOwnsControl)
        {
        delete iControl;
        iControl = NULL;
        __ASSERT_DEBUG(!CoeControl(), Panic(EPanicPostCond_DestroyControl));
        }
    }

EXPORT_C CPbkControlContainerImpl::~CPbkControlContainerImpl()
    {
    if (iOwnsControl)
		{
		delete iControl;
		}
    }

EXPORT_C CCoeControl* CPbkControlContainerImpl::CoeControl() const
    {
    return iControl;
    }

EXPORT_C MPbkKeyEventHandler* CPbkControlContainerImpl::KeyEventHandler() const
    {
    return iKeyEventHandler;
    }

EXPORT_C void CPbkControlContainerImpl::SetKeyEventHandler
        (MPbkKeyEventHandler* aKeyEventHandler)
    {
    iKeyEventHandler = aKeyEventHandler;
    }

EXPORT_C void CPbkControlContainerImpl::SetHelpContext
        (const TCoeHelpContext& aContext)
    {
    iHelpContext = aContext;
    }

EXPORT_C void CPbkControlContainerImpl::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext = iHelpContext;
    }

EXPORT_C void CPbkControlContainerImpl::SetCoeControl(
        CCoeControl* aControl, 
        const TRect& aRect, 
        TBool aOwnsControl/*=ETrue*/)
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

    // Set rectangle
    SetRect(aRect);

    __ASSERT_DEBUG(CoeControl()==aControl, Panic(EPanicPostCond_SetCoeControlL));
    }

EXPORT_C TKeyResponse CPbkControlContainerImpl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // Offer key event first to the key event handler
    if (iKeyEventHandler && iKeyEventHandler->PbkProcessKeyEventL(aKeyEvent,aType))
        {
        return EKeyWasConsumed;
        }
    // Not consumed by the key handler, offer to the control next
    if (iControl)
        {
        return iControl->OfferKeyEventL(aKeyEvent, aType);
        }
    return EKeyWasNotConsumed;
    }

EXPORT_C TInt CPbkControlContainerImpl::CountComponentControls() const
    {
    // iControl is the only subcontrol
    return iControl ? 1 : 0;
    }

EXPORT_C CCoeControl* CPbkControlContainerImpl::ComponentControl(TInt aIndex) const
    {
    // PreCond: Assert that the control is assigned and aIndex is sensible
    __ASSERT_DEBUG(iControl && aIndex==0, Panic(EPanicPreCond_ComponentControl));

	// Suppress the unreferenced variable warning
	(void) aIndex;

    // iControl is the only subcontrol
    return iControl;
    }

EXPORT_C void CPbkControlContainerImpl::SizeChanged()
    {
    if (iControl)
        {
        // Set the control's rectangle to fill this container's rectangle
        iControl->SetRect(Rect());
        }
    }

EXPORT_C void CPbkControlContainerImpl::FocusChanged(TDrawNow aDrawNow)
    {
	if (iControl && !IsNonFocusing())
        {
		iControl->SetFocus(IsFocused(), aDrawNow);
        }
    }


//  End of File  
