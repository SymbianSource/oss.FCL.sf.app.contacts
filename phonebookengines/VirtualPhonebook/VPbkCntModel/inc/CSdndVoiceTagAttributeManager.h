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


#ifndef CSDNDVOICETAGATTRIBUTEMANAGER_H
#define CSDNDVOICETAGATTRIBUTEMANAGER_H

// INCLUDES
#include "cvoicetagattributemanager.h"

// FORWARD DECLARATIONS
class MVPbkContactOperation;   

namespace VPbkCntModel {
    
/**
 * Contact model store Voice tag attribute manager.
 */
NONSHARABLE_CLASS( CSdndVoiceTagAttributeManager ): 
        public CVoiceTagAttributeManager
    {
    public: // Construction and destruction
        /**
         * Creates new instance of this class
         * @param aParam Parameters for creation
         * @return A new instance of this class
         */
        static CSdndVoiceTagAttributeManager* NewL(TAny* aParam);
        ~CSdndVoiceTagAttributeManager();
        
    public: // From CVPbkContactAttributePlugin
        MVPbkContactOperationBase* ListContactsL(
                TUid aAttributeType, 
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperationBase* ListContactsL(
                const MVPbkContactAttribute& aAttribute, 
                MVPbkContactFindObserver& aObserver);
        TBool HasContactAttributeL(
                TUid aAttributeType, 
                const MVPbkStoreContact& aContact) const;
        TBool HasContactAttributeL(
                const MVPbkContactAttribute& aAttribute, 
                const MVPbkStoreContact& aContact) const;
        TBool HasFieldAttributeL(
                TUid aAttributeType, 
                const MVPbkStoreContactField& aField) const;
        TBool HasFieldAttributeL(
                const MVPbkContactAttribute& aAttribute, 
                const MVPbkStoreContactField& aField) const;
        MVPbkContactOperationBase* SetContactAttributeL(
                const MVPbkContactLink& aContactLink, 
                const MVPbkContactAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkContactOperationBase* SetFieldAttributeL(
                MVPbkStoreContactField& aField, 
                const MVPbkContactAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkContactOperationBase* RemoveContactAttributeL(
                const MVPbkContactLink& aContactLink, 
                const MVPbkContactAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkContactOperationBase* RemoveContactAttributeL(
                const MVPbkContactLink& aContactLink, 
                TUid aAttributeType,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkContactOperationBase* RemoveFieldAttributeL(
                MVPbkStoreContactField& aField, 
                const MVPbkContactAttribute& aAttribute,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkContactOperationBase* RemoveFieldAttributeL(
                MVPbkStoreContactField& aField, 
                TUid aAttributeType,
                MVPbkSetAttributeObserver& aObserver);
        MVPbkStoreContactFieldCollection* FindFieldsWithAttributeLC(
                TUid aAttibuteType,
                MVPbkStoreContact& aContact) const;
        MVPbkStoreContactFieldCollection* FindFieldsWithAttributeLC(
                const MVPbkContactAttribute& aAttribute,
                MVPbkStoreContact& aContact) const; 
        MVPbkContactOperation* CreateListContactsOperationL(
                TUid aAttributeType, 
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperation* CreateListContactsOperationL(
                const MVPbkContactAttribute& aAttribute, 
                MVPbkContactFindObserver& aObserver);                      
        
    private: // Implementation
        CSdndVoiceTagAttributeManager(CVPbkContactManager& aContactManager);
        void ConstructL();   
                
    private: // Data
        ///Own: Contact manager
        CVPbkContactManager& iContactManager;
    };
    
}

#endif // CSDNDVOICETAGATTRIBUTEMANAGER_H

// End of File
