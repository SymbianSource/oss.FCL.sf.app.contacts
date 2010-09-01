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
* Description:  Phonebook 2 contact editor dialog UI field.
*
*/

#ifndef MPBK2UIFIELD_H_
#define MPBK2UIFIELD_H_

// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <TPbk2IconId.h>
#include "Pbk2UIFieldProperty.hrh"

// FORWARD DECLARATIONS

/**
 * Phonebook 2 UI field interface.
 *
 */
class MPbk2UIField
    {
    public: // Interface

    	/**
         * Destructor
         *
         */
    	virtual ~MPbk2UIField(){};
    	
        /**
         * Returns the multiplicity information
         *
         * @return  Multiplicity property which tells that can
         *          there be several fields of this type in the contact.
         */
        virtual TPbk2FieldMultiplicity Multiplicity() const = 0;

        /**
         * Returns the maximum length of the field.
         *
         * @return  Static maximum length of the field. Note, that the
         *          contact store can have it's own limits.
         */
        virtual TInt MaxLength() const = 0;

        /**
         * Returns the editing mode for the field editor.
         *
         * @return  Field editing mode..
         */
        virtual TPbk2FieldEditMode EditMode() const = 0;

        /**
         * Returns the default character case for the field editor.
         *
         * @return  Default character case.
         */
        virtual TPbk2FieldDefaultCase DefaultCase() const = 0;

        /**
         * Returns the icon id for the field.
         *
         * @return  Icon id.
         */
        virtual const TPbk2IconId& IconId() const = 0;

        /**
         * Returns the type of the field editor.
         *
         * @return  Type of the field editor (text, date, number etc.).
         */
        virtual TPbk2FieldCtrlTypeExt CtrlType() const = 0;

        /**
         * Returns the field property flags defined in
         * Pbk2UIFieldProperty.hrh.
         *
         * @return  Field property flags.
         */
        virtual TUint Flags() const = 0;
        
        /**
         * Returns the field property order defined in
         * Pbk2UIFieldProperty.hrh.
         *
         * @return  Field property order.
         */
        virtual TPbk2FieldOrder Order() const = 0;

        /**
         * Returns the default label that should be used if the store
         * doesn't define a label.
         *
         * @return  Default label.
         */
        virtual const TDesC& DefaultLabel() const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* FieldExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif /*MPBK2UIFIELD_H_*/

// End of File
