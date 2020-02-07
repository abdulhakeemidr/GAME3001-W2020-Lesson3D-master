#include "Game.h"
#include "Util.h"
// Core Libraries
#include <iostream>
#include <string>

#include <math.h>
#include <Windows.h>

// 3rd Party Libraries
#include <GLEW\glew.h>
//#include <GLUT/freeglut.h>
//#include <IL/il.h> // for ilInit()
//#include "IL\ilu.h"
#include <glm\vec3.hpp>
#include <glm\gtx\color_space.hpp>


#include "TextureManager.h"
#include "Game.h"

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

Game* Game::s_pInstance = 0;

glm::vec2 Game::getTargetPosition()
{
	return m_pTarget->getPosition();
}

SDL_Renderer * Game::getRenderer()
{
	return m_pRenderer;
}

glm::vec2 Game::getMousePosition()
{
	return m_mousePosition;
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::createGameObjects()
{

	m_pShip = new ship();

	m_pTarget = new Target();
}

bool Game::init(const char* title, int xpos, int ypos, int height, int width, bool fullscreen)
{
	int flags = 0;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		std::cout << "SDL Init success" << std::endl;

		// if succeeded create our window
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, height, width, flags);

		// if window creation successful create our renderer
		if (m_pWindow != 0)
		{
			std::cout << "window creation success" << std::endl;
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);

			if (m_pRenderer != 0) // render init success
			{
				std::cout << "renderer creation success" << std::endl;
				SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init failure" << std::endl;
				return false; // render int fail
			}

			//TheTextureManager::Instance()->load("../../Assets/textures/animate-alpha.png", "animate", m_pRenderer);
			createGameObjects();
		}
		else 
		{
			std::cout << "window init failure" << std::endl;
			return false; // window init fail
		}
	}
	else
	{
		std::cout << "SDL init failure" << std::endl;
		return false; //SDL could not intialize
	}

	std::cout << "init success" << std::endl;
	m_bRunning = true; // everything initialized successfully - start the main loop

	return true;
}

void Game::render()
{
	SDL_RenderClear(m_pRenderer); // clear the renderer to the draw colour

	m_pTarget->draw();
	m_pShip->draw();

	//Util::DrawRect(m_pShip->getPosition() - glm::vec2(m_pShip->getWidth() * 0.5, m_pShip->getHeight() * 0.5), m_pShip->getWidth(), m_pShip->getHeight());

	SDL_RenderPresent(m_pRenderer); // draw to the screen
}

void Game::update()
{
	
	m_pShip->update();

	m_pTarget->update();

}

void Game::clean()
{
	std::cout << "cleaning game" << std::endl;

	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

int Game::run()
{
	Uint32 frameStart, frameTime;


	// show console
	AllocConsole();
	freopen("CON", "w", stdout);

	TheGame::Instance()->init("Hello World", 100, 100, 800, 600, false);

	while (TheGame::Instance()->running())
	{
		frameStart = SDL_GetTicks();

		TheGame::Instance()->handleEvents();
		TheGame::Instance()->update();
		TheGame::Instance()->render();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	TheGame::Instance()->clean();

	return 0;
}

void Game::handleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_bRunning = false;
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					m_bRunning = false;
					break;
				case SDLK_w:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here
					m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, -1.0f)); 
					//Maintains x position (m_velocity = 0.0f), increases y velocity by -1.0f (negative y is up)
					break;
				case SDLK_s:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here
					m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 1.0f));
					break;
				
				case SDLK_a:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here by setting it as the targets' current position
					m_pTarget->setVelocity(glm::vec2(-1.0f, m_pTarget->getVelocity().y));
					break;
				case SDLK_d:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here
					m_pTarget->setVelocity(glm::vec2(1.0f, m_pTarget->getVelocity().y));
					//Maintains y position (m_velocity = 0), increases x velocity by 1.0f (positive x is left)
					break;
				case SDLK_0:
					m_pShip->setSteeringState(SteeringState::IDLE);
					break;
				case SDLK_1:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here
					m_pShip->setSteeringState(SteeringState::SEEK);
					break;
				case SDLK_2:
					m_pShip->setTargetPosition(m_pTarget->getPosition()); // The ships target position is set here
					m_pShip->setSteeringState(SteeringState::ARRIVE);
					break;
				case SDLK_3:
					m_pShip->setTargetPosition(m_pTarget->getPosition());
					m_pShip->setSteeringState(SteeringState::AVOID);
					break;
				case SDLK_4:
					m_pShip->setSteeringState(SteeringState::FLEE);
					break;
				case SDLK_RIGHT:
					m_pShip->turnRight();
					break;
				case SDLK_LEFT:
					m_pShip->turnLeft();
					break;
				case SDLK_UP:
					m_pShip->setVelocity(m_pShip->getCurrentDirection());
					m_pShip->move();
					break;

			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
				case SDLK_w:
					if (m_pTarget->getVelocity().y < 0.0f) {
						m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 0.0f)); // Maintains x position, returns y velocity to zero (float)
					}
					break;
				
				case SDLK_s:
					if (m_pTarget->getVelocity().y > 0.0f) {
						m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 0.0f));
					}
					break;
				
				case SDLK_a:
					if (m_pTarget->getVelocity().x < 0.0f) {
						m_pTarget->setVelocity(glm::vec2(0.0f, m_pTarget->getVelocity().y));
					}
					break;
				case SDLK_d:
					if (m_pTarget->getVelocity().x > 0.0f) {
						m_pTarget->setVelocity(glm::vec2(0.0f, m_pTarget->getVelocity().y));
					}
					break;
				case SDLK_RIGHT:
					
					break;
				case SDLK_LEFT:
					
					break;
				case SDLK_UP:
					
					break;
				
			}
			
		default:
			break;
		}
	}
}