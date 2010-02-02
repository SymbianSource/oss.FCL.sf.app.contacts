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
* Description:  A class that can convert sim contacts to etel contacts and vice
*                versa
*
*/



#ifndef CVPBKETELCNTCONVERTER_H
#define CVPBKETELCNTCONVERTER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPhoneBookBuffer;
class CVPbkSimContactBuf;
class CVPbkSimContact;
class CVPbkSimCntField;
class MVPbkSimCntStore;

// CLASS DECLARATION

/**
*  A class that can convert sim contacts to etel contacts and vice versa.
*
*  @lib VPbkSimStoreImpl.lib
*/
class CVPbkETelCntConverter : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkETelCntConverter* NewL();

        /**
        * Two-phased constructor.
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkETelCntConverter* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CVPbkETelCntConverter();

    public: // New functions
        
        /**
        * Converts ETel contacts to VPbk SIM contacts.
        * @param aResultArray an array for the converted contacts.
        *        After the call the array contains a contact or contacts
        * @param aETelContactData an ETel data buffer that can contain one
        *        contact or contacts
        * @param aSimStore the sim store in which the contacts belongs
        */
        void ConvertFromETelToVPbkSimContactsL( 
            RPointerArray<CVPbkSimContactBuf>& aResultArray,
            TDes8& aETelContactData,
            MVPbkSimCntStore& aSimStore ) const;

        /**
        * Converts one ETel contact to one VPbk SIM contact.
        * @param aETelContact one ETel contact. Sim index is not necessary.
        * @param aEmptyContact an empty contact that is filled
        * @exception KErrBadDescriptor if contact header is not found
        */
        void ConvertFromETelToVPbkSimContactL( TDes8& aETelContact,
            CVPbkSimContact& aEmptyContact ) const;

        /**
        * Converts VPbk SIM fields to one ETel contact.
        * @param aFieldArray an array of VPbk SIM fields
        * @param aSimIndex the sim index of the contact that owns fields
        * @return a buffer containing an ETel contact
        */
        HBufC8* ConvertFromVPbkSimFieldsToETelCntLC( 
            const TArray<CVPbkSimCntField*>& aFieldArray, 
            TInt aSimIndex ) const;

        /**
        * Finds the sim index from the ETel contact
        * @param aETelContact a buffer containing one ETel contact
        * @return sim index of the contact, KVPbkSimStoreFirstFreeIndex
        *   if not found, KErrBadDescriptor if descriptor is invalid
        */
        TInt SimIndex( TDes8& aETelContact ) const;
        
        /**
        * Gets the amount of supported fields in the ETel contact
        * @param aETelContact a buffer containing one ETel contact
        * @return the amount of supported fields (TVPbkSimCntFieldType)
        *         or KErrBadDescriptor if aETelContact is not valid.
        */
        TInt FieldCount( TDes8& aETelContact ) const;

        /**
        * Sets the field data to the given field
        * @param aETelContact a buffer containing one ETel contact
        * @param aIndex the index of the field in the contact
        * @param aField the field that will be set by the function
        */
        void FieldAt( TDes8& aETelContact, TInt aIndex, 
            CVPbkSimCntField& aField ) const;

    private: // Construction

        /**
        * C++ default constructor.
        */
        CVPbkETelCntConverter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions
        TBool MoveToNextContact() const;
        TInt StartRead() const;
        TBool GetNextField( CVPbkSimCntField& aField ) const;
        TInt NumberOfFields() const;
        TPtrC8 GetNextContactData( TDes8& aBuffer ) const;
        TInt DoFieldCount( TDes8& aETelContact, TInt& aError ) const;
        
    private:    // Data
        /// Own: the ETel buffer for reading/writing an ETel contact
        CPhoneBookBuffer* iETelBuffer;
        /// The length of the new entry tag
        TInt iNewEntryTagLength;
        /// The length of the contact that has a sim index but no fields
        TInt iMinContactLength;
    };

#endif      // CVPBKETELCNTCONVERTER_H
            
// End of File
