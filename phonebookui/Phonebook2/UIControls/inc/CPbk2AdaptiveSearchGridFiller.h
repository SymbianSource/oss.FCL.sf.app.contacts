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
* Description:  Phonebook 2 contact editor dialog implementation.
*
*/


#ifndef CPBK2ADAPTIVESEARCHGRIDFILLER_H
#define CPBK2ADAPTIVESEARCHGRIDFILLER_H

//  INCLUDES
#include <e32base.h>
#include <aknsfld.h>

// FORWARD DECLARATIONS
class CAknSearchField;
class CFindUtil;
class CPbk2AdaptiveGrid;
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;

// CLASS DECLARATION

/**
 * Phonebook 2 adaptive search grid filler.
 * 
 * This class used to create adaptive search grid used to
 * search contacts from Phonebook 2.
 */
NONSHARABLE_CLASS(CPbk2AdaptiveSearchGridFiller) : public CActive
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aField Search field where adaptive search grid is updated.
         */
        static CPbk2AdaptiveSearchGridFiller* NewL( CAknSearchField& aField, MPbk2ContactNameFormatter& aNameFormatter );

        /**
         * Destructor.
         */
        virtual ~CPbk2AdaptiveSearchGridFiller();

    public: // Interface

        /**
         * Start creating adaptive search grid
         * 
         * Grid will be created asynchronously. When the grid is updated, it is
         * automatically updated to the CAknSearchField object provided in the
         * construction of this object. Client will not be notified when constructing
         * of the grid is ready.
         *
         * @param aModel contains contacts shown in the list box, this is used
         * 				 to create adaptive search grid
         * @param aFindText is text user has entered in the find pane.
         */
    	void StartFilling( const MVPbkContactViewBase& aView, const TDesC& aSearchString );


        /**
         * Stop creating adaptive search grid
         */
    	void StopFilling();

        /**
         * Clear adaptive search grid cache.
         * 
         * Data set (contacts shown in the list box) has changed, e.g. contact is removed or added.
         * This means that adaptive search grids used before, cannot be reused.
         */
    	void ClearCache();
    	

        /**
         * Invalidate Adaptive Search Grid
         * 
         * This means that even if the adaptive search grid doesn't set, it is still given again to the
         * Avkon component. If this is not called, then the grid is given to Avkon components only when
         * it changes.
         */
    	void InvalidateAdaptiveSearchGrid();
    	
        
    	/**
         * Change focus back to Adaptive Search Grid
         * 
         * This means that even if the "C" key is pressed,the focus will be change to parent control.
         * But actually this operation is done to Adaptive Search Grid. So set the focus back to it.
         */
    	 void SetFocusToAdaptiveSearchGrid();
    	

    private: // from CActive
    	
    	void RunL();
    	void DoCancel();
    	TInt RunError( TInt aError );    	
    	
    private: // Implementation

        CPbk2AdaptiveSearchGridFiller( CAknSearchField& aField, MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL();
        
        CPbk2AdaptiveGrid* KeyMapFromCache( const TDesC& aFindText );
        void AddKeyMapToCacheL( const TDesC& aFindText, const TDesC& aKeyMap );
        void SetAdaptiveGridCharsL( const TInt aMaxSpacesNumber );
        CDesC16Array* SplitContactFieldTextIntoArrayLC( const TDesC& aText );
		void BuildGridL( const TDesC& aContactTitle, const TDesC& aSearchString, HBufC*& aKeyMap );
        TInt NumberOfSpacesInString( const TDesC& aSearchString );
        // Used to judge if this contact's title include drgraphs
        TBool IsDigraphContactsTitleL(const TDesC& aContactTitle);
    private: // Data
    	
		/// Own: Containts adaptive search grid.
		HBufC* iKeyMap;
		
		/// Own: Contacts in the model are processed in a steps, this variable is
		///      used to hold the next step.
		TInt iCounter;
		
		/// Ref: Search field where use enters the text. Responsible to show adaptive search grid. 
		CAknSearchField& iSearchField;
		
		/// Own: Cache adaptive grid keymaps 
		RPointerArray<CPbk2AdaptiveGrid> iAdaptiveGridCache;
		
		/// Ref: View containing contacts. 
		const MVPbkContactViewBase* iView;
		
		/// Ref: Used to format contact name 
		MPbk2ContactNameFormatter& iNameFormatter;
		
		/// Own: Search string used to match contacts
		HBufC* iSearchString;
		
		/// Own: Find util used to match contacts and construct character grid
		CFindUtil* iFindUtil;
		
		/// Own: Current adaptive search grid. Used to prevent setting same grid multiple times.
		HBufC* iCurrentGrid;
		
		/// Used to invalidate grid, even if the grid remains same, it is anyway set if this is set to
		/// ETrue
		TBool iInvalidateAdaptiveSearchGrid;
		
		/// Used to get the focus for the search pane,when it is true, the search pane will be drawn and get
		/// the focus
		TBool iSetFocusToSearchGrid;
		/// Used to save the contacts' title which include drgraphs
		RPointerArray<HBufC> iDigraphContactsTitleArray;
    };

#endif // CPBK2ADAPTIVESEARCHGRIDFILLER_H

// End of File
