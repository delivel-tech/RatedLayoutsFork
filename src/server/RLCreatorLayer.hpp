#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RLCreatorLayer : public CCLayer {
     protected:
      bool init();
      void onBackButton(CCObject* sender);
      void keyBackClicked() override;

      void onLeaderboard(CCObject* sender);
      void onFeaturedLayouts(CCObject* sender);
      void onNewRated(CCObject* sender);
      void onSendLayouts(CCObject* sender);

     public:
      static RLCreatorLayer* create();
};
