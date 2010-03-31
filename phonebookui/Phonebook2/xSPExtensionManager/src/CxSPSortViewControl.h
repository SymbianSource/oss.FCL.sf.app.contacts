/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Sort view control.
*
*/


#ifndef     __CXSPSORTVIEWCONTROL_H
#define     __CXSPSORTVIEWCONTROL_H

// INCLUDES
#include <coecntrl.h>     // CCoeControl
#include <cntdef.h>       // TContactItemId


// FORWARD DECLARATIONS
class CAknSingleGraphicStyleListBox;
class CxSPLoader;
class CxSPViewIdChanger;
class MGlobalNoteObserver;
class CAknGlobalNote;
class CGlobalNoteObserver;
class CPbk2UIExtensionView;

typedef CArrayPtrFlat<CxSPLoader> CxSPArray;

/**
 * Sort view control for Extension manager.
 * Shows sortable list of extensions.
 */
class CxSPSortViewControl : public CCoeControl
    {
    public: // Constructor and destructor
        /**
         * Creates a new CxSPSortViewControl.
         *
         * @param aViewIdChanger Reference to view id changer
         * @param aExtensions array of extensions
         */
        static CxSPSortViewControl* NewL( CxSPViewIdChanger& aViewIdChanger,
        									CxSPArray& aExtensions, 
        									CPbk2UIExtensionView& aView);

        /**
         * Creates a new CxSPSortViewControl and leaves it to
         * cleanup stack.
         * @see NewL
         */
        static CxSPSortViewControl* NewLC( CxSPViewIdChanger& aViewIdChanger,
        									CxSPArray& aExtensions, 
        									CPbk2UIExtensionView& aView );

        /**
         * Destructor.
         */
        ~CxSPSortViewControl();

    private: // From CCoeControl, standard control functionality
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    public: // Implementation
        
        CCoeControl& CxSPSortViewControl::ComponentControl() const;
        
    	/**
    	 * Sets the listbox data
    	 */
		void SetListBoxContentsL();

		/**
		 * Marks/unmarks the current item in listbox
		 *
		 * @param aMark if ETrue, mark current item, otherwise unmark it
		 */
		void SetCurrentItemMarkedL( TBool aMark );

		/**
		 * Moves the marked item in list when user presses Ok
		 */
		void MoveMarkedItemL();

		/**
		 * Commits the sorting operation, i.e. performs the real
		 * extensions sorting after user has sorted the list to his liking.
		 *
		 * @param aObserver The observer to be notified after global wait note
		 *        informing user about phonebook exit has been closed.
		 * @return ETrue if real sort happened, i.e. extension order
		 *         changed. Otherwise EFalse.
		 */
		TBool CommitSortL( MGlobalNoteObserver* aObserver );

    private: // Implementation

        /**
         * C++ constructor
         *
         * @param aViewIdChanger Reference to view id changer
         * @param aExtensions array of extensions
         */
        CxSPSortViewControl( CxSPViewIdChanger& aViewIdChanger, CxSPArray& aExtensions, CPbk2UIExtensionView& aView );

        /**
         * Performs the 2nd phase of construction.
         */
        void ConstructL();

    private: // Data

    	/// Ref: view id changer
    	CxSPViewIdChanger& iViewIdChanger;

        /// Own: list box control
        CAknSingleGraphicStyleListBox* iListBox;
        
        /// not own: sortview
        CPbk2UIExtensionView& iView;

        /// Ref: Array of extensions
        CxSPArray& iExtensions;

        /// Own: array that shows the sorted order
        RArray<TUint32> iSortedOrder;

        /// Own: Global note for informing Phonebook2 exit
        CAknGlobalNote* iGlobalNote;

        /// Own: Observer for global note events
        CGlobalNoteObserver* iGlobalNoteObserver;
    };

#endif // __CXSPSORTVIEWCONTROL_H

// End of file
