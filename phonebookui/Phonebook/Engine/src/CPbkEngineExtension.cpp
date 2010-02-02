/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*        CPbkContactEngine extension.
*
*/


// INCLUDE FILES
#include "CPbkEngineExtension.h"
#include <barsread.h>
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <CPbkEngineExtensionInterface.h>
#include "CPbkFieldsInfo.h"

#include <PbkDebug.h>
#include "PbkProfiling.h"

// ================= LOCAL FUNCTIONS =======================

// unnamed namespace for local definitions
namespace {

/**
 * Cleanup item for RImplInfoPtrArray that needs ResetAndDestroy
 * to be called on cleanup.
 */
NONSHARABLE_CLASS(TResetAndDestroyCleanupItem) : 
        public TCleanupItem
    {
    public: // construction
        /**
         * C++ constructor.
         * @param aArray The array that will be cleaned up.
         */
        TResetAndDestroyCleanupItem(RImplInfoPtrArray& aArray) : 
            TCleanupItem(&Cleanup,&aArray)
            {
            }

    private: // Implementation
        /**
         * Performs the actual cleanup.
         * @param aObj Object that will be cleaned up. Must be an instance of
         *             RImplInfoPtrArray.
         */
        static void Cleanup(TAny* aObj)
            {
            reinterpret_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
            }
    };

} // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkEngineExtension::CPbkEngineExtension()
    {
    }

inline void CPbkEngineExtension::ConstructL(RFs& aFs)
	{
	// Load extension plugins
    RImplInfoPtrArray impls;
    CleanupStack::PushL(TResetAndDestroyCleanupItem(impls));
    TEComResolverParams resolverParams;
    // Clients should have wildcard in default_data field
    // e.g. default_data = "*||101f84fe";
    _LIT8(KDummyResolutionData, "dummypbkdata");
    resolverParams.SetDataType(KDummyResolutionData);
    resolverParams.SetWildcardMatch(ETrue);

    __PBK_PROFILE_START(PbkProfiling::EEcomEngineExtensionScan);

    REComSession::ListImplementationsL
        (KPbkEngineExtensionInterfaceUid, 
        resolverParams,        
        impls);

    __PBK_PROFILE_END(PbkProfiling::EEcomEngineExtensionScan);

    // Uid is used to resolve right implementation
    const TInt KUidLenghtInHexFormat = 8;
    const TInt count = impls.Count();

    // Extension parameters
    CPbkEngineExtensionInterface::TEngExtParams params;
    params.iFs = &aFs;

    for (TInt i=0; i < count; ++i)
        {
        TBuf8<KUidLenghtInHexFormat> resolutionData;
        resolutionData.AppendNum(impls[i]->ImplementationUid().iUid, EHex);

        __PBK_PROFILE_START(PbkProfiling::EEcomEngineExtensionLoadAndInit);

        CPbkEngineExtensionInterface* impl = 
            CPbkEngineExtensionInterface::NewLC(resolutionData, &params);

        __PBK_PROFILE_END(PbkProfiling::EEcomEngineExtensionLoadAndInit);

        User::LeaveIfError(iExtensions.Append(impl));
        CleanupStack::Pop(impl);
        }
    CleanupStack::PopAndDestroy();  // impls
	}

CPbkEngineExtension* CPbkEngineExtension::NewL(RFs& aFs)
	{
	CPbkEngineExtension* self = new(ELeave) CPbkEngineExtension;
    CleanupStack::PushL(self);
    self->ConstructL(aFs);
    CleanupStack::Pop(self);
    return self;
	}

CPbkEngineExtension::~CPbkEngineExtension()
	{
    iExtensions.ResetAndDestroy();
	}

void CPbkEngineExtension::AddFieldTypesL(CPbkFieldsInfo& aFieldsInfo)
	{
    TResourceReader pbkResReader;
    TResourceReader cntModelResReader;
    CPbkEngineExtensionInterface::TFieldTypeResource res;

    const TInt count = iExtensions.Count();

    for (TInt i=0; i < count; ++i)
        {
        const TInt resourceCount = 
            iExtensions[i]->AdditionalFieldTypesResourceCount();
        for (TInt j = 0; j < resourceCount; ++j)
            {
            if (iExtensions[i]->GetAdditionalFieldTypesResourceL(res))
                {
                CleanupClosePushL(res);
                pbkResReader.SetBuffer(res.iPbkRes);
                cntModelResReader.SetBuffer(res.iCntModelRes);
                aFieldsInfo.AddFieldTypesFromResourceL
                    (pbkResReader,cntModelResReader, NULL);
                CleanupStack::PopAndDestroy(); // res
                }
            }
        }
	}


TBool CPbkEngineExtension::ModifyFieldTypesL(CPbkFieldsInfo& aFieldsInfo)
    {
    TBool result = EFalse;
    const TInt count = iExtensions.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iExtensions[i]->ModifyFieldTypesL(aFieldsInfo))
            {
            result = ETrue;
            }
        }
    return result;
    }

// End of File
