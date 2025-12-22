#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RLCommunityVotePopup : public geode::Popup<> {
     public:
      static RLCommunityVotePopup* create();
      static RLCommunityVotePopup* create(int levelId);

     private:
      bool setup() override;

      // handlers
      void onSubmit(CCObject*);
      void onInfo(CCObject*);

      int m_levelId = 0;
      int m_designVote = 0;
      int m_difficultyVote = 0;
      int m_gameplayVote = 0;

      CCLabelBMFont* m_designScoreLabel = nullptr;
      CCLabelBMFont* m_difficultyScoreLabel = nullptr;
      CCLabelBMFont* m_gameplayScoreLabel = nullptr;
      CCLabelBMFont* m_modDifficultyLabel = nullptr;

      geode::TextInput* m_designInput = nullptr;
      geode::TextInput* m_difficultyInput = nullptr;
      geode::TextInput* m_gameplayInput = nullptr;

      CCMenuItemSpriteExtra* m_submitBtn = nullptr;
      void refreshFromServer();
};