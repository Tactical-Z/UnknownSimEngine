#pragma once

#include <random>
#include <numbers>
#include "glm/glm.hpp"

namespace Math
{
    namespace Constants{
        inline constexpr glm::vec3 WorldUP = glm::vec3(0.0f, 0.0f, 1.0f);
        inline constexpr double Pi = std::numbers::pi;
        inline constexpr double TwoPi = Pi * 2.0;

        inline constexpr float LightSpeed = 1.0f;
        inline constexpr float GravitationalConstant = 1.0f;
        inline constexpr float SchwarzschildTime = 1.0f;
    }

    namespace RandomNumber{
        inline std::random_device randomDevice;
        inline std::mt19937 gen;

        inline glm::vec3 RandomVecByBounds(const glm::vec3& _min, const glm::vec3& _max){
            std::uniform_real_distribution<float> distribX(_min.x, _max.x);
            std::uniform_real_distribution<float> distribY(_min.y, _max.y);
            std::uniform_real_distribution<float> distribZ(_min.z, _max.z);

            return glm::vec3(distribX(gen), distribY(gen), distribZ(gen));
        }
        
        inline float RandomFloatByBounds(const float& _min, const float& _max){
            std::uniform_real_distribution<float> distrib(_min, _max);
            return distrib(gen);
        }

        inline float RandomFloatByBounds(const glm::vec2& _bounds){
            std::uniform_real_distribution<float> distrib(_bounds.x, _bounds.y);
            return distrib(gen);
        }

    }

    namespace Physics{
        // Schwarzschild radius equiation reordered for mass
        inline float MassFromSchwarzschildRadius(const float& _sRadius)
        {
            return ((_sRadius * (Math::Constants::LightSpeed * Math::Constants::LightSpeed)) / (2 * Math::Constants::GravitationalConstant));
        }

        // Radius in Schwarzschild as unit, return in fraction of speed of light.
        inline float OrbitalVelocity(const float& _orbitRadius, const float& _orbitBodyMass)
        {
            return sqrt((Math::Constants::GravitationalConstant * _orbitBodyMass) / _orbitRadius);
        }

        // Return in Schwarzschild radius unit
        inline float RadiusBetweenTwoPoints(const glm::vec3& _p1, const glm::vec3& _p2)
        {
            return glm::distance(_p1, _p2);
        }
    }

    namespace Util{

        inline float fmax(const float& _f1, const float& _f2){
            return _f1 >= _f2 ? _f1 : _f2;
        }

        inline float fmin(const float& _f1, const float& _f2){
            return _f1 <= _f2 ? _f1 : _f2;
        }

        inline void farraymin(float& _val, const float* _farray, const int& _arrayNum){
            for(int i = 0; i < _arrayNum; ++i)
                _val = Math::Util::fmin(_val, _farray[i]);
        }

        inline void farraymax(float& _val, const float* _farray, const int& _arrayNum){
            for(int i = 0; i < _arrayNum; ++i)
                _val = Math::Util::fmax(_val, _farray[i]);
        }

        inline void farrayminmax(float& _valmin, float& _valmax, const float* _farray, const int& _arrayNum){
            for(int i = 0; i < _arrayNum; ++i){
                _valmin = Math::Util::fmin(_valmin, _farray[i]);
                _valmax = Math::Util::fmax(_valmax, _farray[i]);
            }
        }
    }
    

};