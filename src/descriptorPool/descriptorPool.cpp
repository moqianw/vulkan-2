#include "descriptorPool.h"
#include <iostream>
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
        if (descriptorTypes.empty()) {
            throw std::invalid_argument("Descriptor types cannot be empty.");
        }

        std::vector<vk::DescriptorPoolSize> poolSizes;
        uint32_t totalDescriptorCount = 0;

        for (const auto& [type, count] : descriptorTypes) {
            poolSizes.push_back(vk::DescriptorPoolSize().setType(type).setDescriptorCount(count));
            totalDescriptorCount += count;
        }

        vk::DescriptorPoolCreateInfo createInfo;
        createInfo
            .setPoolSizes(poolSizes)
            .setMaxSets(totalDescriptorCount) // 使用动态计算的 maxSets
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet); // 允许手动释放

        try {
            descriptorPool_ = device.createDescriptorPool(createInfo);
            std::cout << "Descriptor pool created with " << poolSizes.size()
                << " types and max sets: " << createInfo.maxSets << std::endl;
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
            std::cout << ("Failed to allocate descriptor sets: " + std::string(err.what())) << std::endl;
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