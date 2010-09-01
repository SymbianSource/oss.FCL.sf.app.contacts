/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler to manage owncard property during import and export
*                
*
*/


#ifndef CVPBKOWNCARDHANDLER_H
#define CVPBKOWNCARDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkSingleContactLinkOperationObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include "CVPbkVCardParserParamArray.h"
#include "CVPbkVCardData.h"
#include <vprop.h>
#include <vcard.h>


// CLASS DECLARATIONS
/**
 * 
 */
class CVPbkOwnCardHandler : 
        public CBase,
        public MVPbkSingleContactLinkOperationObserver,
        public MVPbkSingleContactOperationObserver,
        private MVPbkContactObserver
      
    {
    public:
        static CVPbkOwnCardHandler* NewL(CVPbkVCardData& aData);
        
        ~CVPbkOwnCardHandler();
        
        /**
		* Retrieve and initialize the ownContactLink if it exists
		* @param    aStore  Store for which ownContactLink must be fetched
		* 
		*/
      	void  DoInitL(MVPbkContactStore& aStore );
      	
      	/**
		* Create X-SELF property to be added to the parser
		* @return  CParserProperty*  X-SELF property to be 
		*								added to the parser
		*/
      	CParserProperty* CreateXSelfPropertyL();
      	
      	/**
		* Set the contactLink as Own Contact
		* @param  aContactLink  contactLink which must be set as OwnContact
		*								
		*/
      	void SetAsOwnContactL(const MVPbkContactLink& aContactLink);
      	
      	/**
		* Check if the given contact is ownContact
		* @return  TBool  ETrue if contact is OwnContact 
		*				  EFalse if contact is not OwnContact
		*/
        
        TBool IsOwnContactL(const MVPbkStoreContact* aContact);
        
    private:
    
        CVPbkOwnCardHandler(CVPbkVCardData& aData);
        
        void ConstructL();
        
        /**
		* From MVPbkSingleContactLinkOperationObserver  
		* Called when the contact link operation is complete
		*/ 
        void VPbkSingleContactLinkOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkContactLink* aContactLink );
	
		/**
		* From MVPbkSingleContactLinkOperationObserver  
		* Called when the contact link operation fails
		*/ 
		void VPbkSingleContactLinkOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError );
                
		/**
		* From MVPbkSingleContactOperationObserver  
		* Called when the contact operation is complete
		*/
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
		
		/**
		* From MVPbkSingleContactOperationObserver  
		* Called when the contact operation fails
		*/
        void VPbkSingleContactOperationFailed
            (MVPbkContactOperationBase& aOperation, TInt aError);
		
		/**
		* From MVPbkContactObserver  
		* Called when a contact operation has succesfully completed.
		*/
        void ContactOperationCompleted(TContactOpResult aResult);
        
        /**
		* From MVPbkContactObserver  
		* Called when a contact operation has failed.
		*/
        void ContactOperationFailed
            (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);
            

    	CParserPropertyValue* GetselfValueL();
    	
    	void CloneContactLinkL(MVPbkContactLink* aContactLink);
		
     private:
     
        CActiveSchedulerWait *iWait;
     	CVPbkVCardData& iData;
     	MVPbkContactLink* iContactLink;
     	MVPbkStoreContact* iContact;
    };

#endif // CVPBKOWNCARDHANDLER_H
// End of file
