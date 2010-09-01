/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A copy policy for copying to sim store
*
*/


#ifndef CPSU2CONTACTCOPYPOLICY_H
#define CPSU2CONTACTCOPYPOLICY_H

//  INCLUDES
#include <CVPbkContactCopyPolicy.h>

// FORWARD DECLARATIONS
class CPbk2FieldPropertyArray;
class CPsu2CopyToSimFieldInfoArray;
class CPsu2SimContactProcessor;
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;

// CLASS DECLARATION

/**
*  A copy policy for copying to sim store
*
*/
class CPsu2ContactCopyPolicy : public CVPbkContactCopyPolicy
    {
    public:  // Constructors and destructor        
        /**
         * Two-phased constructor.
         * @param aParam parameters defined in virtual phonebook
         * @return a new instance of this class
         */
        static CPsu2ContactCopyPolicy* NewL(TParam* aParam);
        
        /**
        * Destructor.
        */
        ~CPsu2ContactCopyPolicy();
    
    public: // New functions
    
        /**
        * Returns the phonebook2 field properties
        * @return the phonebook2 field properties
        */
        inline CPbk2FieldPropertyArray& FieldProperties() const;
        
        /**
        * Returns the copy to sim field information
        * @return the copy to sim field information
        */
        inline CPsu2CopyToSimFieldInfoArray& SimFieldInfos() const;
        
        /**
        * Returns the sim contact processor
        * @return the sim contact processor
        */
        inline CPsu2SimContactProcessor& SimContactProcessor() const;
        
    public: // Functions from base classes

        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact, 
                const MVPbkFieldType& aFieldType,
                const TDateTime& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver);

        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact, 
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver);

        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* AddFieldToContactL(
                MVPbkStoreContact& aContact, 
                const MVPbkFieldType& aFieldType,
                const TDesC8& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver);

        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* CopyTitleFieldsL(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MVPbkContactFieldsCopyObserver& aCopyObserver); 
                
        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* CommitContactL(
                MVPbkStoreContact* aContact,
                MVPbkContactCopyObserver& aCopyObserver);

        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* CopyContactL(
                MVPbkStoreContact& aContact,
                MVPbkContactStore& aTargetStore,
                MVPbkContactCopyObserver& aCopyObserver);
        
        /**
        * From MVPbkContactCopyPolicy
        */
        TBool SupportsContactMerge() const;
        
        /**
        * From MVPbkContactCopyPolicy
        */
        MVPbkContactOperationBase* MergeAndSaveContactsL(
            RPointerArray<MVPbkStoreContact>& aSourceContacts, 
            MVPbkStoreContact& aTarget,
            MVPbkContactCopyObserver& aCopyObserver );

    private: // Construction

        /**
        * C++ constructor.
        */
        CPsu2ContactCopyPolicy(CVPbkContactManager& aContactManager);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        void ReplaceSimContactProcessorL(MVPbkContactStore& aTargetStore);
            
    private: // Data
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: UI field properties
        CPbk2FieldPropertyArray* iPropertyArray;
        /// Own: an array of field information about copying rules
        CPsu2CopyToSimFieldInfoArray* iSimFieldInfos;
        /// Own: a class that handles sim contact splitting
        CPsu2SimContactProcessor* iSimContactProcessor;
        /// Own: sort order manager for createing name formatter
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: the name formatter for setting name field for sim contact
        MPbk2ContactNameFormatter* iNameFormatter;
    };

// INLINE FUNCTIONS
inline CPbk2FieldPropertyArray& CPsu2ContactCopyPolicy::FieldProperties() const
    {
    return *iPropertyArray;
    }
            
inline CPsu2CopyToSimFieldInfoArray& 
        CPsu2ContactCopyPolicy::SimFieldInfos() const
    {
    return *iSimFieldInfos;
    }

#endif // CPSU2CONTACTCOPYPOLICY_H
            
// End of File
