//-----------------------------------------------------------------------------------
//
// Boids Tower Defense (based on Paratrooper code)
// Copyright (C) 2007-2008  Sukender
// For more information, contact us : sukender@free.fr
//
// All rights reserved
//
//-----------------------------------------------------------------------------------

#include "Game.h"

#include <PVLE/Entity/GeomCollisionContainer.h>
#include <PVLE/Util/Math.h>
#include <PVLE/Util/Rand.h>
#include <PVLE/Util/I18N.h>
#include <PVLE/3D/Utility3D.h>
#include <PVLE/3D/CameraShake.h>

#ifdef PVLE_AUDIO
	#include <PVLE/Sound/AutoSoundUpdateCB.h>
	#include <osgAudio/SoundNode.h>
#endif

#include <boost/format.hpp>
#include <osg/StateSet>
#include <osg/MatrixTransform>
#include <osg/LightSource>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/Timer>
#include <osgViewer/Viewer>
#include <osg/Node>

#include <PVLE/Util/Math.h>
#include <PVLE/Physics/VisitorSmart.h>
#include <algorithm>
#include <functional>
//#include <boost/type_traits/add_reference.hpp>

#include <osg/TexEnvCombine>

//#ifdef _DEBUG
//const float DAY_DURATION = 60*.5f;
//#else
const float DAY_DURATION = 60*2;
//#endif

// TODO TD3


Game::Game(osg::Group * pParentGroup, Physics::Space * pParentSpace, Physics::World & phyWorld, bool cheatEnabled)
	: PVLEGame(pParentGroup, pParentSpace),
	phyWorld(phyWorld),
	lightSourceManager(NB_LIGHTS)
{
	std::fill(gameLightSources.begin(), gameLightSources.end(), nullptr);
	pParticlesManager = new osgParticle::AutoParticleSystemUpdater();
	pParentGroup->addChild(pParticlesManager);

	resetGame();
}

Game::~Game() {
	resetGame();		// Required to avoid triggering unecessary events upon destruction.
	clear();			// Delete all 3DPhys before Game is destroyed, so that references to Game are still valid (in callbacks, in LightSourceManager and its classes).
}

float Game::getLightTime() const { return fmodf(lightTime + .5f, 1.f); }

void Game::step(dReal stepSize) {
#ifdef PVLE_AUDIO
	cleanAbandonnedSoundGroup();
#endif

	// Change lights
	static const float dayTimeMultiplier = 1;
	lightTime = fmodf(lightTime + stepSize * dayTimeMultiplier / DAY_DURATION, 1.f);
	updateLights();

	PVLEGame::step(stepSize);
}

void Game::setGameOver() {
	if (state != STATE_GAME_OVER) {
		state = STATE_GAME_OVER;
		auto refGame = pGameCB.lock();
		if (refGame) refGame->gameOver(this);
	}
}

void Game::resetGame() {
	// Set stop
	setGameOver();
	state = STATE_STOP;

	// Reset lights
	lightTime = .9f;
	//dayTimeMultiplier = 1;
	updateLights();
}

void Game::startGame() {
	// Reset
	setScore(0);
	lightSourceManager.clear();

	if (state != STATE_RUN) {
		state = STATE_RUN;
		auto refGame = pGameCB.lock();
		if (refGame) refGame->gameStart(this);
	}
}

#ifdef PVLE_AUDIO
void Game::addSoundUpdateCB(osgAudio::SoundUpdateCB * pSoundCB) {
	ASSERT(pSoundCB);
	//ASSERT(!pSoundCB->getSoundState()->getLooping());		// If looping, the AutoSoundNodeCallback will not be able to delete it. That's not really an error, but looping sounds should be handeled differently.
	abandonnedSoundGroup.push_back(pSoundCB);
}

void Game::cleanAbandonnedSoundGroup() {
	for(TAbandonnedSoundGroup::iterator it=abandonnedSoundGroup.begin(); it!=abandonnedSoundGroup.end(); ) {
		if (!(*it)->getSoundState()->isPlaying()) {
			it = abandonnedSoundGroup.erase(it);		// Sukender: it SHOULD not be a problem during traversal, but...
		} else {
			++it;
		}
	}
}

#else
//void Game::addSoundUpdateCB(osgAudio::SoundUpdateCB * pSoundCB) {}
#endif

void Game::applyCameraShake(CameraShake * pCameraShake) {
	for(auto & pCamera : vCameras) {
		pCameraShake->addCamera(pCamera.get());
	}
}


float Game::computeDayLightIntensity() const {
    // Use osg::Timer to get the elapsed time
    static osg::ElapsedTime time;
    double elapsedTime = time.elapsedTime ();

    // TODO: Rest of the code for computeDayLightIntensity

    // Example: Use a sine function to simulate a day-night cycle
    float dayNightCycleDuration = 60.0f; // Adjust this value based on the desired cycle duration
    float timeOfDay = fmod(elapsedTime, dayNightCycleDuration) / dayNightCycleDuration;

    // Adjust the intensity based on the time of day (sine function for a smooth transition)
    float lightIntensity = 0.5f + 0.5f * std::sin(timeOfDay * 3.14159);

    return lightIntensity;
}

void Game::updateLights() {
	// TODO TD3
	// Day
	if (gameLightSources[LIGHT_DAY]) {
		// Update light
		osg::Light * currentLight = gameLightSources[LIGHT_DAY]->getLight();
		float lightIntensity = computeDayLightIntensity();

		double rotationAngle = osg::DegreesToRadians(0.2);
        osg::Matrix rotationMatrix;
        rotationMatrix.makeRotate(rotationAngle, osg::Vec3(0, 1, 0));

		// Get light position
		auto currentLightPosition = currentLight->getPosition();

		// Transform light position
		auto newLightPosition = currentLightPosition * rotationMatrix;

		currentLight->setPosition(newLightPosition);
		currentLight->setAmbient(osg::Vec4(lightIntensity, lightIntensity, lightIntensity, 0));
		currentLight->setDiffuse(osg::Vec4(lightIntensity, lightIntensity, lightIntensity, 1));
		currentLight->setSpecular(osg::Vec4(lightIntensity, lightIntensity, lightIntensity, 1));

	}
	// Night
	else
	{
		osg::Light * currentLight = gameLightSources[LIGHT_DAY]->getLight();
        osg::Vec4 nightGlobalAmbientColor(0.0, 0.0, 0.2, 1.0);

        // Create a MaterialStateAttribute to set ambient color during the night
        osg::ref_ptr<osg::Material> globalMaterial = new osg::Material;
        globalMaterial->setAmbient(osg::Material::FRONT_AND_BACK, nightGlobalAmbientColor);

        // Apply the MaterialStateAttribute to the entire scene
		currentLight->getOrCreateObserverSet()->setAttributeAndModes(globalMaterial.get(), osg::StateAttribute::ON);
    }
}



