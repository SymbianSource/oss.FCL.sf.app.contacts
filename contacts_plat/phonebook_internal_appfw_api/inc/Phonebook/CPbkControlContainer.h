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
*     Common control container class for Phonebook application.
*
*/


#ifndef __CPbkControlContainer_H__
#define __CPbkControlContainer_H__

//  INCLUDES
#include <coecntrl.h>  // CCoeControl


// FORWARD DECLARATIONS
class MPbkKeyEventHandler;
class MObjectProvider;

// CLASS DECLARATION

/**
 * Generic Phonebook control container class.
 * Control container's task is to dispatch key presses to an application-
 * side key event handler. If key event is not processed application-
 * side, it is passed to the contained control.
 * Do not use this class directly, instead use more safely typed template
 * class CPbkControlContainer.
 *
 * @see CPbkControlContainer
 */
class CPbkControlContainerImpl :
        public CCoeControl
    {
    protected:  // Constructors and destructor
        /**
         * Constructor.
         * @param aAppView  application view of this control container.
         */
        IMPORT_C CPbkControlContainerImpl(MPbkKeyEventHandler* aKeyHandler);

        /**
         * Second phase Constructor.
		 * @param the object provider (mop)
         */
        IMPORT_C void ConstructL(MObjectProvider& aProvider);

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CPbkControlContainerImpl();

    public:  // Interface
        /**
         * Returns the contained control.
         */
        IMPORT_C CCoeControl* CoeControl() const;

        /**
         * Destroys the contained control if it is owned by this container.
         */
        IMPORT_C void DestroyControl();

        /**
         * Returns the key event handler.
         */
        IMPORT_C MPbkKeyEventHandler* KeyEventHandler() const;

        /**
         * Sets the key event handler aKeyEventHandler.
         */
        IMPORT_C void SetKeyEventHandler(MPbkKeyEventHandler* aKeyEventHandler);

        /**
         * Set controls help context aContext.
         */
        IMPORT_C void SetHelpContext(const TCoeHelpContext& aContext);

        /**
         * Get controls help context to aContext.
         */
        IMPORT_C void GetHelpContext(TCoeHelpContext& aContext) const;
        
    protected:  // Interface
        /**
         * Set the contained control to aControl and this container's
         * rectangle to aRect.
         *
         * @param aControl  the control to set in this container. 
         * @param aRect     Initial rectangle for this container and the 
         *                  contained control.
         * @param aOwnsControl  if ETrue this object takes ownership of 
         *                      aControl.
         */
        IMPORT_C void SetCoeControl(
                    CCoeControl* aControl, 
                    const TRect& aRect, 
                    TBool aOwnsControl=ETrue);

    protected:  // from CCoeControl
        /**
         * Receives key events. 
         * Routes events first to iAppView, if not consumed by iAppView
         * routes to iControl.
         */
        IMPORT_C TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
         * Always returns 1.
         */ 
        IMPORT_C TInt CountComponentControls() const;

        /**
         * Always returns iControl
         */
        IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * This control is only a container; sets iControl's rectangle 
         * to this control's rectangle.
         */
        IMPORT_C void SizeChanged();

        /**
         * Forwards focus changed events.
         */
        IMPORT_C void FocusChanged(TDrawNow aDrawNow);

    private:  // data
        /// Own: the view-side UI control
        CCoeControl* iControl;
        /// Own: ETrue if this object owns iControl
        TBool iOwnsControl;
        /// Ref: this control container's key event handler
        MPbkKeyEventHandler* iKeyEventHandler;
        /// Own: This controls help context 
        TCoeHelpContext iHelpContext;
    };


/**
 * Thin template wrapper for CPbkControlContainerImpl.
 * Implements Control() function to return the actual control type
 * passed to ConstructL.
 */
template<class ControlType>
class CPbkControlContainer :
        public CPbkControlContainerImpl
    {
    public:  // New functions
        /**
         * Creates an instance of this class.
         * @param aKeyEventHandler key event handler
		 * @param aProvider the object provider (mop)
         */
        static CPbkControlContainer<ControlType>* NewL
			(MPbkKeyEventHandler* aKeyEventHandler,
			MObjectProvider& aProvider);

        /**
         * Like NewL above, but leaves the created instance on the cleanup stack.
		 * @see CPbkControlContainer<ControlType>* NewL
         */
        static CPbkControlContainer<ControlType>* NewLC
			(MPbkKeyEventHandler* aKeyEventHandler,
			MObjectProvider& aProvider);

        /**
         * Returns the contained control.
         */
        inline ControlType* Control() const;

        /**
         * Set the contained control to aControl and this container's
         * rectangle to aRect.
         *
         * @param aControl  the control to set in this container. 
         * @param aRect     Initial rectangle for this container and the 
         *                  contained control.
         * @param aOwnsControl  if ETrue this object takes ownership of 
         *                      aControl.
         */
        inline void SetControl(
            ControlType* aControl, 
            const TRect& aRect,
            TBool aOwnsControl=ETrue);

    protected:  // Constructors and destructor
        /**
         * Constructor.
         * @param aKeyEventHandler key event handler
         */
        inline CPbkControlContainer(MPbkKeyEventHandler* aKeyEventHandler);

        // Compiler-generated destructor is ok for this class
    };

template<class ControlType>
CPbkControlContainer<ControlType>* CPbkControlContainer<ControlType>::NewL
        (MPbkKeyEventHandler* aKeyEventHandler, MObjectProvider& aProvider)
    {
    CPbkControlContainer<ControlType>* self = 
        CPbkControlContainer<ControlType>::NewLC(aKeyEventHandler, aProvider);
    CleanupStack::Pop();  // self
    return self;
    }

template<class ControlType>
CPbkControlContainer<ControlType>* CPbkControlContainer<ControlType>::NewLC
        (MPbkKeyEventHandler* aKeyEventHandler, MObjectProvider& aProvider)
    {
    CPbkControlContainer<ControlType>* self = 
        new(ELeave) CPbkControlContainer<ControlType>(aKeyEventHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aProvider);
    return self;
    }

template<class ControlType>
inline CPbkControlContainer<ControlType>::CPbkControlContainer(MPbkKeyEventHandler* aKeyEventHandler)
    : CPbkControlContainerImpl(aKeyEventHandler)
    {
    }

template<class ControlType>
inline ControlType* CPbkControlContainer<ControlType>::Control() const
    {
    return static_cast<ControlType*>(CPbkControlContainerImpl::CoeControl());
    }

template<class ControlType>
inline void CPbkControlContainer<ControlType>::SetControl(
        ControlType* aControl, 
        const TRect& aRect,
        TBool aOwnsControl/*=ETrue*/)
    {
    CPbkControlContainerImpl::SetCoeControl(aControl, aRect, aOwnsControl);
    }


#endif  // __CPbkControlContainer_H__
            
// End of File
