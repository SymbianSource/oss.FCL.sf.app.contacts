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
* Description:  An engine level attribute manager implementation that is
*                constructed in CVPbkContactManager. 
*                Loads store level attribute managers as ECOM components.
*
*/


#ifndef CVPBKCONTACTATTRIBUTEMANAGER_H
#define CVPBKCONTACTATTRIBUTEMANAGER_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactAttributeManager.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CVPbkContactAttributePlugin;

/**
 *  An engine level attribute manager for Virtual Phonebook clients. 
 *  Loads store level attribute managers as ECOM components.
 *
 */
NONSHARABLE_CLASS(CVPbkContactAttributeManager) :   
        public CBase,
        public MVPbkContactAttributeManager
    {
    public: // Construction and destruction  
        /**
         * @param aContactManager for attribute manager implementations.
         */
        static CVPbkContactAttributeManager* NewL(
            CVPbkContactManager& aContactManager );
            
        ~CVPbkContactAttributeManager();

    public: // from base class MVPbkContactAttributeManager
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
                TUid aAttributeType,
                MVPbkStoreContact& aContact) const;
        MVPbkStoreContactFieldCollection* FindFieldsWithAttributeLC(
                const MVPbkContactAttribute& aAttribute,
                MVPbkStoreContact& aContact) const;
        MVPbkStoreContactField* FindFieldWithAttributeL(
                const MVPbkContactAttribute& aAttr,
                MVPbkStoreContact& aContact) const;
        MVPbkContactOperationBase* HasContactAttributeL(
                TUid aAttributeType, 
                const MVPbkStoreContact& aContact,
                MVPbkSingleAttributePresenceObserver& aObserver) const;
        MVPbkContactOperationBase* HasContactAttributeL(
                const MVPbkContactAttribute& aAttribute, 
                const MVPbkStoreContact& aContact,
                MVPbkSingleAttributePresenceObserver& aObserver) const;
        MVPbkContactOperationBase* HasFieldAttributeL(
                TUid aAttributeType, 
                const MVPbkStoreContactField& aField,
                MVPbkSingleAttributePresenceObserver& aObserver) const;
        MVPbkContactOperationBase* HasFieldAttributeL(
                const MVPbkContactAttribute& aAttribute, 
                const MVPbkStoreContactField& aField,
                MVPbkSingleAttributePresenceObserver& aObserver) const;
        MVPbkContactOperationBase* FindFieldsWithAttributeL(
                TUid aAttributeType,
                MVPbkStoreContact& aContact,
                MVPbkMultiAttributePresenceObserver& aObserver) const;
        MVPbkContactOperationBase* FindFieldsWithAttributeL(
                const MVPbkContactAttribute& aAttribute,
                MVPbkStoreContact& aContact,
                MVPbkMultiAttributePresenceObserver& aObserver) const;

    private: // Implementation
        CVPbkContactAttributeManager(CVPbkContactManager& aContactManager);
        void ConstructL();
        void LoadAttributePluginsL();

    private: // Data
        /// Ref: Contact Manager
        CVPbkContactManager& iContactManager;
        /// Own: Array of attribute plugins
        RPointerArray<CVPbkContactAttributePlugin> iAttributePlugins;
    };

#endif // CVPBKCONTACTATTRIBUTEMANAGER_H

// End of File

