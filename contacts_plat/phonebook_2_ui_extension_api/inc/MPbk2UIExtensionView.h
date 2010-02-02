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
* Description:  Phonebook 2 UI extension view interface.
*
*/


#ifndef MPBK2UIEXTENSIONVIEW_H
#define MPBK2UIEXTENSIONVIEW_H

// INCLUDES
#include <w32std.h>

// FORWARD DECLARATIONS
class CPbk2ViewState;
class CEikMenuPane;
class TVwsViewId;

//Use this UID to access extension 2 for this interface. Used as a parameter
//to  UIExtensionViewExtension() method.
const TUid KMPbk2UIExtensionView2Uid = { 2 };

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension view interface.
 * New Phonebook 2 views can be provided by extension views. 
 * The extension information resource PHONEBOOK2_EXTENSION_INFORMATION 
 * has to contain the view ID definitions in the PHONEBOOK2_EXTENSION_VIEW_ID
 * resource(s) and the view graph modifications in the PBK2_VIEW_GRAPH resource.
 * Extension views are loaded by CPbk2UIExtensionView. 
 *
 * @see CPbk2UIExtensionView
 * @see Pbk2UIExtension.rh
 */
class MPbk2UIExtensionView
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UIExtensionView()
            {}

        /**
         * Handles status pane size change.
         */
        virtual void HandleStatusPaneSizeChange() = 0;

        /**
         * Returns view state.
         *
         * @return  View state.
         */
        virtual CPbk2ViewState* ViewStateLC() const = 0;

        /**
         * Handles key events.
         *
         * @param aKeyEvent     The event to handle.
         * @param aType         Event type.
         * @return  ETrue if the event was handled, EFalse otherwise.
         */
        virtual TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType ) = 0;

        /**
         * Returns view specific menu filtering flags.
         *
         * @return  View specific menu filtering flags.
         * @see Pbk2MenuFilteringFlags.hrh
         */
        virtual TInt GetViewSpecificMenuFilteringFlagsL() const = 0;

        /**
         * Activates the view.
         *
         * @param aPrevViewId       Previous view id.
         * @param aCustomMessageId  Custom message id.
         * @param aCustomMessage    Custom message containing view state
         *                           object CPbk2ViewState.
         */
        virtual void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage ) = 0;

        /**
         * Deactivates the view.
         */
        virtual void DoDeactivate() = 0;

        /**
         * Handles a command.
         *
         * @param aCommand  The command to handle.
         */
        virtual void HandleCommandL(
                TInt aCommand ) = 0;

        /**
         * Initialises menu pane.
         *
         * @param aResourceId   Menu pane resource id.
         * @param aMenuPane     Menu pane.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) = 0;

        /**
         * Handles a pointer event.
         *
         * @param aPointerEvent     The pointer event to handle.
         */
        virtual void HandlePointerEventL(
                const TPointerEvent& aPointerEvent ) = 0;

        /**
         * Handles long stylus tap events.
         *
         * @param aPenEventLocation        Control relative tap coordinates.
         * @param aPenEventScreenLocation  Absolute tap coordinates.
         */
        virtual void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* UIExtensionViewExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2UIEXTENSIONVIEW_H

// End of File
