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
*      Cleanup utility which resets a supplied pointer at PopAndDestroy().
*
*/


#ifndef     __DigCleanupResetPointer_H__
#define     __DigCleanupResetPointer_H__


//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Helper class for implementing CleanupResetPointerPushL function.
 * @see CleanupResetPointerPushL
 */
template<class PointerType>
class DigCleanupResetPointer 
    {
    private:
        /// Nampespace class -- constructor not implemented.
        DigCleanupResetPointer();

        /**
         * TCleanupItem callback for CleanupResetPointerPushL.
         * @see DigCleanupResetPointer::PushL
         */
        static void ResetPointerCallback(TAny* aPtrToPtrToReset);

    public:
        /**
         * Pushes a cleanup item on the cleanup stack which calls 
         * ResetPointerCallback on PopAndDestroy() to reset
         * aPointerToReset.
         *
         * @param aPointerToReset   pointer to the pointer to reset to NULL.
         * @exception   KErrNoMemory if cleanup stack runs out of memory. If this
         *              leave occurs the pointer is still reset to NULL (standard
         *              CleanupStack::PushL semantics).
         */
        inline static void PushL(PointerType*& aPointerToReset)
            {
            CleanupStack::PushL(TCleanupItem(ResetPointerCallback, &aPointerToReset));
            }
    };

template<class PointerType>
void DigCleanupResetPointer<PointerType>::ResetPointerCallback(TAny* aPtrToPtrToReset)
    {
    *static_cast<PointerType**>(aPtrToPtrToReset) = NULL;
    }

/**
 * Pushes a "reset pointer" cleanup item on the cleanup stack. 
 * When the "reset pointer" item is destroyed with CleanupStack::PopAndDestroy()
 * it resets the pointer variable passed in to NULL.<p>
 * 
 * Example:<br>
 * <pre>
 * CleanupResetPointerPushL(iFooDialog);
 * iFooDialog = new(ELeave) CFooDialog;
 * iFooDialog->ExecuteLD(R_FOO_DIALOG);
 * CleanupStack::PopAndDestroy();  // resets iFooDialog to NULL
 * </pre><p>
 *
 * Note that in the example above iFooDialog will be reset to NULL:
 * <ol>
 * <li>By the CleanupStack::PopAndDestroy() call after the ExecuteLD() call, or</li>
 * <li>by standard cleanup mechanism if ExecuteLD leaves.</li>
 * </ol>
 *
 * <p>The idea is to prevent a TRAP like here:</p>
 * <pre>
 * iFooDialog = new(ELeave) CFooDialog;
 * TRAPD(err, iFooDialog->ExecuteLD(R_FOO_DIALOG));
 * iFooDialog = NULL;
 * if (err) User::Leave(err);
 * </pre>
 *
 * @param aPointerToReset   pointer variable to reset to NULL on 
 *                          CleanupStack::PopAndDestroy() or leave.
 *
 * @exception   KErrNoMemory if cleanup stack runs out of memory. If this
 *              leave occurs the pointer is still reset to NULL (standard
 *              CleanupStack::PushL semantics).
 */
template<class PointerType>
inline void CleanupResetPointerPushL(PointerType*& aPointerToReset)
    {
    DigCleanupResetPointer<PointerType>::PushL(aPointerToReset);
    }


#endif
            

// End of File
