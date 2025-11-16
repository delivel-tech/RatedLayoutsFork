#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(RLLevelSearchLayer, LevelSearchLayer)
{
    struct Fields
    {
        web::WebTask m_webTask;
    };

    bool init(int type)
    {
        if (!LevelSearchLayer::init(type))
        {
            return false;
        }

        auto quickSearchMenu = this->getChildByID("quick-search-menu");

        if (quickSearchMenu)
        {
            auto buttonSprite = SearchButton::create("GJ_longBtn04_001.png", "Send Layouts", 0.5f, "GJ_sFollowedIcon_001.png");
            auto tabButton = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(RLLevelSearchLayer::onSuggestedLevelsButton));
            quickSearchMenu->addChild(tabButton);

            auto ratedButtonSprite = SearchButton::create("GJ_longBtn04_001.png", "Rated Layouts", 0.5f, "GJ_sFollowedIcon_001.png");
            auto ratedTabButton = CCMenuItemSpriteExtra::create(
                ratedButtonSprite,
                this,
                menu_selector(RLLevelSearchLayer::onRatedLevelsButton));
            quickSearchMenu->addChild(ratedTabButton);

            quickSearchMenu->updateLayout();
        }
        return true;
    }


    // these were absolute trash to do, legit had to go find snippets from other repos
    // just to get this working lol
    // but i actually did it, so poggers
    void onSuggestedLevelsButton(CCObject *sender)
    {
        m_fields->m_webTask.cancel();

        web::WebRequest()
            .param("type", 1)
            .param("amount", 1000)
            .get("https://gdrate.arcticwoof.xyz/getLevels")
            .listen([this](web::WebResponse *res)
                    {
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
                                    if (!first) levelIDs += ",";
                                    levelIDs += numToString(levelID.unwrap());
                                    first = false;
                                }
                            }
                        }

                        if (!levelIDs.empty()) {
                            auto searchObject = GJSearchObject::create(SearchType::Type19, levelIDs);
                            auto browserLayer = LevelBrowserLayer::create(searchObject);
                            auto scene = CCScene::create();
                            scene->addChild(browserLayer);
                            auto transitionFade = CCTransitionFade::create(0.5f, scene);
                            CCDirector::sharedDirector()->pushScene(transitionFade);
                        } else {
                            log::warn("No levels found in response");
                        }
                    } else {
                        log::error("Failed to parse response JSON");
                    }
                } else {
                    log::error("Failed to fetch levels from server");
                } });
    }

    void onRatedLevelsButton(CCObject *sender)
    {
        m_fields->m_webTask.cancel();

        web::WebRequest()
            .param("type", 0)
            .param("amount", 1000)
            .get("https://gdrate.arcticwoof.xyz/getLevels")
            .listen([this](web::WebResponse *res)
                    {
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
                                    if (!first) levelIDs += ",";
                                    levelIDs += numToString(levelID.unwrap());
                                    first = false;
                                }
                            }
                        }

                        if (!levelIDs.empty()) {
                            auto searchObject = GJSearchObject::create(SearchType::Type19, levelIDs);
                            auto browserLayer = LevelBrowserLayer::create(searchObject);
                            auto scene = CCScene::create();
                            scene->addChild(browserLayer);
                            auto transitionFade = CCTransitionFade::create(0.5f, scene);
                            CCDirector::sharedDirector()->pushScene(transitionFade);
                        } else {
                            log::warn("No levels found in response");
                        }
                    } else {
                        log::error("Failed to parse response JSON");
                    }
                } else {
                    log::error("Failed to fetch levels from server");
                } });
    }
};