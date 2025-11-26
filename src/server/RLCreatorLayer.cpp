#include "RLCreatorLayer.hpp"

#include "LevelSearchLayer.cpp"
#include "RLLeaderboardLayer.hpp"

bool RLCreatorLayer::init() {
      if (!CCLayer::init())
            return false;

      auto winSize = CCDirector::sharedDirector()->getWinSize();

      auto bg = createLayerBG();
      if (bg)
            this->addChild(bg);

      addSideArt(this, SideArt::All, SideArtStyle::Layer, false);

      auto backMenu = CCMenu::create();
      backMenu->setPosition({0, 0});

      auto backButtonSpr =
          CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
      auto backButton = CCMenuItemSpriteExtra::create(
          backButtonSpr, this, menu_selector(RLCreatorLayer::onBackButton));
      backButton->setPosition({25, winSize.height - 25});
      backMenu->addChild(backButton);
      this->addChild(backMenu);

      auto mainMenu = CCMenu::create();
      mainMenu->setPosition({winSize.width / 2, winSize.height / 2});
      mainMenu->setContentSize({300.f, 240.f});
      mainMenu->setLayout(RowLayout::create()
                              ->setGap(6.f)
                              ->setGrowCrossAxis(true)
                              ->setCrossAxisOverflow(false));

      this->addChild(mainMenu);

      auto featuredSpr = CCSprite::create("RL_featuredBtn.png"_spr);
      if (!featuredSpr) featuredSpr = CCSprite::create("RL_featuredBtn.png"_spr);
      auto featuredItem = CCMenuItemSpriteExtra::create(
          featuredSpr, this, menu_selector(RLCreatorLayer::onFeaturedLayouts));
      featuredItem->setID("featured-button");
      mainMenu->addChild(featuredItem);

      auto leaderboardSpr = CCSprite::create("RL_leaderboardBtn.png"_spr);
      if (!leaderboardSpr) leaderboardSpr = CCSprite::create("RL_leaderboardBtn.png"_spr);
      auto leaderboardItem = CCMenuItemSpriteExtra::create(
          leaderboardSpr, this, menu_selector(RLCreatorLayer::onLeaderboard));
      leaderboardItem->setID("leaderboard-button");
      mainMenu->addChild(leaderboardItem);

      auto newlySpr = CCSprite::create("RL_newRatedBtn.png"_spr);
      if (!newlySpr) newlySpr = CCSprite::create("RL_newRatedBtn.png"_spr);
      auto newlyItem = CCMenuItemSpriteExtra::create(
          newlySpr, this, menu_selector(RLCreatorLayer::onNewRated));
      newlyItem->setID("newly-rated-button");
      mainMenu->addChild(newlyItem);

      auto sendSpr = CCSprite::create("RL_sendLayoutsBtn.png"_spr);
      if (!sendSpr) sendSpr = CCSprite::create("RL_sendLayoutsBtn.png"_spr);
      auto sendItem = CCMenuItemSpriteExtra::create(
          sendSpr, this, menu_selector(RLCreatorLayer::onSendLayouts));
      sendItem->setID("send-layouts-button");
      mainMenu->addChild(sendItem);
      mainMenu->updateLayout();

      this->setKeypadEnabled(true);

      return true;
}

void RLCreatorLayer::onBackButton(CCObject* sender) {
      CCDirector::sharedDirector()->popSceneWithTransition(
          0.5f, PopTransition::kPopTransitionFade);
}

void RLCreatorLayer::onLeaderboard(CCObject* sender) {
      auto leaderboardLayer = RLLeaderboardLayer::create();
      auto scene = CCScene::create();
      scene->addChild(leaderboardLayer);
      auto transitionFade = CCTransitionFade::create(0.5f, scene);
      CCDirector::sharedDirector()->pushScene(transitionFade);
}

void RLCreatorLayer::onFeaturedLayouts(CCObject* sender) {
      web::WebRequest()
          .param("type", 2)
          .param("amount", 1000)
          .get("https://gdrate.arcticwoof.xyz/getLevels")
          .listen([this](web::WebResponse* res) {
                if (res && res->ok()) {
                      auto jsonResult = res->json();

                      if (jsonResult) {
                            auto json = jsonResult.unwrap();
                            std::string levelIDs;
                            bool first = true;

                            if (json.contains("levelIds")) {
                                  auto levelsArr = json["levelIds"];

                                  // iterate
                                  for (auto levelIDValue : levelsArr) {
                                        auto levelID = levelIDValue.as<int>();
                                        if (levelID) {
                                              if (!first)
                                                    levelIDs += ",";
                                              levelIDs += numToString(levelID.unwrap());
                                              first = false;
                                        }
                                  }
                            }

                            if (!levelIDs.empty()) {
                                  auto searchObject =
                                      GJSearchObject::create(SearchType::Type19, levelIDs);
                                  auto browserLayer = LevelBrowserLayer::create(searchObject);
                                  auto scene = CCScene::create();
                                  scene->addChild(browserLayer);
                                  auto transitionFade = CCTransitionFade::create(0.5f, scene);
                                  CCDirector::sharedDirector()->pushScene(transitionFade);
                            } else {
                                  log::warn("No levels found in response");
                                  Notification::create("No featured levels found",
                                                       NotificationIcon::Warning)
                                      ->show();
                            }
                      } else {
                            log::error("Failed to parse response JSON");
                      }
                } else {
                      log::error("Failed to fetch levels from server");
                      Notification::create("Failed to fetch levels from server",
                                           NotificationIcon::Error)
                          ->show();
                }
          });
}

void RLCreatorLayer::onNewRated(CCObject* sender) {
      web::WebRequest()
          .param("type", 3)
          .param("amount", 1000)
          .get("https://gdrate.arcticwoof.xyz/getLevels")
          .listen([this](web::WebResponse* res) {
                if (res && res->ok()) {
                      auto jsonResult = res->json();

                      if (jsonResult) {
                            auto json = jsonResult.unwrap();
                            std::string levelIDs;
                            bool first = true;

                            if (json.contains("levelIds")) {
                                  auto levelsArr = json["levelIds"];

                                  // iterate
                                  for (auto levelIDValue : levelsArr) {
                                        auto levelID = levelIDValue.as<int>();
                                        if (levelID) {
                                              if (!first)
                                                    levelIDs += ",";
                                              levelIDs += numToString(levelID.unwrap());
                                              first = false;
                                        }
                                  }
                            }

                            if (!levelIDs.empty()) {
                                  auto searchObject =
                                      GJSearchObject::create(SearchType::Type19, levelIDs);
                                  auto browserLayer = LevelBrowserLayer::create(searchObject);
                                  auto scene = CCScene::create();
                                  scene->addChild(browserLayer);
                                  auto transitionFade = CCTransitionFade::create(0.5f, scene);
                                  CCDirector::sharedDirector()->pushScene(transitionFade);
                            } else {
                                  log::warn("No levels found in response");
                                  Notification::create("No levels found",
                                                       NotificationIcon::Warning)
                                      ->show();
                            }
                      } else {
                            log::error("Failed to parse response JSON");
                      }
                } else {
                      log::error("Failed to fetch levels from server");
                      Notification::create("Failed to fetch levels from server",
                                           NotificationIcon::Error)
                          ->show();
                }
          });
}

void RLCreatorLayer::onSendLayouts(CCObject* sender) {
      web::WebRequest()
          .param("type", 1)
          .param("amount", 1000)
          .get("https://gdrate.arcticwoof.xyz/getLevels")
          .listen([this](web::WebResponse* res) {
                if (res && res->ok()) {
                      auto jsonResult = res->json();

                      if (jsonResult) {
                            auto json = jsonResult.unwrap();
                            std::string levelIDs;
                            bool first = true;
                            if (json.contains("levelIds")) {
                                  auto levelsArr = json["levelIds"];

                                  // iterate
                                  for (auto levelIDValue : levelsArr) {
                                        auto levelID = levelIDValue.as<int>();
                                        if (levelID) {
                                              if (!first)
                                                    levelIDs += ",";
                                              levelIDs += numToString(levelID.unwrap());
                                              first = false;
                                        }
                                  }
                            }

                            if (!levelIDs.empty()) {
                                  auto searchObject =
                                      GJSearchObject::create(SearchType::Type19, levelIDs);
                                  auto browserLayer = LevelBrowserLayer::create(searchObject);
                                  auto scene = CCScene::create();
                                  scene->addChild(browserLayer);
                                  auto transitionFade = CCTransitionFade::create(0.5f, scene);
                                  CCDirector::sharedDirector()->pushScene(transitionFade);
                            } else {
                                  log::warn("No levels found in response");
                                  Notification::create("No send layouts found",
                                                       NotificationIcon::Warning)
                                      ->show();
                            }
                      } else {
                            log::error("Failed to parse response JSON");
                      }
                } else {
                      log::error("Failed to fetch levels from server");
                      Notification::create("Failed to fetch levels from server",
                                           NotificationIcon::Error)
                          ->show();
                }
          });
}

void RLCreatorLayer::keyBackClicked() { this->onBackButton(nullptr); }

RLCreatorLayer* RLCreatorLayer::create() {
      auto ret = new RLCreatorLayer();
      if (ret && ret->init()) {
            ret->autorelease();
            return ret;
      }
      CC_SAFE_DELETE(ret);
      return nullptr;
}
