//
//  CSPong.cpp
//  CCSPong
//  Created by Scott Downie on 30/06/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <CSPong.h>

#include <Game/GameState.h>
#include <Game/Particles/ParticleEffectComponentFactory.h>
#include <CSProfiling/CSProfiling.h>

#include <ChilliSource/Core/String/StringUtils.h>
#include <ChilliSource/Core/Container/ParamDictionary.h>
#include <ChilliSource/Input/Accelerometer.h>
#include <ChilliSource/Rendering/Model.h>

//---------------------------------------------------------
/// Implements the body of the CreateApplication method
/// which creates the derviced CSPong application
///
/// @author S Downie
///
/// @return Instance of CS::Application
//---------------------------------------------------------
CS::Application* CreateApplication()
{
    return new CSPong::App();
}

#if defined(CS_TARGETPLATFORM_ANDROID) && defined(CS_ANDROIDFLAVOUR_GOOGLEPLAY)

std::string GetGooglePlayLvlPublicKey()
{
    //Enter your Google Play LVL public key here if you are building for Google Play on Android
    return "";
}

#endif

namespace CSPong
{
    namespace
    {
        const std::string k_numParticlesVarName = "numParticles";
        const std::string k_generatedParticleFileName = "Particles/SmokeStream/Generated/[var=numParticles]_particles_emitted.csparticle";
        const u32 maxRunNum = 5;
        const u32 runTime = 5; // seconds
        const u32 k_minParticles = 0;
        const u32 k_maxParticles = 10000;
        const u32 k_particlesStep = 1000;
    }
    
    //---------------------------------------------------------
    //---------------------------------------------------------
    void App::CreateSystems()
    {
        CreateSystem<CS::CSModelProvider>();
        CreateSystem<CS::CSAnimProvider>();
        CreateSystem<CS::Accelerometer>();
		CreateSystem<ParticleEffectComponentFactory>();
		CSProfiling::MetricsSystem* metricsSystem = CreateSystem<CSProfiling::MetricsSystem>(maxRunNum, runTime, k_minParticles, k_maxParticles, k_particlesStep);

        
        // build path based on the first number of particles emitted, the min
        std::string particlePath = CS::StringUtils::InsertVariables
        (
             k_generatedParticleFileName,
             {
                 std::make_pair(k_numParticlesVarName, TO_STRING(k_minParticles))
             }
        );
        
		// start off with the first particle type
        GetSystem<ParticleEffectComponentFactory>()->AssignBallParticleFileNames({particlePath});

		// reset the game state and re-run the test
		m_metricsTimerStoppedConnection = metricsSystem->GetTimerStoppedEvent().OpenConnection([=]()
		{
			// go to the next run within a particle
			if (!metricsSystem->AreRunsOver())
			{
				GetStateManager()->Change(CS::StateSPtr(new GameState()));
			}
			else
			{
				// if we have gone through all runs for all particles, then quit
				if (metricsSystem->AreAllRunsOver())
				{
					CS::Application::Get()->Quit();
					exit(1);
				}
				// if we haven't, then increment the particles emitted and go to the first run
				else
				{
					u32 currentParticles = metricsSystem->IncrementParticles();
                    
                    // build path based on current particles emitted
                    std::string particlePath = CS::StringUtils::InsertVariables
                    (
                        k_generatedParticleFileName,
                        {
                            std::make_pair(k_numParticlesVarName, TO_STRING(currentParticles))
                        }
                    );
                    
					CS::Application::Get()->GetSystem<ParticleEffectComponentFactory>()->AssignBallParticleFileNames({particlePath});
					GetStateManager()->Change(CS::StateSPtr(new GameState()));
				}
			}
		});
    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    void App::OnInit()
    {

    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    void App::PushInitialState()
    {
        GetStateManager()->Push(CS::StateSPtr(new GameState()));
    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    void App::OnDestroy()
    {
		m_metricsTimerStoppedConnection->Close();
    }
}

