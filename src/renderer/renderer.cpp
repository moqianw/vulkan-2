#include "renderer.h"
#include "../context.h"
namespace easy3d {
    const std::array<glm::vec2, 3> vertices = {
        glm::vec2(0.0,-0.5),
        glm::vec2(0.5,0.5),
        glm::vec2(-0.5,0.5)
    };
    const std::array<Uniform<glm::vec3>,1> colors{ glm::vec3(1.0,1.0,0.0) };
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

        // �ȴ���һ֡���
        if (device.waitForFences(frame.inFlightFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to wait for fence.");
        }

        // ��ȡ��һ֡ͼ��
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

        // ����դ���Ա�����ʹ��
        device.resetFences(frame.inFlightFence);

        // ��ʼ���������¼
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);


        frame.cmdBuffer.begin(beginInfo);

        // ��Ⱦ����
        vk::RenderPassBeginInfo renderPassBegin{};
        renderPassBegin
            .setRenderPass(context.renderprocess->renderpass)
            .setFramebuffer(context.swapchain->framebuffers[imageIndex])
            .setRenderArea({ {0, 0}, context.swapchain->swapchaininfo.imageExtent })//��������
            .setClearValues(vk::ClearValue{}.setColor(
                vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f})));

        frame.cmdBuffer.beginRenderPass(renderPassBegin, vk::SubpassContents::eInline);
        //����Ⱦ����
        frame.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, context.renderprocess->pipeline);
        vk::DeviceSize offset = 0;
        frame.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.renderprocess->layout, 0, sets_[currentFrame_], {});
        frame.cmdBuffer.bindVertexBuffers(0, VertexBuffer.device->buffer, offset);
        frame.cmdBuffer.draw(3, 1, 0, 0);
        frame.cmdBuffer.endRenderPass();
        frame.cmdBuffer.end();

        // �ύ�������
        vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        vk::SubmitInfo submitInfo{};
        submitInfo
            .setWaitSemaphores(frame.imageAvailableSemaphore)
            .setWaitDstStageMask(waitStages)
            .setCommandBuffers(frame.cmdBuffer)
            .setSignalSemaphores(frame.renderFinishedSemaphore);
        context.queues->queues[0].submit(submitInfo, frame.inFlightFence);

        // ͼ�����
        vk::PresentInfoKHR presentInfo{};
        presentInfo
            .setWaitSemaphores(frame.renderFinishedSemaphore)
            .setSwapchains(context.swapchain->swapchain)
            .setImageIndices(imageIndex);

        if (context.queues->queues[1].presentKHR(presentInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present image.");
        }

        // �л�����һ֡
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
        void* ptr = device.mapMemory(VertexBuffer.host->memory, 0, VertexBuffer.host->size);
        memcpy(ptr, vertices.data(), sizeof(vertices));
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
        VertexBuffer.host.reset();
    }

    void Renderer::createUniformBuffer()
    {
        UniformBuffer.resize(2);
        for (auto& uniform : UniformBuffer) {
            uniform.host.reset(new Buffer(*Context::GetInstance().device
                , sizeof(vertices)
                , vk::BufferUsageFlagBits::eTransferSrc
                , vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            uniform.device.reset(new Buffer(*Context::GetInstance().device
                , sizeof(vertices)
                , vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst
                , vk::MemoryPropertyFlagBits::eDeviceLocal));
        }

    }

    void Renderer::bufferUniformData()
    {
        auto& device = Context::GetInstance().device->device;
        for (auto& uniform : UniformBuffer) {
            void* ptr = device.mapMemory(uniform.host->memory, 0, uniform.host->size);
            memcpy(ptr, (void*)colors.data(), sizeof(vertices));
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
            uniform.host.reset();
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
				{vk::DescriptorType::eUniformBuffer, 2}
			}));
        sets_ = desPool_->allocateDescriptorSet(2, Context::GetInstance().renderprocess->setlayouts);
    }

    void Renderer::updateSets()
    {
        for (int i = 0; i < sets_.size(); i++) {
            vk::DescriptorBufferInfo bufferinfo;
            bufferinfo.setBuffer(UniformBuffer[i].device->buffer)
                .setOffset(0)
                .setRange(UniformBuffer[i].device->size);
            vk::WriteDescriptorSet writer;
            writer.setBufferInfo(bufferinfo)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDstBinding(0)
                .setDstSet(sets_[i])
                .setDstArrayElement(0)
                .setDescriptorCount(1);
            Context::GetInstance().device->device.updateDescriptorSets(writer, {});
        }
    }

    void Renderer::initFrameResources() {
        auto& device = Context::GetInstance().device->device;
        for (auto& frame: frameResources_) {
            // �����������

            frame.cmdBuffer = cmdPool_->allocateCommandBuffer(1)[0];
            
            // �����ź���
            vk::SemaphoreCreateInfo semaphoreInfo;
            frame.imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
            frame.renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);

            // ����դ��
            vk::FenceCreateInfo fenceInfo;
            fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled); // ��ʼ����Ϊ signaled
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
