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
*    Plug-in interface for Phonebook engine extensions.
*
*/


#ifndef __CPbkEngineExtensionInterface_H__
#define __CPbkEngineExtensionInterface_H__

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// CONSTANTS
const TUid KPbkEngineExtensionInterfaceUid = { 0x101f84fd };

// FORWARD DECLARATIONS
class TResourceReader;
class CPbkFieldsInfo;


// CLASS DECLARATION

/**
 * @internal This interface is internal to Phonebook.
 *
 * Plug-in interface for Phonebook engine extensions.
 */
class CPbkEngineExtensionInterface : public CBase
    {
    public: // types
        /**
         * Extra parameters for NewLC.
         */
        struct TEngExtParams
            {
            TEngExtParams() { iFs = NULL; iSpare = NULL; }
            ///Ref: Open file server session.
            RFs* iFs;
            ///Ref: Spare
            TAny* iSpare;
            };

    public: // Constructor and destructor
        /**
         * Constructor. 
         * @param[in] aResolutionData eCom resolution data.
         * @param[in] aParam Extra parameters.
         */
        static CPbkEngineExtensionInterface* NewLC
            (const TDesC8& aResolutionData, TEngExtParams* aParams = NULL);
        
        /**
         * Destructor.
         */
        ~CPbkEngineExtensionInterface();

    public: // Interface
        /**
         * Structure for field type.
         * Contains Phonebook resource and Contact model resources.
         */
        struct TFieldTypeResource
            {
            HBufC8* iPbkRes;
            HBufC8* iCntModelRes;
            void Close();
            };

        /**
         * Gets additional field type resource.
         *
         * @param aResource additional field types resource.
         * @return true if aResource was set, false otherwise.
         */
		virtual TBool GetAdditionalFieldTypesResourceL
            (TFieldTypeResource& aResource) =0;

        /**
         * Modifies field type(s) in aFieldsInfo.
         *
         * @param aFieldsInfo   container of all field types.
         * @return ETrue if field type(s) were modified.
         */
        virtual TBool ModifyFieldTypesL(CPbkFieldsInfo& /* aFieldsInfo */) 
            { return EFalse; }
        
        /**
         * Returns number of separate resources that this extension 
         * wants to add to the field types. The 
         * GetAdditionalFieldTypesResourceL function will be called
         * as many times as this function indicates.
         * However, as the GetAdditionalFieldTypesResourceL function
         * does not take any indexing variable, the extension has to keep
         * track of the calls itself and return different resource 
         * each time the GetAdditionalFieldTypesResourceL is called.
         *
         * Default implementation returns one so that it is not necessary
         * for every extension to implement this.
         * 
         * @return Number of separate resources that this extension 
         *         wants to add to the field types.
         */
        virtual TInt AdditionalFieldTypesResourceCount() const
            { return 1; }

    protected: // Implementation
        CPbkEngineExtensionInterface();

    private:  // Data
        /// Own: destructor id key
        TUid iDtor_ID_Key;
    };

// INLINE FUNCTIONS

inline CPbkEngineExtensionInterface* CPbkEngineExtensionInterface::NewLC
		(const TDesC8& aResolutionData, TEngExtParams* aParams)
	{
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aResolutionData);
    TAny* ptr = REComSession::CreateImplementationL
        (KPbkEngineExtensionInterfaceUid,
        _FOFF(CPbkEngineExtensionInterface,iDtor_ID_Key),
        aParams,
        resolverParams);
    CPbkEngineExtensionInterface* impl = 
        static_cast<CPbkEngineExtensionInterface*>(ptr);
    CleanupStack::PushL(impl);
    return impl;
	}

inline CPbkEngineExtensionInterface::~CPbkEngineExtensionInterface()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline void CPbkEngineExtensionInterface::TFieldTypeResource::Close()
    {
    delete iPbkRes;
    delete iCntModelRes;
    }

inline CPbkEngineExtensionInterface::CPbkEngineExtensionInterface()
    {
    }

#endif // __CPbkEngineExtensionInterface_H__

// End of File
