//
// Created by SVR00003 on 2017/9/15.
//

#ifndef XRSDK_XRAPITYPES_H
#define XRSDK_XRAPITYPES_H


#include <stdbool.h>
#include "XrApiConfig.h"   // needed for XRAPI_EXPORT


//-----------------------------------------------------------------
// Java
//-----------------------------------------------------------------

#if defined( ANDROID )
#include <jni.h>
#elif defined( __cplusplus )
typedef struct _JNIEnv JNIEnv;
typedef struct _JavaVM JavaVM;
typedef class _jobject * jobject;
#else
typedef const struct JNINativeInterface * JNIEnv;
typedef const struct JNIInvokeInterface * JavaVM;
typedef void * jobject;
#endif

typedef struct
{
    JavaVM *    Vm;                 // Java Virtual Machine
    JNIEnv *    Env;                // Thread specific environment
    jobject     ActivityObject;     // Java activity object
} xrJava;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrJava, 12 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrJava, 24 );

//-----------------------------------------------------------------
// Basic Types
//-----------------------------------------------------------------

typedef signed int xrResult;

// xrResult isn't actually an enum type and the the success / failure types are not
// defined anywhere for GearVR XrApi. This needs to be remedied. For now, I'm defining
// these here and will try to address this larger issue in a follow-on changeset.
// errors are < 0, successes are >= 0
// Except where noted, these match error codes from PC CAPI.
typedef enum xrSuccessResult_
{
    xrSuccess                      = 0,
} xrSuccessResult;

typedef enum xrErrorResult_
{
    xrError_MemoryAllocationFailure    = -1000,
    xrError_NotInitialized             = -1004,
    xrError_InvalidParameter           = -1005,
    xrError_DeviceUnavailable          = -1010,    // device is not connected, or not connected as input device
    xrError_InvalidOperation           = -1015,

    // enums not in CAPI
    xrError_UnsupportedDeviceType      = -1050,    // specified device type isn't supported on GearVR
    xrError_NoDevice                   = -1051,    // specified device ID does not map to any current device
    xrError_NotImplemented             = -1052,    // executed an incomplete code path - this should not be possible in public releases.

    xrResult_EnumSize                  = 0x7fffffff
} xrErrorResult;

typedef struct xrVector2f_
{
    float x, y;
} xrVector2f;

XRAPI_ASSERT_TYPE_SIZE( xrVector2f, 8 );

typedef struct xrVector3f_
{
    float x, y, z;
} xrVector3f;

XRAPI_ASSERT_TYPE_SIZE( xrVector3f, 12 );

typedef struct xrVector4f_
{
    float x, y, z, w;
} xrVector4f;

XRAPI_ASSERT_TYPE_SIZE( xrVector4f, 16 );

// Quaternion.
typedef struct xrQuatf_
{
    float x, y, z, w;
} xrQuatf;

XRAPI_ASSERT_TYPE_SIZE( xrQuatf, 16 );

// Row-major 4x4 matrix.
typedef struct xrMatrix4f_
{
    float M[4][4];
} xrMatrix4f;

XRAPI_ASSERT_TYPE_SIZE( xrMatrix4f, 64 );

// Position and orientation together.
typedef struct xrPosef_
{
    xrQuatf    Orientation;
    xrVector3f Position;
} xrPosef;

XRAPI_ASSERT_TYPE_SIZE( xrPosef, 28 );

typedef struct xrRectf_
{
    float x;
    float y;
    float width;
    float height;
} xrRectf;

XRAPI_ASSERT_TYPE_SIZE( xrRectf, 16 );

typedef enum
{
    XRAPI_FALSE = 0,
    XRAPI_TRUE
} xrBooleanResult;

typedef enum
{
    XRAPI_EYE_LEFT,
    XRAPI_EYE_RIGHT,
    XRAPI_EYE_COUNT
} xrEye;

//-----------------------------------------------------------------
// Structure Types
//-----------------------------------------------------------------

typedef enum
{
    XRAPI_STRUCTURE_TYPE_INIT_PARMS     = 1,
    XRAPI_STRUCTURE_TYPE_MODE_PARMS     = 2,
    XRAPI_STRUCTURE_TYPE_FRAME_PARMS    = 3,
} xrStructureType;

//-----------------------------------------------------------------
// System Properties and Status
//-----------------------------------------------------------------

typedef enum
{
    XRAPI_DEVICE_TYPE_GEARVR_START          = 0,

    XRAPI_DEVICE_TYPE_NOTE4                 = XRAPI_DEVICE_TYPE_GEARVR_START,
    XRAPI_DEVICE_TYPE_NOTE5                 = 1,
    XRAPI_DEVICE_TYPE_S6                    = 2,
    XRAPI_DEVICE_TYPE_S7                    = 3,
    XRAPI_DEVICE_TYPE_NOTE7                 = 4,            // No longer supported.
    XRAPI_DEVICE_TYPE_S8                    = 5,

    XRAPI_DEVICE_GEARVR_END                 = 63,


    XRAPI_DEVICE_TYPE_UNKNOWN               = -1,
} xrDeviceType;

typedef enum
{
    XRAPI_HEADSET_TYPE_R320                 = 0,            // Note4 Innovator
    XRAPI_HEADSET_TYPE_R321                 = 1,            // S6 Innovator
    XRAPI_HEADSET_TYPE_R322                 = 2,            // Commercial 1
    XRAPI_HEADSET_TYPE_R323                 = 3,            // Commercial 2 (USB Type C)
    XRAPI_HEADSET_TYPE_R324                 = 4,            // Commercial 3 (USB Type C)


    XRAPI_HEADSET_TYPE_UNKNOWN              = -1,
} xrHeadsetType;

typedef enum
{
    XRAPI_DEVICE_REGION_UNSPECIFIED,
    XRAPI_DEVICE_REGION_JAPAN,
    XRAPI_DEVICE_REGION_CHINA,
} xrDeviceRegion;

typedef enum
{
    XRAPI_VIDEO_DECODER_LIMIT_4K_30FPS,
    XRAPI_VIDEO_DECODER_LIMIT_4K_60FPS,
} xrVideoDecoderLimit;

typedef enum
{
    XRAPI_SYS_PROP_DEVICE_TYPE,
    XRAPI_SYS_PROP_MAX_FULLSPEED_FRAMEBUFFER_SAMPLES,
    // Physical width and height of the display in pixels.
    XRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE,
    XRAPI_SYS_PROP_DISPLAY_PIXELS_HIGH,
    // Refresh rate of the display in cycles per second.
    // Currently 60Hz.
    XRAPI_SYS_PROP_DISPLAY_REFRESH_RATE,
    // With a display resolution of 2560x1440, the pixels at the center
    // of each eye cover about 0.06 degrees of visual arc. To wrap a
    // full 360 degrees, about 6000 pixels would be needed and about one
    // quarter of that would be needed for ~90 degrees FOV. As such, Eye
    // images with a resolution of 1536x1536 result in a good 1:1 mapping
    // in the center, but they need mip-maps for off center pixels. To
    // avoid the need for mip-maps and for significantly improved rendering
    // performance this currently returns a conservative 1024x1024.
    XRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH,
    XRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT,
    // This is a product of the lens distortion and the screen size,
    // but there is no truly correct answer.
    // There is a tradeoff in resolution and coverage.
    // Too small of an FOV will leave unrendered pixels visible, but too
    // large wastes resolution or fill rate.  It is unreasonable to
    // increase it until the corners are completely covered, but we do
    // want most of the outside edges completely covered.
    // Applications might choose to render a larger FOV when angular
    // acceleration is high to reduce black pull in at the edges by
    // the time warp.
    // Currently symmetric 90.0 degrees.
    XRAPI_SYS_PROP_SUGGESTED_EYE_FOV_DEGREES_X,     // Horizontal field of view in degrees
    XRAPI_SYS_PROP_SUGGESTED_EYE_FOV_DEGREES_Y,     // Vertical field of view in degrees
    // Path to the external SD card. On Android-M, this path is dynamic and can
    // only be determined once the SD card is mounted. Returns an empty string if
    // device does not support an ext sdcard or if running Android-M and the SD card
    // is not mounted.
    XRAPI_SYS_PROP_EXT_SDCARD_PATH,
    XRAPI_SYS_PROP_DEVICE_REGION,
    // Video decoder limit for the device.
    XRAPI_SYS_PROP_VIDEO_DECODER_LIMIT,
    XRAPI_SYS_PROP_HEADSET_TYPE,

    // A single press and release of the back button in less than this time is considered
    // a 'short press'.
    XRAPI_SYS_PROP_BACK_BUTTON_SHORTPRESS_TIME,     // in seconds
    // Pressing the back button twice within this time is considered a 'double tap'.
    XRAPI_SYS_PROP_BACK_BUTTON_DOUBLETAP_TIME,      // in seconds

    XRAPI_SYS_PROP_DOMINANT_HAND,                   // returns an xrHandedness enum for left or right hand

    // Returns XRAPI_TRUE if Multiview rendering support is available for this system,
    // otherwise XRAPI_FALSE.
    XRAPI_SYS_PROP_MULTIVIEW_AVAILABLE = 128,
    // Returns XRAPI_TRUE if submission of SRGB Layers is supported for this system,
    // otherwise XRAPI_FALSE.
    XRAPI_SYS_PROP_SRGB_LAYER_SOURCE_AVAILABLE,

    XRAPI_SYS_PROP_BUILD_PRODUCT,

    // Lens seperation
    XRAPI_SYS_PROP_LENS_SEPERATION = 1024,
    XRAPI_SYS_PROP_WIDTH_METERS,
    XRAPI_SYS_PROP_HEIGHT_METERS,
    XRAPI_SYS_PROP_UNITY_VSYNC_CNT,
    XRAPI_SYS_PROP_WFD_TYPE,
    XRAPI_SYS_PROP_WFD_WHRATIO,
    XRAPI_SYS_PROP_SCREEN_TYPE,
    XRAPI_SYS_PROP_CHROMATIC_CORRECTION_STATE,
    XRAPI_SYS_PROP_CHROMATIC_ABERRATION_0,
    XRAPI_SYS_PROP_CHROMATIC_ABERRATION_1,
    XRAPI_SYS_PROP_CHROMATIC_ABERRATION_2,
    XRAPI_SYS_PROP_CHROMATIC_ABERRATION_3,
} xrSystemProperty;


typedef enum
{
    XRAPI_HAND_UNKNOWN  = 0,
    XRAPI_HAND_LEFT     = 1,
    XRAPI_HAND_RIGHT    = 2
} xrHandedness;

typedef enum
{
    XRAPI_SYS_STATUS_DOCKED,                        // Device is docked.
    XRAPI_SYS_STATUS_MOUNTED,                       // Device is mounted.
    XRAPI_SYS_STATUS_THROTTLED,                     // Device is in powersave mode.
    XRAPI_SYS_STATUS_THROTTLED2,                    // Device is in extreme powersave mode.

    // enum  4 used to be XRAPI_SYS_STATUS_THROTTLED_WARNING_LEVEL.

    XRAPI_SYS_STATUS_RENDER_LATENCY_MILLISECONDS = 5,// Average time between render tracking sample and scanout.
    XRAPI_SYS_STATUS_TIMEWARP_LATENCY_MILLISECONDS, // Average time between timewarp tracking sample and scanout.
    XRAPI_SYS_STATUS_SCANOUT_LATENCY_MILLISECONDS,  // Average time between Vsync and scanout.
    XRAPI_SYS_STATUS_APP_FRAMES_PER_SECOND,         // Number of frames per second delivered through Xrapi_SubmitFrame.
    XRAPI_SYS_STATUS_SCREEN_TEARS_PER_SECOND,       // Number of screen tears per second (per eye).
    XRAPI_SYS_STATUS_EARLY_FRAMES_PER_SECOND,       // Number of frames per second delivered a whole display refresh early.
    XRAPI_SYS_STATUS_STALE_FRAMES_PER_SECOND,       // Number of frames per second delivered late.

    XRAPI_SYS_STATUS_HEADPHONES_PLUGGED_IN,         // Returns XRAPI_TRUE if headphones are plugged into the device.
    XRAPI_SYS_STATUS_RECENTER_COUNT,                // Returns the current HMD recenter count. Defaults to 0.
    XRAPI_SYS_STATUS_SYSTEM_UX_ACTIVE,              // Returns XRAPI_TRUE if a system UX layer is active

    XRAPI_SYS_STATUS_FRONT_BUFFER_PROTECTED = 128,  // True if the front buffer is allocated in TrustZone memory.
    XRAPI_SYS_STATUS_FRONT_BUFFER_565,              // True if the front buffer is 16-bit 5:6:5
    XRAPI_SYS_STATUS_FRONT_BUFFER_SRGB,             // True if the front buffer uses the sRGB color space.

} xrSystemStatus;

//-----------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------

typedef enum
{
    XRAPI_INITIALIZE_SUCCESS            =  0,
    XRAPI_INITIALIZE_UNKNOWN_ERROR      = -1,
    XRAPI_INITIALIZE_PERMISSIONS_ERROR  = -2,
} xrInitializeStatus;

typedef enum
{
    XRAPI_GRAPHICS_API_OPENGL_ES_2   = ( 0x10000 | 0x0200 ), // OpenGL ES 2.x context
    XRAPI_GRAPHICS_API_OPENGL_ES_3   = ( 0x10000 | 0x0300 ), // OpenGL ES 3.x context
    XRAPI_GRAPHICS_API_OPENGL_COMPAT = ( 0x20000 | 0x0100 ), // OpenGL Compatibility Profile
    XRAPI_GRAPHICS_API_OPENGL_CORE_3 = ( 0x20000 | 0x0300 ), // OpenGL Core Profile 3.x
    XRAPI_GRAPHICS_API_OPENGL_CORE_4 = ( 0x20000 | 0x0400 ), // OpenGL Core Profile 4.x
} xrGraphicsAPI;

typedef enum {
    XRAPI_SCREEN_MODE_DEFAULT = 0x0,
    XRAPI_SCREEN_MODE_DUAL = XRAPI_SCREEN_MODE_DEFAULT,
    XRAPI_SCREEN_MODE_SPLIT = 0x1,
    XRAPI_SCREEN_MODE_MULTIVIEW = 0x2,

    XRAPI_IVR_SDK_COMPATIBLE = 0x00200000,
} xrInitFlags;

typedef struct
{
    xrStructureType    Type;
    int                 ProductVersion;
    int                 MajorVersion;
    int                 MinorVersion;
    int                 PatchVersion;
    xrGraphicsAPI      GraphicsAPI;
    xrJava             Java;
    //values contain in xrInitFlags
    unsigned int      Flags;
} xrInitParms;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrInitParms, 40 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrInitParms, 56 );

//-----------------------------------------------------------------
// VR Mode
//-----------------------------------------------------------------

// NOTE: the first two flags use the first two bytes for backwards compatibility on little endian systems.
typedef enum
{
    // If set, warn and allow the app to continue at 30 FPS when throttling occurs.
    // If not set, display the level 2 error message which requires the user to undock.
    XRAPI_MODE_FLAG_ALLOW_POWER_SAVE            = 0x000000FF,

    // When an application moves backwards on the activity stack,
    // the activity window it returns to is no longer flagged as fullscreen.
    // As a result, Android will also render the decor view, which wastes a
    // significant amount of bandwidth.
    // By setting this flag, the fullscreen flag is reset on the window.
    // Unfortunately, this causes Android life cycle events that mess up
    // several NativeActivity codebases like Stratum and UE4, so this
    // flag should only be set for specific applications.
    // Use "adb shell dumpsys SurfaceFlinger" to verify
    // that there is only one HWC next to the FB_TARGET.
    XRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN     = 0x0000FF00,

    // The WindowSurface passed in is an ANativeWindow.
    XRAPI_MODE_FLAG_NATIVE_WINDOW               = 0x00010000,

    // Create the front buffer in TrustZone memory to allow protected DRM
    // content to be rendered to the front buffer. This functionality
    // requires the WindowSurface to be allocated from TimeWarp, via
    // specifying the nativeWindow via XRAPI_MODE_FLAG_NATIVE_WINDOW.
    XRAPI_MODE_FLAG_FRONT_BUFFER_PROTECTED      = 0x00020000,

    // Create a 16-bit 5:6:5 front buffer.
    XRAPI_MODE_FLAG_FRONT_BUFFER_565            = 0x00040000,

    // Create a front buffer using the sRGB color space.
    XRAPI_MODE_FLAG_FRONT_BUFFER_SRGB           = 0x00080000,
} xrModeFlags;

typedef struct
{
    xrStructureType    Type;

    // Combination of xrModeFlags flags.
    unsigned int        Flags;

    // The Java VM is needed for the time warp thread to create a Java environment.
    // A Java environment is needed to access various system services. The thread
    // that enters VR mode is responsible for attaching and detaching the Java
    // environment. The Java Activity object is needed to get the windowManager,
    // packageName, systemService, etc.
    xrJava             Java;

    XRAPI_PADDING_32_BIT( 4 );

    // If not zero, then use this display for asynchronous time warp rendering.
    // Using EGL this is an EGLDisplay.
    unsigned long long  Display;

    // If not zero, then use this window surface for asynchronous time warp rendering.
    // Using EGL this can be the EGLSurface created by the application for the ANativeWindow.
    // Preferrably this is the ANativeWIndow itself (requires XRAPI_MODE_FLAG_NATIVE_WINDOW).
    unsigned long long  WindowSurface;

    // If not zero, then resources from this context will be shared with the asynchronous time warp.
    // Using EGL this is an EGLContext.
    unsigned long long  ShareContext;
} xrModeParms;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrModeParms, 48 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrModeParms, 56 );

// VR context
// To allow multiple Android activities that live in the same address space
// to cooperatively use the XrApi, each activity needs to maintain its own
// separate contexts for a lot of the video related systems.
typedef struct xrMobile xrMobile;

//-----------------------------------------------------------------
// Tracking
//-----------------------------------------------------------------

// Full rigid body pose with first and second derivatives.
typedef struct xrRigidBodyPosef_
{
    xrPosef        Pose;
    xrVector3f     AngularVelocity;
    xrVector3f     LinearVelocity;
    xrVector3f     AngularAcceleration;
    xrVector3f     LinearAcceleration;
    XRAPI_PADDING( 4 );
    double          TimeInSeconds;          // Absolute time of this pose.
    double          PredictionInSeconds;    // Seconds this pose was predicted ahead.
} xrRigidBodyPosef;

XRAPI_ASSERT_TYPE_SIZE( xrRigidBodyPosef, 96 );

// Bit flags describing the current status of sensor tracking.
typedef enum
{
    XRAPI_TRACKING_STATUS_ORIENTATION_TRACKED   = 0x0001,   // Orientation is currently tracked.
    XRAPI_TRACKING_STATUS_POSITION_TRACKED      = 0x0002,   // Position is currently tracked.
    XRAPI_TRACKING_STATUS_HMD_CONNECTED         = 0x0080    // HMD is available & connected.
} xrTrackingStatus;

// Tracking state at a given absolute time.
typedef struct xrTracking2_
{
    // Sensor status described by xrTrackingStatus flags.
    unsigned int        Status;

    XRAPI_PADDING( 4 );

    // Predicted head configuration at the requested absolute time.
    // The pose describes the head orientation and center eye position.
    xrRigidBodyPosef   HeadPose;
    struct
    {
        xrMatrix4f         ProjectionMatrix;
        xrMatrix4f         ViewMatrix;
    } Eye[ XRAPI_EYE_COUNT ];
} xrTracking2;

XRAPI_ASSERT_TYPE_SIZE( xrTracking2, 360 );

typedef struct xrTracking_
{
    // Sensor status described by xrTrackingStatus flags.
    unsigned int        Status;

    XRAPI_PADDING( 4 );

    // Predicted head configuration at the requested absolute time.
    // The pose describes the head orientation and center eye position.
    xrRigidBodyPosef   HeadPose;
} xrTracking;

XRAPI_ASSERT_TYPE_SIZE( xrTracking, 104 );

//-----------------------------------------------------------------
// Texture Swap Chain
//-----------------------------------------------------------------

typedef enum
{
    XRAPI_TEXTURE_TYPE_2D,              // 2D textures.
    XRAPI_TEXTURE_TYPE_2D_EXTERNAL,     // External 2D texture.
    XRAPI_TEXTURE_TYPE_2D_ARRAY,        // Texture array.
    XRAPI_TEXTURE_TYPE_CUBE,            // Cube maps.
    XRAPI_TEXTURE_TYPE_MAX,
} xrTextureType;

typedef enum
{
    XRAPI_TEXTURE_FORMAT_NONE,
    XRAPI_TEXTURE_FORMAT_565,
    XRAPI_TEXTURE_FORMAT_5551,
    XRAPI_TEXTURE_FORMAT_4444,
    XRAPI_TEXTURE_FORMAT_8888,
    XRAPI_TEXTURE_FORMAT_8888_sRGB,
    XRAPI_TEXTURE_FORMAT_RGBA16F,
    XRAPI_TEXTURE_FORMAT_DEPTH_16,
    XRAPI_TEXTURE_FORMAT_DEPTH_24,
    XRAPI_TEXTURE_FORMAT_DEPTH_24_STENCIL_8,

} xrTextureFormat;

typedef enum
{
    XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_BLACK           = 0x1,
    XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_LOADING_ICON    = 0x2,
    XRAPI_TEXTURE_SWAPCHAIN_EXTERNAL_TEXTRUEID      = 0x3
} xrDefaultTextureSwapChain;

typedef enum
{
    XRAPI_TEXTURE_SWAPCHAIN_FULL_MIP_CHAIN      = -1
} xrTextureSwapChainSettings;

typedef struct xrTextureSwapChain xrTextureSwapChain;

//-----------------------------------------------------------------
// Frame Submission
//-----------------------------------------------------------------

typedef enum
{
    // To get gamma correct sRGB filtering of the eye textures, the textures must be
    // allocated with GL_SRGB8_ALPHA8 format and the window surface must be allocated
    // with these attributes:
    // EGL_GL_COLORSPACE_KHR,  EGL_GL_COLORSPACE_SRGB_KHR
    //
    // While we can reallocate textures easily enough, we can't change the window
    // colorspace without relaunching the entire application, so if you want to
    // be able to toggle between gamma correct and incorrect, you must allocate
    // the framebuffer as sRGB, then inhibit that processing when using normal
    // textures.
    XRAPI_FRAME_FLAG_INHIBIT_SRGB_FRAMEBUFFER                   = 1,
    // Flush the warp swap pipeline so the images show up immediately.
    // This is expensive and should only be used when an immediate transition
    // is needed like displaying black when resetting the HMD orientation.
    XRAPI_FRAME_FLAG_FLUSH                                      = 2,
    // This is the final frame. Do not accept any more frames after this.
    XRAPI_FRAME_FLAG_FINAL                                      = 4,

    // enum  8 used to be XRAPI_FRAME_FLAG_TIMEWARP_DEBUG_GRAPH_SHOW.

    // enum 16 used to be XRAPI_FRAME_FLAG_TIMEWARP_DEBUG_GRAPH_FREEZE.

    // enum 32 used to be XRAPI_FRAME_FLAG_TIMEWARP_DEBUG_GRAPH_LATENCY_MODE.

    // Don't show the volume layer whent set.
    XRAPI_FRAME_FLAG_INHIBIT_VOLUME_LAYER                       = 64,

    // enum 128 used to be XRAPI_FRAME_FLAG_SHOW_LAYER_COMPLEXITY.

    // enum 256 used to be XRAPI_FRAME_FLAG_SHOW_TEXTURE_DENSITY.


} xrFrameFlags;

typedef enum
{
    // Enable writing to the alpha channel
    // NOTE: *_WRITE_ALPHA is DEPRECATED. Please do not write any new code which
    // relies on it's use.
    XRAPI_FRAME_LAYER_FLAG_WRITE_ALPHA                              = 1,
    // Correct for chromatic aberration. Quality/perf trade-off.
    XRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION          = 2,
    // Used for some HUDs, but generally considered bad practice.
    XRAPI_FRAME_LAYER_FLAG_FIXED_TO_VIEW                            = 4,
    // Spin the layer - for loading icons
    XRAPI_FRAME_LAYER_FLAG_SPIN                                     = 8,
    // Clip fragments outside the layer's TextureRect
    XRAPI_FRAME_LAYER_FLAG_CLIP_TO_TEXTURE_RECT                     = 16,

} xrFrameLayerFlags;

typedef enum
{
    XRAPI_FRAME_LAYER_EYE_LEFT,
    XRAPI_FRAME_LAYER_EYE_RIGHT,
    XRAPI_FRAME_LAYER_EYE_MAX
} xrFrameLayerEye;

typedef enum
{
    XRAPI_FRAME_LAYER_BLEND_ZERO,
    XRAPI_FRAME_LAYER_BLEND_ONE,
    XRAPI_FRAME_LAYER_BLEND_SRC_ALPHA,
    // NOTE: *_DST_ALPHA blend modes are DEPRECATED. Please do not
    // write any new code which relies on it's use.
    XRAPI_FRAME_LAYER_BLEND_DST_ALPHA,
    XRAPI_FRAME_LAYER_BLEND_ONE_MINUS_DST_ALPHA,
    XRAPI_FRAME_LAYER_BLEND_ONE_MINUS_SRC_ALPHA
} xrFrameLayerBlend;

typedef enum
{
    // enum 0-3 have been deprecated. Explicit indices
    // for frame layers should be used instead.
    XRAPI_FRAME_LAYER_TYPE_MAX = 4
} xrFrameLayerType;

typedef enum
{
    XRAPI_EXTRA_LATENCY_MODE_OFF,
    XRAPI_EXTRA_LATENCY_MODE_ON,
    XRAPI_EXTRA_LATENCY_MODE_DYNAMIC
} xrExtraLatencyMode;

// Note that any layer textures that are dynamic must be triple buffered.
typedef struct
{
    // Because OpenGL ES does not support clampToBorder, it is the
    // application's responsibility to make sure that all mip levels
    // of the primary eye texture have a black border that will show
    // up when time warp pushes the texture partially off screen.
    xrTextureSwapChain *   ColorTextureSwapChain;

    // DEPRECATED: Please do not write any new code which relies on DepthTextureSwapChain.
    // The depth texture is optional for positional time warp.
    xrTextureSwapChain *   DepthTextureSwapChain;

    // Index to the texture from the set that should be displayed.
    int                     TextureSwapChainIndex;

    // Points on the screen are mapped by a distortion correction
    // function into ( TanX, TanY, -1, 1 ) vectors that are transformed
    // by this matrix to get ( S, T, Q, _ ) vectors that are looked
    // up with texture2dproj() to get texels.
    xrMatrix4f             TexCoordsFromTanAngles;

    // Only texels within this range should be drawn.
    // This is a sub-rectangle of the [(0,0)-(1,1)] texture coordinate range.
    xrRectf                TextureRect;

    XRAPI_PADDING( 4 );

    // The tracking state for which ModelViewMatrix is correct.
    // It is ok to update the orientation for each eye, which
    // can help minimize black edge pull-in, but the position
    // must remain the same for both eyes, or the position would
    // seem to judder "backwards in time" if a frame is dropped.
    xrRigidBodyPosef       HeadPose;

    // If not zero, this fence will be used to determine whether or not
    // rendering to the color and depth texture swap chains has completed.
    unsigned long long      CompletionFence;
} xrFrameLayerTexture;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrFrameLayerTexture, 200 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrFrameLayerTexture, 208 );

typedef struct
{
    // Image used for each eye.
    xrFrameLayerTexture    Textures[XRAPI_FRAME_LAYER_EYE_MAX];

    // Speed and scale of rotation when XRAPI_FRAME_LAYER_FLAG_SPIN is set in xrFrameLayer::Flags
    float                   SpinSpeed;  // Radians/Second
    float                   SpinScale;

    // Color scale for this layer (including alpha)
    float                   ColorScale;

    // padding for deprecated variable.
    XRAPI_PADDING( 4 );

    // Layer blend function.
    xrFrameLayerBlend      SrcBlend;
    xrFrameLayerBlend      DstBlend;

    // Combination of xrFrameLayerFlags flags.
    int                     Flags;
} xrFrameLayer;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrFrameLayer, 432 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrFrameLayer, 448 );

typedef struct
{
    // These are fixed clock levels in the range [0, 3].
    int                     CpuLevel;
    int                     GpuLevel;

    // These threads will get SCHED_FIFO.
    int                     MainThreadTid;
    int                     RenderThreadTid;
} xrPerformanceParms;

XRAPI_ASSERT_TYPE_SIZE( xrPerformanceParms, 16 );

typedef struct
{
    xrStructureType        Type;

    XRAPI_PADDING( 4 );

    // Layers composited in the time warp.
    xrFrameLayer           Layers[XRAPI_FRAME_LAYER_TYPE_MAX];
    int                     LayerCount;

    // Combination of xrFrameFlags flags.
    int                     Flags;

    // Application controlled frame index that uniquely identifies this particular frame.
    // This must be the same frame index that was passed to Xrapi_GetPredictedDisplayTime()
    // when synthesis of this frame started.
    long long               FrameIndex;

    // WarpSwap will not return until at least this many V-syncs have
    // passed since the previous WarpSwap returned.
    // Setting to 2 will reduce power consumption and may make animation
    // more regular for applications that can't hold full frame rate.
    int                     MinimumVsyncs;

    // Latency Mode.
    xrExtraLatencyMode     ExtraLatencyMode;

    // DEPRECATED: Please do not write any code which relies on ExternalVelocity.
    // Rotation from a joypad can be added on generated frames to reduce
    // judder in FPS style experiences when the application framerate is
    // lower than the V-sync rate.
    // This will be applied to the view space distorted
    // eye vectors before applying the rest of the time warp.
    // This will only be added when the same xrFrameParms is used for
    // more than one V-sync.
    xrMatrix4f             ExternalVelocity;

    // DEPRECATED: Please do not write any code which relies on SurfaceTextureObject.
    // jobject that will be updated before each eye for minimal
    // latency.
    // IMPORTANT: This should be a JNI weak reference to the object.
    // The system will try to convert it into a global reference before
    // calling SurfaceTexture->Update, which allows it to be safely
    // freed by the application.
    jobject                 SurfaceTextureObject;

    // CPU/GPU performance parameters.
    xrPerformanceParms     PerformanceParms;

    // For handling HMD events and power level state changes.
    xrJava                 Java;
} xrFrameParms;

XRAPI_ASSERT_TYPE_SIZE_32_BIT( xrFrameParms, 1856 );
XRAPI_ASSERT_TYPE_SIZE_64_BIT( xrFrameParms, 1936 );


//-----------------------------------------------------------------
// Head Model
//-----------------------------------------------------------------

typedef struct
{
    float   InterpupillaryDistance; // Distance between eyes.
    float   EyeHeight;              // Eye height relative to the ground.
    float   HeadModelDepth;         // Eye offset forward from the head center at EyeHeight.
    float   HeadModelHeight;        // Neck joint offset down from the head center at EyeHeight.
} xrHeadModelParms;

XRAPI_ASSERT_TYPE_SIZE( xrHeadModelParms, 16 );

#endif //XRSDK_XRAPITYPES_H
