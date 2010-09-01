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
*		Phonebook Field formatter factory.
*
*/


// INCLUDE FILES
#include "PbkFieldFormatterFactory.h"
#include "CPbkNumberGroupingFormatter.h"
#include "CPbkPhoneNumberDefaultFormatter.h"

#include <featmgr.h>
#include <bldvariant.hrh>

// ==================== MEMBER FUNCTIONS ====================

MPbkPhoneNumberFormat* PbkFieldFormatterFactory::CreatePhoneNumberFormatterL
        (TInt aMaxPhoneNumberDisplayLength)
    {
    MPbkPhoneNumberFormat* ret = NULL;
    if (FeatureManager::FeatureSupported(KFeatureIdPhoneNumberGrouping))
        {
        ret = CPbkNumberGroupingFormatter::NewL(aMaxPhoneNumberDisplayLength);
        }
    else
        {
	    ret = CPbkPhoneNumberDefaultFormatter::NewL(aMaxPhoneNumberDisplayLength);
        }
    return ret;
    }

// End of File
