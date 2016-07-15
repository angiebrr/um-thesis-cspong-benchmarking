//
//  GameEntityFactory.cpp
//  CSPong
//  Created by Scott Downie on 02/07/2014.
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

#include <Game/GameEntityFactory.h>

#include <Game/ScoringSystem.h>
#include <Game/Ball/BallControllerComponent.h>
#include <Game/Ball/BallStaticMeshComponent.h>
#include <Game/Camera/CameraTiltComponent.h>
#include <Game/Physics/DynamicBodyComponent.h>
#include <Game/Physics/StaticBodyComponent.h>
#include <Game/Physics/TriggerComponent.h>
#include <Game/Particles/ParticleEffectComponentFactory.h>

#include <ChilliSource/Core/Base.h>
#include <ChilliSource/Core/Entity.h>
#include <ChilliSource/Core/Resource.h>
#include <ChilliSource/Rendering/Base.h>
#include <ChilliSource/Rendering/Camera.h>
#include <ChilliSource/Rendering/Lighting.h>
#include <ChilliSource/Rendering/Model.h>
#include <ChilliSource/Rendering/Texture.h>
#include <ChilliSource/Rendering/Particle.h>

namespace CSPong
{
    CS_DEFINE_NAMEDTYPE(GameEntityFactory);
    
    namespace
    {
        const f32 k_paddlePercentageOffsetFromCentre = 0.4f;
    }
    
    //---------------------------------------------------
    //---------------------------------------------------
    GameEntityFactoryUPtr GameEntityFactory::Create(PhysicsSystem* in_physicsSystem, ScoringSystem* in_scoringSystem)
    {
        return GameEntityFactoryUPtr(new GameEntityFactory(in_physicsSystem, in_scoringSystem));
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    GameEntityFactory::GameEntityFactory(PhysicsSystem* in_physicsSystem, ScoringSystem* in_scoringSystem)
    : m_physicsSystem(in_physicsSystem), m_scoringSystem(in_scoringSystem)
    {
        
    }
    //----------------------------------------------------------
    //----------------------------------------------------------
    bool GameEntityFactory::IsA(CS::InterfaceIDType in_interfaceId) const
    {
        return in_interfaceId == GameEntityFactory::InterfaceID;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntityUPtr GameEntityFactory::CreateCamera() const
    {   
        CS::EntityUPtr camera(CS::Entity::Create());
        
        CameraTiltComponentSPtr cameraTiltComponent(new CameraTiltComponent(CS::Vector3::k_unitPositiveZ));
        camera->AddComponent(cameraTiltComponent);
        
        const f32 k_fov = 3.14f / 3.0f;
        const f32 k_viewportHeight = 100.0f;
        f32 dist = (0.5f * k_viewportHeight) / std::tan(k_fov * 0.5f);
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        
        CS::CameraComponentSPtr cameraComponent = renderFactory->CreatePerspectiveCameraComponent(k_fov, 10.0f, 150.0f);
        camera->AddComponent(cameraComponent);
        camera->GetTransform().SetLookAt(CS::Vector3::k_unitNegativeZ * dist, CS::Vector3::k_zero, CS::Vector3::k_unitPositiveY);
        
        return camera;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntityUPtr GameEntityFactory::CreateDiffuseLight() const
    { 
        CS::EntityUPtr light(CS::Entity::Create());
        
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        
        CS::DirectionalLightComponentSPtr lightComponent = renderFactory->CreateDirectionalLightComponent(1024);
        lightComponent->SetIntensity(2.0f);
        lightComponent->SetColour(CS::Colour::k_orange);
        
        const f32 k_distanceFromGround = 70.0f;
        CS::Vector3 lightDir(-1, 1, 1);
        lightDir.Normalise();
        CS::Vector3 lightPos = -lightDir * k_distanceFromGround;
        
        lightComponent->SetShadowVolume(130.0f, 130.0f, 1.0f, 130.0f);
        lightComponent->SetShadowTolerance(0.05f);
        light->GetTransform().SetLookAt(lightPos, CS::Vector3::k_zero, CS::Vector3::k_unitPositiveY);
        light->AddComponent(lightComponent);
        
        return light;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntityUPtr GameEntityFactory::CreateAmbientLight() const
    {
        CS::EntityUPtr light(CS::Entity::Create());
        
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        
        CS::AmbientLightComponentSPtr lightComponent = renderFactory->CreateAmbientLightComponent();
        lightComponent->SetColour(CS::Colour::k_white * 0.5f);
        light->AddComponent(lightComponent);
        
        return light;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntitySPtr GameEntityFactory::CreateBall() const
    {   
        auto resourcePool = CS::Application::Get()->GetResourcePool();
        CS::MeshCSPtr mesh = resourcePool->LoadResource<CS::Mesh>(CS::StorageLocation::k_package, "Models/Ball.csmodel");
        CS::MaterialCSPtr material = resourcePool->LoadResource<CS::Material>(CS::StorageLocation::k_package, "Materials/Models/Models.csmaterial");
        
        CS::EntitySPtr ball(CS::Entity::Create());
        
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        BallStaticMeshComponentSPtr meshComponent(new BallStaticMeshComponent());
        meshComponent->AttachMesh(mesh, material);
        ball->AddComponent(meshComponent);
        
        CS::Vector2 collisionSize = mesh->GetAABB().GetSize().XY();
        DynamicBodyComponentSPtr dynamicBody(new DynamicBodyComponent(m_physicsSystem, collisionSize, 1.0f, 0.0f, 1.0f));
        ball->AddComponent(dynamicBody);
        
        BallControllerComponentSPtr ballControllerComponent(new BallControllerComponent(dynamicBody.get()));
        ball->AddComponent(ballControllerComponent);
        
        m_scoringSystem->AddBallBody(dynamicBody);

        auto particleECFSystem = CS::Application::Get()->GetSystem<ParticleEffectComponentFactory>();
        bool loopParticles = false;
        particleECFSystem->AddBallParticles(ball, loopParticles);
        
        return ball;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntityUPtr GameEntityFactory::CreateArena() const
    {
        CS::EntityUPtr arena(CS::Entity::Create());
        
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        auto resourcePool = CS::Application::Get()->GetResourcePool();
        
        CS::MeshCSPtr mesh = resourcePool->LoadResource<CS::Mesh>(CS::StorageLocation::k_package, "Models/Arena.csmodel");
        CS::MaterialCSPtr material = resourcePool->LoadResource<CS::Material>(CS::StorageLocation::k_package, "Materials/Models/Models.csmaterial");
        
        const f32 k_border = 1.0f;
        const CS::Vector2 k_arenaDimensions(mesh->GetAABB().GetSize().XY() * 0.95f);
        
        CS::StaticMeshComponentSPtr meshComponent = renderFactory->CreateStaticMeshComponent(mesh, material);
        meshComponent->SetShadowCastingEnabled(false);
        arena->AddComponent(meshComponent);
        
        CS::EntitySPtr bottomEdge(CS::Entity::Create());
        bottomEdge->GetTransform().SetPosition(CS::Vector3(0.0f, -k_arenaDimensions.y * 0.5f - k_border * 0.5f, 0.0f));
        StaticBodyComponentSPtr bottomEdgeStaticBody(new StaticBodyComponent(m_physicsSystem, CS::Vector2(k_arenaDimensions.x + k_border * 2.0f, k_border)));
        bottomEdge->AddComponent(bottomEdgeStaticBody);
        arena->AddEntity(bottomEdge);
        
        CS::EntitySPtr topEdge(CS::Entity::Create());
        topEdge->GetTransform().SetPosition(CS::Vector3(0.0f, k_arenaDimensions.y * 0.5f + k_border * 0.5f, 0.0f));
        StaticBodyComponentSPtr topEdgeStaticBody(new StaticBodyComponent(m_physicsSystem, CS::Vector2(k_arenaDimensions.x + k_border * 2.0f, k_border)));
        topEdge->AddComponent(topEdgeStaticBody);
        arena->AddEntity(topEdge);
        
        CS::EntitySPtr leftEdge(CS::Entity::Create());
        leftEdge->GetTransform().SetPosition(CS::Vector3(-k_arenaDimensions.x * 0.5f - k_border * 0.5f, 0.0f, 0.0f));
        StaticBodyComponentSPtr leftEdgeStaticBody(new StaticBodyComponent(m_physicsSystem, CS::Vector2(k_border, k_arenaDimensions.y + k_border * 2.0f)));
        TriggerComponentSPtr leftEdgeTrigger(new TriggerComponent(m_physicsSystem, CS::Vector2(k_border * 2.0f, k_arenaDimensions.y)));
        leftEdge->AddComponent(leftEdgeStaticBody);
        leftEdge->AddComponent(leftEdgeTrigger);
        arena->AddEntity(leftEdge);
        
        CS::EntitySPtr rightEdge(CS::Entity::Create());
        rightEdge->GetTransform().SetPosition(CS::Vector3(k_arenaDimensions.x * 0.5f + k_border * 0.5f, 0.0f, 0.0f));
        StaticBodyComponentSPtr rightEdgeStaticBody(new StaticBodyComponent(m_physicsSystem, CS::Vector2(k_border, k_arenaDimensions.y + k_border * 2.0f)));
        TriggerComponentSPtr rightEdgeTrigger(new TriggerComponent(m_physicsSystem, CS::Vector2(k_border * 2.0f, k_arenaDimensions.y)));
        rightEdge->AddComponent(rightEdgeStaticBody);
        rightEdge->AddComponent(rightEdgeTrigger);
        arena->AddEntity(rightEdge);
        
        m_scoringSystem->AddGoalTrigger(leftEdge, 1);
        m_scoringSystem->AddGoalTrigger(rightEdge, 0);
        
        return arena;
    }
    //------------------------------------------------------------
    //------------------------------------------------------------
    CS::EntityUPtr GameEntityFactory::CreateScoreSprite(const CS::Vector2& in_size, CS::AlignmentAnchor in_alignmentAnchor) const
    {
        auto renderFactory = CS::Application::Get()->GetSystem<CS::RenderComponentFactory>();
        auto resPool = CS::Application::Get()->GetResourcePool();
        auto digitAtlas = resPool->LoadResource<CS::TextureAtlas>(CS::StorageLocation::k_package, "TextureAtlases/Digits/Digits.csatlas");
        auto digitMaterial = resPool->LoadResource<CS::Material>(CS::StorageLocation::k_package, "Materials/Digits/Digits.csmaterial");
        CS::SpriteComponentSPtr spriteComponent = renderFactory->CreateSpriteComponent(in_size, digitAtlas, "0", digitMaterial, CS::SizePolicy::k_fitMaintainingAspect);
        spriteComponent->SetColour(0.5f, 0.5f, 0.5f, 0.5f);
        spriteComponent->SetOriginAlignment(in_alignmentAnchor);
        
        CS::EntityUPtr scoreEnt = CS::Entity::Create();
        scoreEnt->AddComponent(spriteComponent);
        return scoreEnt;
    }
}