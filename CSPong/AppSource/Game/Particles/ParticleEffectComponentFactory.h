//
//  ParticleEffectComponentFactory.h
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

#ifndef _APPSOURCE_GAME_PARTICLES_PARTICLEEFFECTCOMPONENTFACTORY_H_
#define _APPSOURCE_GAME_PARTICLES_PARTICLEEFFECTCOMPONENTFACTORY_H_

#include <ForwardDeclarations.h>
#include <Game/Physics/DynamicBodyComponent.h>

#include <ChilliSource/Core/System.h>
#include <ChilliSource/Core/Event.h>

namespace CSPong
{
    //------------------------------------------------------------
    /// An enum that describes the different types of particle 
    /// effects that have been created for the game.
    ///
    /// @author Angela Gross
    //------------------------------------------------------------
    enum class ParticleType
    {
        k_blueIceCreamBurst,
        k_smokeStream,
        k_smokeStreamBase,
        k_smokeStreamTimes10,
        k_smokeStreamTimes50,
        k_smokeStreamTimes100,
        k_smokeStreamTimes500,
        k_smokeStreamTimes1000,
    };

    //------------------------------------------------------------
    /// Creates different particle effects- instant/looping or on
    /// collision. And, since it is an app system, these effects
    /// will be able to be chosen during other game states.
    ///
    /// @author Angela Gross
    //------------------------------------------------------------
    class ParticleEffectComponentFactory final : public CS::AppSystem
    {

    public:

        CS_DECLARE_NAMEDTYPE(ParticleEffectComponentFactory);

        //------------------------------------------------------------
        /// Creates a new instance of the system.
        ///
        /// @author Angela Gross
        ///
        /// @return The new instance.
        //------------------------------------------------------------
        static ParticleEffectComponentFactoryUPtr Create();
        //------------------------------------------------------------
        /// Essentially a destructor- will reset collision connections 
        /// and empty the member vector.
        ///
        /// @author Angela Gross
        //------------------------------------------------------------
        void OnDestroy() override;
        //------------------------------------------------------------
        /// @author Angela Gross
        ///
        /// @param Comparison Type
        ///
        /// @return Whether the class matches the comparison type
        //------------------------------------------------------------
        bool IsA(CS::InterfaceIDType in_interfaceId) const override;
        //------------------------------------------------------------
        /// Manually resets collision connection pointers and clears
        /// the collision connection vector.
        ///
        /// @author Angela Gross
        //------------------------------------------------------------
        void ReleaseCollisionConnections();
        //------------------------------------------------------------
        /// Creates a particle effect component based on the given
        /// particle type. If the given particle type is invalid, then 
        /// it creates a smoke stream.
        ///
        /// @author Angela Gross
        ///
        /// @param The type of particle effect to create.
        /// @param Whether or not the particle effect should loop.
        ///
        /// @return The new particle effect component. 
        //------------------------------------------------------------
        CS::ParticleEffectComponentUPtr CreateParticleEffectComponent(const ParticleType in_particleType, const bool in_looping) const;
        //------------------------------------------------------------
        /// Creates a particle effect component based on the given
        /// particle definition file
        ///
        /// @author Angela Gross
        ///
        /// @param The file name of the CS particle definition
        /// @param Whether or not the particle effect should loop.
        ///
        /// @return The new particle effect component.
        //------------------------------------------------------------
        CS::ParticleEffectComponentUPtr CreateParticleEffectComponent(const std::string in_particleFileName, const bool in_looping) const;
        //------------------------------------------------------------
        /// Creates a particle effect component that will play when
        /// the given collision event is triggered based on the given
        /// particle type. If the given particle type is invalid, then 
        /// it creates a smoke stream. (Note that this means the 
        /// particle effect will not loop and will only play on 
        /// collision. Also, a SHARED pointer is returned because
        /// of the delgate using the particle effect)
        ///
        /// @author Angela Gross
        ///
        /// @param The type of particle effect to create.
        /// @param The collision delegate event used to trigger the
        /// collision you want to play the particle effect during.
        ///
        /// @return The new particle effect component. 
        //------------------------------------------------------------
        CS::ParticleEffectComponentSPtr CreateOnCollisionParticleEffectComponent(const ParticleType in_particleType, CS::IConnectableEvent<DynamicBodyComponent::CollisionDelegate>& in_collisionEvent);
        //------------------------------------------------------------
        /// Creates and adds particle components to the ball entity
        /// according to what particle types are in 
        /// m_ballParticleTypes.
        ///
        /// @author Angela Gross
        ///
        /// @param The entity that represents the ball
        //------------------------------------------------------------
        void AddBallParticles(CS::EntitySPtr in_ballEntity);
        //------------------------------------------------------------
        /// Sets the ball particles
        ///
        /// @author Angela Gross
        ///
        /// @param in_particleTypes
        //------------------------------------------------------------
        void AssignBallParticleTypes(std::initializer_list<ParticleType> in_particleTypes);
        //------------------------------------------------------------
        /// Sets the ball particles
        ///
        /// @author Angela Gross
        ///
        /// @param in_particleFileName
        //------------------------------------------------------------
        void AssignBallParticleFileNames(std::initializer_list<std::string> in_particleFileNames);

    private:
        //----------------------------------------------------------
        /// Private constructor to enforce use of factory method.
        ///
        /// @author Angela Gross
        //----------------------------------------------------------
        ParticleEffectComponentFactory();

        std::vector<std::string> m_ballParticleFileNames;
        std::vector<ParticleType> m_ballParticleTypes;
        std::vector<CS::EventConnectionSPtr> m_collisionConnections;
    };
}

#endif