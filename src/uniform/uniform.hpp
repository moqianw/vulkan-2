#pragma once
#include "vulkan/vulkan.hpp"

namespace easy3d {

    template<class T>
    class Uniform final {
    public:
        T data;

        static vk::DescriptorSetLayoutBinding getBinding(uint32_t binding, vk::ShaderStageFlags stageFlags = vk::ShaderStageFlagBits::eFragment);
    };

    template<class T>
    inline vk::DescriptorSetLayoutBinding Uniform<T>::getBinding(uint32_t binding, vk::ShaderStageFlags stageFlags) {
        vk::DescriptorSetLayoutBinding bindings;
        bindings.setBinding(binding)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setStageFlags(stageFlags)
            .setDescriptorCount(1);
        return bindings;
    }

    // 其他实现略
}
