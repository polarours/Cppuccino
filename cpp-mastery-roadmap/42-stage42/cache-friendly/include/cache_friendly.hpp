#pragma once

#include <vector>
#include <iostream>
#include <numeric>

namespace cache_friendly {

struct AoSParticle {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    float life;
};

struct SoAParticles {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;
    std::vector<float> life;
    std::size_t count = 0;

    void resize(std::size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
        mass.resize(n); life.resize(n);
        count = n;
    }

    void addParticle(float px, float py, float pz,
                     float pvx, float pvy, float pvz,
                     float pmass, float plife) {
        x.push_back(px); y.push_back(py); z.push_back(pz);
        vx.push_back(pvx); vy.push_back(pvy); vz.push_back(pvz);
        mass.push_back(pmass); life.push_back(plife);
        count++;
    }
};

class AoSParticleSystem {
public:
    void add(float x, float y, float z, float mass) {
        particles_.push_back({x, y, z, 0, 0, 0, mass, 1.0f});
    }

    void updatePositions(float dt) {
        for (auto& p : particles_) {
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.z += p.vz * dt;
        }
    }

    float totalMass() const {
        float sum = 0;
        for (const auto& p : particles_) sum += p.mass;
        return sum;
    }

    std::size_t size() const { return particles_.size(); }

private:
    std::vector<AoSParticle> particles_;
};

class SoAParticleSystem {
public:
    void add(float x, float y, float z, float mass) {
        particles_.addParticle(x, y, z, 0, 0, 0, mass, 1.0f);
    }

    void updatePositions(float dt) {
        for (std::size_t i = 0; i < particles_.count; ++i) {
            particles_.x[i] += particles_.vx[i] * dt;
            particles_.y[i] += particles_.vy[i] * dt;
            particles_.z[i] += particles_.vz[i] * dt;
        }
    }

    float totalMass() const {
        float sum = 0;
        for (std::size_t i = 0; i < particles_.count; ++i) {
            sum += particles_.mass[i];
        }
        return sum;
    }

    std::size_t size() const { return particles_.count; }

private:
    SoAParticles particles_;
};

} // namespace cache_friendly
