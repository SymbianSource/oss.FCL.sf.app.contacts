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
* Description:  Phonebook 2 SIM contact processor.
*                A class that processes SIM contacts into appropritate form
*                for the copying process. Handles errors related to SIM
*                contact fields
*
*/


#ifndef CPSU2SIMCONTACTPROCESSOR_H
#define CPSU2SIMCONTACTPROCESSOR_H

//  INCLUDES
#include <e32base.h>
#include "Pbk2USimUI.hrh"

// FORWARD DECLARATIONS
class MVPbkBaseContact;
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkFieldTypeList;
class MVPbkFieldType;
class MVPbkBaseContactField;
class MPbk2FieldProperty;
class MPbk2ContactNameFormatter;
class CPsu2CopyToSimFieldInfoArray;
class CVPbkFieldTypeRefsList;
class MVPbkContactFieldTextData;
class CPsu2CharConv;
class RFs;

// CLASS DECLARATION

/**
 * Phonebook 2 SIM contact processor.
 * A class that processes SIM contacts into appropritate form
 * for the copying process. Handles errors related to SIM
 * contact fields
 */
class CPsu2SimContactProcessor : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aTargetStore                The target store for contacts.
         * @param aCopyToSimFieldInfoArray    Defines the fields to copy.
         * @param aNameFormatter              Phonebook 2 name formatter.
         * @param aMasterFieldTypeList        The list of all field types.
         * @param aIncludedTypes              An optional list of the field
         *                                    types that are copied if found
         *                                    from source contact. If NULL
         *                                    then all possible fields are
         *                                    copied.
         * @param aFs                         File system session for
         *                                    character conversion.
         * @return  A new instance of this class.
         */
        static CPsu2SimContactProcessor* NewL(
                MVPbkContactStore& aTargetStore,
                CPsu2CopyToSimFieldInfoArray& aCopyToSimFieldInfoArray,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                RFs& aFs );

        /**
         * Destructor.
         */
        virtual ~CPsu2SimContactProcessor();

    public: // Interface

        /**
         * Changes the state of the process if the error is SIM related.
         * If the error was handled the state of processor changes which
         * affects to SIM contact processing.
         *
         * @param aError    The error from the SIM store when using CommitL.
         * @return  ETrue if the error was handled.
         */
        TBool HandleSimError(
                TInt aError );

        /**
         * Creates a new sim contact according to included properties and
         * the source contact.
         *
         * @param aSourceContact    The contact to copy to SIM.
         * @param aSimContacts      An array into which SIM contacts
         *                          are appended. Client owns contacts.
         * @return   SIM contact that can be saved to SIM store.
         */
        void CreateSimContactsL(
                MVPbkStoreContact& aSourceContact,
                RPointerArray<MVPbkStoreContact>& aSimContacts );

        /**
         * If HandleSimError returns ETrue then this must be called to split
         * the current SIM contact.
         *
         * @param aSimContact   The SIM contact that is edited.
         * @param aSimContacts  An array into which SIM contacts
         *                      are splitted. Client owns new contacts.
         */
        void CreateFixedSimContactsL(
                MVPbkStoreContact& aSimContact,
                RPointerArray<MVPbkStoreContact>& aSimContacts );

        /**
         * If email or nick name fields get full
         * those fields are dropped
         *
         * @return ETrue if email or nick name fields are dropped
         */
        TBool DetailsDropped();

        /**
         * Returns the target store for the SIM contacts.
         *
         * @return  Target store for the SIM contacts.
         */
        inline MVPbkContactStore& TargetStore() const;

	      /**
         * Add the contact fields from the source contact to the target
         *
         * @param aSource    The source contact
         * @prarm aTarget    The target contact
         */
        void AddNameFieldsL(
                MVPbkStoreContact& aSource,
                MVPbkStoreContact& aTarget );
        
    private: // Implementation
        CPsu2SimContactProcessor(
                MVPbkContactStore& aTargetStore,
                CPsu2CopyToSimFieldInfoArray& aCopyToSimFieldInfoArray,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MVPbkFieldTypeList& aMasterFieldTypeList );
        void ConstructL( RFs& aFs );
        void RemoveFieldTypesFromIncludedTypes( 
                TPsu2ErrorCode aBlockingError );
        void CopyReadingFieldsL(
                MVPbkStoreContact& aSource,
                MVPbkStoreContact& aTarget );
        void AppendSupportedFieldsL(
                MVPbkBaseContact& aSource,
                MVPbkStoreContact& aTarget );
        TBool SplitToSimContactsL(
                MVPbkStoreContact* aSourceContact );
        MVPbkStoreContact* SplitContactLC(
                MVPbkStoreContact& aSimContact );
        TBool CopyFieldL(
                const MVPbkBaseContactField& aFieldToCopy,
                MVPbkStoreContact& aTarget,
                const MVPbkFieldType& aSimType );
        void CopyTitleFieldDataL(
                MVPbkStoreContact& aSource,
                MVPbkStoreContact& aTarget,
                const MVPbkFieldType& aSimTitleFieldType );
        TBool CopyFieldDataL(
                const TDesC& aSource,
                MVPbkContactFieldTextData& aTarget,
                const MVPbkFieldType& aSimType );
        void TruncateAndCopyFieldDataL(
                const TDesC& aSource,
                MVPbkContactFieldTextData& aTarget );
        TInt MaxNumberOfFieldL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aType );
        void RemoveContactsThatHaveOnlyNameL();
        TBool IsValidContactToSaveL(
                MVPbkStoreContact& aSimContact );

    private: // Data
        /// Ref: The target store of the copied contact
        MVPbkContactStore& iTargetStore;
        /// Own: Defines the fields to copy
        RPointerArray<const MVPbkFieldType> iIncludedTypes;
        /// Ref: Copy to SIM field infos
        CPsu2CopyToSimFieldInfoArray& iCopyToSimFieldInfoArray;
        /// Ref: Phonebook 2 name formatter for title/name setting
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: The list of all the field types
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Own: the number of KErrGsmSimServAnrFull errors from TSY
        ///      intialized to 0
        TInt iNumOfAdditionalNumberErrors;
        /// Own: a collection of TPsu2ErrorCode
        TUint8 iSimErrors;
        /// Own: The maximum matching priority of SIM fields
        TInt iSimMaxMatchPriority;
        /// Own: An array for new SIM contacts
        RPointerArray<MVPbkStoreContact> iNewSimContacts;
        /// Own: Converter for checking field length in UCS-2 encoding
        CPsu2CharConv* iCharConvUcs2;
        /// Own: Converter for checking field length in SMS 7-bit encoding
        CPsu2CharConv* iCharConvSms7Bit;
    };


// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::TargetStore
// --------------------------------------------------------------------------
//
inline MVPbkContactStore& CPsu2SimContactProcessor::TargetStore() const
    {
    return iTargetStore;
    }

#endif // CPSU2SIMCONTACTPROCESSOR_H

// End of File
