// ParticleBehavior.h
#pragma once
#include "ComputeShader.h"
#include <string>
#include <memory>

class ParticleBehavior 
{
public:
    struct RenderState
    {
        GLenum blendSrc = GL_SRC_ALPHA;
        GLenum blendDst = GL_ONE_MINUS_SRC_ALPHA;
    };

    virtual ~ParticleBehavior() {}

    // Initialize any behavior-specific resources
    virtual void Initialize() {}

    // Clean up any behavior-specific resources
    virtual void Cleanup() {}

    // Called once during particle system setup
    virtual void CreateParticleBuffers(size_t maxParticles) = 0;

    // Called each frame before compute dispatch
    virtual void BindParticleBuffers(GLuint baseBindingPoint) = 0;

    // Called after CPU-side data initialization (optional)
    virtual void UpdateParticleBuffers() = 0;

    // Update behavior-specific uniforms and parameters
    virtual void UpdateUniforms(ComputeShader& computeShader) = 0;

	// Update emitter-specific uniforms and parameters
    virtual void UpdateEmitterUniforms(ComputeShader& emitterShader) = 0;

    // Get the path to the compute shader for this behavior
    virtual std::string GetComputeShaderPath() const = 0;

    // Get the path to the emitter shader for this behavior
    virtual std::string GetEmitterShaderPath() const = 0;

    // Optional: Override default particle initialization
    virtual void InitializeParticle(int index) = 0;

    // Optional: Additional behavior-specific buffers or resources
    virtual void SetupAdditionalResources() {}

    // Optional: Get render state for this behavior
    virtual RenderState GetRenderState() const { return {}; }

    // Optional: Additional ImGUI options
    virtual void CustomGUI() {}
};