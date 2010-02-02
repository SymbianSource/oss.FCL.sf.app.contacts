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
*       Phonebook Application view superclass.
*
*/



#ifndef __CPbkAppViewBase_H__
#define __CPbkAppViewBase_H__


//  INCLUDES
#include <aknview.h>    // CAknView
#include <MPbkKeyEventHandler.h>


// FORWARD DECLARATIONS
class CPbkAppUiBase;
class CPbkContactEngine;
class CPbkViewState;
class CEikMenuBar;
class CAknTitlePane;
class CAknContextPane;
class CPbkAiwInterestArray;


// CLASS DECLARATION

/**
 * Phonebook Application view superclass. Provides some common application 
 * view functionality. 
 * All Phonebook's application views have to be derived from this class
 * because CPbkAppUi assumes it.
 */
class CPbkAppViewBase : public CAknView, public MPbkKeyEventHandler
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
         */
        IMPORT_C CPbkAppViewBase();
        
        /**
         * Destructor.
         */
        IMPORT_C ~CPbkAppViewBase();

    public:  // Interface
        /**
         * Returns Phonebooks Application Uid.
         */
        IMPORT_C TUid ApplicationUid() const;

        /**
         * Returns the Phonebook application UI object.
         */
        IMPORT_C CPbkAppUiBase* PbkAppUi() const;

        /**
         * Returns the Phonebook engine object
         */
        IMPORT_C CPbkContactEngine* Engine() const;

        /**
         * Returns the application's title pane, if not present returns NULL.
         */
        IMPORT_C CAknTitlePane* TitlePane() const;

        /**
         * Returns the application's context pane, if not present returns NULL.
         */
        IMPORT_C CAknContextPane* ContextPane() const;        

        /**
         * Creates and returns an object containing this view's current state.
         * Used by CPbkAppUi in view switching.
         *
         * @return  this view's current state. The returned object is left on
         *          the cleanup stack.
         */
        IMPORT_C virtual CPbkViewState* GetViewStateLC() const;

        /**
         * Creates a pop up menubar of aResourceId.
         * Runs DoLaunchPopupL inside a TRAP.
         */
        IMPORT_C void LaunchPopupMenuL(TInt aResourceId);

        /**
         * Creates Phonebook AIW interest item based on aInterestId
         * and attaches it to the AIW services.
         * @param aInterestId id of the AIW interest
         * @param aMenuResourceId resource id of the menu to attach the id
         * @param aInterestResourceId resource id of the AIW interest
         * @param aAttachBaseServiceInterest if ETrue, attaches the AIW
         *        resource id to AIW base services also (enables command
         *        handling via AIW without menu commands)
         */
        IMPORT_C void CreateInterestItemAndAttachL(
            TInt aInterestId,
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest);

    protected:  // from CAknView
        IMPORT_C void HandleCommandL(TInt aCommand);
	    IMPORT_C void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private: // from CAknView
        IMPORT_C void ProcessCommandL(TInt aCommand);

    private:  // from MPbkKeyEventHandler
        IMPORT_C TBool PbkProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // Interface for derived classes
        /**
         * Handles command keys application-side. If a key event is not handled
         * by this function, it is forwared to this view's UI control.
         *
         * @param aKeyEvent key event as in CCoeControl::OfferKeyEventL.
         * @param aType     key event type as in CCoeControl::OfferKeyEventL.
         * @return ETrue if the key event was consumed, EFalse otherwise.
         *
         * @see CPbkControlContainer##OfferKeyEventL
         * @see CCoeControl##OfferKeyEventL
         */
        IMPORT_C virtual TBool HandleCommandKeyL
                (const TKeyEvent& aKeyEvent, TEventCode aType);
    
    private:  // Implementation
        void DoLaunchPopupL(TInt aResourceId);
        CCoeControl* GetStatusPaneControlSafely(TInt aCtrlId) const;        
        void ClosePopup();

    protected: // data
        /// Ref: AIW interest item array
        CPbkAiwInterestArray* iAiwInterestArray;

    private: // Data
        /// Own: popup menu
        CEikMenuBar* iPopup;
        /// Own: testing mode
        TUint iTestMode;
    };

#endif  // __CPbkAppViewBase_H__

// End of File
