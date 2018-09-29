/**
 * Copyright 2017
 * 
 * This file is part of On-line POMDP Planning Toolkit (OPPT).
 * OPPT is free software: you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License published by the Free Software Foundation, 
 * either version 2 of the License, or (at your option) any later version.
 * 
 * OPPT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with OPPT. 
 * If not, see http://www.gnu.org/licenses/.
 */
#include "oppt/plugin/Plugin.hpp"
#include "plugins/rewardPlugins/DefaultRewardOptions.hpp"

namespace oppt
{
class DefaultRewardPlugin: public RewardPlugin
{
public :
    DefaultRewardPlugin():
        RewardPlugin() {

    }

    virtual ~DefaultRewardPlugin() = default;

    virtual bool load(RobotEnvironment* const robotEnvironment,
                      const std::string& optionsFile) override {
        robotEnvironment_ = robotEnvironment;
        parseOptions_<DefaultRewardOptions>(optionsFile);
        return true;
    }

    virtual FloatType getReward(const PropagationResultSharedPtr& propagationResult) const override {
        auto options = static_cast<DefaultRewardOptions*>(options_.get());
        if (!propagationResult)
            ERROR("Propagation report is null");
	
	/** Make sure we have some collision information */
        if (!propagationResult->madeCollisionReport) {
            oppt::CollisionReportSharedPtr collisionReport =
                robotEnvironment_->getRobot()->makeDiscreteCollisionReport(propagationResult->nextState);
            propagationResult->collided = collisionReport->collides;
            propagationResult->madeCollisionReport= true;
        }
        
        if (propagationResult->collided) 
            return -options->illegalMovePenalty;
	
        //if (!robotEnvironment_->isValid(propagationResult)) {
            //return -options->illegalMovePenalty;
        if (robotEnvironment_->isTerminal(propagationResult)) 
            return options->exitReward;        

        return -options->stepPenalty;
    }

    virtual std::pair<FloatType, FloatType> getMinMaxReward() const override {
        return std::make_pair(-static_cast<DefaultRewardOptions*>(options_.get())->illegalMovePenalty,
                              static_cast<DefaultRewardOptions*>(options_.get())->exitReward);
    }

private:
    const RobotEnvironment* robotEnvironment_;

};

OPPT_REGISTER_REWARD_PLUGIN(DefaultRewardPlugin)

}