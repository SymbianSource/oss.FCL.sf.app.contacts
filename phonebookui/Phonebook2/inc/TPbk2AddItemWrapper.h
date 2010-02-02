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
* Description:  Wrapper for selectable fields in add item dialog.
*
*/


#ifndef TPBK2ADDITEMWRAPPER_H
#define TPBK2ADDITEMWRAPPER_H

//  INCLUDES
#include <e32std.h>        // CBase

// FORWARD DECLARATIONS
class MPbk2FieldProperty;
class CPbk2FieldPropertyGroup;
class TPbk2IconId;

// CLASS DECLARATION

/**
 * Wrapper for selectable fields in add item dialog.
 */
class TPbk2AddItemWrapper
    {
    public: // Constructors and destructor

        /**
         * Constructor for creating a wrapper for CPbk2FieldProperty.
         * @param aFieldProperty Field property
         */
        IMPORT_C TPbk2AddItemWrapper(
                const MPbk2FieldProperty& aFieldProperty );

        /**
         * Constructor for creating a wrapper for CPbk2FieldPropertyGroup.
         * @param aPropertyGroup Property group
         */
        IMPORT_C TPbk2AddItemWrapper(
                const CPbk2FieldPropertyGroup& aPropertyGroup );

    public: // Interface

        /**
         * Returns the number of properties in this wrapper.
         *
         * @return  Number of properties in this wrapper.
         */
        IMPORT_C TInt PropertyCount() const;

        /**
         * Returns the field property at gíven index.
         *
         * @param aIndex    The index of the field property.
         * @return  Field property at gíven index.
         */
        IMPORT_C const MPbk2FieldProperty& PropertyAt(
                TInt aIndex ) const;

        /**
         * Returns the label of the instance.
         *
         * @return  Label.
         */
        const TDesC& Label() const;

        /**
         * Returns the icon id of the instance.
         *
         * @return  Icon id.
         */
        const TPbk2IconId& IconId() const;

        /**
         * Returns the add item ordering value of the instance.
         *
         * @return  Add item ordering value.
         */
        TInt AddItemOrdering() const;
        /**
         * Returns group.
         *
         * @return  Group if exist.
         */
        const CPbk2FieldPropertyGroup* Group() const;

    private: // Data
        /// Ref: Single field property
        const MPbk2FieldProperty* iFieldProperty;
        /// Ref: Group of properties
        const CPbk2FieldPropertyGroup* iPropertyGroup;
    };

#endif // TPBK2ADDITEMWRAPPER_H

// End of File
