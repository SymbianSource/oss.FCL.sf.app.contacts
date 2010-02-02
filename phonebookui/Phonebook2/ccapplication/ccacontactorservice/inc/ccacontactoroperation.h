/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for communication method launching operations
*
*/


#ifndef CCACONTACTOROPERATION_H
#define CCACONTACTOROPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 * Base class for communication method launching operations
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorOperation) : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CCCAContactorOperation();
    
    /**
     * Executes the operation
     */
    virtual void ExecuteLD()=0;

protected:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam Communication parameter, for example phone number or e-mail address
     * @param aName Name of the contact to which the communication is started (optional)
     */
    CCCAContactorOperation(const TDesC& aParam, const TDesC& aName = KNullDesC);

    const TDesC& iParam;
    
    const TDesC& iName;

    };

#endif // CCACONTACTOROPERATION_H
// End of File
