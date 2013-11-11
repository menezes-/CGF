/*
 *  PlayMapAI.h
 *  Testbed for AI experiments
 *
 *  Created by Marcelo Cohen on 05/11.
 *  Copyright 2011 PUCRS. All rights reserved.
 *
 */

#ifndef PLAY_MAPAI_H_
#define PLAY_MAPAI_H_

#include <list>
#include <string>
#include "GameState.h"
#include "Sprite.h"
#include "InputManager.h"
#include <MapLoader.h>

struct Kinematic
{
    sf::Vector3f pos;
    sf::Vector3f vel;
    sf::Vector3f heading;
    float maxForce;
    float maxSpeed;
    float maxTurnRate;
};

class PlayMapAI : public cgf::GameState
{
public:

    void init();
    void cleanup();

    void pause();
    void resume();

    void handleEvents(cgf::Game* game);
    void update(cgf::Game* game);
    void draw(cgf::Game* game);

    // Implement Singleton Pattern
    static PlayMapAI* instance()
    {
        return &m_PlayMapAI;
    }

protected:

    PlayMapAI() {}

private:

    static PlayMapAI m_PlayMapAI;

    int x, y;
    float cameraSpeed;
    float zvel;
    cgf::Sprite player;
    cgf::Sprite ghost;

    sf::RenderWindow* screen;

    std::list<sf::Vector3f> trail;
    bool showTrails;

    Kinematic playerK, enemyK;
    cgf::InputManager* im;
    sf::Font font;
    sf::Text text;
    tmx::MapLoader* map;
    bool firstTime;

    void checkCollision(cgf::Game* game);
    void checkCollision(cgf::Game* game, Kinematic& obj);
    void centerMapOnPlayer();

    enum {
         CHASE_BEHAVIOR=0, ARRIVE_BEHAVIOR, PURSUIT_BEHAVIOR, FLEE_BEHAVIOR, EVADE_BEHAVIOR
    };

    const static std::string modes[];

    int steerMode;
    sf::Vector3f chase(Kinematic& vehicle, sf::Vector3f& target); // ir diretamente ao jogador
    sf::Vector3f arrive(Kinematic& vehicle, sf::Vector3f& target, float decel=0.3); // ir diretamente ao jogador
    sf::Vector3f pursuit(Kinematic& vehicle, Kinematic& target); // perseguir o jogador, prevendo a posição futura
	sf::Vector3f flee(Kinematic& vehicle, sf::Vector3f& target, float panicDistance=100);  // fugir do jogador
    sf::Vector3f evade(Kinematic& vehicle, Kinematic& target);
};

#endif
