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
* Description:  Phonebook 2 contact assigner factory.
*
*/


#ifndef PBK2CONTACTDATAASSIGNERFACTORY_H
#define PBK2CONTACTDATAASSIGNERFACTORY_H

// INCLUDES
#include <e32std.h>
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class MPbk2ContactAssigner;
class MPbk2ContactAssignerObserver;
class CVPbkContactManager;
class CPbk2FieldPropertyArray;
class MVPbkFieldType;

// CLASS DECLARATION

/**
 * Phonebook 2 assigner factory.
 */
class Pbk2ContactAssignerFactory
    {
    public: // Factory interface

        /**
         * Creates and returns a contact data assigner.
         *
         * @param aObserver         Observer.
         * @param aMimeType         MIME type of the data to assign.
         * @param aFieldType        Field type of the data to assign.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aFieldProperties  Phonebook field properties.
         * @return  Contact data assigner.
         */
        static MPbk2ContactAssigner* CreateContactDataAssignerL(
                MPbk2ContactAssignerObserver& aObserver,
                TInt aMimeType,
                const MVPbkFieldType* aFieldType,
                CVPbkContactManager& aContactManager,
                CPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Creates and returns a contact attribute assigner.
         *
         * @param aObserver                 Observer.
         * @param aContactManager           Virtual Phonebook
         *                                  contact manager.
         * @param aAttributeAssignData      Contact attribute assign data.
         * @param aAttributeRemoval         ETrue if the attribute is to
         *                                  removed, EFalse if the attribute
         *                                  is to be added.
         * @return  Contact attribute assigner.
         */
        static MPbk2ContactAssigner* CreateContactAttributeAssignerL(
                MPbk2ContactAssignerObserver& aObserver,
                CVPbkContactManager& aContactManager,
                TPbk2AttributeAssignData aAttributeAssignData,
                TBool aAttributeRemoval );
    };

#endif // PBK2CONTACTDATAASSIGNERFACTORY_H

// End of File
