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
* Description:  Dummy Implementation for CPbkSINDHandler WINS environment
*
*/


// INCLUDE FILES
#include "CPbkSINDHandler.h"

// ================= MEMBER FUNCTIONS =======================

inline CPbkSINDHandler::CPbkSINDHandler()
    {
    }

inline void CPbkSINDHandler::ConstructL()
    {
    }

CPbkSINDHandler* CPbkSINDHandler::NewL()
    {    
    CPbkSINDHandler* self = new (ELeave) CPbkSINDHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }

CPbkSINDHandler::~CPbkSINDHandler()
    {                
    }
    
TInt CPbkSINDHandler::VoiceTagField(TContactItemId /*aContactId*/)
    {   
    return NULL;
    }
    
void CPbkSINDHandler::GetContextCompleted(MNssContext* /*aContext*/)
    {   
    }
    
void CPbkSINDHandler::GetContextListCompleted(MNssContextListArray* /*aContextList*/)
    {            
    }
    
void CPbkSINDHandler::GetContextFailed(TNssGetContextClientFailCode /*aFailCode*/)
    {        
    }    

void CPbkSINDHandler::GetTagListCompleted(MNssTagListArray* /*aTagList*/)
    {        
    }
    
    
void CPbkSINDHandler::GetTagFailed(TNssGetTagClientFailCode /*aFailCode*/)
    {    
    }    
    
// End of file
