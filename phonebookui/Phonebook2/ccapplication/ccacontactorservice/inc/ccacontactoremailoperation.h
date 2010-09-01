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
* Description:  Class implementing the email operation
*
*/


#ifndef CCACONTACTOREMAILOPERATION_H
#define CCACONTACTOREMAILOPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CCCAContactorOperation;

/**
 * Class implementing the email operation
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorEmailOperation) : public CCCAContactorOperation
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCAContactorEmailOperation();

    /**
     * Two-phased constructor.
     * 
     * @param aParam The address to send email to.
     * @param aName The name of the contact, to be shown in the "to" field
     */
    static CCCAContactorEmailOperation* NewL(const TDesC& aParam, const TDesC& aName);

    /**
     * Two-phased constructor.
     * 
     * @param aParam The address to send email to.
     * @param aName The name of the contact, to be shown in the "to" field
     */
    static CCCAContactorEmailOperation* NewLC(const TDesC& aParam, const TDesC& aName);
    
    /**
     * Executes the operation (starts the email editor)
     */
    virtual void ExecuteLD();

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam The address to send email to.
     * @param aName The name of the contact, to be shown in the "to" field
     */
    CCCAContactorEmailOperation(const TDesC& aParam, const TDesC& aName);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    };

#endif // CCACONTACTOREMAILOPERATION_H
// End of File
