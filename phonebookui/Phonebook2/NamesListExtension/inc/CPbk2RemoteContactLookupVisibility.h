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


#ifndef  CPBK2RemoteContactLookupVISIBILITY_H_
#define  CPBK2RemoteContactLookupVISIBILITY_H_

//  INCLUDES
#include <e32base.h>

// VPbk
#include <MVPbkContactViewObserver.h>

// Pbk2
#include <MPbk2StoreConfigurationObserver.h>
#include "MPbk2RemoteContactLookupVisibility.h"
#include <MPbk2CmdItemVisibilityObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class CPbk2RemoteContactLookupVisibilityImpl;
class MPbk2ApplicationServices;

/**
 * //TODO (if needed at all for RCL)
 */
NONSHARABLE_CLASS( CPbk2RemoteContactLookupVisibility ) : public CBase,
    public MPbk2StoreConfigurationObserver
	{
	public: // Construction & destruction

        /* @param aCmdId: a nameslist command item id
         * @MPbk2ApplicationServices& aAppServices: Pbk2 App Services
         * object.
         */
		static CPbk2RemoteContactLookupVisibility* NewL(		    
		    TInt aCmdId, MPbk2ApplicationServices& aAppServices);
		
		~CPbk2RemoteContactLookupVisibility();
    
    public: // visibility observer setting
    
        /*
        * Set's the observer for the impl class and  
        * triggers and async callback to observer
        *
        * @param aObserver: (i.e. nameslistcontrol's ready state obj)
        * obj who is interested in visibility on/off events
        * 
        */        
        void SetVisibilityObserver
            (MPbk2CmdItemVisibilityObserver* aObserver);          
        
        /*
         * Sets visibility
         *
         * @param aVisibility new visiblity value
        */
        void SetVisibility(TBool aVisibility);   
        
        /*
         * Get visibility
         *
         * @return visiblity value
        */
        TBool Visibility();        

    private: // MPbk2StoreConfigurationObserver
    
        void ConfigurationChanged();
 
        void ConfigurationChangedComplete();
    
    
	private:
	
		CPbk2RemoteContactLookupVisibility(MPbk2ApplicationServices& aAppServices);
	
	    void ConstructL( TInt aCmdId );
		
		void StartObservingStoreConfigurationL();  
        void Reset();
		void HandleConfigurationChangedL();    
        void ReadRemoteContactLookupVisibilityKeyL();		                                          
        void StopObservingStoreConfiguration();
        void NotifyVisibilityChange();        
        void StartL();               
	
	private: // data
	    
	    // Handle to implementor in bridge pattern
	    // Taking care of decesion logic on visibility plus
	    // informing the visibility observer
	    CPbk2RemoteContactLookupVisibilityImpl* iImpl;
    
        // ref, used to reach view suplier & database views
        MPbk2ApplicationServices& iApplicationServices;
	};

#endif /* CPBK2RemoteContactLookupVISIBILITY_H_*/
