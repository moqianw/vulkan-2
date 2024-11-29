#include "renderer.h"
#include "../context.h"
namespace easy3d {
    const std::array<glm::vec2, 3> vertices = {
        glm::vec2(0.0,-0.5),
        glm::vec2(0.5,0.5),
        glm::vec2(-0.5,0.5)
    };
    const std::array<Uniform<glm::vec3>,1> colors{ glm::vec3(1.0,1.0,0.0) };
    std::array<Uniform<glm::mat4>, 3> matrixs{
        glm::mat4(1.0),
        glm::mat4(1.0),
        glm::mat4(1.0)
    };
    float r = 0.001;
	Renderer::Renderer()
	{
        try {
            initCommandPool();
            initFrameResources();
            createVertexBuffer();
            bufferVertexData();
            createUniformBuffer();
            bufferUniformData();
            initDescriptorPool();
            updateSets();
        }
        catch (const vk::SystemError& err) {
            std::cerr <<err.what() << std::endl;
        }

	}
	Renderer::~Renderer()
	{
        cleanupFrameResources();
	}
    void Renderer::Render() {
        auto& context = Context::GetInstance();
        auto& device = context.device->device;
        auto& frame = frameResources_[currentFrame_];

        // 等待上一帧完成
        if (device.waitForFences(frame.inFlightFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to wait for fence.");
        }

        // 获取下一帧图像
        auto result = device.acquireNextImageKHR(
            context.swapchain->swapchain,
            std::numeric_limits<uint64_t>::max(),
            frame.imageAvailableSemaphore,
            nullptr
        );

        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to acquire next image.");
        }
        uint32_t imageIndex = result.value;

        // 重置栅栏以便重新使用
        device.resetFences(frame.inFlightFence);

        // 开始命令缓冲区记录
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);


        frame.cmdBuffer.begin(beginInfo);

        // 渲染过程
        vk::RenderPassBeginInfo renderPassBegin{};
        renderPassBegin
            .setRenderPass(context.renderprocess->renderpass)
            .setFramebuffer(context.swapchain->framebuffers[imageIndex])
            .setRenderArea({ {0, 0}, context.swapchain->swapchaininfo.imageExtent })//绘制区域
            .setClearValues(vk::ClearValue{}.setColor(
                vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f})));

        frame.cmdBuffer.beginRenderPass(renderPassBegin, vk::SubpassContents::eInline);
        //绑定渲染管线
        frame.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, context.renderprocess->pipeline);
        vk::DeviceSize offset = 0;
        //数据更新
        matrixs[2].data = glm::rotate(matrixs[2].data, r, glm::vec3(0.0, 0.0, 1.0));

        bufferUniformData();


        frame.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.renderprocess->layout, 0, {sets_[currentFrame_] }, {});
        frame.cmdBuffer.bindVertexBuffers(0, VertexBuffer.device->buffer, offset);
        frame.cmdBuffer.draw(3, 1, 0, 0);
        frame.cmdBuffer.endRenderPass();
        frame.cmdBuffer.end();

        // 提交命令缓冲区
        vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        vk::SubmitInfo submitInfo{};
        submitInfo
            .setWaitSemaphores(frame.imageAvailableSemaphore)
            .setWaitDstStageMask(waitStages)
            .setCommandBuffers(frame.cmdBuffer)
            .setSignalSemaphores(frame.renderFinishedSemaphore);
        context.queues->queues[0].submit(submitInfo, frame.inFlightFence);

        // 图像呈现
        vk::PresentInfoKHR presentInfo{};
        presentInfo
            .setWaitSemaphores(frame.renderFinishedSemaphore)
            .setSwapchains(context.swapchain->swapchain)
            .setImageIndices(imageIndex);

        if (context.queues->queues[1].presentKHR(presentInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present image.");
        }

        // 切换到下一帧
        currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::createVertexBuffer()
    {
		VertexBuffer.host.reset(new Buffer(*Context::GetInstance().device
			, sizeof(vertices)
			, vk::BufferUsageFlagBits::eTransferSrc
			, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        VertexBuffer.device.reset(new Buffer(*Context::GetInstance().device
            , sizeof(vertices)
            , vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst
            , vk::MemoryPropertyFlagBits::eDeviceLocal));

    }

    void Renderer::bufferVertexData()
    {
        auto& device = Context::GetInstance().device->device;
        memcpy(VertexBuffer.host->map, vertices.data(), sizeof(vertices));
        //transformbuffer
        vk::CommandBuffer cmdBuffer_ = cmdPool_->allocateCommandBuffer(1)[0];
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuffer_.begin(begininfo);
        vk::BufferCopy region;
        region.setSize(VertexBuffer.device->size)
            .setDstOffset(0)
            .setSrcOffset(0);
        cmdBuffer_.copyBuffer(VertexBuffer.host->buffer, VertexBuffer.device->buffer,region);
        cmdBuffer_.end();
        vk::SubmitInfo submitinfo;
        submitinfo.setCommandBuffers(cmdBuffer_);
        Context::GetInstance().queues->queues[0].submit(submitinfo);
        device.waitIdle();
        cmdPool_->freeCommandBuffer(cmdBuffer_);

    }

    void Renderer::createUniformBuffer()
    {
        UniformBuffer1.resize(2);
        for (auto& uniform : UniformBuffer1) {
            uniform.host.reset(new Buffer(*Context::GetInstance().device
                , sizeof(vertices)
                , vk::BufferUsageFlagBits::eTransferSrc
                , vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            uniform.device.reset(new Buffer(*Context::GetInstance().device
                , sizeof(vertices)
                , vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst
                , vk::MemoryPropertyFlagBits::eDeviceLocal));
        }
        UniformBuffer2.resize(2);
        for (auto& uniform : UniformBuffer2) {
            uniform.host.reset(new Buffer(*Context::GetInstance().device
                , sizeof(matrixs)
                , vk::BufferUsageFlagBits::eTransferSrc
                , vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            uniform.device.reset(new Buffer(*Context::GetInstance().device
                , sizeof(matrixs)
                , vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst
                , vk::MemoryPropertyFlagBits::eDeviceLocal));
        }
    }

    void Renderer::bufferUniformData()
    {
        auto& device = Context::GetInstance().device->device;
        for (auto& uniform : UniformBuffer1) {
            memcpy(uniform.host->map, (void*)colors.data(), sizeof(colors));
            //transformbuffer
            vk::CommandBuffer cmdBuffer_ = cmdPool_->allocateCommandBuffer(1)[0];
            vk::CommandBufferBeginInfo begininfo;
            begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            cmdBuffer_.begin(begininfo);
            vk::BufferCopy region;
            region.setSize(uniform.device->size)
                .setDstOffset(0)
                .setSrcOffset(0);
            cmdBuffer_.copyBuffer(uniform.host->buffer, uniform.device->buffer, region);
            cmdBuffer_.end();
            vk::SubmitInfo submitinfo;
            submitinfo.setCommandBuffers(cmdBuffer_);
            Context::GetInstance().queues->queues[0].submit(submitinfo);
            device.waitIdle();
            cmdPool_->freeCommandBuffer(cmdBuffer_);

        }
        for (auto& uniform : UniformBuffer2) {
            memcpy(uniform.host->map, (void*)matrixs.data(), sizeof(matrixs));
            //transformbuffer
            vk::CommandBuffer cmdBuffer_ = cmdPool_->allocateCommandBuffer(1)[0];
            vk::CommandBufferBeginInfo begininfo;
            begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            cmdBuffer_.begin(begininfo);
            vk::BufferCopy region;
            region.setSize(uniform.device->size)
                .setDstOffset(0)
                .setSrcOffset(0);
            cmdBuffer_.copyBuffer(uniform.host->buffer, uniform.device->buffer, region);
            cmdBuffer_.end();
            vk::SubmitInfo submitinfo;
            submitinfo.setCommandBuffers(cmdBuffer_);
            Context::GetInstance().queues->queues[0].submit(submitinfo);
            device.waitIdle();
            cmdPool_->freeCommandBuffer(cmdBuffer_);

        }
    }

    void Renderer::initCommandPool()
    {
        cmdPool_.reset(new CommandPool(Context::GetInstance().device->device));
    }

    void Renderer::initDescriptorPool()
    {
		desPool_.reset(new DescriptorPool(Context::GetInstance().device->device,
			{
				{vk::DescriptorType::eUniformBuffer, 4}
			}));
        sets_ = desPool_->allocateDescriptorSet(2, Context::GetInstance().renderprocess->setlayouts);
    }

    void Renderer::updateSets()
    {
        for (int i = 0; i < sets_.size(); i++) {
            vk::DescriptorBufferInfo bufferinfo;
            bufferinfo.setBuffer(UniformBuffer1[i].device->buffer)
                .setOffset(0)
                .setRange(UniformBuffer1[i].device->size);
            vk::WriteDescriptorSet writer;
            writer.setBufferInfo(bufferinfo)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDstBinding(0)
                .setDstSet(sets_[i])
                .setDstArrayElement(0)
                .setDescriptorCount(1);
            Context::GetInstance().device->device.updateDescriptorSets(writer, {});
        }
        for (int i = 0; i < sets_.size(); i++) {
            vk::DescriptorBufferInfo bufferinfo;
            bufferinfo.setBuffer(UniformBuffer2[i].device->buffer)
                .setOffset(0)
                .setRange(UniformBuffer2[i].device->size);
            vk::WriteDescriptorSet writer;
            writer.setBufferInfo(bufferinfo)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDstBinding(1)
                .setDstSet(sets_[i])
                .setDstArrayElement(0)
                .setDescriptorCount(1);
            Context::GetInstance().device->device.updateDescriptorSets(writer, {});
        }
    }

    void Renderer::initFrameResources() {
        auto& device = Context::GetInstance().device->device;
        for (auto& frame: frameResources_) {
            // 分配命令缓冲区

            frame.cmdBuffer = cmdPool_->allocateCommandBuffer(1)[0];
            
            // 创建信号量
            vk::SemaphoreCreateInfo semaphoreInfo;
            frame.imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
            frame.renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);

            // 创建栅栏
            vk::FenceCreateInfo fenceInfo;
            fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled); // 初始设置为 signaled
            frame.inFlightFence = device.createFence(fenceInfo);
        }
    }
    void Renderer::cleanupFrameResources() {
        VertexBuffer.device.reset();
        VertexBuffer.host.reset();
        auto& device = Context::GetInstance().device->device;

        for (auto& frame : frameResources_) {
            device.destroyFence(frame.inFlightFence);
            device.destroySemaphore(frame.imageAvailableSemaphore);
            device.destroySemaphore(frame.renderFinishedSemaphore);
            cmdPool_->freeCommandBuffer(frame.cmdBuffer);
        }

        cmdPool_.reset();
    }

}
