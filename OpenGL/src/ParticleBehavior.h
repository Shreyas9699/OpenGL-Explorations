// ParticleBehavior.h
#pragma once
#include "ComputeShader.h"
#include <string>
#include <memory>

class ParticleBehavior 
{
public:
    virtual ~ParticleBehavior() = default;

    // Initialize any behavior-specific resources
    virtual void Initialize() {}

    // Clean up any behavior-specific resources
    virtual void Cleanup() {}

    // Update behavior-specific uniforms and parameters
    virtual void UpdateUniforms(ComputeShader& computeShader) = 0;
    virtual void UpdateEmitterUniforms(ComputeShader& emitterShader) = 0;

    // Get the path to the compute shader for this behavior
    virtual std::string GetComputeShaderPath() const = 0;

    // Get the path to the emitter shader for this behavior
    virtual std::string GetEmitterShaderPath() const = 0;

    // Optional: Override default particle initialization
    virtual void InitializeParticle(void* particleData, int index) {}

    // Optional: Additional behavior-specific buffers or resources
    virtual void SetupAdditionalResources() {}

    // Optional: Additional ImGUI options
    virtual void CustomGUI() {}
};