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
* Description:  Contact model store Voice tag attribute manager
*
*/


#ifndef CSINDVOICETAGATTRIBUTEMANAGER_H
#define CSINDVOICETAGATTRIBUTEMANAGER_H

// INCLUDES
#include "cvoicetagattributemanager.h"

#include "ccontactlink.h"

// FORWARD DECLARATIONS
class MVPbkContactOperation;
class CVPbkFieldTypeSelector;
class MVPbkFieldType;
class CNssVASDBMgr;
class MNssContext;
class MNssContextMgr;
class MNssTagMgr;

namespace VPbkCntModel {

/**
 * Contact model store Voice tag attribute manager.
 */
NONSHARABLE_CLASS( CSindVoiceTagAttributeManager ): 
        public CVoiceTagAttributeManager
    {
    public: 
        /**
         * Creates a new instance of this class.
         *
         * @param aParam,
         * @return a new instance of this class             
         */
        static CSindVoiceTagAttributeManager* NewL(TAny* aParam);

        /**
         * Destructor.
         */
        ~CSindVoiceTagAttributeManager();

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
        CSindVoiceTagAttributeManager(CVPbkContactManager& aContactManager);
        void ConstructL();

    private: // Data
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Vas databse manager
        mutable CNssVASDBMgr* iVasDbManager;
        /// Own: Nss context
        mutable MNssContext* iContext;
        /// Ref: Context manager
        mutable MNssContextMgr* iNssContextManager;
        /// Ref: Tag manager
        mutable MNssTagMgr* iTagManager;
        /// Error code received from observer call-back methods.
        mutable TInt iErr;
        /// Id of the voice tagged field id
        TInt iVoiceTaggedFieldId;
        /// Has the SindHandler been initialized
        mutable TBool iInitialized;
    };

}

#endif // CSINDVOICETAGATTRIBUTEMANAGER_H

// End of File
