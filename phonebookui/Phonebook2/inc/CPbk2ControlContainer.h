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
* Description:  Generic Phonebook 2 control container.
*
*/



#ifndef CPBK2CONTROLCONTAINER_H
#define CPBK2CONTROLCONTAINER_H

//  INCLUDES
#include <coecntrl.h>
#include <aknlongtapdetector.h>

// FORWARD DECLARATIONS
class MPbk2KeyEventHandler;
class MObjectProvider;
class MPbk2PointerEventHandler;

// CLASS DECLARATION

class MPbk2ControlContainerForegroundEventObserver
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ControlContainerForegroundEventObserver()
            {}

        /**
         * Foreground event handling function intended for overriding by sub classes. 
         * @param aForeground - Indicates the required focus state of the control.
         */
         
        virtual void HandleForegroundEventL(TBool aForeground) = 0;
        
    };

/**
 * Generic Phonebook 2 control container implementation class.
 * Control container's task is to dispatch key presses to an
 * application-side key event handler. If key event is not
 * processed application-side, it is passed to the contained control.
 * Do not use this class directly, instead use more safely typed template
 * class CPbk2ControlContainer.
 *
 * @see CPbkControlContainer
 */
class CPbk2ControlContainerImpl : public CCoeControl,
                                  public MAknLongTapDetectorCallBack
    {
    protected: // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aKeyHandler   Key event handler.
         * @param aPointerHandler Pointer event handler.
         */
        IMPORT_C CPbk2ControlContainerImpl(
                MPbk2KeyEventHandler* aKeyHandler,
                MPbk2PointerEventHandler* aPointerHandler );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CPbk2ControlContainerImpl();

        /**
         * Second phase constructor.
         *
         * @param aProvider     The object provider (MOP).
         */
        IMPORT_C void ConstructL(
                MObjectProvider& aProvider );

    public: // Interface

        /**
         * Returns the contained control.
         *
         * @return  Contained control.
         */
        IMPORT_C CCoeControl* CoeControl() const;

        /**
         * Destroys the contained control if it is owned by this container.
         */
        IMPORT_C void DestroyControl();

        /**
         * Returns the key event handler.
         *
         * @return  Key event handler.
         */
        IMPORT_C MPbk2KeyEventHandler* KeyEventHandler() const;
        
        /**
         * Returns the pointer event handler.
         *
         * @return  Pointer event handler.
         */
        IMPORT_C MPbk2PointerEventHandler* PointerEventHandler() const;

        /**
         * Sets the key event handler aKeyEventHandler.
         *
         * @param aKeyEventHandler  Key event handler to set.
         */
        IMPORT_C void SetKeyEventHandler(
                MPbk2KeyEventHandler* aKeyEventHandler );

        /**
         * Set controls help context.
         *
         * @param aContect  Help context to set.
         */
        IMPORT_C void SetHelpContext(
                const TCoeHelpContext& aContext );

        /**
         * Get controls help context.
         *
         * @param aContext  Retrieved help context.
         */
        IMPORT_C void GetHelpContext(
                TCoeHelpContext& aContext ) const;
                
        /**
         * Returns the long tap detector. Should be called only if pen
         * input is supported.
         *
         * @return  Reference to the long tap detector.
         */
        IMPORT_C CAknLongTapDetector& LongTapDetectorL();

    protected: // Interface

        /**
         * Set the contained control to aControl and this container's
         * rectangle to aRect.
         *
         * @param aControl      The control to set in this container.
         * @param aRect         Initial rectangle for this container and
         *                      the contained control.
         * @param aOwnsControl  If ETrue this object takes ownership of
         *                      aControl.
         */
        IMPORT_C void SetCoeControl(
                    CCoeControl* aControl,
                    const TRect& aRect,
                    TBool aOwnsControl = ETrue );

    protected: // From CCoeControl

        IMPORT_C TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        IMPORT_C TInt CountComponentControls() const;
        IMPORT_C CCoeControl* ComponentControl(
                TInt aIndex ) const;
        IMPORT_C void SizeChanged();
        IMPORT_C void FocusChanged(
                TDrawNow aDrawNow );
        IMPORT_C void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
                
    public: // From MAknLongTapDetectorCallBack

        IMPORT_C void HandleLongTapEventL(
                const TPoint& aPenEventLocation, 
                const TPoint& aPenEventScreenLocation );                

    public:
        /*
         * Register for HandleForegroundEventL events. 
         * 
         * Note: 
         * SetCoeControl also removes the foreground Observer
         * 
         * @param aObserver - Observer which implements MPbk2ControlContainerFocusObserver class
         */
        IMPORT_C void AddForegroundObserver( 
                MPbk2ControlContainerForegroundEventObserver* aObserver );
        
        /*
         * UnRegister from HandleForegroundEventL events. 
         * 
         * Note: 
         * SetCoeControl also removes the foreground Observer
         * 
         * @param aObserver - Observer which implements MPbk2ControlContainerFocusObserver class
         */
        IMPORT_C void RemoveForegroundObserver();
        
    private: // Data
        /// Own: The view-side UI control
        CCoeControl* iControl;
        /// Own: Indicates whether this object owns iControl
        TBool iOwnsControl;
        /// Ref: This control container's key event handler
        MPbk2KeyEventHandler* iKeyEventHandler;
        /// ref: This control ccontainer's pointer event handler
        MPbk2PointerEventHandler* iPointerEventHandler;
        /// Own: This controls help context
        TCoeHelpContext iHelpContext;
        /// Own: Long tap detector
        CAknLongTapDetector* iLongTapDetector;
        /// Doesnt Own : Observer for HandleForegroundEventL events.
        MPbk2ControlContainerForegroundEventObserver* iObserver;
    };


/**
 * Thin template wrapper for CPbk2ControlContainerImpl.
 * Implements Control() function to return the actual control type
 * passed to ConstructL.
 */
template<class ControlType>
class CPbk2ControlContainer : public CPbk2ControlContainerImpl
    {
    protected:  // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aKeyEventHandler  Key event handler.
         * @param aPointerEventHandler  Pointer event handler.
         */
        inline CPbk2ControlContainer(
                MPbk2KeyEventHandler* aKeyEventHandler,
                MPbk2PointerEventHandler* aPointerEventHandler );

        /// Compiler-generated destructor is ok for this class

    public:  // Interface

        /**
         * Creates a new instance of this class.
         *
         * @param aKeyEventHandler  Key event handler.
         * @param aPointerEventHandler  Pointer event handler.
         * @param aProvider         The object provider (MOP).
         * @return New instance of this class
         */
        static CPbk2ControlContainer<ControlType>* NewL(
                MPbk2KeyEventHandler* aKeyEventHandler,
                MPbk2PointerEventHandler* aPointerEventHandler,
                MObjectProvider& aProvider );

        /**
         * Creates a new instance of this class.
         *
         * @param aKeyEventHandler  Key event handler.
         * @param aPointerEventHandler  Pointer event handler.
         * @param aProvider         The object provider (MOP).
         * @return New instance of this class
         */
        static CPbk2ControlContainer<ControlType>* NewLC(
                MPbk2KeyEventHandler* aKeyEventHandler,
                MPbk2PointerEventHandler* aPointerEventHandler,
                MObjectProvider& aProvider );

        /**
         * Returns the contained control.
         *
         * @return  Contained control.
         */
        inline ControlType* Control() const;

        /**
         * Set the contained control to aControl and this container's
         * rectangle to aRect.
         *
         * @param aControl      The control to set in this container.
         * @param aRect         Initial rectangle for this container and
         *                      the contained control.
         * @param aOwnsControl  If ETrue this object takes ownership of
         *                      aControl.
         */
        inline void SetControl(
                ControlType* aControl,
                const TRect& aRect,
                TBool aOwnsControl = ETrue );
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2ControlContainer<ControlType>::CPbk2ControlContainer
// --------------------------------------------------------------------------
//
template<class ControlType>
inline CPbk2ControlContainer<ControlType>::CPbk2ControlContainer
        ( MPbk2KeyEventHandler* aKeyEventHandler,
          MPbk2PointerEventHandler* aPointerEventHandler ) :
            CPbk2ControlContainerImpl( aKeyEventHandler, aPointerEventHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainer<ControlType>::NewL
// --------------------------------------------------------------------------
//
template<class ControlType>
CPbk2ControlContainer<ControlType>* CPbk2ControlContainer<ControlType>::NewL
        ( MPbk2KeyEventHandler* aKeyEventHandler,
          MPbk2PointerEventHandler* aPointerEventHandler,
          MObjectProvider& aProvider )
    {
    CPbk2ControlContainer<ControlType>* self =
        CPbk2ControlContainer<ControlType>::NewLC
            ( aKeyEventHandler, aPointerEventHandler, aProvider );
    CleanupStack::Pop(); // self
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainer<ControlType>::NewLC
// --------------------------------------------------------------------------
//
template<class ControlType>
CPbk2ControlContainer<ControlType>* CPbk2ControlContainer<ControlType>::NewLC
        ( MPbk2KeyEventHandler* aKeyEventHandler,
          MPbk2PointerEventHandler* aPointerEventHandler,
          MObjectProvider& aProvider )
    {
    CPbk2ControlContainer<ControlType>* self =
        new ( ELeave ) CPbk2ControlContainer<ControlType>
            ( aKeyEventHandler, aPointerEventHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aProvider );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainer<ControlType>::Control
// --------------------------------------------------------------------------
//
template<class ControlType>
inline ControlType* CPbk2ControlContainer<ControlType>::Control() const
    {
    return static_cast<ControlType*>
        ( CPbk2ControlContainerImpl::CoeControl() );
    }

// --------------------------------------------------------------------------
// CPbk2ControlContainer<ControlType>::SetControl
// --------------------------------------------------------------------------
//
template<class ControlType>
inline void CPbk2ControlContainer<ControlType>::SetControl(
        ControlType* aControl, const TRect& aRect,
        TBool aOwnsControl/*=ETrue*/ )
    {
    CPbk2ControlContainerImpl::SetCoeControl
        ( aControl, aRect, aOwnsControl );
    }


#endif // CPBK2CONTROLCONTAINER_H

// End of File
