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
        const std::string k_generatedFileName = "Particles/SmokeStream/Generated/[var=numParticles]_particles.csparticle";
    }
    
    //---------------------------------------------------------
    //---------------------------------------------------------
    void App::CreateSystems()
    {
        // member variables that need to accessible by the GameEntityFactory
        m_areParticlesLooping = false;
        m_numParticleEffects = 10;

        CSProfiling::MetricsSystem::ArgData metricsArgData;
        // misc particle effect information
        metricsArgData.m_areParticlesLooping = m_areParticlesLooping;
        metricsArgData.m_numParticleEffects = m_numParticleEffects;
        // changing and constant values
        metricsArgData.m_isTMPChanging = true;
        metricsArgData.m_isPPEChanging = true;
        metricsArgData.m_tmpParticles = 0; // this will only be used if m_isTMPChanging == false
        metricsArgData.m_ppeParticles = 0; // this will only be used if m_isPPEChanging == false
        // only changing variables will use min, max, and step
        metricsArgData.m_minParticles = 0; 
        metricsArgData.m_maxParticles = 10000;
        metricsArgData.m_particlesStep = 500;
        metricsArgData.m_ppeStep = 0.0f; // % of m_particlesStep from 0 to 1 if  m_isPPEChanging == true
        metricsArgData.m_tmpStep = 1.0f; // % of m_particlesStep from 0 to 1 if  m_isTMPChanging == true
        // number of runs per step and how long each run is
        metricsArgData.m_maxRunNum = 5;
        metricsArgData.m_runTime = 5; //seconds

        CreateSystem<CS::CSModelProvider>();
        CreateSystem<CS::CSAnimProvider>();
        CreateSystem<CS::Accelerometer>();
        CreateSystem<ParticleEffectComponentFactory>();
        CSProfiling::MetricsSystem* metricsSystem = CreateSystem<CSProfiling::MetricsSystem>(metricsArgData);

        // build path based on the first number of particles emitted (i.e. min)
        std::string particlePath = CS::StringUtils::InsertVariables
        (
             k_generatedFileName,
             {
                 std::make_pair(k_numParticlesVarName, TO_STRING(metricsArgData.m_minParticles))
             }
        );
        
        // start off with the first particle type
        m_currentParticleFileName = particlePath;

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
                        k_generatedFileName,
                        {
                            std::make_pair(k_numParticlesVarName, TO_STRING(currentParticles))
                        }
                    );
                    
                    m_currentParticleFileName = particlePath;
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
    //---------------------------------------------------------
    //---------------------------------------------------------
    u32 App::GetNumParticleEffects() const
    {
        return m_numParticleEffects;
    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    bool App::AreParticlesLooping() const
    {
        return m_areParticlesLooping;
    }
    //---------------------------------------------------------
    //---------------------------------------------------------
    std::string App::GetCurrentParticleFileName() const
    {
        return m_currentParticleFileName;
    }
}

