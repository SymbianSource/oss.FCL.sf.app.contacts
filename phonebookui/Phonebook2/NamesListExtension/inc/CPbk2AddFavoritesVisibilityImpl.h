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
* Description:  
*
*/


#ifndef  CPBK2ADDFAVORITESVISIBILITYIMPL_H_
#define  CPBK2ADDFAVORITESVISIBILITYIMPL_H_

//  INCLUDES
#include <e32base.h>
#include <MPbk2CmdItemVisibilityObserver.h>
#include "MPbk2AddFavoritesVisibility.h"

/** 
 * Implements the decision logic 
 * whether "Add favorites" should be visible or not. It
 * informs the observer asycronously about the decesion
 *
 * Depending on KCRUidPhonebook CenRep store's 
 * KPhonebookAddFavoritiesVisibility key value (variation):
 *
 *    # Never shown
 *    # Enabled until first favorite added
 *    # Always enabled 
 * 
 * The following may/may not affect the visibility:
 * 
 * # Are there Top Contacts? (favorites)
 * # Has top contact feature ever used ? 
 * # Are there any contacts in nameslist contacts view
 *
 */
 
NONSHARABLE_CLASS( CPbk2AddFavoritesVisibilityImpl) : public CActive,
    public MPbk2AddFavoritesVisibilityImpl
	{
	public: // Construction & destruction

        /*
         * Factory method
         *
         * @param aObserver: client of visibility, who is to receive
         * ON/OFF events when visibility is changed (i.e. a top contact
         * is added)
         * 
         * @param aBridge: Does all VPbk async task handling (i.e. db view
         * creation, store configuration observation) and then forwards the necessary
         * parameters when Impl class needs them
         * 
         * @param aCmdId command id to use when notifying aObserver
        */
		static CPbk2AddFavoritesVisibilityImpl* NewL( TInt aCmdId);
		
		~CPbk2AddFavoritesVisibilityImpl();

    public: 
    
        /* Needed for unit testing purposes at least. This class
         * wont give a callback if visibility transition has not
         * occurred (i.e. EFalse to ETrue or vice versa)
         * In that case, Unit Test must have access to newly
         * calculated visibility value after NotifyVisibilityChange()         
        */ 
        TBool Visibility();

        /*  
         * Sets visibility
         *
         * @param aVisibility new visiblity value
        */
        void SetVisibility(TBool aVisibility);
       
    public: // MPbk2AddFavoritesVisibilityImpl
        
        void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver);
		void NotifyVisibilityChange(const TInt aTopCount, const TInt aContactCount);
		TVariationFlag Variation();

    private: // CActive
    
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
        		   
	private: // constructor
	
	    CPbk2AddFavoritesVisibilityImpl( TInt aCmdId);
	
	    void ConstructL();
	
	private: // utility    		
    
        void StartL();
        TBool CalculateVisibilityL(const TInt aTopCount, const TInt aContactCount);
        		
        void ReadAddFavoritiesVisibilityKeyL();		                                                  
        void SetAddFavoritiesVisibilityKeyOFFL();

        void PrepareAsyncVisibilityCallback();    

	private: // data
	        
	    // This is a cenrep key value, which can be configured
	    // This variates the behaviour of visibility
	    // (i.e. always off/on or first time use)        
	    TVariationFlag iVariation; // own
	        
	    // Ref: visibility observer 
	    MPbk2CmdItemVisibilityObserver* iObserver;
	    // Own: Controls visibility
	    TBool iVisibilityOkToShow; 	    
	    // Own: previous and new visiblity
	    TBool iPreviousVisibility; // own
        TBool iNewVisibility;	    
	    // Own: add favorites command item id
	    TInt iCmdId;
	};

#endif /* CPBK2ADDFAVORITESVISIBILITYIMPL_H_*/
