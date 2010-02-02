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
* Description:  An observer for vcard conversion
*
*/


#ifndef MPBK2VCARDCONVERTEROBSERVER_H
#define MPBK2VCARDCONVERTEROBSERVER_H

#include <e32def.h>

class MPbk2vCardConverterObserver
	{
	public:
		
		/**
		 * Called when all contacts have been converted.
		 * @param aCount count of converted contacts
		 */
		virtual void ConversionDone(TInt aCount) = 0;
		
		/**
		 * Called when error occurs.
		 * @param aError error code
		 */
		virtual void ConversionError(TInt aError) = 0;
	};

#endif // MPBK2VCARDCONVERTEROBSERVER_H

// End of file
