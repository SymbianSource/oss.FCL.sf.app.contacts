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
* Description:  Phonebook 2 application services interface.
*
*/


#ifndef MPBK2APPLICATIONSERVICES_H
#define MPBK2APPLICATIONSERVICES_H

// INCLUDES
#include <e32std.h>

#include <vpbkvariant.hrh>    // for TVPbkLocalVariantFlags

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MPbk2CommandHandler;
class MPbk2StoreObservationRegister;
class MPbk2StoreValidityInformer;
class MPbk2ContactNameFormatter;
class CPbk2SortOrderManager;
class CSendUi;
class CPbk2StorePropertyArray;
class CPbk2FieldPropertyArray;
class CPbk2StoreConfiguration;
class MPbk2ContactViewSupplier;

// Use this UID to access application services extension 2. Used as a parameter
// to MPbk2ApplicationServicesExtension() method.
const TUid KMPbk2ApplicationServicesExtension2Uid = { 2 };

// CLASS DECLARATION

/**
 * Phonebook 2 application services interface.
 * Shares commonly used objects.
 */
class MPbk2ApplicationServices
    {
    public:  // Interface

        /**
         * Returns Virtual Phonebook contact manager.
         *
         * @return  Virtual Phonebook contact manager.
         */
        virtual CVPbkContactManager& ContactManager() const = 0;

        /**
         * Returns Phonebook 2 command handler, which handles
         * all the commands.
         *
         * @return  Pointer to Phonebook 2 command handler (might be NULL).
         */
        virtual MPbk2CommandHandler* CommandHandlerL() = 0;

        /**
         * Returns the name formatter object.
         *
         * @return  Phonebook 2 name formatter.
         */
        virtual MPbk2ContactNameFormatter& NameFormatter() const = 0;

        /**
         * Returns the sort order manager.
         *
         * @return  Sort order manager.
         */
        virtual CPbk2SortOrderManager& SortOrderManager() const = 0;

        /**
         * Returns Phonebook 2 global send UI.
         *
         * @return  Pointer to SendUi instance (might be NULL).
         */
        virtual CSendUi* SendUiL() = 0;

        /**
         * Returns store property array.
         *
         * @return  Store property array.
         */
        virtual CPbk2StorePropertyArray& StoreProperties() const = 0;

        /**
         * Returns Phonebook 2 field properties.
         *
         * @return  Phonebook 2 field properties.
         */
        virtual CPbk2FieldPropertyArray& FieldProperties() const = 0;

        /**
         * Returns Phonebook 2 store configuration.
         *
         * @return  Phonebook 2 store configuration.
         */
        virtual CPbk2StoreConfiguration& StoreConfiguration() const = 0;

        /**
         * Returns Phonebook 2 contact store observation register.
         *
         * @return  Phonebook 2 store observation register.
         */
        virtual MPbk2StoreObservationRegister&
            StoreObservationRegister() const = 0;

        /**
         * Returns Phonebook 2 contact store validity informer.
         *
         * @return  Phonebook 2 store validity informer.
         */
        virtual MPbk2StoreValidityInformer&
            StoreValidityInformer() const = 0;

        /**
         * Returns Phonebook 2 view supplier.
         *
         * @return  Phonebook 2 view supplier.
         */
        virtual MPbk2ContactViewSupplier& ViewSupplier() const = 0;

        /**
         * Returns global menu filtering flags.
         *
         * @return  Global menu filtering flags.
         */
        virtual TInt GlobalMenuFilteringFlagsL() const = 0;

        /**
         * Used to check if the feature defined by aFeature if on or off.
         * @param aFeature the feature to check
         * @return ETrue if the feature is on, EFalse otherwise
         */
        virtual TBool LocallyVariatedFeatureEnabled(
            TVPbkLocalVariantFlags aFeatureFlag) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* MPbk2ApplicationServicesExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Protected destructor
        ~MPbk2ApplicationServices()
            {}
    };

#endif // MPBK2APPLICATIONSERVICES_H

// End of File
