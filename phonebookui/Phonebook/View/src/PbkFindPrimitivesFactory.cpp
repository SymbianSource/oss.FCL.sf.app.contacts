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
*           Phonebook find primitives factory class
*
*/


// INCLUDE FILES
#include "PbkFindPrimitivesFactory.h"
#include <PbkConfig.hrh>
#include "CPbkChineseFindPrimitives.h"
#include "CPbkJapaneseFindPrimitives.h"
#include "CPbkFindPrimitives.h"

#include <featmgr.h>
#include <bldvariant.hrh>


// ================= MEMBER FUNCTIONS =======================

MPbkFindPrimitives* PbkFindPrimitivesFactory::CreateL
        (MPbkContactNameFormat& aContactNameFormatter)
    {
    MPbkFindPrimitives* ret = NULL;
    if (FeatureManager::FeatureSupported(KFeatureIdChinese))
        {
        ret = CPbkChineseFindPrimitives::NewL(aContactNameFormatter);
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        ret = CPbkJapaneseFindPrimitives::NewL(aContactNameFormatter);
        }
    else 
        {
        ret = CPbkFindPrimitives::NewL(aContactNameFormatter);
        }
    return ret;
    }


//  End of File  
