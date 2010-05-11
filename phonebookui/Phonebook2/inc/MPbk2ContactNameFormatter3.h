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
* Description:  Phonebook 2 contact name formatter interface.
*
*/


#ifndef MPBK2CONTACTNAMEFORMATTER3_H
#define MPBK2CONTACTNAMEFORMATTER3_H

//Use this UID to access field property interface extension 3. Used as
//a parameter to ContactNameFormatterExtension() method.
const TUid MPbk2ContactNameFormatterExtension3Uid = { 3 };

/**
 * This class is an extension to MPbk2ContactNameFormatter.
 * See documentation of MPbk2ContactNameFormatter from header
 * MPbk2ContactNameFormatter.h
 * 
 * you can access this extension by calling
 * MPbk2ContactNameFormatter->ContactNameFormatterExtension()
 */
class MPbk2ContactNameFormatter3
    {
    public: // Interface

        /**
         * Return iterator that contains all fields that are used for making
         * the formatted name.
         *
         * @param   aFieldTypeList  Field type list that is used to store
         *                          field types for the iterator. The list
         *                          will be resetted before use.
         * @param   aContactFields  Field collection representing
         *                          the data in a contact.
         * @return  Iterator for fields that are used for the formated title.
         */
        virtual CVPbkBaseContactFieldTypeListIterator* TitleWithCompanyNameFieldsLC(
                CVPbkFieldTypeRefsList& aFieldTypeList,
                const MVPbkBaseContactFieldCollection& aContactFields ) = 0;
                
    protected:
    
        virtual ~MPbk2ContactNameFormatter3()
                {}

     
      
    };

#endif // MPBK2CONTACTNAMEFORMATTER3_H

// End of File
