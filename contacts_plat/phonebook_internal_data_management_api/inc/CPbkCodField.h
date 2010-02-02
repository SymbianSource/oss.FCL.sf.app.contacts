/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides information about phonebook's Object for call fields.
*
*/



#ifndef __CPbkCodField_H__
#define __CPbkCodField_H__

//  INCLUDES
#include <e32base.h>
#include <PbkFields.hrh> // TPbkFieldId
#include <cntdef.h> // TContactItemId

// FORWARD DECLARATIONS
class CPbkContactEngine;
class TPbkContactItemField;
class CPbkContactItem;
// CLASS DECLARATION

/**
*  Provides information about phonebook's Object for call fields.
*  These fields have been defined in PbkFields.hrh file and field value is
*  either EPbkFieldIdCodTextID or EPbkFieldIdCodImageID. The first field value
*  is used if contact contains text object for call and the latter is used
*  when contact contains image object for call.
*  This API can be used for field value fetching and contains also getter for
*  text. If image value needs to be fetched then CPbkImageManager.h (PbkView.dll)
*  should be used instead.
*  @lib PbkEng.lib
*  @since Series 60 3.0
*/
class CPbkCodField : public CBase
    {
    public:
    
        /**
        * Two-phased constructor.
        * @param aPbkContactEngine Reference to Phonebook contact engine. 
        * @return New instance
        */
        IMPORT_C static CPbkCodField* NewL(CPbkContactEngine& aPbkContactEngine);

        /**
        * Destructor.
        */
        ~CPbkCodField();
    
    public: // New functions

        /**
         * The object for call phonebook field id value of contact.
         * The object for call field id value is either EPbkFieldIdCodTextID or 
         * EPbkFieldIdCodImageID if field is found. 
         * KErrNotFound returned if field object for call is not found.
         * @param aPbkContactItem Reference to a contact item          
         * @return Search result. 
         */
        IMPORT_C TPbkFieldId CodFieldId(CPbkContactItem& aPbkContactItem) const;

        /**
         * The object for call phonebook field id value of contact.
         * The object for call field id value is either EPbkFieldIdCodTextID or 
         * EPbkFieldIdCodImageID if field is found. 
         * KErrNotFound returned if field object for call is not found.         
         * @param aPbkContactItemId Contact item id value of searched item
         * @return Search result.
         */
        IMPORT_C TPbkFieldId CodFieldIdL(TContactItemId aContactItemId) const;
        
        /**
        * Returns the string of text for call field. If field id is 
        * EPbkFieldIdCodTextID then text for call string is returned.
        * The filename of image is returned if object for call field id value
        * is EPbkFieldIdCodImageID.
        * Leaves with KErrNotFound if the contact item id is not found.
        * @param aContactItemId Contact item id        
        * @return The string of text for call field.
        * @exception KErrNotSupported if the field content cannot be
        * represented as text.         
        */
        IMPORT_C HBufC* GetTextL(TContactItemId aContactItemId) const;
        
    private:
        /**
        * C++ constructor.
        */
        CPbkCodField(CPbkContactEngine& aPbkContactEngine);
        
    private:
        /// Ref: Phonebook contact engine
        CPbkContactEngine& iPbkContactEngine;
    };



#endif // __CPbkCodField_H__

// End of File
