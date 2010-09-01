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
*       Class to be used by listbox model for analyzing if
*       TPbkContactItemField has speed dial or voice tag.
*
*/

#ifndef __CPBKFIELDANALYZER_H__
#define __CPBKFIELDANALYZER_H__

#include <e32base.h>
#include "MPbkFieldAnalyzer.h"

//  FORWARD DECLARATIONS
class CPbkFieldArray;
class TPbkContactItemField;

class CPbkFieldAnalyzer : public CBase,
                          public MPbkFieldAnalyzer
	{
public:
    /**
     * Creates a new instance of this class.
     *
     * @param aEngine       Contact engine needed for MPbkFieldAnalyzer
     *                      functions. This object does NOT take
     *                      ownership of aContact.
     */
    static CPbkFieldAnalyzer* NewL( const CPbkFieldArray& aArray );

    /**
     * Destructor.
     */
    ~CPbkFieldAnalyzer();

public: // From MPbkFieldAnalyzer
    TBool HasSpeedDialL( const TPbkContactItemField& aField ) const;
    TBool HasVoiceTagL( const TPbkContactItemField& aField ) const;
    
private:
	TBool IsFieldOwned( const TPbkContactItemField& aField ) const;

private:
    CPbkFieldAnalyzer( const CPbkFieldArray& aArray );

private: // Data member
    /// Ref: Contact item to display
    const CPbkFieldArray& iFieldArray;
    };

#endif // __CPBKFIELDANALYZER_H__

