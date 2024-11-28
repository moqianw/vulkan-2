#include "descriptorPool.h"
namespace easy3d {
    // 实现

    DescriptorPool::DescriptorPool(vk::Device& device, const std::vector<std::pair<vk::DescriptorType, uint32_t>>& descriptorTypes)
        : device(device)
    {
        init_DescriptorPool(descriptorTypes);
    }

    DescriptorPool::~DescriptorPool()
    {
        device.destroyDescriptorPool(descriptorPool_);
    }

    void DescriptorPool::init_DescriptorPool(const std::vector<std::pair<vk::DescriptorType, uint32_t>>& descriptorTypes)
    {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        for (const auto& [type, count] : descriptorTypes) {
            poolSizes.push_back(vk::DescriptorPoolSize().setType(type).setDescriptorCount(count));
        }

        vk::DescriptorPoolCreateInfo createInfo;
        createInfo
            .setPoolSizes(poolSizes)
            .setMaxSets(poolSizes.size() * 10); // 假设每种类型最多分配10倍的描述符集

        try {
            descriptorPool_ = device.createDescriptorPool(createInfo);
        }
        catch (vk::SystemError& err) {
            throw std::runtime_error("Failed to create descriptor pool: " + std::string(err.what()));
        }
    }

    std::vector<vk::DescriptorSet> DescriptorPool::allocateDescriptorSet(uint32_t size, vk::DescriptorSetLayout layout)
    {
        std::vector<vk::DescriptorSetLayout> layouts(size, layout);
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo
            .setDescriptorPool(descriptorPool_)
            .setSetLayouts(layouts)
            .setDescriptorSetCount(size);

        try {
            return device.allocateDescriptorSets(allocInfo);
        }
        catch (vk::SystemError& err) {
            throw std::runtime_error("Failed to allocate descriptor sets: " + std::string(err.what()));
        }
    }

    void DescriptorPool::freeDescriptorSet(const vk::DescriptorSet& set)
    {
        device.freeDescriptorSets(descriptorPool_, { set });
    }

    void DescriptorPool::freeDescriptorSet(const std::vector<vk::DescriptorSet>& sets)
    {
        device.freeDescriptorSets(descriptorPool_, sets);
    }

    void DescriptorPool::reset()
    {
        try {
            device.resetDescriptorPool(descriptorPool_);
        }
        catch (vk::SystemError& err) {
            throw std::runtime_error("Failed to reset descriptor pool: " + std::string(err.what()));
        }
    }


}