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
* Description:  Phonebook 2 search pane filter. This acts as a wrapper to the FindBox in
*     NameslistView. Info needs to be passed to this filter before the FindBox.
*
*/
#ifndef CPBK2PREDICTIVESEARCHFILTER_H_
#define CPBK2PREDICTIVESEARCHFILTER_H_

// INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <w32std.h>
#include <coecntrl.h>   // CCoeControl
#include <eikappui.h>

// FORWARD DECLARATIONS
class CAknSearchField;
class MPbk2FilteredViewStack;
class MPbk2ContactNameFormatter;
class CRunAsynch;
class MVPbkViewContact;
class CPbk2PredictiveViewStack;

/// Unnamed namespace for local definitions
namespace {

/// Search field length
const TInt KSearchFieldLength = 40;

} /// namespace

/**
 * Phonebook 2 predictive search filter.
 */

NONSHARABLE_CLASS(CPbk2PredictiveSearchFilter) : public CCoeControl
    {
    
    enum TEditorMode
        {
        ENone = 0,
        EPaste
        };
    
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2PredictiveSearchFilter* NewL();

        /**
         * Destructor.
         */
        ~CPbk2PredictiveSearchFilter();
        
    public: // Implementation
        /**
         * Filters the KeyEvent
         * 
         */
        void FilterL( const TKeyEvent& aKeyEvent, TEventCode aType, CAknSearchField* aSearchField );
        
        /**
         * Updates the FindPane Display text.
         * 
         * Called when a character is added to or deleted from the FindPane.
         * The descriptor aFindPaneText contains the entire new inline text string,
         * not just the new text to be combined with the old inline text.
         * 
         * @param aFindPaneText Descriptor which holds the entire new inline text string.
         * Once this function completes, this param will hold the
         * respective predictive search key which can be passed to
         * the ViewStack for the actual Search.
         * 
         *  
         * @returns ETrue, if the filter data is updated successfully. 
         *      Only then we need to pass the find query to the ViewStack 
         */
        TBool StartFindPaneInlineEditL( TDes& aFindPaneText );
        
        /**
         * Handler to the Predictive Search Results when there is no match
         * found for a query
         * @param aQueryString - Holds the Actual Search Query
         * @param aMatchSeq - Holds the matched item. The aMatchSeq will be 
         *      empty, if there is no match found. 
         */
        void HandlePSNoMatchL( const TDesC& aQueryString, const TDesC& aMatchSeq );
        
        /**
         * Sets the AKN FindPane
         * @param aSearchField : Ptr to the Avkon findPane
         */
        void SetAknSearchFieldL( CAknSearchField* aSearchField );
        
        /**
         * Checks if predictive search is supported
         *
         * @return ETrue if predictive input activated else EFalse
         *  
        */
        TBool IsPredictiveActivated() const;
        
        /**
         * Check whether the FindPane filter is locked or not
         * 
         * Before passing any keys to the FindPane this Function
         * needs to be checked. Only Backspace should be passed
         * if the the findpane is locked, which will release the lock.
         * 
         * @return ETrue if FindPane is locked else EFalse
        */ 
        TBool IsLocked() const;
        
        /**
         * Reset the findPane Filter
         * The state of the FindPane filter is reset
         * This needs to be called along with the FindPane Reset
         */ 
        void ResetL();
        
        /**
         * Reset the FindPane 
         */ 
        void FindPaneResetL();
        
        /**
         * Fills the aSearchText with the actual Search String.
         * NOTE: 
         * -----
         * When predictive search is enabled, the search text will be
         * different from the findpane text, since the findpane will show the
         * first matched item in the listbox and 
         * not the excat chars for which the search was initiated.
         * If you want to get the FindPane text, use the API FindPaneTextL()
         * 
         * When Predictive Search is Disabled, both GetSearchTextL &
         * FindPaneTextL will return the same data.
         * 
         * @param aSearchText : This will be filled with the actual string
         *          for which the search was initiated.
         */ 
        void GetSearchTextL( RBuf& aSearchText );
        
        /**
         * Is editor in Paste Mode
         * @return ETrue if the findpane is in PasteMode else EFalse
         */ 
        TBool IsPasteMode();
        
        /**
         * Is Filtering of FindPane Text is ongoing
         * Dont perforn any operation on the Findpane or FindPane filter
         * when Filtering is ongoing.
         * @return ETrue-if Filtering is ongoing else EFalse
         */ 
        TBool IsFiltering();
        
        
        /**
         * Commits the matched string to the FindPane
         * 
         * aViewStack - Holds the items displayed on the NamesList
         * aNameFormatter - Phonebook 2 contact name formatter interface
         */            
        TInt CommitFindPaneTextL( MPbk2FilteredViewStack& aViewStack,
              MPbk2ContactNameFormatter& aNameFormatter );
        
        /**
         * Commits the matched string of specific contact to the FindPane
         * 
         * aViewStack - Holds the items displayed on the NamesList
         * aNameFormatter - Phonebook 2 contact name formatter interface
         */
        void CommitFindPaneTextL( MPbk2FilteredViewStack& aViewStack,
                                        MPbk2ContactNameFormatter& aNameFormatter, 
                                        const TInt aIndex );
        
        
        /**
         * Foreground event handling function.
         * 
         * @ param  aForeground-Indicates the required focus state of the 
         * control. 
         */
        void HandleForegroundEventL(TBool aForeground);
        
        /**
         * Gets the text in the find box. Returns NULL if not
         * applicable or there is no text in the box.
         *            
         * @return  Find box text or NULL.
         */
        HBufC* FindPaneTextL();
        
        /**
         * Redraws search box content
         */
        void ReDrawL(TBool aAsynchronous = EFalse);
             
        void LookupL( const MVPbkViewContact& aContact, CPbk2PredictiveViewStack& aView, 
                MPbk2ContactNameFormatter& aNameFormatter, TDes& aMatchSeqChr );
        
    private:
        /*
         * Add this FindPane filter to the View Stack
         * This takes the high priority then FEP
         */
         void AddToViewStackL();
         
        /*
         * Remove the filter from the View Stack
         */ 
         void RemoveFromViewStack();
         
         void CommitFindPaneTextL( const TDesC& aMatchSeq );
         
         TBool IsMirroredInputLang();
         
         void SetSearchBoxTextL( const TDesC& aText, TInt aCursorPos, TBool aDrawNow = ETrue );
        
    private: // Implementation
        CPbk2PredictiveSearchFilter();
        void ConstructL();

    public: // From CCoeControl
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        
    private: // Data
        /// Own: Is true when filtering
        TBool iFiltering;
        
        /// Ref : eikappui
        CEikAppUi& iAppUi;
        
        /// Owns: Contains the actual string for which Predictive search was initiated
        RBuf iPredictiveTextEntered;        
        
        ///Doesnt Own - FindPane
        CAknSearchField* iSearchField;        
        
        /// To know whether Filter is locked or not
        TBool iLocked;
        
        /// To know the mode of the findPane filter
        TEditorMode iEditorMode;
        
        /// ETrue if the Filter has been added to the View Stack
        TBool iViewStack;
        
        /// ETrue if Predictive search is enabled
        TBool iPredictiveSearchEnabled;
        
        ///Identify a Long Key Press
        TBool iLongKeyPress;
        
        ///Serach Text which is displayed on the FindPane
        ///Display Text
        ///Owns
        HBufC* iSearchText;
        
        /// ETrue if the Filter should be added to the View Stack
        TBool iAddToViewStack;
        
        /// If ETrue space should be removed
        TBool iRemoveSpace;
        
        /// ETrue If virtual keyboard was used to tap query
        TBool iVKbUsed;
        
        // Own: Use to run method in asynchronous mode
        CRunAsynch* iRunAsynch;
    };

#endif /* CPBK2PREDICTIVESEARCHFILTER_H_ */
