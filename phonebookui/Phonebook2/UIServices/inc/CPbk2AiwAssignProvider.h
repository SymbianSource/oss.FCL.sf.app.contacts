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
* Description:  Phonebook 2 assign service AIW provider.
*
*/


#ifndef CPBK2AIWASSIGNPROVIDER_H
#define CPBK2AIWASSIGNPROVIDER_H

//  INCLUDES
#include <AiwServiceIfMenu.h>
#include "MPbk2ConnectionObserver.h"
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2ServerAppIPC.h>

//  FORWARD DECLARATIONS
class CPbk2ServerAppConnection;

//  CLASS DECLARATION

/**
 * Phonebook 2 assign service AIW provider.
 * Responsible for:
 * - Implementing AIW provider API for assign service
 * - Verifying parameters passed by the consumer
 * - Packing the consumer passed data to an IPC package
 */
class CPbk2AiwAssignProvider : public CAiwServiceIfMenu,
                               public MPbk2ConnectionObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2AiwAssignProvider* NewL();

        /**
         * Destructor.
         */
        ~CPbk2AiwAssignProvider();

    public: // From CAiwServiceIfMenu
        void InitializeMenuPaneL(
                CAiwMenuPane& aMenuPane,
                TInt aIndex,
                TInt aCascadeId,
                const CAiwGenericParamList& aInParamList );
        void HandleMenuCmdL(
                TInt aMenuCmdId,
                const CAiwGenericParamList& aInParamList,
                CAiwGenericParamList& aOutParamList,
                TUint aCmdOptions = 0,
                const MAiwNotifyCallback* aCallback = NULL );

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
        CPbk2AiwAssignProvider();
        void ConstructL();
        HBufC8* PackParameterToBufferL(
                TGenericParamId aParamId,
                const CAiwGenericParamList& aInParamList ) const;
        HBufC16* PackParameterToBuffer16L(
                TGenericParamId aParamId,
                const CAiwGenericParamList& aInParamList ) const;
        HBufC8* PackSelectorToBufferL(
                const CAiwGenericParamList& aInParamList ) const;
        HBufC8* PackFilterToBufferL(
                const CAiwGenericParamList& aInParamList ) const;
        void CreateDataPackageL(
                const CAiwGenericParamList& aInParamList,
                const TGenericParamId aSemanticId );
        TPbk2AttributeAssignData CreateAttributePackageL(
                const CAiwGenericParamList& aInParamList,
                const TGenericParamId aSemanticId );
        void CreateConfigurationPackageL(
                const CAiwGenericParamList& aInParamList );
        void CreateInstructionPackageL(
                const CAiwGenericParamList& aInParamList );
        void ValidateInstructionPackageL();
        void ValidateMenuServiceL(
                const TInt& aCmdId );
        HBufC* ClientTitlePaneL();
        HBufC* ClientOrientation() const;
        void LaunchAttributeAssignL(
                const CAiwGenericParamList& aInParamList,
                const TGenericParamId aSemanticId );
        void LaunchDataAssignL(
                const CAiwGenericParamList& aInParamList,
                const TGenericParamId aSemanticId );
        HBufC* PackImppParametersToBufferL(
                const CAiwGenericParamList& aInParamList);

    private: // Data
        /// Own: Phonebook 2 UI services dll resource file
        RPbk2LocalizedResourceFile iUiServicesResourceFile;
        /// Own: Connection to server application
        CPbk2ServerAppConnection* iConnection;
        /// Own: AIW event param list for event sending
        CAiwGenericParamList* iEventParamList;
        /// Ref: AIW notify call back
        const MAiwNotifyCallback* iCallback;
        /// Ref: AIW incoming parameter list
        const CAiwGenericParamList* iInParamList;
        /// Own: Data IPC package
        HBufC8* iDataPackage;
        /// Own: Configuration IPC package
        HBufC8* iConfigurationPackage;
        /// Own: Assign instructions IPC package
        HBufC8* iInstructionsPackage;
        /// Own: Attribute assign data
        TPbk2AttributeAssignData iAttributeData;
    };

#endif // CPBK2AIWASSIGNPROVIDER_H

// End of File
