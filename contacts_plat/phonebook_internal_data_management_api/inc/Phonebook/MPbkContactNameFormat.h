/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Abstract Phonebook contact name formatting interface.
*
*/


#ifndef __MPbkContactNameFormat_H__
#define __MPbkContactNameFormat_H__

// INCLUDES
#include <PbkFields.hrh>    // TPbkFieldId

// FORWARD DECLARATIONS
class MPbkFieldDataArray;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Abstract Phonebook contact name formatting interface.
 */
class MPbkContactNameFormat
    {
    public:  // Interface
        /**
         * Virtual destructor.
         */
        virtual ~MPbkContactNameFormat() { }

        /**
         * Gets a title text for a contact.
         *
         * @param   aContactData a field data array representing the data in
         *          the contact.
         * @return  a buffer containing the title, NULL if no title can be 
         *          created. Caller is responsible for deleting the returned 
         *          buffer.
         */
        virtual HBufC* GetContactTitleOrNullL
            (const MPbkFieldDataArray& aContactData) const = 0;

        /**
         * Gets a title text for a contact or qtn_phob_unnamed.
         *
         * @param   aContactData a field data array representing the data in
         *          the contact.
         * @return  a buffer containing the title or qtn_phob_unnamed if no
         *          title can be generated. Caller is responsible for deleting 
         *          the returned buffer.
         */
        virtual HBufC* GetContactTitleL
            (const MPbkFieldDataArray& aContactData) const = 0;

        /**
         * Returns name text to use for unnamed contacts.
         */
        virtual const TDesC& UnnamedText() const =0;

        /**
         * Returns ETrue if field is part of the fields used to build the
         * contact title.
         *
         * @param aFieldId  id of field.
         * @return  ETrue if aFieldId is type of a field used to build contact
         *          titles.
         * @see GetContactTitleOrNullL
         */ 
        virtual TBool IsTitleField(TPbkFieldId aFieldId) const = 0;

        /**
         * Gets a title text for a contact.
         *
         * @param   aContactData    a field data array representing the data
         *                          in the contact.
         * @param   aTitle  a buffer where the title text is formatted.
         *                  The buffer must have enough capacity to hold the 
         *                  formatted title, otherwise this function will 
         *                  panic.
         * @see ContactTitleLength
         * @postcond aTitle.Length()==ContactTitleLength(aContactData)
         */
        virtual void GetContactTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const = 0;

        /**
         * Calculates title text length for a contact
         *
         * @param   aContactData a field data array representing the data in
         *          the contact.
         * @return  the length of the formatted contact title in characters.
         */
        virtual TInt ContactTitleLength
            (const MPbkFieldDataArray& aContactData) const = 0;
    
        /**
         * Formats aTitle according to Phonebook name formatting rules.
         *
         * @return a buffer containing the formatted title.
         */
        virtual HBufC* FormatNameDescriptorL(const TDesC& aTitle) const = 0;

        /**
         * Returns filtered contact data which contains only fields that will
         * be used in contact name formatting.
         *
         * @param aContactData  contact data to filter title fields from.
         * @return  a filtered contact data that contains those fields from
         *          aContactData which participate in contact title formatting.
         *          The fields are in presentation order. The returned 
         *          reference is invalidated if aContactData is modified and
         *          when this function is called the next time.
         */
        virtual const MPbkFieldDataArray& FilterContactTitleFields
            (const MPbkFieldDataArray& aContactData) const =0;

        /**
         * Gets a title text for a contact. To be used in contact
         * lists only, because the formatting does not remove leading spaces.
         *
         * @param   aContactData    a field data array representing the data
         *                          in the contact.
         * @param   aTitle  a buffer where the title text is formatted.
         *                  The buffer must have enough capacity to hold the 
         *                  formatted title, otherwise this function will 
         *                  panic.
         * @see ContactTitleLength
         * @postcond aTitle.Length()==ContactListTitleLength(aContactData)
         */
        virtual void GetContactListTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const = 0;

        /**
         * Calculates title text length for a contact. To be used in contact
         * lists only, because the formatting does not remove leading spaces.
         *
         * @param   aContactData a field data array representing the data in
         *          the contact.
         * @return  the length of the formatted contact title in characters.
         */
        virtual TInt ContactListTitleLength
            (const MPbkFieldDataArray& aContactData) const = 0;

        /**
         * Returns filtered contact data which contains only fields that will
         * be used in contact find.
         *
         * @param aContactData  contact data to filter find fields from.
         * @return  a filtered contact data that contains those fields from
         *          aContactData which participate in contact find.
         *          The fields are in presentation order. The returned 
         *          reference is invalidated if aContactData is modified and
         *          when this function is called the next time.
         */
        virtual const MPbkFieldDataArray& FilterContactFieldsForFind
            (const MPbkFieldDataArray& aContactData) const =0;
    };


#endif // __MPbkContactNameFormat_H__

// End of File
