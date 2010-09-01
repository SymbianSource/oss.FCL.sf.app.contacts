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
* Description:  Phonebook 2 move top contacts dialog.
*
*/


#ifndef CPBK2MOVETOPCONTACTSDLG_H
#define CPBK2MOVETOPCONTACTSDLG_H

//  INCLUDES
#include <AknDialog.h>
#include <coecobs.h>
#include <eiklbo.h>
#include <eikcapc.h>
#include <MPbk2ContactUiControlUpdate.h>

//  FORWARD DECLARATIONS
class MVPbkViewContact;
class MVPbkContactLink;
class CPbk2NlxReorderingModel;
class CEikColumnListBox;
class CPbk2EcePresenceEngine;
// CLASS DECLARATIONS

/**
 * Phonebook 2 move(organize) top contacts dialog
 */
class CPbk2NlxMoveTopContactsDlg : public CAknDialog,
	MEikListBoxObserver,
	MPointerEventObserver,
	MPbk2ContactUiControlUpdate
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2NlxMoveTopContactsDlg* NewL(CPbk2NlxReorderingModel& aModel);

        /**
         * Destructor.
         */
        ~CPbk2NlxMoveTopContactsDlg();

    public: // From CEikDialog
        TInt ExecuteLD();

    protected: // From CEikDialog
        void PreLayoutDynInitL();
        void PostLayoutDynInitL();
        TBool OkToExitL(
                TInt aButtonId );
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandleResourceChange( TInt aType );
    	void HandleControlEventL( 
    			CCoeControl* aControl, 
    			TCoeEvent aEventType );        
        
    private: // from MEikListBoxObserver
    	void HandleListBoxEventL( 
    			CEikListBox* aListBox,
    			TListBoxEvent aEventType );
    private: //MPointerEvenetObserver
    	bool PointerEvent(CEikCaptionedControl *aControl,
    			const TPointerEvent& aPointerEvent);
    	
    private: // from MPbk2ContactUiControlUpdate
        void UpdateContact( const MVPbkContactLink& aContactLink );
        
    private: // Implementation
        CPbk2NlxMoveTopContactsDlg(CPbk2NlxReorderingModel& aModel);
        void ConstructL();
        
        void SetNewTitleTextL();
        void RestoreTitlePaneTextL();

        void ContinueDragL();

    private: // Data
        /// Own: Link to focused contact
        MVPbkContactLink* iFocusedContactLink;
        /// Ref: Listbox control
        CEikColumnListBox* iListBox;
        /// Ref: Contact listbox model
        CPbk2NlxReorderingModel& iModel;
        /// Own: Last selected listbox item
        ///		 ready for dragging
        TInt iPrevSelectedItem;
        // Own: Old title text
        HBufC* iOldTitleText;
    };

#endif // CPBK2FETCHDLG_H

// End of File
