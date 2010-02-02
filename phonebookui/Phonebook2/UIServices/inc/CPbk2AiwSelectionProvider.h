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
* Description:  Phonebook 2 selection service AIW provider.
*
*/


#ifndef CPBK2AIWSELECTIONPROVIDER_H
#define CPBK2AIWSELECTIONPROVIDER_H

// INCLUDE FILES
#include <AiwServiceIfBase.h>
#include "MPbk2ConnectionObserver.h"

//  FORWARD DECLARATIONS
class CPbk2ServerAppConnection;
class RVPbkContactFieldDefaultPriorities;
class CVPbkFieldTypeSelector;

// CLASS DECLARATION

/**
 * Phonebook 2 selection service AIW provider.
 * Responsible for:
 * - Implementing AIW provider API for selection service
 * - Verifying parameters passed by the consumer
 * - Packing the consumer passed data to an IPC package
 */
class CPbk2AiwSelectionProvider : public CAiwServiceIfBase,
                                  public MPbk2ConnectionObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2AiwSelectionProvider* NewL();

        /**
         * Destructor.
         */
        ~CPbk2AiwSelectionProvider();

    public: // From CAiwServiceIfBase
        void InitialiseL(
                MAiwNotifyCallback& aFrameworkCallback,
                const RCriteriaArray& aInterest );
        void HandleServiceCmdL(
                const TInt& aCmdId,
                const CAiwGenericParamList& aInParamList,
                CAiwGenericParamList& aOutParamList,
                TUint aCmdOptions = 0,
                const MAiwNotifyCallback* aCallback = NULL );

    public: // From MPbk2ConnectionObserver
        void OperationCompleteL(
                const TDesC8& aContactLinks,
                TInt aExtraResultData,
                const TDesC& aField );
        void OperationCanceledL();
        void OperationErrorL(
                TInt aError );
        TBool OkToExitL(
                TInt aCommandId,
                TPbk2ExitCommandParams aExitParameter );
        TBool AcceptSelectionL(
                TInt aNumberOfSelectedContacts,
                HBufC8& aContactLink );

    private: // Implementation
        CPbk2AiwSelectionProvider();
        HBufC8* PackParameterToBufferL(
                TGenericParamId aParamId,
                const CAiwGenericParamList& aInParamList) const;
        void PackParameterToBufferL(
                HBufC8*& aTargetBuffer,
                TGenericParamId aParamId,
                const CAiwGenericParamList& aInParamList );
        void CreateConfigurationPackageL(
                const CAiwGenericParamList& aInParamList );
        void CreateInstructionPackageL(
                const CAiwGenericParamList& aInParamList );
        void ValidateInstructionPackageL();
        HBufC* ClientTitlePaneL();

    private: // Data
        /// Ref: AIW notify call back
        const MAiwNotifyCallback* iCallback;
        /// Ref: AIW incoming parameter list
        const CAiwGenericParamList* iInParamList;
        /// Own: AIW event param list for event sending
        CAiwGenericParamList* iEventParamList;
        /// Own: Connection to server application
        CPbk2ServerAppConnection* iConnection;
        /// Own: Configuration IPC package
        HBufC8* iConfigurationPackage;
        /// Own: Instructions IPC package
        HBufC8* iInstructionsPackage;
        /// Own: Contact view filter buffer
        HBufC8* iContactViewFilterBuffer;
        /// Own: Default priorities buffer
        HBufC8* iDefaultPrioritiesBuffer;
    };

#endif // CPBK2AIWSELECTIONPROVIDER_H

// End of File
