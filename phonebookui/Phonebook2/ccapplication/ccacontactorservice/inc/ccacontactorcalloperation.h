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
* Description:  Class implementing the call operation
*
*/


#ifndef CCACONTACTORCALLOPERATION_H
#define CCACONTACTORCALLOPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CCCAContactorOperation;

/**
 * Class implementing the call operation
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorCallOperation) : public CCCAContactorOperation
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCAContactorCallOperation();

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to call.
     * @param aContactLinkArray Streamed contact link array of contact to be communicated at
     */
    static CCCAContactorCallOperation* NewL(const TDesC& aParam, const TDesC8& aContactLinkArray);

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to call.
     * @param aContactLinkArray Streamed contact link array of contact to be communicated at
     */
    static CCCAContactorCallOperation* NewLC(const TDesC& aParam, const TDesC8& aContactLinkArray);
    
    /**
     * Executes the operation (makes the call)
     */
    virtual void ExecuteLD();

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam The phone number to call.
     * @param aContactLinkArray Streamed contact link array of contact to be communicated at
     */
    CCCAContactorCallOperation(const TDesC& aParam, const TDesC8& aContactLinkArray);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    
    //Doesnt Own : Streamed contact link array of contact
    const TDesC8& iContactLinkArray;

    };

#endif // CCACONTACTORCALLOPERATION_H
// End of File
