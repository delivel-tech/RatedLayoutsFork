#pragma once

#include <Geode/Geode.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;

class RLUserControl : public geode::Popup<> {
     public:
      static RLUserControl* create();
      static RLUserControl* create(int accountId);

     private:
      int m_targetAccountId = 0;
      CCMenuItemToggler* m_excludedToggler = nullptr;
      CCMenuItemSpriteExtra* m_applyButton = nullptr;
      LoadingSpinner* m_applySpinner = nullptr;
      bool m_isInitializing = false;
      bool m_persistedExcluded = false;
      void onApplyChanges(CCObject* sender);
      void onToggleChanged(CCObject* sender);
      bool setup() override;
};
