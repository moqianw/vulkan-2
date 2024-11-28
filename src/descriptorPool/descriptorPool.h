#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>
#include <stdexcept>

namespace easy3d {

    class DescriptorPool final {
    public:
        DescriptorPool(vk::Device& device, const std::vector<std::pair<vk::DescriptorType, uint32_t>>& descriptorTypes);
        ~DescriptorPool();

        std::vector<vk::DescriptorSet> allocateDescriptorSet(uint32_t size, vk::DescriptorSetLayout layout);
        void freeDescriptorSet(const vk::DescriptorSet& set);
        void freeDescriptorSet(const std::vector<vk::DescriptorSet>& sets);
        void reset();

    private:
        vk::Device& device;
        vk::DescriptorPool descriptorPool_;

        void init_DescriptorPool(const std::vector<std::pair<vk::DescriptorType, uint32_t>>& descriptorTypes);
    };

    
} // namespace easy3d
