/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CVPBKVCARDOPERATIONFACTORY_H
#define CVPBKVCARDOPERATIONFACTORY_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class CParserVCard;
class MVPbkContactOperation;
class CVPbkVCardFieldTypeProperty;
class CVPbkVCardIdNameMapping;
class MVPbkFieldTypeList;
class CParserProperty;
class MVPbkStoreContact;
class RWriteStream;
class MVPbkSingleContactOperationObserver;
class MVPbkContactCopyObserver;
class MVPbkContactLink;
class MVPbkContactStore;
class CVPbkContactManager;
class CVPbkVCardData;
class RReadStream;

// CLASS DECLARATIONS

NONSHARABLE_CLASS( VPbkVCardOperationFactory )
    {
    public:

        static MVPbkContactOperation* CreateExportOperationL( 
            CVPbkVCardData& aData,
            RWriteStream& aDestStream, 
            const MVPbkStoreContact& aSourceItem, 
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed );

        static MVPbkContactOperation* CreateExportOperationL( 
            CVPbkVCardData& aData,
            RWriteStream& aDestStream, 
            const MVPbkContactLink& aContactLink, 
            MVPbkSingleContactOperationObserver& aObserver,
            CVPbkContactManager& aContactManager,
            const TBool aBeamed );

        static MVPbkContactOperation* CreateImportOperationL( 
            CVPbkVCardData& aData,
            RReadStream& aSourceStream, 
            MVPbkContactStore& aTargetStore, 
            MVPbkContactCopyObserver& aObserver,
            const TBool aSync );
        
        static MVPbkContactOperation* CreateImportOperationL( 
            CVPbkVCardData& aData,
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            RReadStream& aSourceStream, 
            MVPbkContactStore& aTargetStore, 
            MVPbkSingleContactOperationObserver& aObserver );
            
        static MVPbkContactOperation* CreateImportCompactBCardOperationL(
            CVPbkVCardData& aData,
            MVPbkContactStore& aTargetStore,
	        RReadStream &aSourceStream,
            MVPbkContactCopyObserver& aObserver );
        
        static MVPbkContactOperation* CreateImportCompactBCardOperationL(
            CVPbkVCardData& aData,
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            MVPbkContactStore& aTargetStore,
            RReadStream &aSourceStream,
            MVPbkSingleContactOperationObserver& aObserver );
            
        static MVPbkContactOperation* CreateImportOperationL( 
            CVPbkVCardData& aData, 
            const MVPbkContactLink& aReplaceContact, 
            MVPbkContactStore& aTargetStore, 
            RReadStream& aSourceStream,
            MVPbkSingleContactOperationObserver& aObserver );

    private:
        VPbkVCardOperationFactory();

    };

#endif // CVPBKVCARDOPERATIONFACTORY_H
//End of file

