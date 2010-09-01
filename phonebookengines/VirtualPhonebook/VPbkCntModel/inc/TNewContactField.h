/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A wrapper for a new contact field.
*
*/


#ifndef VPBKCNTMODEL_TNEWCONTACTFIELD_H
#define VPBKCNTMODEL_TNEWCONTACTFIELD_H


// INCLUDES
#include "TContactField.h"


namespace VPbkCntModel {

// CLASS DECLARATIONS

/**
 * A wrapper for a new contact field.
 */
NONSHARABLE_CLASS( TNewContactField ): public TContactField
    {
    public:  // Constructor and destructor
        /**
         * Constructor. 
         */
        TNewContactField
            (CContact& aParentContact, CContactItemField* aField);

        /**
         * Destructor.
         */
        ~TNewContactField();

    public:  // New functions
        CContactItemField* NativeField() const
            {
            return TContactField::NativeField();
            }

        void ReleaseNativeField()
            {
            TContactField::ResetNativeField();
            }
    };

}  // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_TNEWCONTACTFIELD_H
//End of file


