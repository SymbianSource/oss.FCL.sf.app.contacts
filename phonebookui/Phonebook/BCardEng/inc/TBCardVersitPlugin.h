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
* Description:  Plug-in
*
*/


#ifndef __TBCARDVERSITPLUGIN_H__
#define __TBCARDVERSITPLUGIN_H__

//  INCLUDES
#include <vobserv.h>

// CLASS DECLARATION

NONSHARABLE_CLASS(TBCardVersitPlugin) : public MVersitPlugIn
	{
    public: // Interface
        /**
         * Default constructor.
         */
        TBCardVersitPlugin();
    
    public: // from MVersitPlugIn
        TBool AddSpace();
        TBool DeleteAllSpaces();
        TBool NeedsBlankLine();
        void RemoveEscaping(TPtr16& aText);
        void AddEscaping(HBufC16*& aText);
        TBool WrapLine(RWriteStream& aStream,TInt& aCurrentLineLength,const TPtr8& aText);
        TBool EncodingType(
            Versit::TVersitEncoding& aEncoding,
            TBool aRequiresEncoding,
            Versit::TVersitEncoding aDefaultEncoding,
			TUid aPropertyUid,
            TUint aPropertyCharsetId);
        const TDesC8& EncodingName(Versit::TVersitEncoding aEncoding);

    };

#endif // __TBCARDVERSITPLUGIN_H__
            
// End of File
