#include <Geode/Geode.hpp>
#include <argon/argon.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "ModRatePopup.hpp"

using namespace geode::prelude;

class $modify(RLLevelInfoLayer, LevelInfoLayer)
{
    bool init(GJGameLevel *level, bool challenge)
    {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        auto leftMenu = this->getChildByID("left-side-menu");
        bool isPlatformer = this->m_level->isPlatformer();

        if (Mod::get()->getSavedValue<int>("role") == 1)
        {
            // add a mod button
            auto iconSprite = CCSprite::create("rlStarIconBig.png"_spr);
            CCSprite *buttonSprite = nullptr;

            if (isPlatformer)
            {
                buttonSprite = CCSpriteGrayscale::create("rlStarIconBig.png"_spr);
            }
            else
            {
                buttonSprite = CCSprite::create("rlStarIconBig.png"_spr);
            }

            auto modButtonSpr = CircleButtonSprite::create(
                buttonSprite,
                CircleBaseColor::Cyan,
                CircleBaseSize::Medium);

            auto modButtonItem = CCMenuItemSpriteExtra::create(
                modButtonSpr,
                this,
                menu_selector(RLLevelInfoLayer::onModButton));
            modButtonItem->setID("mod-button");

            leftMenu->addChild(modButtonItem);
        }
        else if (Mod::get()->getSavedValue<int>("role") == 2)
        {
            // add an admin button
            CCSprite *buttonSprite = nullptr;

            if (isPlatformer)
            {
                buttonSprite = CCSpriteGrayscale::create("rlStarIconBig.png"_spr);
            }
            else
            {
                buttonSprite = CCSprite::create("rlStarIconBig.png"_spr);
            }

            auto modButtonSpr = CircleButtonSprite::create(
                buttonSprite,
                CircleBaseColor::Blue,
                CircleBaseSize::Medium);

            auto modButtonItem = CCMenuItemSpriteExtra::create(
                modButtonSpr,
                this,
                menu_selector(RLLevelInfoLayer::onAdminButton));
            modButtonItem->setID("admin-button");

            leftMenu->addChild(modButtonItem);
        }

        leftMenu->updateLayout();

        return true;
    }

    void onModButton(CCObject *sender)
    {
        if (this->m_level->isPlatformer())
        {
            FLAlertLayer::create(
                "Action Unavailable",
                "You cannot perform this action on <cy>platformer levels</c>.",
                "OK")
                ->show();
            return;
        }
        requestStatus(GJAccountManager::get()->m_accountID);

        if (Mod::get()->getSavedValue<int>("role") == 1)
        {
            log::info("Mod button clicked!");
            auto popup = ModRatePopup::create("Mod: Rate Layout", this->m_level);
            popup->show();
        }
        else
        {
            Notification::create("You do not have the required role to perform this action", NotificationIcon::Error)->show();
        }
    }

    void onAdminButton(CCObject *sender)
    {
        if (this->m_level->isPlatformer())
        {
            FLAlertLayer::create(
                "Action Unavailable",
                "You cannot perform this action on <cy>platformer levels</c>.",
                "OK")
                ->show();
            return;
        }
        requestStatus(GJAccountManager::get()->m_accountID);

        if (Mod::get()->getSavedValue<int>("role") == 2)
        {
            log::info("Admin button clicked!");
            auto popup = ModRatePopup::create("Admin: Rate Layout", this->m_level);
            popup->show();
        }
        else
        {
            Notification::create("You do not have the required role to perform this action", NotificationIcon::Error)->show();
        }
    }

    void requestStatus(int accountId)
    {
        // argon my beloved <3
        std::string token;
        auto res = argon::startAuth([](Result<std::string> res)
                                    {
        if (!res) {
            log::warn("Auth failed: {}", res.unwrapErr());
            Notification::create(res.unwrapErr(), NotificationIcon::Error)->show();
        }
        auto token = std::move(res).unwrap();
        log::debug("token obtained: {}", token);
        Mod::get()->setSavedValue("argon_token", token); }, [](argon::AuthProgress progress)
                                    { log::debug("auth progress: {}", argon::authProgressToString(progress)); });
        if (!res)
        {
            log::warn("Failed to start auth attempt: {}", res.unwrapErr());
            Notification::create(res.unwrapErr(), NotificationIcon::Error)->show();
        }

        // json boody crap
        matjson::Value jsonBody = matjson::Value::object();
        jsonBody["argonToken"] = Mod::get()->getSavedValue<std::string>("argon_token");
        jsonBody["accountId"] = accountId;

        // verify the user's role
        auto postReq = web::WebRequest();
        postReq.bodyJSON(jsonBody);
        auto postTask = postReq.post("https://gdrate.arcticwoof.xyz/access");

        // handle the response
        postTask.listen([this](web::WebResponse *response)
                        {
            log::info("Received response from server");

            if (!response->ok())
            {
                log::warn("Server returned non-ok status: {}", response->code());
                return;
            }

            auto jsonRes = response->json();
            if (!jsonRes)
            {
                log::warn("Failed to parse JSON response");
                return;
            }

            auto json = jsonRes.unwrap();
            int role = json["role"].asInt().unwrapOrDefault();
            
            // role check lol
            if (role == 1) {
                log::info("User has mod role");
                Mod::get()->setSavedValue<int>("role", role);
            } else if (role == 2) {
                log::info("User has admin role. Enable featured layouts");
                Mod::get()->setSavedValue<int>("role", role);
            } else {
                Mod::get()->setSavedValue<int>("role", 0);
            } });
    }
};