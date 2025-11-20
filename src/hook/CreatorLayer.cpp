#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "../server/RLLeaderboardLayer.hpp"

using namespace geode::prelude;

class $modify(RLCreatorLayer, CreatorLayer) {
      bool init() {
            if (!CreatorLayer::init()) return false;
            // add a menu on the left side of the layer
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto bottomLeftMenu = static_cast<CCMenu*>(this->getChildByID("bottom-left-menu"));
            // leaderboard buttons
            // most stars button
            auto leaderboardSpr = CCSprite::create("rlRankIcon.png"_spr);
            auto lbCircleButton = CircleButtonSprite::create(
                leaderboardSpr, CircleBaseColor::Cyan, CircleBaseSize::Small);
            auto lbButton = CCMenuItemSpriteExtra::create(
                lbCircleButton, this, menu_selector(RLCreatorLayer::onLeaderboard));
            bottomLeftMenu->addChild(lbButton);
            bottomLeftMenu->updateLayout();
            return true;
      }
      void onLeaderboard(CCObject* sender) {
            auto leaderboardLayer = RLLeaderboardLayer::create();
            auto scene = CCScene::create();
            scene->addChild(leaderboardLayer);
            auto transitionFade = CCTransitionFade::create(0.5f, scene);
            CCDirector::sharedDirector()->pushScene(transitionFade);
      }
};