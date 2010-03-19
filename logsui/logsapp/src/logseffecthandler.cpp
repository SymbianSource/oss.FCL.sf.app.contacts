/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
//USER
#include "logseffecthandler.h"
#include "logslogger.h"

//SYSTEM
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <hbinstance.h>

const int logsMoveByExtra = 20;
const int logsEffectDelayBetween = 200;
const int logsEffectAppearDurationInMs = 500;
const int logsEffectDissappearDurationInMs = 300;

const int logsDissappearByMovingIndex = 0;
const int logsAppearByMovingIndex = 2;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEffectHandler::LogsEffectHandler() : QObject()
{
    LOGS_QDEBUG( "logs [UI] -> LogsEffectHandler::LogsEffectHandler()" );
    
    QPropertyAnimation* itemAppearByMovingAnimation = new QPropertyAnimation();
    QPropertyAnimation* itemAppearByMovingAnimation2 = new QPropertyAnimation();
    QPropertyAnimation* itemDissappearByMovingAnimation = new QPropertyAnimation();
    QPropertyAnimation* itemDissappearByMovingAnimation2 = new QPropertyAnimation();
    
    mItemAppearByFadingAnimation = new QPropertyAnimation();
    mItemDissappearByFadingAnimation = new QPropertyAnimation();
    
    mMoveGroup = new QSequentialAnimationGroup;
    mMoveGroup2 = new QSequentialAnimationGroup;
    mFadeGroup = new QSequentialAnimationGroup;
    
    mMoveGroup->addAnimation(itemDissappearByMovingAnimation);
    mMoveGroup->addPause(logsEffectDelayBetween);
    mMoveGroup->addAnimation(itemAppearByMovingAnimation);
    
    mMoveGroup2->addAnimation(itemDissappearByMovingAnimation2);
    mMoveGroup2->addPause(logsEffectDelayBetween);
    mMoveGroup2->addAnimation(itemAppearByMovingAnimation2);
    
    mFadeGroup->addAnimation(mItemDissappearByFadingAnimation);
    mFadeGroup->addAnimation(mItemAppearByFadingAnimation);

    connect(mMoveGroup, SIGNAL(currentAnimationChanged(QAbstractAnimation *)), 
            this, SLOT( moveAnimationChanged(QAbstractAnimation *)));
    connect(mFadeGroup, SIGNAL(currentAnimationChanged(QAbstractAnimation *)), 
            this, SLOT( fadeAnimationChanged(QAbstractAnimation *)));        
    
    LOGS_QDEBUG( "logs [UI] <- LogsEffectHandler::LogsEffectHandler()" );
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEffectHandler::~LogsEffectHandler()
{
    LOGS_QDEBUG( "logs [UI] -> LogsEffectHandler::~LogsEffectHandler()" );
    
    // Groups delete attached animations
    delete mMoveGroup; 
    delete mMoveGroup2; 
    delete mFadeGroup;
    
    LOGS_QDEBUG( "logs [UI] <- LogsEffectHandler::~LogsEffectHandler()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::moveAnimationChanged(QAbstractAnimation *currentAnimation)
{
    if ( mMoveGroup->indexOfAnimation(currentAnimation) == 1 ) {
        emit dissappearByMovingComplete();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::fadeAnimationChanged(QAbstractAnimation *currentAnimation)
{
    if ( mFadeGroup->indexOfAnimation(currentAnimation) == 1 ) {
        emit dissappearByFadingComplete();
    }
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::startDissappearAppearByFadingEffect(QObject& effectTarget)
{
    initOpacityEffect(*mItemDissappearByFadingAnimation, &effectTarget, 
                      1.0, 0.0, logsEffectDissappearDurationInMs);
    initOpacityEffect(*mItemAppearByFadingAnimation, &effectTarget, 
                      0.0, 1.0, logsEffectAppearDurationInMs);
    mFadeGroup->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::startDissappearAppearByMovingEffect(
        QObject& effectTarget, QObject& secondaryEffectTarget, 
        bool dissappearToLeft, int origX)
{
    doStartDissappearAppearByMovingEffect(
            *mMoveGroup, effectTarget, dissappearToLeft, origX);
    doStartDissappearAppearByMovingEffect(
            *mMoveGroup2, secondaryEffectTarget, dissappearToLeft, origX);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::doStartDissappearAppearByMovingEffect(
        QSequentialAnimationGroup& group, QObject& effectTarget, 
        bool dissappearToLeft, int origX)
{
    initDissappearByMovingEffect(
            static_cast<QPropertyAnimation&>(*group.animationAt(logsDissappearByMovingIndex)),
            &effectTarget, dissappearToLeft, origX);
    initAppearByMovingEffect(
            static_cast<QPropertyAnimation&>(*group.animationAt(logsAppearByMovingIndex)), 
            &effectTarget, !dissappearToLeft, origX);
    group.start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::initAppearByMovingEffect(
    QPropertyAnimation& animation, QObject* effectTarget, bool appearFromLeft, int origX)
{
    LOGS_QDEBUG( "logs [UI] -> LogsEffectHandler::startAppearByMovingEffect()" );
    
    Q_ASSERT(!hbInstance->allMainWindows().isEmpty());   
    int moveBy = hbInstance->allMainWindows().at(0)->layoutRect().width() + logsMoveByExtra; 
    int startPos = appearFromLeft ? -moveBy : moveBy;
    QEasingCurve easing(QEasingCurve::OutQuad); // decelerating
    initMoveHorizontallyEffect(
            animation, effectTarget, startPos, origX,
            logsEffectAppearDurationInMs, easing);
    
    LOGS_QDEBUG( "logs [UI] <- LogsEffectHandler::startAppearByMovingEffect()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::initDissappearByMovingEffect(
    QPropertyAnimation& animation, QObject* effectTarget, bool dissappearToLeft, int origX)
{
    LOGS_QDEBUG( "logs [UI] -> LogsEffectHandler::startDissappearByMovingEffect()" );
    
    Q_ASSERT(!hbInstance->allMainWindows().isEmpty());   
    int moveBy = hbInstance->allMainWindows().at(0)->layoutRect().width() + logsMoveByExtra; 
    int endPos = dissappearToLeft ? -moveBy : moveBy;
    QEasingCurve easing(QEasingCurve::InQuad); // accelerating
    initMoveHorizontallyEffect(
            animation, effectTarget, origX, endPos,
            logsEffectDissappearDurationInMs, easing);
    
    LOGS_QDEBUG( "logs [UI] <- LogsEffectHandler::startDissappearByMovingEffect()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::initOpacityEffect(
         QPropertyAnimation& animation, QObject* effectTarget, 
         qreal startVal, qreal endVal, int duration)
{
    if (animation.state() == QAbstractAnimation::Running ){
        animation.stop();
    }
    animation.setTargetObject(effectTarget);
    animation.setPropertyName("opacity");
    animation.setStartValue(startVal);
    animation.setEndValue(endVal);
    animation.setDuration(duration);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsEffectHandler::initMoveHorizontallyEffect( 
        QPropertyAnimation& animation, QObject* effectTarget,
        int startVal, int endVal, int duration, const QEasingCurve& easingCurve)
{
    if (animation.state() == QAbstractAnimation::Running ){
        animation.stop();
    }
    animation.setTargetObject(effectTarget);
    animation.setPropertyName("x");
    animation.setStartValue(startVal);
    animation.setEndValue(endVal);
    animation.setDuration(duration);
    animation.setEasingCurve(easingCurve);
}
