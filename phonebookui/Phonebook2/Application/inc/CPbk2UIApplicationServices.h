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
* Description:  Phonebook 2 application services.
*
*/


#ifndef CPBK2UIAPPLICATIONSERVICES_H
#define CPBK2UIAPPLICATIONSERVICES_H

// INCLUDES
#include <CPbk2ApplicationServices.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CPbk2CommandHandler;
class CSendUi;

// CLASS DECLARATION

/**
 * Phonebook 2 application services.
 * Responsible for owning and delivering commonly used shared objects
 * and providing some common application level services.
 */
class CPbk2UIApplicationServices : public CBase,
                                   public MPbk2ApplicationServices,
                                   public MPbk2ApplicationServices2
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2UIApplicationServices* NewL();

        /**
         * Destructor.
         */
        ~CPbk2UIApplicationServices();

    public: // From MPbk2ApplicationServices
        CVPbkContactManager& ContactManager() const;
        MPbk2CommandHandler* CommandHandlerL();
        MPbk2ContactNameFormatter& NameFormatter() const;
        CPbk2SortOrderManager& SortOrderManager() const;
        CSendUi* SendUiL();
        CPbk2StorePropertyArray& StoreProperties() const;
        CPbk2FieldPropertyArray& FieldProperties() const;
        CPbk2StoreConfiguration& StoreConfiguration() const;
        MPbk2StoreObservationRegister& StoreObservationRegister() const;
        MPbk2StoreValidityInformer& StoreValidityInformer() const;
        MPbk2ContactViewSupplier& ViewSupplier() const;
        TInt GlobalMenuFilteringFlagsL() const;
        TBool LocallyVariatedFeatureEnabled(
            TVPbkLocalVariantFlags aFeatureFlag);
        TAny* MPbk2ApplicationServicesExtension( TUid aExtensionUid );

    public: // From MPbk2ApplicationServices2
        CPbk2StoreManager& StoreManager() const;
        CPbk2ServiceManager& ServiceManager() const;

    private: // Implementation
        CPbk2UIApplicationServices();

        void ConstructL();

    private: // Data

        /// Own: Base Application services
        CPbk2ApplicationServices* iAppServices;
        /// Own: Command handler
        CPbk2CommandHandler* iCommandHandler;
        /// Own: Send UI
        CSendUi* iSendUi;
        /// Ref: Open file server session from coeenv
        RFs& iFsSession;
    };

#endif // CPBK2UIAPPLICATIONSERVICES_H

// End of File
