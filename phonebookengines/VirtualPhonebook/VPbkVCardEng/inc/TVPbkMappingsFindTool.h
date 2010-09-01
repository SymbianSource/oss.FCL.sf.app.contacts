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
* Description:  
*
*/


#ifndef TVPBKMAPPINGSFINDTOOL_H
#define TVPBKMAPPINGSFINDTOOL_H

#include "CVPbkVCardIdNameMapping.h"

/**
 * Class searches from array a name or an id, specified by name or id.
 */
NONSHARABLE_CLASS( TVPbkMappingsFindTool )
	{
    public: 
        static TInt FindIdByName( 
                    const TDesC8& aName, 
                    const TArray<CVPbkVCardIdNameMapping*>& aArray );

        static const TDesC8& FindNameById(
            TInt aId, const TArray<CVPbkVCardIdNameMapping*>& aArray );
    private:
        TVPbkMappingsFindTool() { }
	};

TInt TVPbkMappingsFindTool::FindIdByName(
    const TDesC8& aName, const TArray<CVPbkVCardIdNameMapping*>& aArray )
    {
    TInt retVal( KErrNotFound );
    for ( TInt i = 0; 
          i < aArray.Count() && ( retVal == KErrNotFound ); 
          ++i )
        {
        const CVPbkVCardIdNameMapping* mapping = aArray[ i ];
        if ( mapping->Name().Compare( aName ) == 0 )
            {
            retVal = mapping->Id();
            }
        }
    return retVal;
    }

const TDesC8& TVPbkMappingsFindTool::FindNameById(
    TInt aId, const TArray<CVPbkVCardIdNameMapping*>& aArray )
    {
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        const CVPbkVCardIdNameMapping* mapping = aArray[ i ];
        if ( mapping->Id() == aId )
            {
            return mapping->Name();
            }
        }
    return KNullDesC8;
    }

#endif // TVPBKMAPPINGSFINDTOOL_H
//End of file

