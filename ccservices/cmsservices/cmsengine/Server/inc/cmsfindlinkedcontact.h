/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description: This class provides interface to find the First Contact in DefaultCntDbUri 
*  which is linked to an XSPId.
*
*/

#ifndef __CMSFINDLINKEDCONTACT_H__
#define __CMSFINDLINKEDCONTACT_H__

// INCLUDES
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <badesca.h>
#include <e32base.h>

//FORWARD DECLARATIONS
class MVPbkStoreContact;
class CVPbkContactManager;
class MVPbkContactStoreList;
class MVPbkContactLink;

/*
 * This class provides interface to find the First Contact in DefaultCntDbUri which is linked to
 * an XSPId.
 * 
 *  To be used on a XSPContact
    **********************************
    If contacts is from XSP Store - then the challenge is to find whether this xspid 
    is linked to the contact in DefaultCntDbUri or not    
    How?
    We need to search the entire DefaultCntDbUri() for any contact which is linked to this xspcontact
    if more than 1 contact is linked to the same xspid, smartly choose anyone link(first link)
    Once having found this linked contact in the DefaultCntDbUri(), use this found
    contact to retreive the info
 */


NONSHARABLE_CLASS( CCmsFindLinkedContact ) : public CActive,         
                   public MVPbkSingleContactOperationObserver,
                   public MVPbkContactFindFromStoresObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Public constructor
        *
        * @param aContactManager -
        * @return Pointer to CCmsFindLinkedContact instance
        */
        static CCmsFindLinkedContact* NewL( CVPbkContactManager& aContactManager );

	/**
        * Fetches the Linked Contact
        *
        * @param aXSPContactLink - XSP Contact
        * @return Contact that was linked to this XSP Contact. If the xsp contact was not 
	* linked to any contact, then it returns the link of the xsp contact.
        */
        const MVPbkContactLink& FetchLinkedContactL( const MVPbkContactLink& aXSPContactLink );
   
    public:  //static

        /**
        * Implementation of word parser function that separates the field data 
        * into words. 
        *
        * @param aWordParserParam TVPbkWordParserCallbackParam
        * @return Parsing status code  
        */
        static TInt WordParserL( TAny* aWordParserParam );
    
    private:  //From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );
    
    private: // From CActive
            void RunL();
            void DoCancel();
            TInt RunError(
                    TInt aError );    
        
    private: 
        
        /**
        * C++ constructor
        *
        * @param aContactManager -        
        * @return CCmsFindLinkedContact
        */
        CCmsFindLinkedContact( CVPbkContactManager& aContactManager );           
        
        /**
        * Dtor
        *
        */
        ~CCmsFindLinkedContact();
    
    private: // from MVPbkContactFindFromStoresObserver
        
        void FindFromStoreSucceededL( MVPbkContactStore& aStore, 
            MVPbkContactLinkArray* aResultsFromStore );

        void FindFromStoreFailed( 
                MVPbkContactStore& aStore, TInt aError );

        void FindFromStoresOperationComplete() ;

    private:
        
        /**
        * Retreives the XSP Id from the MVPbkStoreContact.
        * Now find the contact in DefaultCntDbUri() for any contact 
        * which is linked to this xspcontact.
        * If more than 1 contact is linked to the same xspid, smartly 
        * choose anyone link(first link). Once having found this linked 
        * contact in the DefaultCntDbUri(), use this found
        * contact to retreive the info
        *
        * @param aXSPContact - XSP Contact
        */
        void RetreiveAndFindLinkedContactL( const MVPbkStoreContact& aXSPContact );
        
        
        /**
         * Issues Request
         *                  
         */
        void IssueRequest();
        
    private:  // Data structures
            /// Process states
            enum TState
                {
                ERetreiveXSPInfo,
                EFindLinkedContact,
                EComplete
                };
            
    private: //Data
        
        //Contact manager. Owned.
        CVPbkContactManager&                    iContactManager;
        
        //XSP Contact - Owns
        MVPbkStoreContact*                      iRetreivedXSPContact;        
        
        /// Array of find operations in xSP stores. Owned.
        RPointerArray<MVPbkContactOperationBase> iOperationArray;
        
        //Owns
        MVPbkContactLink*                       iContactLink;
        
        //State of the operatiob
        TState                                  iState;
        
        //OWN: Active scheduler wait for waiting the completion
        CActiveSchedulerWait                    iWait;
    };

#endif  //__CMSFINDLINKEDCONTACT_H__


// End of File
