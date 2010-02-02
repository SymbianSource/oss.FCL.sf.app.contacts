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


#ifndef  CPBK2REMOTECONTACTLOOKUPVISIBILITYIMPL_H_
#define  CPBK2REMOTECONTACTLOOKUPVISIBILITYIMPL_H_

//  INCLUDES
#include <e32base.h>
#include <MPbk2CmdItemVisibilityObserver.h>
#include "MPbk2RemoteContactLookupVisibility.h"

/** 
 *     //TODO (if needed at all for RCL)
 */
 
NONSHARABLE_CLASS( CPbk2RemoteContactLookupVisibilityImpl) : public CActive,
    public MPbk2RemoteContactLookupVisibilityImpl
	{
	public: // Construction & destruction

        /*
         * Factory method
         * 
         * @param aCmdId command id to use when notifying aObserver
        */
		static CPbk2RemoteContactLookupVisibilityImpl* NewL( TInt aCmdId);
		
		~CPbk2RemoteContactLookupVisibilityImpl();

    public: 
    
        /* Needed for unit testing purposes at least. This class
         * wont give a callback if visibility transition has not
         * occurred          
        */ 
        TBool Visibility();

        /*
         * Sets visibility
         *
         * @param aVisibility new visiblity value
        */
        void SetVisibility(TBool aVisibility);
       
    public: // MPbk2RemoteContactLookupVisibilityImpl
        
        void SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver);
		void NotifyVisibilityChange();
		TVariationFlag Variation();

    private: // CActive
    
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
        		   
	private: // constructor
	
	    CPbk2RemoteContactLookupVisibilityImpl( TInt aCmdId);
	
	    void ConstructL();
	
	private: // utility    		
    
        void StartL();
        TBool CalculateVisibilityL(const TInt aTopCount, const TInt aContactCount);
        		
        void ReadRemoteContactLookupVisibilityKeyL();		                                                  
        void SetRemoteContactLookupVisibilityKeyOFFL();

        void PrepareAsyncVisibilityCallback();    

	private: // data
	        
	    // This is a cenrep key value, which can be configured
	    // This variates the behaviour of visibility
	    // (i.e. always off/on or first time use)        
	    TVariationFlag iVariation; // own
	        
	    // Ref: visibility observer 
	    MPbk2CmdItemVisibilityObserver* iObserver;
	  	    	    
	    // Own: previous visiblity
	    TBool iPreviousVisibility; // own
	    // Own: add favorites command item id
	    TInt iCmdId;
	};

#endif /* CPBK2REMOTECONTACTLOOKUPVISIBILITYIMPL_H_*/
