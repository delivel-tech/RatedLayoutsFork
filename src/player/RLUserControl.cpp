#include "RLUserControl.hpp"

#include <Geode/Geode.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;

RLUserControl* RLUserControl::create() {
      auto ret = new RLUserControl();

      if (ret && ret->initAnchored(380.f, 240.f, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
      }

      CC_SAFE_DELETE(ret);
      return nullptr;
};

RLUserControl* RLUserControl::create(int accountId) {
      auto ret = new RLUserControl();
      ret->m_targetAccountId = accountId;

      if (ret && ret->initAnchored(380.f, 240.f, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
      }

      CC_SAFE_DELETE(ret);
      return nullptr;
};

bool RLUserControl::setup() {
      setTitle("Rated Layouts User Control");

      // Menu for our controls
      auto menu = CCMenu::create();
      menu->setPosition({0, 0});

      // excluded toggle
      auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
      auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
      if (onSprite && offSprite) {
            onSprite->setScale(.8f);
            offSprite->setScale(.8f);
            m_excludedToggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(RLUserControl::onToggleChanged));
            m_excludedToggler->setID("rl-excluded-toggle");
            m_excludedToggler->setSizeMult(1.5f);
            m_excludedToggler->setPosition({35, m_mainLayer->getContentSize().height / 2 + 70});
            menu->addChild(m_excludedToggler);

            auto label = CCLabelBMFont::create("Leaderboard Excluded", "bigFont.fnt");
            label->setAnchorPoint({0.f, .5f});
            label->setPosition({m_excludedToggler->getPositionX() + 20.f, m_excludedToggler->getPositionY()});
            label->setScale(.35f);
            m_mainLayer->addChild(label);
      }

      // Add an apply button to submit the final state
      auto applySprite = ButtonSprite::create("Apply", 1.f);
      m_applyButton = CCMenuItemSpriteExtra::create(applySprite, this, menu_selector(RLUserControl::onApplyChanges));
      m_applyButton->setPosition({m_mainLayer->getContentSize().width / 2, 0});
      // spinner
      m_applySpinner = LoadingSpinner::create(36.f);
      m_applySpinner->setPosition({m_applyButton->getPosition()});
      m_applySpinner->setVisible(false);
      menu->addChild(m_applySpinner);
      menu->addChild(m_applyButton);

      m_mainLayer->addChild(menu);

      // fetch profile data to determine initial excluded state
      if (m_targetAccountId > 0) {
            matjson::Value jsonBody = matjson::Value::object();
            jsonBody["argonToken"] = Mod::get()->getSavedValue<std::string>("argon_token");
            jsonBody["accountId"] = m_targetAccountId;

            auto postReq = web::WebRequest();
            postReq.bodyJSON(jsonBody);
            auto postTask = postReq.post("https://gdrate.arcticwoof.xyz/profile");

            Ref<RLUserControl> thisRef = this;
            postTask.listen([thisRef](web::WebResponse* response) {
                  if (!thisRef || !thisRef->m_mainLayer) return;

                  if (!response->ok()) {
                        log::warn("Profile fetch returned non-ok status: {}", response->code());
                        return;
                  }

                  auto jsonRes = response->json();
                  if (!jsonRes) {
                        log::warn("Failed to parse JSON response for profile");
                        return;
                  }

                  auto json = jsonRes.unwrap();
                  bool isExcluded = json["excluded"].asBool().unwrapOrDefault();
                  log::info("Profile excluded state for {}: {}", thisRef->m_targetAccountId, isExcluded);

                  if (thisRef && thisRef->m_excludedToggler) {
                        thisRef->m_isInitializing = true;
                        thisRef->m_excludedToggler->toggle(isExcluded);
                        thisRef->m_persistedExcluded = isExcluded;
                        thisRef->m_isInitializing = false;
                  }
            });
      }

      return true;
}

void RLUserControl::onToggleChanged(CCObject* sender) {
      // prevent handling toggles done by initializer
      if (m_isInitializing) return;

      auto toggler = static_cast<CCMenuItemToggler*>(sender);
      bool priorState = toggler && toggler->isToggled();
      bool newState = !priorState;
}

void RLUserControl::onApplyChanges(CCObject* sender) {
      bool desired = false;
      if (m_excludedToggler) desired |= m_excludedToggler->isToggled();
      if (desired == m_persistedExcluded) {
            Notification::create("No changes to apply", NotificationIcon::Info)->show();
            return;
      }

      // get argon token
      auto token = Mod::get()->getSavedValue<std::string>("argon_token");
      if (token.empty()) {
            Notification::create("Authentication token not found", NotificationIcon::Error)->show();
            return;
      }

      // prepare payload using desired value
      matjson::Value jsonBody = matjson::Value::object();
      jsonBody["accountId"] = GJAccountManager::get()->m_accountID;
      jsonBody["argonToken"] = token;
      jsonBody["targetAccountId"] = m_targetAccountId;
      jsonBody["exclude"] = desired ? 1 : 0;

      // disable UI while request is in-flight
      if (m_applyButton) m_applyButton->setEnabled(false);
      if (m_excludedToggler) m_excludedToggler->setEnabled(false);
      if (m_applySpinner) {
            m_applySpinner->setVisible(true);
            m_applyButton->setEnabled(false);
            m_applyButton->setVisible(false);
      }

      auto postReq = web::WebRequest();
      postReq.bodyJSON(jsonBody);
      log::info("Applying exclude for account {} to {}", m_targetAccountId, jsonBody["exclude"].asInt().unwrapOrDefault());
      auto postTask = postReq.post("https://gdrate.arcticwoof.xyz/setUser");

      Ref<RLUserControl> thisRef = this;
      postTask.listen([thisRef, jsonBody, desired](web::WebResponse* response) {
            if (!thisRef) return;

            // re-enable UI regardless
            if (thisRef->m_excludedToggler) thisRef->m_excludedToggler->setEnabled(true);
            if (thisRef->m_applyButton) thisRef->m_applyButton->setEnabled(true);
            if (thisRef->m_applySpinner) thisRef->m_applySpinner->setVisible(false);

            if (!response->ok()) {
                  log::warn("setUser returned non-ok status: {}", response->code());
                  Notification::create("Failed to update user", NotificationIcon::Error)->show();
                  // revert toggler to persisted state
                  if (thisRef->m_excludedToggler) {
                        thisRef->m_isInitializing = true;
                        thisRef->m_excludedToggler->toggle(thisRef->m_persistedExcluded);
                        thisRef->m_isInitializing = false;
                  }
                  if (thisRef->m_applyButton) thisRef->m_applyButton->setEnabled(false);
                  if (thisRef->m_applySpinner) thisRef->m_applySpinner->setVisible(false);
                  return;
            }

            auto jsonRes = response->json();
            if (!jsonRes) {
                  log::warn("Failed to parse setUser response");
                  Notification::create("Invalid server response", NotificationIcon::Error)->show();
                  if (thisRef->m_excludedToggler) {
                        thisRef->m_isInitializing = true;
                        thisRef->m_excludedToggler->toggle(thisRef->m_persistedExcluded);
                        thisRef->m_isInitializing = false;
                  }
                  if (thisRef->m_applyButton) thisRef->m_applyButton->setEnabled(false);
                  if (thisRef->m_applySpinner) thisRef->m_applySpinner->setVisible(false);
                  return;
            }

            auto json = jsonRes.unwrap();
            bool success = json["success"].asBool().unwrapOrDefault();
            if (success) {
                  thisRef->m_persistedExcluded = desired;
                  Notification::create("User has been updated!", NotificationIcon::Success)->show();
                  if (thisRef->m_applyButton) thisRef->m_applyButton->setEnabled(false);
                  thisRef->onClose(nullptr);
            } else {
                  Notification::create("Failed to update user", NotificationIcon::Error)->show();
                  // revert toggler to persisted state
                  if (thisRef->m_excludedToggler) {
                        thisRef->m_isInitializing = true;
                        thisRef->m_excludedToggler->toggle(thisRef->m_persistedExcluded);
                        thisRef->m_isInitializing = false;
                  }
            }
      });
}