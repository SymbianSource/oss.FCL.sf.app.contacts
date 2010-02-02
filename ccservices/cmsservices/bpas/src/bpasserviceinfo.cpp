/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BPAS service information holder
*
*/


#include <e32base.h>

#include "bpasserviceinfo.h"



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CBPASServiceInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASServiceInfo* CBPASServiceInfo::NewL(TServiceId aServiceId)
    {
    CBPASServiceInfo* self = NewLC(aServiceId);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CBPASServiceInfo::NewLC
// ---------------------------------------------------------------------------
//
CBPASServiceInfo* CBPASServiceInfo::NewLC(TServiceId aServiceId)
    {
    CBPASServiceInfo* self = new( ELeave ) CBPASServiceInfo(aServiceId);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// CBPAServiceInfo::CBPAServiceInfo
// ---------------------------------------------------------------------------
//
CBPASServiceInfo::CBPASServiceInfo(TServiceId aServiceId):
iServiceId( aServiceId ),
iBrandId( NULL ),
iLanguageId(ELangInternationalEnglish), //Default is always 47(ELangInternationalEnglish)
iServiceType(NULL)
    {
    }
    
// ---------------------------------------------------------------------------
// CBPAServiceInfo::~CBPAServiceInfo
// ---------------------------------------------------------------------------
//
CBPASServiceInfo::~CBPASServiceInfo()
    {
    if(iBrandId)
        delete iBrandId;
    }

// ---------------------------------------------------------------------------
// CBPAServiceInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CBPASServiceInfo::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CBPAServiceInfo::ServiceId
// ---------------------------------------------------------------------------
//
EXPORT_C TServiceId CBPASServiceInfo::ServiceId() const
    {
	return iServiceId;	
	}
	 
// ---------------------------------------------------------------------------
// CBPAServiceInfo::BrandId
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CBPASServiceInfo::BrandId( ) const
    {
    return iBrandId ? *iBrandId : TPtrC8(KNullDesC8);
    }

// ---------------------------------------------------------------------------
// CBPAServiceInfo::ServiceType
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CBPASServiceInfo::ServiceType( ) const
    {
    return iServiceType;
    }

// ---------------------------------------------------------------------------
// CBPAServiceInfo::LanguageId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CBPASServiceInfo::LanguageId() const
    {
    return iLanguageId;
    }

// ---------------------------------------------------------------------------
// CBPAServiceInfo::SetLanguageID
// ---------------------------------------------------------------------------
//
void CBPASServiceInfo::SetLanguageId(TInt aLanguageId )
    {
    iLanguageId = aLanguageId;
    }
    
// ---------------------------------------------------------------------------
// CBPAServiceInfo::SetServiceType
// ---------------------------------------------------------------------------
//
void CBPASServiceInfo::SetServiceType(TInt aServiceType )
    {
    iServiceType = aServiceType;
    }

// ---------------------------------------------------------------------------
// CBPAServiceInfo::SetBrandIdL
// ---------------------------------------------------------------------------
//
void CBPASServiceInfo::SetBrandIdL( const TDesC8& aBrandId )
    {
    if (iBrandId)
        {
        delete iBrandId;
        iBrandId = NULL;
        }
    iBrandId = aBrandId.AllocL();
    }

//eof    
    
