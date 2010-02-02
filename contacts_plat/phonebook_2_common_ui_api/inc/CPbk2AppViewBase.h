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
* Description:  Phonebook 2 application view base.
*
*/


#ifndef CPBK2APPVIEWBASE_H
#define CPBK2APPVIEWBASE_H

// INCLUDES
#include <aknview.h>
#include <mpbk2keyeventhandler.h>
#include <mpbk2pointereventhandler.h>

// FORWARD DECLARATIONS
class CPbk2ViewState;
class CEikMenuBar;
class CAknStylusPopUpMenu;

// CLASS DECLARATION

/**
 * Phonebook 2 application view base.
 * Provides some common application view functionality.
 * All Phonebook 2's application views have to be
 * derived from this class because CPbk2AppUi assumes it.
 */
class CPbk2AppViewBase : public CAknView,
                         public MPbk2KeyEventHandler,
                         public MPbk2PointerEventHandler
    {
    public: // Constructors and destructor

        /**
         * Constructor.
         */
        IMPORT_C CPbk2AppViewBase();

        /**
         * Destructor.
         */
        IMPORT_C ~CPbk2AppViewBase();

    public: // Interface

        /**
         * Returns Phonebook 2's application uid.
         *
         * @return  Application uid.
         */
        IMPORT_C TUid ApplicationUid() const;

        /**
         * Creates and returns an object containing this view's current state,
         * which is used in view switching.
         *
         * @return  This view's current state. The returned object is
         *          left on the cleanup stack.
         */
        IMPORT_C virtual CPbk2ViewState* ViewStateLC() const;

        /**
         * Creates a pop up menubar of aResourceId.
         * Runs DoLaunchPopupL inside a TRAP.
         *
         * @param aResourceId   Resource id of the popup.
         */
        IMPORT_C void LaunchPopupMenuL(
                TInt aResourceId );

        /**
         * Returns the menu filtering flags for the view.
         *
         * @see Pbk2MenuFilteringFlags.hrh for possible flags.
         * @return  The menu filtering flags for the view.
         */
        IMPORT_C TInt GetMenuFilteringFlagsL() const;

        /**
         * Creates a given stylus pop up menu at the given
         * screen coordinates.
         *
         * @param aResourceId   Resource id of the popup.
         * @param aPoint        The position where stylus was tapped.
         */
        IMPORT_C void LaunchStylusPopupMenuL(
                TInt aResourceId,
                const TPoint& aPoint );
        
        /**
         * Creates a given stylus pop up menu at the given
         * screen coordinates. The only difference with LaunchStylusPopupMenuL
         * is in return value.
         *
         * @param aResourceId   Resource id of the popup.
         * @param aPoint        The position where stylus was tapped.
         * 
         * @return Reference to a launched popup menu, can be used for menu dimming
         */
        IMPORT_C CAknStylusPopUpMenu& LaunchStylusPopupMenu2L(
                TInt aResourceId,
                const TPoint& aPoint );
        
        /**
         * Closes PopupMenu
         *
         */
        IMPORT_C void ClosePopup();

        

    protected: // Interface for derived classes

        /**
         * Handles command keys application-side.
         *
         * @see CCoeControl::OfferKeyEventL
         * @param aKeyEvent     Key event as in CCoeControl::OfferKeyEventL.
         * @param aType         Key event type as in CCoeControl::OfferKeyEventL.
         * @return  ETrue if the key event was consumed, EFalse otherwise.
         */
        IMPORT_C virtual TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

        /**
         * Handles pointer events application-side.
         *
         * @param aPointerEvent     Pointer event.
         */
        IMPORT_C virtual void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );

        /**
         * Handles long tap pointer events.
         *
         * @param aPenEventLocation        Long tap event location relative
         *                                 to parent control.
         * @param aPenEventScreenLocation  Long tap event location relative
         *                                 to screen.
         * @see MAknLongTapDetectorCallBack
         */
        IMPORT_C virtual void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );

        /**
         * Gets view specific menu filtering flags.
         *
         * @return  View specific menu filtering flags.
         */
        virtual TInt GetViewSpecificMenuFilteringFlagsL() const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* AppViewBaseExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    private: // From CAknView
        IMPORT_C void ProcessCommandL(
                TInt aCommand );

    private: // From MPbk2KeyEventHandler
        IMPORT_C TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // From MPbk2PointerEventHandler
        IMPORT_C void Pbk2ProcessPointerEventL(
                const TPointerEvent& aPointerEvent );
        IMPORT_C void Pbk2HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );

    private: // Implementation
        void DoLaunchPopupL(
                TInt aResourceId );
        void DoLaunchStylusPopupL(
                TInt aResourceId,
                const TPoint& aPoint );
        void CloseStylusPopup();
        void CreatePopupMenuL( TInt aResourceId );

    private: // Data
        /// Own: Popup menu
        CEikMenuBar* iPopup;
        /// Own: Stylus popup menu
        CAknStylusPopUpMenu* iStylusPopup;
    };

#endif  // CPBK2APPVIEWBASE_H

// End of File
