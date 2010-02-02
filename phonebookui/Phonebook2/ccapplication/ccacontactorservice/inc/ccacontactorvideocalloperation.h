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
* Description:  Class implementing the video call operation
*
*/


#ifndef CCACONTACTORVIDEOCALLOPERATION_H
#define CCACONTACTORVIDEOCALLOPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CCCAContactorOperation;

/**
 * Class implementing the video call operation
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorVideocallOperation) : public CCCAContactorOperation
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCAContactorVideocallOperation();

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to video call
     */
    static CCCAContactorVideocallOperation* NewL(const TDesC& aParam);

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to video call
     */
    static CCCAContactorVideocallOperation* NewLC(const TDesC& aParam);
    
    /**
     * Executes the operation (makes the video call)
     */
    virtual void ExecuteLD();

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam The phone number to video call
     */
    CCCAContactorVideocallOperation(const TDesC& aParam);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    };

#endif // CCACONTACTORVIDEOCALLOPERATION_H
// End of File
