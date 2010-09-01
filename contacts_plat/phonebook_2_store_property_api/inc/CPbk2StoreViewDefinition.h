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
* Description:  Phonebook 2 store view definition.
*
*/


#ifndef CPBK2STOREVIEWDEFINITION_H
#define CPBK2STOREVIEWDEFINITION_H

// INCLUDE FILES
#include <e32base.h>
#include <Pbk2ContactView.hrh>
#include <TPbk2IconId.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CVPbkContactViewDefinition;

// CLASS DECLARATION

/**
 * Phonebook 2 store view definition.
 */
class CPbk2StoreViewDefinition : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreViewDefinition* NewL();

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   Resource reader pointed to
         *                   a PHONEBOOK2_STORE_VIEW resource.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreViewDefinition* NewLC(
                TResourceReader& aReader );

        /**
         * Creates a new instance of this class.
         *
         * @param aViewDef  Store view definition to copy.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreViewDefinition* NewLC(
                const CPbk2StoreViewDefinition& aViewDef );

        /**
         * Destructor.
         */
        ~CPbk2StoreViewDefinition();

    public: // Interface

        /**
         * Returns the type of the view.
         *
         * @return  Type of the view.
         */
        IMPORT_C TPbk2ContactViewType ViewType() const;

        /**
         * Returns the view location.
         *
         * @return  Contact view location.
         */
        IMPORT_C TPbk2ContactViewLocation ViewLocation() const;

        /**
         * Returns the Virtual Phonebook view definition.
         *
         * @return  Virtual Phonebook view definition.
         */
        IMPORT_C const CVPbkContactViewDefinition& ViewDefinition() const;

        /**
         * Returns view icon id that is displayed at the front of the
         * contacts of the view.
         *
         * @return  View icon id.
         */
        IMPORT_C const TPbk2IconId& ViewItemIconId() const;

        /**
         * Sets the type of the view.
         *
         * @param aType     Type of the view.
         */
        IMPORT_C void SetViewType(
                TPbk2ContactViewType aType );

        /**
         * Sets the view location.
         *
         * @param aLocation     View location.
         */
        IMPORT_C void SetViewLocation(
                TPbk2ContactViewLocation aLocation );

        /**
         * Sets the Virtual Phonebook view definition.
         *
         * @param aViewDefinition   Virtual Phonebook view definition.
         *                          Ownership is taken.
         */
        IMPORT_C void SetViewDefinition(
                CVPbkContactViewDefinition* aViewDefinition );

        /**
         * Sets the id of the icon that is displayed at the front of
         * the contacts. The id is one of TPbk2IconIds or an id from
         * an UI extension.
         *
         * @param aIconId   Icon id.
         */
        IMPORT_C void SetViewItemIcon(
                const TPbk2IconId& aIconId );

    private: // Implementation
        CPbk2StoreViewDefinition();
        void ConstructL(
                TResourceReader& aReader );
        void ConstructL(
                const CPbk2StoreViewDefinition& aViewDef );

    private: // Data
        /// Own: View type
        TPbk2ContactViewType iViewType;
        /// Own: View location
        TPbk2ContactViewLocation iViewLocation;
        /// Own: Virtual Phonebook contact view definition
        CVPbkContactViewDefinition* iViewDefinition;
        /// Own: View icon id
        TPbk2IconId iViewItemIconId;
    };

#endif // CPBK2STOREVIEWDEFINITION_H

// End of File
