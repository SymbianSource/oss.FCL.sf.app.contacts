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
* Description:  A contact copy inspector.
*
*/


#ifndef CPSU2CONTACTCOPYINSPECTOR_H
#define CPSU2CONTACTCOPYINSPECTOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MPbk2ContactNameFormatter;
class CVPbkBaseContactFieldTypeListIterator;

/**
 * The class inspects if the phone contact was copied completely to
 * sim contact(s).
 */
class CPsu2ContactCopyInspector : public CBase
    {
    public:
        /**
         * Static constructor for this class
         *
         * @param aNameFormatter Name formatter
         * @return New instance of this class
         */
        static CPsu2ContactCopyInspector* NewL( 
            MPbk2ContactNameFormatter& aNameFormatter );
        
        /** 
         * Destructor
         */
        ~CPsu2ContactCopyInspector();
        
    public: // Interface   
        
        /**
         * Inspects if all the fields from aSourceContact where copied to
         * aTargetContacts. 
         * @param aSourceContact the contact to copy to sim
         * @param aSimContacts an array into which sim contacts are appended.
         *        Client owns contacts.
         * @return ETrue if all fields were copied otherwise EFalse.
         */
        TBool IsCopiedCompletelyL( MVPbkStoreContact& aSourceContact,
            RPointerArray<MVPbkStoreContact>& aSimContacts );
        
    private: // Implementation
        CPsu2ContactCopyInspector( MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL();
        TBool CheckDataFieldsL( MVPbkStoreContact& aSourceContact,
            RPointerArray<MVPbkStoreContact>& aSimContacts );
        TInt ExpectedSimContactCountL( MVPbkStoreContact& aSourceContact );
        TBool HasReadingFields( CVPbkBaseContactFieldTypeListIterator& aTitleFields, 
            MVPbkStoreContact& aSourceContact );
        
    private: // Data
        /// Ref: Phonebook2 name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Reference to the source contact
        MVPbkStoreContact* iSourceContact; 
        /// Ref: Reference to the target contacts
        RPointerArray<MVPbkStoreContact>* iSimContacts;        
    };

#endif // CPSU2CONTACTCOPYINSPECTOR_H

// End of file
