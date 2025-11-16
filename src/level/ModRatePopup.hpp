#pragma once

#include <Geode/Geode.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;

class ModRatePopup : public geode::Popup<std::string, GJGameLevel*>
{
public:
    static ModRatePopup *create(std::string title = "Rate Layout", GJGameLevel* level = nullptr);

private:
    std::string m_title;
    GJDifficultySprite* m_difficultySprite;
    bool m_isDemonMode;
    bool m_isFeatured;
    CCMenu* m_normalButtonsContainer;
    CCMenu* m_demonButtonsContainer;
    CCNode* m_difficultyContainer;
    int m_selectedRating;
    int m_levelId;
    bool setup(std::string title, GJGameLevel* level) override;
    void onSubmitButton(CCObject* sender);
    void onToggleFeatured(CCObject* sender);
    void onToggleDemon(CCObject* sender);
    void onRatingButton(CCObject* sender);
    void updateDifficultySprite(int rating);
};
