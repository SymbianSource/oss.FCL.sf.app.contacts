/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "cmscontactfielditem.h"

//KONSTS
_LIT( KCmsServerLogDir,                             "CCA" );
_LIT( KCmsServerLogFile,                            "cmsserver.txt" );

class CCmsContactFieldItem;

class CmsServerUtils
    {
    public:
        
        static HBufC* ReadMsgParam16LC( TInt aMsgIndex, const RMessage2& aMessage );
        
        static HBufC8* ReadMsgParam8LC( TInt aMsgIndex, const RMessage2& aMessage );
        
        static TInt ConvertDesc( const TDesC8& aNumberDesc );
    
        static TPtr8 DescriptorCast( const TDesC8& aConstData );

        static HBufC8* ConvertDescLC( const TDesC& a16bitDescriptor );

        static HBufC* ConvertDescLC( const TDesC8& a8bitDescriptor );

        static TInt MatchFieldType( CCmsContactFieldItem::TCmsContactField aCmsFieldType );
        
        static TBool IsFeatureSupportedL( TInt aFeature, const TDesC& aScheme );
        
        static TBool IsVoIPSupportedL();
    
        static CCmsContactFieldItem::TCmsContactField MatchFieldType( TInt aPhonebookResId );
    
 
    };
