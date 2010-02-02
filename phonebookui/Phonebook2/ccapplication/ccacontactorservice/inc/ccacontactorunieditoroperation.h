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
* Description:  Class implementing the unieditor operation (SMS/MMS)
*
*/


#ifndef CCACONTACTORUNIEDITOROPERATION_H
#define CCACONTACTORUNIEDITOROPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CCCAContactorOperation;

/**
 * Class implementing the unieditor operation (SMS/MMS)
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorUniEditorOperation) : public CCCAContactorOperation
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCAContactorUniEditorOperation();

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to send the message to
     * @param aName The name of the contact, to be shown in the "to" field
     */
    static CCCAContactorUniEditorOperation* NewL(const TDesC& aParam, const TDesC& aName);

    /**
     * Two-phased constructor.
     * 
     * @param aParam The phone number to send the message to
     * @param aName The name of the contact, to be shown in the "to" field
     */
    static CCCAContactorUniEditorOperation* NewLC(const TDesC& aParam, const TDesC& aName);
    
    /**
     * Executes the operation (starts editor for sending the message)
     */
    virtual void ExecuteLD();

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam The phone number to send the message to
     * @param aName The name of the contact, to be shown in the "to" field
     */
    CCCAContactorUniEditorOperation(const TDesC& aParam, const TDesC& aName);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    };

#endif // CCACONTACTORUNIEDITOROPERATION_H
// End of File
