#pragma once
#include "Scene.hpp"


class MenuScene : public Scene
{
public:
	MenuScene(SceneManager* m) : Scene(m) { }

protected:
	virtual void init() override;
	virtual void finish() override;
	virtual void processEvent(const sf::Event & ev) override;
	virtual void update() override;
	virtual void render() override;
};