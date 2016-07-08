//
//  ParticleEffectComponentFactory.cpp
//  CSPong
//  Created by Angela Gross on 12/01/2016.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2016 Tag Games Limited
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

#include <Game/Particles/ParticleEffectComponentFactory.h>

#include <ChilliSource/Rendering/Particle.h>

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/Rendering/Base.h>

#include <sstream>

namespace CSPong
{
    CS_DEFINE_NAMEDTYPE(ParticleEffectComponentFactory);

    //---------------------------------------------------
    //---------------------------------------------------
    ParticleEffectComponentFactoryUPtr ParticleEffectComponentFactory::Create()
    {
        return ParticleEffectComponentFactoryUPtr(new ParticleEffectComponentFactory());
    }
    //---------------------------------------------------
    //---------------------------------------------------
    void ParticleEffectComponentFactory::OnDestroy()
    {
        ReleaseCollisionConnections();
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    ParticleEffectComponentFactory::ParticleEffectComponentFactory()
    {

    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    bool ParticleEffectComponentFactory::IsA(CS::InterfaceIDType in_interfaceId) const
    {
        return in_interfaceId == ParticleEffectComponentFactory::InterfaceID;
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    void ParticleEffectComponentFactory::ReleaseCollisionConnections()
    {
        for (std::vector<CS::EventConnectionSPtr>::size_type i = 0; i != m_collisionConnections.size(); i++)
        {
            m_collisionConnections[i].reset();
        }

        m_collisionConnections.clear();
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    CS::ParticleEffectComponentSPtr ParticleEffectComponentFactory::CreateOnCollisionParticleEffectComponent(const ParticleType in_particleType, CS::IConnectableEvent<DynamicBodyComponent::CollisionDelegate>& in_collisionEvent)
    {
        CS::ParticleEffectComponentSPtr particleEffectComponent = CreateParticleEffectComponent(in_particleType, false);

        //The m_collisionConnections vector keeps the event connection in scope. In order to avoid having two copies of a shared pointer
        //and having the program hang on exit, the event connection is pushed directly onto the m_collisionConnections vector.
        m_collisionConnections.push_back
        (
            in_collisionEvent.OpenConnection([=](const CS::Vector2& in_collisionDirection, CS::Entity* in_collidedEntity)
            {
                if (particleEffectComponent != nullptr && !particleEffectComponent->IsPlaying())
                {
                    particleEffectComponent->Play();
                }
            })
        );

        return particleEffectComponent;
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    CS::ParticleEffectComponentUPtr ParticleEffectComponentFactory::CreateParticleEffectComponent(const ParticleType in_particleType, const bool in_looping) const
    {
        auto resourcePool = CS::Application::Get()->GetResourcePool();
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();

        CS::ParticleEffectCSPtr particleEffect = nullptr;
        switch (in_particleType)
        {
        case ParticleType::k_blueIceCreamBurst:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/BlueIceCreamBurst/Base.csparticle");
            break;
        case ParticleType::k_smokeStreamBase:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Base_Transparent_NoCulling.csparticle");
            break;
        case ParticleType::k_smokeStreamTimes10:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Times10_Transparent_NoCulling.csparticle");
            break;
        case ParticleType::k_smokeStreamTimes50:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Times50_Transparent_NoCulling.csparticle");
            break;
        case ParticleType::k_smokeStreamTimes100:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Times100_Transparent_NoCulling.csparticle");
            break;
        case ParticleType::k_smokeStreamTimes500:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Times500_Transparent_NoCulling.csparticle");
            break;
        case ParticleType::k_smokeStreamTimes1000:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Times1000_Transparent_NoCulling.csparticle");
            break;
        default:
            particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, "Particles/SmokeStream/Base.csparticle");
            break;
        }

        CS::ParticleEffectComponentUPtr particleComponent = renderFactory->CreateParticleEffectComponent(particleEffect);

        if (in_looping)
        {
            particleComponent->SetPlaybackType(CS::ParticleEffectComponent::PlaybackType::k_looping);
        }
        else
        {
            particleComponent->SetPlaybackType(CS::ParticleEffectComponent::PlaybackType::k_once);
        }

        return particleComponent;
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    CS::ParticleEffectComponentUPtr ParticleEffectComponentFactory::CreateParticleEffectComponent(const std::string in_particleFileName, const bool in_looping) const
    {
        auto resourcePool = CS::Application::Get()->GetResourcePool();
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        
        CS::ParticleEffectCSPtr particleEffect = resourcePool->LoadResource<CS::ParticleEffect>(CS::StorageLocation::k_package, in_particleFileName);
        CS::ParticleEffectComponentUPtr particleComponent = renderFactory->CreateParticleEffectComponent(particleEffect);
        
        if (in_looping)
        {
            particleComponent->SetPlaybackType(CS::ParticleEffectComponent::PlaybackType::k_looping);
        }
        else
        {
            particleComponent->SetPlaybackType(CS::ParticleEffectComponent::PlaybackType::k_once);
        }
        
        return particleComponent;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    void ParticleEffectComponentFactory::AddBallParticles(CS::EntitySPtr in_ballEntity)
    {
        if(m_ballParticleTypes.size() > 0)
        {
            for (std::vector<ParticleType>::size_type i = 0; i != m_ballParticleTypes.size(); i++)
            {
                in_ballEntity->AddComponent( CreateParticleEffectComponent(m_ballParticleTypes[i], true) );
            }
        }
        else if(m_ballParticleFileNames.size() > 0)
        {
            for(std::vector<std::string>::size_type i = 0; i != m_ballParticleFileNames.size(); i++)
            {
                in_ballEntity->AddComponent( CreateParticleEffectComponent(m_ballParticleFileNames[i], true) );
            }
        }
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    void ParticleEffectComponentFactory::AssignBallParticleTypes(std::initializer_list<ParticleType> in_particleTypes)
    {
        m_ballParticleTypes = in_particleTypes;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    void ParticleEffectComponentFactory::AssignBallParticleFileNames(std::initializer_list<std::string> in_particleFileNames)
    {
        m_ballParticleFileNames = in_particleFileNames;
    }
    
}