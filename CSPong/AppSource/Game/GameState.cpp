//
//  GameState.cpp
//  CSPong
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

#include <Game/GameState.h>

#include <Common/TransitionSystem.h>
#include <Game/GameEntityFactory.h>
#include <Game/Ball/BallControllerComponent.h>
#include <Game/Physics/PhysicsSystem.h>
#include <Game/Particles/ParticleEffectComponentFactory.h>

#include <CSProfiling/CSProfiling.h>

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/Delegate.h>
#include <ChilliSource/Core/Scene.h>
#include <ChilliSource/Rendering/Camera.h>
#include <ChilliSource/Rendering/Lighting.h>

namespace CSPong
{
    namespace
    {
        const bool k_bSaveShiny = false;
    }
    
    //------------------------------------------------------
    //------------------------------------------------------
    void GameState::CreateSystems()
    {
        m_transitionSystem = CreateSystem<TransitionSystem>(1.0f, 1.0f);
        m_physicsSystem = CreateSystem<PhysicsSystem>();
        m_scoringSystem = CreateSystem<ScoringSystem>();
        m_gameEntityFactory = CreateSystem<GameEntityFactory>(m_physicsSystem, m_scoringSystem);
        m_metricsSystem = CS::Application::Get()->GetSystem<CSProfiling::MetricsSystem>();
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    void GameState::OnInit()
    {   
        GetScene()->SetClearColour(CS::Colour::k_black);
        
        CS::EntitySPtr camera = m_gameEntityFactory->CreateCamera();
        GetScene()->Add(camera);

        CS::EntitySPtr ambientLight = m_gameEntityFactory->CreateAmbientLight();
        GetScene()->Add(ambientLight);
        
        CS::EntitySPtr diffuseLight = m_gameEntityFactory->CreateDiffuseLight();
        GetScene()->Add(diffuseLight);
        
        CS::EntitySPtr arena = m_gameEntityFactory->CreateArena();
        GetScene()->Add(arena);
        
        m_ball = m_gameEntityFactory->CreateBall();
        GetScene()->Add(m_ball);

        m_transitionInConnection = m_transitionSystem->GetTransitionInFinishedEvent().OpenConnection([=]()
        {
            m_ball->GetComponent<BallControllerComponent>()->Activate();
            m_metricsSystem->StartTimer();
        });
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    void GameState::OnDestroy()
    {
        
    }
}