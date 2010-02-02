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
*		View node class
*
*/


// INCLUDE FILES
#include <DigViewGraph.h>
#include <barsc.h>      // RResourceFile
#include <barsread.h>   // TResourceReader
#include <e32std.h>     


/// Unnamed namespace for local definitions
namespace {
void CleanupResetAndDestroy(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<CArrayPtrFlat<CDigViewNode>*>(aObj)->ResetAndDestroy();
        delete aObj;
        }
    }

// LOCAL DEBUG CODE
#ifdef _DEBUG
// MODULE DATA STRUCTURES
enum TPanicCode
    {    
    EPanicPre_MergeNodesL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CDigViewGraph");
    User::Panic(KPanicText,aReason);
    }
#endif
}

// ================= MEMBER FUNCTIONS =======================


//
// CDigViewNode
// 
EXPORT_C CDigViewNode::~CDigViewNode()
    {
    }

EXPORT_C TBool CDigViewNode::FindTransition
        (const TDigViewTransition& aTransition) const
    {
    for (TInt i=0; i < iTransitions.Count(); ++i)
        {
        if (iTransitions[i] == aTransition)
            return ETrue;
        }
    return EFalse;
    }

/**
 * Finds transition based on aEvent, in case not found returns NULL.
 *
 * @param aEvent event to be searched
 */
TDigViewTransition* CDigViewNode::FindTransition(const TInt aEvent)
    {
    for (TInt i=0; i < iTransitions.Count(); ++i)
        {
        if (iTransitions[i].iEvent == aEvent)
            return &iTransitions[i];
        }
    return NULL;
    }

/**
 * Appends or modify existing transition(s). 
 *
 * @param aNewNode holds transitions to be processed
 */
void CDigViewNode::MergeL(CDigViewNode& aNewNode)
    {
    TInt transCount = aNewNode.iTransitions.Count();
    while (transCount-- > 0)
        {
        TDigViewTransition newTrans = aNewNode.iTransitions.At(transCount);
        TDigViewTransition* trans = FindTransition(newTrans.iEvent);
        if (trans)
            {
            trans->iTargetViewId = newTrans.iTargetViewId;
            }
        else 
            {
            // Add new transition
            iTransitions.AppendL(newTrans);
            }
        }
    iAdditionalResId = aNewNode.iAdditionalResId;
    }


//
// CDigViewGraph
//
EXPORT_C CDigViewGraph* CDigViewGraph::NewL(TResourceReader& aResReader)
    {
    CDigViewGraph* self = new(ELeave) CDigViewGraph;
    CleanupStack::PushL(self);
    self->ConstructL(aResReader);
    CleanupStack::Pop();  //self
    return self;
    }

void CDigViewGraph::ConstructL(TResourceReader& aResReader)
    {
    iNodes = CreateUnlinkedNodesFromResourceL(aResReader);
    LinkNodesWithTransitions();    
    }

EXPORT_C CDigViewGraph::~CDigViewGraph()
    {    
    if (iNodes)
        {
        iNodes->ResetAndDestroy();
        }
    delete iNodes;
    }

EXPORT_C CDigViewNode* CDigViewGraph::FindNodeWithViewId(TUid aViewId) const
    {
    if (aViewId != KNullUid)
        {
        TInt count = iNodes->Count();
        while (count-- > 0)
            {
            CDigViewNode* node = iNodes->At(count);
            if (node->iViewId == aViewId)
                return node;
            }
        }
    return NULL;
    }

EXPORT_C void CDigViewGraph::ModifyViewGraphL(TResourceReader& aResReader)
    {
    CDigViewNodeArray* nodes = CreateUnlinkedNodesFromResourceL(aResReader);
    // The nodes of the array are not cleaned up by design, only the array
    // holding them. The node cleanup is handled in MergeNodesL.
    CleanupStack::PushL(nodes); 
    MergeNodesL(*nodes); // The ownership of the array's nodes is taken here
    CleanupStack::PopAndDestroy(nodes);

    LinkNodesWithTransitions();
    }

/**
 * Creates a new view node array from resource file.
 *
 * @param aResReader containing DIG_VIEW_GRAPH resource
 */
CDigViewGraph::CDigViewNodeArray* CDigViewGraph::
        CreateUnlinkedNodesFromResourceL(TResourceReader& aResReader)
    {
    TInt count = aResReader.ReadInt16();
    CDigViewNodeArray* nodes = new (ELeave) CDigViewNodeArray(1);
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy,nodes));
    if (count > 1)
        {
        nodes->SetReserveL(count);
        }
    
    while (count-- > 0)
        {            
        CDigViewNode* node = new(ELeave) CDigViewNode;
        CleanupStack::PushL(node);
        
        // DIG_VIEW_NODE
        node->iViewId.iUid = aResReader.ReadInt32();
        node->iDefaultPrevViewId.iUid = aResReader.ReadInt32(); 
        node->iDefaultPreviousNode = NULL;
        node->iPreviousNode = NULL;        
        node->iExitNode = aResReader.ReadInt8();
        
        TInt transCount = aResReader.ReadInt16(); // transitions[]
        node->iTransitions.SetReserveL(transCount);
        while (transCount-- > 0)
            {
            // DIG_VIEW_TRANSITION
            TDigViewTransition trans;
            trans.iEvent = aResReader.ReadInt32();            
            trans.iTargetViewId.iUid = aResReader.ReadInt32();
            trans.iNode = NULL;
            node->iTransitions.AppendL(trans);
            }        
        node->iAdditionalResId = aResReader.ReadInt32();

        nodes->AppendL(node);
        CleanupStack::Pop();
        }
    CleanupStack::Pop(nodes);
    return nodes;
    }   

/**
 * Links nodes with transitions.
 */
void CDigViewGraph::LinkNodesWithTransitions()
    {
    for (TInt i(0); i < iNodes->Count(); i++)
        {
        CDigViewNode* node = iNodes->At(i);
        node->iDefaultPreviousNode = FindNodeWithViewId
                (node->iDefaultPrevViewId);
        TInt transCount = node->iTransitions.Count();
        while (transCount-- > 0)
            {
            TDigViewTransition& trans = node->iTransitions.At(transCount);
            trans.iNode = FindNodeWithViewId(trans.iTargetViewId);
            }
        }
    }

/**
 * Merges two node arrays, iNode and aChangedNodes. 
 * Handles also destruction of nodes existing in aChangedNodes.
 *
 * @param aChangedNodes array to be merged with iNode
 */
void CDigViewGraph::MergeNodesL(CDigViewNodeArray& aChangedNodes)
    {
    __ASSERT_DEBUG(iNodes != &aChangedNodes, Panic(EPanicPre_MergeNodesL));

    for ( TInt j = aChangedNodes.Count()-1; j >= 0; --j )
        {
        // Changed node has to be destroyed if not appended
        CDigViewNode* changedNode = aChangedNodes.At(j);
        CDigViewNode* node = FindNodeWithViewId(changedNode->ViewId());
        if (node)
            {
            // The changedNode is not in cleanupstack yet,
            // @see CDigViewGraph::ModifyViewGraphL
            CleanupStack::PushL(changedNode);
            node->MergeL(*changedNode);
            aChangedNodes.Delete(j);
            CleanupStack::PopAndDestroy(changedNode);
            }
        else
            {
            // The changedNode is not in cleanupstack yet,
            // @see CDigViewGraph::ModifyViewGraphL
            CleanupStack::PushL(changedNode);
            iNodes->AppendL(changedNode);
            CleanupStack::Pop(changedNode);
            }
        }
    }


//  End of File  
