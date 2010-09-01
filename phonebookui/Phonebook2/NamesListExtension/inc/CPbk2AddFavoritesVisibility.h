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


#ifndef  CPBK2ADDFAVORITESVISIBILITY_H_
#define  CPBK2ADDFAVORITESVISIBILITY_H_

//  INCLUDES
#include <e32base.h>

// VPbk
#include <MVPbkContactViewObserver.h>

// Pbk2
#include <MPbk2StoreConfigurationObserver.h>
#include "MPbk2AddFavoritesVisibility.h"
#include <MPbk2CmdItemVisibilityObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class CPbk2AddFavoritesVisibilityImpl;
class MPbk2ApplicationServices;

/**
 * In order to know whether or not "Add favorites" should be visible
 * The following must be known:
 * 
 * (1) Are there Top Contacts? (favorites) 
 * (2) Number of contacts in all contacts view
 * (3) Whether "Settings / Memory selection" contains Phone memory
 * (4) Store Configuration changes
 *
 * (1) & (2) require all contacts and top contacts database
 * view creation and and error handling regarding those. 
 *
 * (4) requires store configuration observation & event handling
 * 
 * This class does all these and gathers all data needed for decesion
 * and forwards it to CPbk2AddFavoritesVisibilityImpl class
 */
NONSHARABLE_CLASS( CPbk2AddFavoritesVisibility ) : public CBase,
    public MVPbkContactViewObserver,
    public MPbk2StoreConfigurationObserver
	{
	public: // Construction & destruction

        /* @param aCmdId: a nameslist command item id
         * @MPbk2ApplicationServices& aAppServices: Pbk2 App Services
         * object.
         */
		static CPbk2AddFavoritesVisibility* NewL(		    
		    TInt aCmdId, MPbk2ApplicationServices& aAppServices);
		
		~CPbk2AddFavoritesVisibility();
    
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
    
        	
	private: //	MVPbkContactViewObserver
	    
	    void ContactViewReady(
	            MVPbkContactViewBase& aView );
	            
	    void ContactViewUnavailable(
            MVPbkContactViewBase& aView );
            
	    void ContactAddedToView(
            MVPbkContactViewBase& aView,
            TInt aIndex,
            const MVPbkContactLink& aContactLink );
            
	    void ContactRemovedFromView(
            MVPbkContactViewBase& aView,
            TInt aIndex,
            const MVPbkContactLink& aContactLink );
            
	    void ContactViewError(
            MVPbkContactViewBase& aView,
            TInt aError,
            TBool aErrorNotified );
	    
	private:
	
		CPbk2AddFavoritesVisibility(MPbk2ApplicationServices& aAppServices);
	
	    void ConstructL( TInt aCmdId );
		
        TBool IsContactDatabaseLoadedL() const;
        TBool ObserveContactViewsL();
		void StartObservingStoreConfigurationL();  
        void Reset();
		void HandleConfigurationChangedL();    
        void ReadAddFavoritiesVisibilityKeyL();		                                          
        void StopObservingStoreConfiguration();
        void NotifyVisibilityChange();        
        void UpdateContactCount(MVPbkContactViewBase& aView);
        void SetContactCount(MVPbkContactViewBase& aView, TInt aCount);
        TInt TopContactCount();
        TInt AllContactsCount();        
        void StartL();               
	
	private: // data
	          
	    // Ref: top contacts view
	    MVPbkContactViewBase* iTopContactsView;

	    // Ref: all contacts view
	    MVPbkContactViewBase* iAllContactsView;	    	    
	    
	    // Handle to implementor in bridge pattern
	    // Taking care of decesion logic on visibility plus
	    // informing the visibility observer
	    CPbk2AddFavoritesVisibilityImpl* iImpl;

	    // Own:  top contact count if the view is in valid, KErrNotFound otherwise
        TInt iTcCount;
        // Own:  all contacts view count if the view is valid, KErrNotFound otherwise
        TInt iAllContactsCount;    
        // ref, used to reach view suplier & database views
        MPbk2ApplicationServices& iApplicationServices;
	};

#endif /* CPBK2ADDFAVORITESVISIBILITY_H_*/
