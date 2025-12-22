#include "RLAddDialogue.hpp"

using namespace geode::prelude;

RLAddDialogue* RLAddDialogue::create() {
      auto popup = new RLAddDialogue();
      if (popup && popup->initAnchored(400.f, 90.f, "GJ_square05.png")) {
            popup->autorelease();
            return popup;
      }
      CC_SAFE_DELETE(popup);
      return nullptr;
}

bool RLAddDialogue::setup() {
      setTitle("Add Custom Dialogue");

      m_dialogueInput = TextInput::create(360.f, "Dialogue...", "chatFont.fnt");
      m_mainLayer->addChild(m_dialogueInput);
      m_dialogueInput->setPosition({m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f - 5.f});

      // submit button
      auto submitSpr = ButtonSprite::create("Submit", "goldFont.fnt", "GJ_button_01.png");
      auto submitBtn = CCMenuItemSpriteExtra::create(submitSpr, this, menu_selector(RLAddDialogue::onSubmit));

      submitBtn->setPosition({m_mainLayer->getContentSize().width / 2.f, 0.f});
      m_buttonMenu->addChild(submitBtn);
      return true;
}

void RLAddDialogue::onSubmit(CCObject* sender) {
      if (!m_dialogueInput) return;
      std::string dialogueText = m_dialogueInput->getString();
      if (dialogueText.empty()) {
            Notification::create("Dialogue cannot be empty!", NotificationIcon::Error)->show();
            return;
      }
      // send to server
      matjson::Value body = matjson::Value::object();
      body["body"] = dialogueText;
      body["accountId"] = GJAccountManager::get()->m_accountID;
      body["argonToken"] = Mod::get()->getSavedValue<std::string>("argon_token");

      auto req = web::WebRequest();
      req.bodyJSON(body);
      Ref<RLAddDialogue> thisRef = this;
      req.post("https://gdrate.arcticwoof.xyz/setDialogue").listen([thisRef](web::WebResponse* res) {
            if (!res || !res->ok()) {
                  Notification::create("Failed to submit dialogue!", NotificationIcon::Error)->show();
                  return;
            }
            Notification::create("Dialogue submitted successfully!", NotificationIcon::Success)->show();
            if (thisRef) thisRef->removeFromParent();
      });
}