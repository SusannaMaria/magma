#define MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(FeaturesType, StructureType)\
template<>\
inline const VkPhysicalDevice##FeaturesType *Device::getEnabledExtendedFeatures<VkPhysicalDevice##FeaturesType>() const noexcept\
{\
    return reinterpret_cast<const VkPhysicalDevice##FeaturesType *>(findExtendedFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_##StructureType));\
}

// Keep features in alphabetical order!
namespace magma
{
#ifdef VK_EXT_conditional_rendering
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(ConditionalRenderingFeaturesEXT, CONDITIONAL_RENDERING_FEATURES_EXT)
#endif
#ifdef VK_EXT_custom_border_color
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(CustomBorderColorFeaturesEXT, CUSTOM_BORDER_COLOR_FEATURES_EXT)
#endif
#ifdef VK_EXT_descriptor_indexing
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(DescriptorIndexingFeaturesEXT, DESCRIPTOR_INDEXING_FEATURES_EXT)
#endif
#ifdef VK_EXT_fragment_shader_interlock
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(FragmentShaderInterlockFeaturesEXT, FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT)
#endif
#ifdef VK_EXT_inline_uniform_block
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(InlineUniformBlockFeaturesEXT, INLINE_UNIFORM_BLOCK_FEATURES_EXT)
#endif
#ifdef VK_EXT_line_rasterization
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(LineRasterizationFeaturesEXT, LINE_RASTERIZATION_FEATURES_EXT)
#endif
#ifdef VK_EXT_primitive_topology_list_restart
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(PrimitiveTopologyListRestartFeaturesEXT, PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT)
#endif
#ifdef VK_EXT_subgroup_size_control
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(SubgroupSizeControlFeaturesEXT, SUBGROUP_SIZE_CONTROL_FEATURES_EXT)
#endif
#ifdef VK_EXT_transform_feedback
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(TransformFeedbackFeaturesEXT, TRANSFORM_FEEDBACK_FEATURES_EXT)
#endif
#ifdef VK_EXT_vertex_attribute_divisor
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(VertexAttributeDivisorFeaturesEXT, VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT)
#endif
#ifdef VK_KHR_8bit_storage
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(8BitStorageFeaturesKHR, 8BIT_STORAGE_FEATURES_KHR)
#endif
#ifdef VK_KHR_16bit_storage
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(16BitStorageFeaturesKHR, 16BIT_STORAGE_FEATURES_KHR)
#endif
#ifdef VK_KHR_buffer_device_address
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(BufferDeviceAddressFeaturesKHR, BUFFER_DEVICE_ADDRESS_FEATURES_KHR)
#endif
#ifdef VK_KHR_multiview
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(MultiviewFeaturesKHR, MULTIVIEW_FEATURES_KHR)
#endif
#ifdef VK_KHR_separate_depth_stencil_layouts
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(SeparateDepthStencilLayoutsFeaturesKHR, SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR)
#endif
#ifdef VK_KHR_shader_atomic_int64
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(ShaderAtomicInt64FeaturesKHR, SHADER_ATOMIC_INT64_FEATURES_KHR)
#endif
#ifdef VK_KHR_shader_clock
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(ShaderClockFeaturesKHR, SHADER_CLOCK_FEATURES_KHR)
#endif
#ifdef VK_KHR_shader_float16_int8
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(Float16Int8FeaturesKHR, FLOAT16_INT8_FEATURES_KHR)
#endif
#ifdef VK_KHR_variable_pointers
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(VariablePointerFeaturesKHR, VARIABLE_POINTER_FEATURES_KHR)
#endif
#ifdef VK_KHR_vulkan_memory_model
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(VulkanMemoryModelFeaturesKHR, VULKAN_MEMORY_MODEL_FEATURES_KHR)
#endif
#ifdef VK_NV_compute_shader_derivatives
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(ComputeShaderDerivativesFeaturesNV, COMPUTE_SHADER_DERIVATIVES_FEATURES_NV)
#endif
#ifdef VK_NV_cooperative_matrix
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(CooperativeMatrixFeaturesNV, COOPERATIVE_MATRIX_FEATURES_NV)
#endif
#ifdef VK_NV_mesh_shader
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(MeshShaderFeaturesNV, MESH_SHADER_FEATURES_NV)
#endif
#ifdef VK_NV_shading_rate_image
MAGMA_SPECIALIZE_DEVICE_GET_ENABLED_EXTENDED_FEATURES(ShadingRateImageFeaturesNV, SHADING_RATE_IMAGE_FEATURES_NV)
#endif
} // namespace magma
