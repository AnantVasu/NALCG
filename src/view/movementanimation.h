#ifndef _NALCG_MOVEMENT_ANIMATION_H_
#define _NALCG_MOVEMENT_ANIMATION_H_

#include "genericanimation.h"

class AnimationManager;
class MovementAnimation : public GenericAnimation
{
public:
    MovementAnimation(const Vector3& destination, SceneNode *movingNode,
        SceneNode *targetPiece, SceneManager *sceneMgr, AnimationManager *animationManager)
        : GenericAnimation(movingNode, sceneMgr), mDestination(destination),
        mTargetPiece(targetPiece), mTargetPieceName(targetPiece ? targetPiece->getName() : "NO_TARGET"),
        mAnimationManager(animationManager)
    {
    }

    virtual ~MovementAnimation();
    virtual void dimLights();
    virtual void restoreLights();

protected:
    virtual void playAnimation(const std::string& animationName, double time,
        bool stop = false, SceneNode* animatedNode = 0, bool loop = true);
    virtual void resetAnimation(const std::string& animationName)
    {
        playAnimation(animationName, 0, true);
    }

    Vector3 mDestination;
    SceneNode* mTargetPiece;
    const std::string mTargetPieceName;
    AnimationManager* mAnimationManager;
};

#endif // _NALCG_MOVEMENT_ANIMATION_H_
