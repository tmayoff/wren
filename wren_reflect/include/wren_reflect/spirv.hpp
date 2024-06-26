#pragma once

#include <spirv/1.2/spirv.hpp>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren::reflect {

using spirv_t = std::vector<uint32_t>;

inline auto get_vk_shader_stage(
    spv::ExecutionModel const &execution_model)
    -> tl::expected<VK_NS::ShaderStageFlagBits, std::error_code> {
  switch (execution_model) {
    case spv::ExecutionModelVertex:
      return VK_NS::ShaderStageFlagBits::eVertex;
    case spv::ExecutionModelTessellationControl:
      return VK_NS::ShaderStageFlagBits::eTessellationControl;
    case spv::ExecutionModelTessellationEvaluation:
      return VK_NS::ShaderStageFlagBits::eTessellationEvaluation;
    case spv::ExecutionModelGeometry:
      return VK_NS::ShaderStageFlagBits::eGeometry;
    case spv::ExecutionModelFragment:
      return VK_NS::ShaderStageFlagBits::eFragment;
    case spv::ExecutionModelGLCompute:
      return VK_NS::ShaderStageFlagBits::eCompute;
    default:
      return tl::make_unexpected(
          std::make_error_code(std::errc::invalid_argument));
  }
}

}  // namespace wren::reflect

namespace spv {

auto to_string(spv::StorageClass storage_class) {
  switch (storage_class) {
    case StorageClassUniformConstant:
    case StorageClassInput:
      return "input";
    case StorageClassUniform:
    case StorageClassOutput:
      return "output";
    case StorageClassWorkgroup:
    case StorageClassCrossWorkgroup:
    case StorageClassPrivate:
    case StorageClassFunction:
    case StorageClassGeneric:
    case StorageClassPushConstant:
    case StorageClassAtomicCounter:
    case StorageClassImage:
    case StorageClassStorageBuffer:
    case StorageClassMax:
      break;
  }
  return "";
}

auto to_string(spv::Op op) {
  switch (op) {
    case spv::OpNop:
    case spv::OpUndef:
    case spv::OpSourceContinued:
    case spv::OpSource:
    case spv::OpSourceExtension:
    case spv::OpName:
    case spv::OpMemberName:
    case spv::OpString:
    case spv::OpLine:
    case spv::OpExtension:
    case spv::OpExtInstImport:
    case spv::OpExtInst:
    case spv::OpMemoryModel:
    case spv::OpEntryPoint:
      return "OpEntryPoint";
    case spv::OpExecutionMode:
    case spv::OpCapability:
    case spv::OpTypeVoid:
      return "OpTypeVoid";
    case spv::OpTypeBool:
      return "OpTypeBool";
    case spv::OpTypeInt:
      return "OpTypeInt";
    case spv::OpTypeFloat:
      return "OpTypeFloat";
    case spv::OpTypeVector:
      return "OpTypeVector";
    case spv::OpTypeMatrix:
    case spv::OpTypeImage:
    case spv::OpTypeSampler:
    case spv::OpTypeSampledImage:
    case spv::OpTypeArray:
    case spv::OpTypeRuntimeArray:
    case spv::OpTypeStruct:
    case spv::OpTypeOpaque:
    case spv::OpTypePointer:
      return "OpTypePointer";
    case spv::OpTypeFunction:
    case spv::OpTypeEvent:
    case spv::OpTypeDeviceEvent:
    case spv::OpTypeReserveId:
    case spv::OpTypeQueue:
    case spv::OpTypePipe:
    case spv::OpTypeForwardPointer:
    case spv::OpConstantTrue:
    case spv::OpConstantFalse:
    case spv::OpConstant:
    case spv::OpConstantComposite:
    case spv::OpConstantSampler:
    case spv::OpConstantNull:
    case spv::OpSpecConstantTrue:
    case spv::OpSpecConstantFalse:
    case spv::OpSpecConstant:
    case spv::OpSpecConstantComposite:
    case spv::OpSpecConstantOp:
    case spv::OpFunction:
    case spv::OpFunctionParameter:
    case spv::OpFunctionEnd:
    case spv::OpFunctionCall:
    case spv::OpVariable:
      return "OpVariable";
    case spv::OpImageTexelPointer:
    case spv::OpLoad:
    case spv::OpStore:
    case spv::OpCopyMemory:
    case spv::OpCopyMemorySized:
    case spv::OpAccessChain:
    case spv::OpInBoundsAccessChain:
    case spv::OpPtrAccessChain:
    case spv::OpArrayLength:
    case spv::OpGenericPtrMemSemantics:
    case spv::OpInBoundsPtrAccessChain:
    case spv::OpDecorate:
    case spv::OpMemberDecorate:
    case spv::OpDecorationGroup:
    case spv::OpGroupDecorate:
    case spv::OpGroupMemberDecorate:
    case spv::OpVectorExtractDynamic:
    case spv::OpVectorInsertDynamic:
    case spv::OpVectorShuffle:
    case spv::OpCompositeConstruct:
    case spv::OpCompositeExtract:
    case spv::OpCompositeInsert:
    case spv::OpCopyObject:
    case spv::OpTranspose:
    case spv::OpSampledImage:
    case spv::OpImageSampleImplicitLod:
    case spv::OpImageSampleExplicitLod:
    case spv::OpImageSampleDrefImplicitLod:
    case spv::OpImageSampleDrefExplicitLod:
    case spv::OpImageSampleProjImplicitLod:
    case spv::OpImageSampleProjExplicitLod:
    case spv::OpImageSampleProjDrefImplicitLod:
    case spv::OpImageSampleProjDrefExplicitLod:
    case spv::OpImageFetch:
    case spv::OpImageGather:
    case spv::OpImageDrefGather:
    case spv::OpImageRead:
    case spv::OpImageWrite:
    case spv::OpImage:
    case spv::OpImageQueryFormat:
    case spv::OpImageQueryOrder:
    case spv::OpImageQuerySizeLod:
    case spv::OpImageQuerySize:
    case spv::OpImageQueryLod:
    case spv::OpImageQueryLevels:
    case spv::OpImageQuerySamples:
    case spv::OpConvertFToU:
    case spv::OpConvertFToS:
    case spv::OpConvertSToF:
    case spv::OpConvertUToF:
    case spv::OpUConvert:
    case spv::OpSConvert:
    case spv::OpFConvert:
    case spv::OpQuantizeToF16:
    case spv::OpConvertPtrToU:
    case spv::OpSatConvertSToU:
    case spv::OpSatConvertUToS:
    case spv::OpConvertUToPtr:
    case spv::OpPtrCastToGeneric:
    case spv::OpGenericCastToPtr:
    case spv::OpGenericCastToPtrExplicit:
    case spv::OpBitcast:
    case spv::OpSNegate:
    case spv::OpFNegate:
    case spv::OpIAdd:
    case spv::OpFAdd:
    case spv::OpISub:
    case spv::OpFSub:
    case spv::OpIMul:
    case spv::OpFMul:
    case spv::OpUDiv:
    case spv::OpSDiv:
    case spv::OpFDiv:
    case spv::OpUMod:
    case spv::OpSRem:
    case spv::OpSMod:
    case spv::OpFRem:
    case spv::OpFMod:
    case spv::OpVectorTimesScalar:
    case spv::OpMatrixTimesScalar:
    case spv::OpVectorTimesMatrix:
    case spv::OpMatrixTimesVector:
    case spv::OpMatrixTimesMatrix:
    case spv::OpOuterProduct:
    case spv::OpDot:
    case spv::OpIAddCarry:
    case spv::OpISubBorrow:
    case spv::OpUMulExtended:
    case spv::OpSMulExtended:
    case spv::OpAny:
    case spv::OpAll:
    case spv::OpIsNan:
    case spv::OpIsInf:
    case spv::OpIsFinite:
    case spv::OpIsNormal:
    case spv::OpSignBitSet:
    case spv::OpLessOrGreater:
    case spv::OpOrdered:
    case spv::OpUnordered:
    case spv::OpLogicalEqual:
    case spv::OpLogicalNotEqual:
    case spv::OpLogicalOr:
    case spv::OpLogicalAnd:
    case spv::OpLogicalNot:
    case spv::OpSelect:
    case spv::OpIEqual:
    case spv::OpINotEqual:
    case spv::OpUGreaterThan:
    case spv::OpSGreaterThan:
    case spv::OpUGreaterThanEqual:
    case spv::OpSGreaterThanEqual:
    case spv::OpULessThan:
    case spv::OpSLessThan:
    case spv::OpULessThanEqual:
    case spv::OpSLessThanEqual:
    case spv::OpFOrdEqual:
    case spv::OpFUnordEqual:
    case spv::OpFOrdNotEqual:
    case spv::OpFUnordNotEqual:
    case spv::OpFOrdLessThan:
    case spv::OpFUnordLessThan:
    case spv::OpFOrdGreaterThan:
    case spv::OpFUnordGreaterThan:
    case spv::OpFOrdLessThanEqual:
    case spv::OpFUnordLessThanEqual:
    case spv::OpFOrdGreaterThanEqual:
    case spv::OpFUnordGreaterThanEqual:
    case spv::OpShiftRightLogical:
    case spv::OpShiftRightArithmetic:
    case spv::OpShiftLeftLogical:
    case spv::OpBitwiseOr:
    case spv::OpBitwiseXor:
    case spv::OpBitwiseAnd:
    case spv::OpNot:
    case spv::OpBitFieldInsert:
    case spv::OpBitFieldSExtract:
    case spv::OpBitFieldUExtract:
    case spv::OpBitReverse:
    case spv::OpBitCount:
    case spv::OpDPdx:
    case spv::OpDPdy:
    case spv::OpFwidth:
    case spv::OpDPdxFine:
    case spv::OpDPdyFine:
    case spv::OpFwidthFine:
    case spv::OpDPdxCoarse:
    case spv::OpDPdyCoarse:
    case spv::OpFwidthCoarse:
    case spv::OpEmitVertex:
    case spv::OpEndPrimitive:
    case spv::OpEmitStreamVertex:
    case spv::OpEndStreamPrimitive:
    case spv::OpControlBarrier:
    case spv::OpMemoryBarrier:
    case spv::OpAtomicLoad:
    case spv::OpAtomicStore:
    case spv::OpAtomicExchange:
    case spv::OpAtomicCompareExchange:
    case spv::OpAtomicCompareExchangeWeak:
    case spv::OpAtomicIIncrement:
    case spv::OpAtomicIDecrement:
    case spv::OpAtomicIAdd:
    case spv::OpAtomicISub:
    case spv::OpAtomicSMin:
    case spv::OpAtomicUMin:
    case spv::OpAtomicSMax:
    case spv::OpAtomicUMax:
    case spv::OpAtomicAnd:
    case spv::OpAtomicOr:
    case spv::OpAtomicXor:
    case spv::OpPhi:
    case spv::OpLoopMerge:
    case spv::OpSelectionMerge:
    case spv::OpLabel:
    case spv::OpBranch:
    case spv::OpBranchConditional:
    case spv::OpSwitch:
    case spv::OpKill:
    case spv::OpReturn:
    case spv::OpReturnValue:
    case spv::OpUnreachable:
    case spv::OpLifetimeStart:
    case spv::OpLifetimeStop:
    case spv::OpGroupAsyncCopy:
    case spv::OpGroupWaitEvents:
    case spv::OpGroupAll:
    case spv::OpGroupAny:
    case spv::OpGroupBroadcast:
    case spv::OpGroupIAdd:
    case spv::OpGroupFAdd:
    case spv::OpGroupFMin:
    case spv::OpGroupUMin:
    case spv::OpGroupSMin:
    case spv::OpGroupFMax:
    case spv::OpGroupUMax:
    case spv::OpGroupSMax:
    case spv::OpReadPipe:
    case spv::OpWritePipe:
    case spv::OpReservedReadPipe:
    case spv::OpReservedWritePipe:
    case spv::OpReserveReadPipePackets:
    case spv::OpReserveWritePipePackets:
    case spv::OpCommitReadPipe:
    case spv::OpCommitWritePipe:
    case spv::OpIsValidReserveId:
    case spv::OpGetNumPipePackets:
    case spv::OpGetMaxPipePackets:
    case spv::OpGroupReserveReadPipePackets:
    case spv::OpGroupReserveWritePipePackets:
    case spv::OpGroupCommitReadPipe:
    case spv::OpGroupCommitWritePipe:
    case spv::OpEnqueueMarker:
    case spv::OpEnqueueKernel:
    case spv::OpGetKernelNDrangeSubGroupCount:
    case spv::OpGetKernelNDrangeMaxSubGroupSize:
    case spv::OpGetKernelWorkGroupSize:
    case spv::OpGetKernelPreferredWorkGroupSizeMultiple:
    case spv::OpRetainEvent:
    case spv::OpReleaseEvent:
    case spv::OpCreateUserEvent:
    case spv::OpIsValidEvent:
    case spv::OpSetUserEventStatus:
    case spv::OpCaptureEventProfilingInfo:
    case spv::OpGetDefaultQueue:
    case spv::OpBuildNDRange:
    case spv::OpImageSparseSampleImplicitLod:
    case spv::OpImageSparseSampleExplicitLod:
    case spv::OpImageSparseSampleDrefImplicitLod:
    case spv::OpImageSparseSampleDrefExplicitLod:
    case spv::OpImageSparseSampleProjImplicitLod:
    case spv::OpImageSparseSampleProjExplicitLod:
    case spv::OpImageSparseSampleProjDrefImplicitLod:
    case spv::OpImageSparseSampleProjDrefExplicitLod:
    case spv::OpImageSparseFetch:
    case spv::OpImageSparseGather:
    case spv::OpImageSparseDrefGather:
    case spv::OpImageSparseTexelsResident:
    case spv::OpNoLine:
    case spv::OpAtomicFlagTestAndSet:
    case spv::OpAtomicFlagClear:
    case spv::OpImageSparseRead:
    case spv::OpSizeOf:
    case spv::OpTypePipeStorage:
    case spv::OpConstantPipeStorage:
    case spv::OpCreatePipeFromPipeStorage:
    case spv::OpGetKernelLocalSizeForSubgroupCount:
    case spv::OpGetKernelMaxNumSubgroups:
    case spv::OpTypeNamedBarrier:
    case spv::OpNamedBarrierInitialize:
    case spv::OpMemoryNamedBarrier:
    case spv::OpModuleProcessed:
    case spv::OpExecutionModeId:
    case spv::OpDecorateId:
    case spv::OpSubgroupBallotKHR:
    case spv::OpSubgroupFirstInvocationKHR:
    case spv::OpSubgroupAllKHR:
    case spv::OpSubgroupAnyKHR:
    case spv::OpSubgroupAllEqualKHR:
    case spv::OpSubgroupReadInvocationKHR:
    case spv::OpGroupIAddNonUniformAMD:
    case spv::OpGroupFAddNonUniformAMD:
    case spv::OpGroupFMinNonUniformAMD:
    case spv::OpGroupUMinNonUniformAMD:
    case spv::OpGroupSMinNonUniformAMD:
    case spv::OpGroupFMaxNonUniformAMD:
    case spv::OpGroupUMaxNonUniformAMD:
    case spv::OpGroupSMaxNonUniformAMD:
    case spv::OpFragmentMaskFetchAMD:
    case spv::OpFragmentFetchAMD:
    case spv::OpSubgroupShuffleINTEL:
    case spv::OpSubgroupShuffleDownINTEL:
    case spv::OpSubgroupShuffleUpINTEL:
    case spv::OpSubgroupShuffleXorINTEL:
    case spv::OpSubgroupBlockReadINTEL:
    case spv::OpSubgroupBlockWriteINTEL:
    case spv::OpSubgroupImageBlockReadINTEL:
    case spv::OpSubgroupImageBlockWriteINTEL:
    case spv::OpDecorateStringGOOGLE:
    case spv::OpMemberDecorateStringGOOGLE:
    case spv::OpMax:
      break;
  }
  return "";
}
}  // namespace spv
