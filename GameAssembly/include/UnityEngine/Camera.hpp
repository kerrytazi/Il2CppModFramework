#pragma once

#include "UnityEngine/Behaviour.hpp"

namespace il2cpp { template <typename T> class Array; }

namespace UnityEngine
{

class Material;
class RenderTexture;

struct Color;
struct Rect;
struct Vector2;
struct Vector3;
struct Matrix4x4;

enum class RenderingPath;
enum class TransparencySortMode;
enum class CameraType;
enum class CameraClearFlags;
enum class DepthTextureMode;
enum class StereoTargetEyeMask;

namespace SceneManagement { struct Scene; }
namespace Rendering { enum class OpaqueSortMode; }
namespace Rendering { enum class CameraEvent; }
namespace Rendering { class CommandBuffer; }

class __autogen Camera : public UnityEngine::Behaviour
{
public:

	enum class GateFitMode;
	enum class ProjectionMatrixMode;
	enum class MonoOrStereoscopicEye;

	__autogen System::Single get_nearClipPlane();
	__autogen System::Single get_farClipPlane();
	__autogen System::Single get_fieldOfView();
	__autogen UnityEngine::RenderingPath get_renderingPath();
	__autogen UnityEngine::RenderingPath get_actualRenderingPath();
	__autogen System::Boolean get_allowHDR();
	__autogen System::Boolean get_allowMSAA();
	__autogen System::Boolean get_allowDynamicResolution();
	__autogen System::Boolean get_forceIntoRenderTexture();
	__autogen System::Single get_orthographicSize();
	__autogen System::Boolean get_orthographic();
	__autogen UnityEngine::Rendering::OpaqueSortMode get_opaqueSortMode();
	__autogen UnityEngine::TransparencySortMode get_transparencySortMode();
	__autogen UnityEngine::Vector3 get_transparencySortAxis();
	__autogen System::Single get_depth();
	__autogen System::Single get_aspect();
	__autogen UnityEngine::Vector3 get_velocity();
	__autogen System::Int32 get_cullingMask();
	__autogen System::Int32 get_eventMask();
	__autogen System::Boolean get_layerCullSpherical();
	__autogen UnityEngine::CameraType get_cameraType();
	__autogen UnityEngine::Material* get_skyboxMaterial();
	__autogen il2cpp::Array<System::Single>* get_layerCullDistances();
	__autogen System::Boolean get_useOcclusionCulling();
	__autogen UnityEngine::Matrix4x4 get_cullingMatrix();
	__autogen UnityEngine::Color get_backgroundColor();
	__autogen UnityEngine::CameraClearFlags get_clearFlags();
	__autogen UnityEngine::DepthTextureMode get_depthTextureMode();
	__autogen System::Boolean get_clearStencilAfterLightingPass();
	__autogen UnityEngine::Camera::ProjectionMatrixMode get_projectionMatrixMode();
	__autogen System::Boolean get_usePhysicalProperties();
	__autogen System::Int32 get_iso();
	__autogen System::Single get_shutterSpeed();
	__autogen System::Single get_aperture();
	__autogen System::Single get_focusDistance();
	__autogen System::Single get_focalLength();
	__autogen System::Int32 get_bladeCount();
	__autogen UnityEngine::Vector2 get_curvature();
	__autogen System::Single get_barrelClipping();
	__autogen System::Single get_anamorphism();
	__autogen UnityEngine::Vector2 get_sensorSize();
	__autogen UnityEngine::Vector2 get_lensShift();
	__autogen UnityEngine::Camera::GateFitMode get_gateFit();
	__autogen UnityEngine::Rect get_rect();
	__autogen UnityEngine::Rect get_pixelRect();
	__autogen System::Int32 get_pixelWidth();
	__autogen System::Int32 get_pixelHeight();
	__autogen System::Int32 get_scaledPixelWidth();
	__autogen System::Int32 get_scaledPixelHeight();
	__autogen UnityEngine::RenderTexture* get_targetTexture();
	__autogen UnityEngine::RenderTexture* get_activeTexture();
	__autogen System::Int32 get_targetDisplay();
	__autogen UnityEngine::Matrix4x4 get_cameraToWorldMatrix();
	__autogen UnityEngine::Matrix4x4 get_worldToCameraMatrix();
	__autogen UnityEngine::Matrix4x4 get_projectionMatrix();
	__autogen UnityEngine::Matrix4x4 get_nonJitteredProjectionMatrix();
	__autogen System::Boolean get_useJitteredProjectionMatrixForTransparentRendering();
	__autogen UnityEngine::Matrix4x4 get_previousViewProjectionMatrix();
	__autogen static UnityEngine::Camera* get_main();
	__autogen static UnityEngine::Camera* get_current();
	__autogen UnityEngine::SceneManagement::Scene get_scene();
	__autogen System::Boolean get_stereoEnabled();
	__autogen System::Single get_stereoSeparation();
	__autogen System::Single get_stereoConvergence();
	__autogen System::Boolean get_areVRStereoViewMatricesWithinSingleCullTolerance();
	__autogen UnityEngine::StereoTargetEyeMask get_stereoTargetEye();
	__autogen UnityEngine::Camera::MonoOrStereoscopicEye get_stereoActiveEye();
	__autogen static System::Int32 get_allCamerasCount();
	__autogen static il2cpp::Array<UnityEngine::Camera*>* get_allCameras();
	__autogen System::Int32 get_commandBufferCount();

	__autogen System::Void set_nearClipPlane(System::Single val);
	__autogen System::Void set_farClipPlane(System::Single val);
	__autogen System::Void set_fieldOfView(System::Single val);
	__autogen System::Void set_renderingPath(UnityEngine::RenderingPath val);
	__autogen System::Void set_allowHDR(System::Boolean val);
	__autogen System::Void set_allowMSAA(System::Boolean val);
	__autogen System::Void set_allowDynamicResolution(System::Boolean val);
	__autogen System::Void set_forceIntoRenderTexture(System::Boolean val);
	__autogen System::Void set_orthographicSize(System::Single val);
	__autogen System::Void set_orthographic(System::Boolean val);
	__autogen System::Void set_opaqueSortMode(UnityEngine::Rendering::OpaqueSortMode val);
	__autogen System::Void set_transparencySortMode(UnityEngine::TransparencySortMode val);
	__autogen System::Void set_transparencySortAxis(UnityEngine::Vector3 val);
	__autogen System::Void set_depth(System::Single val);
	__autogen System::Void set_aspect(System::Single val);
	__autogen System::Void set_cullingMask(System::Int32 val);
	__autogen System::Void set_eventMask(System::Int32 val);
	__autogen System::Void set_layerCullSpherical(System::Boolean val);
	__autogen System::Void set_cameraType(UnityEngine::CameraType val);
	__autogen System::Void set_layerCullDistances(il2cpp::Array<System::Single>* val);
	__autogen System::Void set_useOcclusionCulling(System::Boolean val);
	__autogen System::Void set_cullingMatrix(UnityEngine::Matrix4x4 val);
	__autogen System::Void set_backgroundColor(UnityEngine::Color val);
	__autogen System::Void set_clearFlags(UnityEngine::CameraClearFlags val);
	__autogen System::Void set_depthTextureMode(UnityEngine::DepthTextureMode val);
	__autogen System::Void set_clearStencilAfterLightingPass(System::Boolean val);
	__autogen System::Void set_usePhysicalProperties(System::Boolean val);
	__autogen System::Void set_iso(System::Int32 val);
	__autogen System::Void set_shutterSpeed(System::Single val);
	__autogen System::Void set_aperture(System::Single val);
	__autogen System::Void set_focusDistance(System::Single val);
	__autogen System::Void set_focalLength(System::Single val);
	__autogen System::Void set_bladeCount(System::Int32 val);
	__autogen System::Void set_curvature(UnityEngine::Vector2 val);
	__autogen System::Void set_barrelClipping(System::Single val);
	__autogen System::Void set_anamorphism(System::Single val);
	__autogen System::Void set_sensorSize(UnityEngine::Vector2 val);
	__autogen System::Void set_lensShift(UnityEngine::Vector2 val);
	__autogen System::Void set_gateFit(UnityEngine::Camera::GateFitMode val);
	__autogen System::Void set_rect(UnityEngine::Rect val);
	__autogen System::Void set_pixelRect(UnityEngine::Rect val);
	__autogen System::Void set_targetTexture(UnityEngine::RenderTexture* val);
	__autogen System::Void set_targetDisplay(System::Int32 val);
	__autogen System::Void set_worldToCameraMatrix(UnityEngine::Matrix4x4 val);
	__autogen System::Void set_projectionMatrix(UnityEngine::Matrix4x4 val);
	__autogen System::Void set_nonJitteredProjectionMatrix(UnityEngine::Matrix4x4 val);
	__autogen System::Void set_useJitteredProjectionMatrixForTransparentRendering(System::Boolean val);
	__autogen System::Void set_scene(UnityEngine::SceneManagement::Scene val);
	__autogen System::Void set_stereoSeparation(System::Single val);
	__autogen System::Void set_stereoConvergence(System::Single val);
	__autogen System::Void set_stereoTargetEye(UnityEngine::StereoTargetEyeMask val);

	__autogen System::Void AddCommandBuffer(UnityEngine::Rendering::CameraEvent evt, UnityEngine::Rendering::CommandBuffer* buffer);
	__autogen System::Void RemoveCommandBuffer(UnityEngine::Rendering::CameraEvent evt, UnityEngine::Rendering::CommandBuffer* buffer);

	enum class GateFitMode
	{
		Vertical = 1,
		Horizontal = 2,
		Fill = 3,
		Overscan = 4,
		None = 0,
	};

	enum class ProjectionMatrixMode
	{
		Explicit = 0,
		Implicit = 1,
		PhysicalPropertiesBased = 2,
	};

	enum class MonoOrStereoscopicEye
	{
		Left = 0,
		Right = 1,
		Mono = 2,
	};
};

} // namespace UnityEngine
