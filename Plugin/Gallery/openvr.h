#pragma once
namespace vr    //involved clip
{
    // right-handed system
    // +y is up
    // +x is to the right
    // -z is forward
    // Distance unit is  meters
    struct HmdMatrix34_t
    {
        float m[3][4];
    };
    struct HmdMatrix44_t
    {
        float m[4][4];
    };
    struct HmdVector3_t
    {
        float v[3];
    };
    struct HmdVector4_t
    {
        float v[4];
    };
    struct HmdVector3d_t
    {
        double v[3];
    };
    struct HmdVector2_t
    {
        float v[2];
    };
    struct HmdQuaternion_t
    {
        double w, x, y, z;
    };
    struct HmdColor_t
    {
        float r, g, b, a;
    };
    struct HmdQuad_t
    {
        HmdVector3_t vCorners[4];
    };
    struct HmdRect2_t
    {
        HmdVector2_t vTopLeft;
        HmdVector2_t vBottomRight;
    };
    enum ETrackingResult
    {
        TrackingResult_Uninitialized = 1,

        TrackingResult_Calibrating_InProgress = 100,
        TrackingResult_Calibrating_OutOfRange = 101,

        TrackingResult_Running_OK = 200,
        TrackingResult_Running_OutOfRange = 201,
    };
    struct TrackedDevicePose_t
    {
        HmdMatrix34_t mDeviceToAbsoluteTracking;
        HmdVector3_t vVelocity;				// velocity in tracker space in m/s
        HmdVector3_t vAngularVelocity;		// angular velocity in radians/s (?)
        ETrackingResult eTrackingResult;
        bool bPoseIsValid;
        // This indicates that there is a device connected for this spot in the pose array.
        // It could go from true to false if the user unplugs the device.
        bool bDeviceIsConnected;
    };
    /** Provides a single frame's timing information to the app */
    struct Compositor_FrameTiming
    {
        uint32_t m_nSize; // Set to sizeof( Compositor_FrameTiming )
        uint32_t m_nFrameIndex;
        uint32_t m_nNumFramePresents; // number of times this frame was presented
        uint32_t m_nNumMisPresented; // number of times this frame was presented on a vsync other than it was originally predicted to
        uint32_t m_nNumDroppedFrames; // number of additional times previous frame was scanned out
        uint32_t m_nReprojectionFlags;

        /** Absolute time reference for comparing frames.  This aligns with the vsync that running start is relative to. */
        double m_flSystemTimeInSeconds;

        /** These times may include work from other processes due to OS scheduling.
        * The fewer packets of work these are broken up into, the less likely this will happen.
        * GPU work can be broken up by calling Flush.  This can sometimes be useful to get the GPU started
        * processing that work earlier in the frame. */
        float m_flPreSubmitGpuMs; // time spent rendering the scene (gpu work submitted between WaitGetPoses and second Submit)
        float m_flPostSubmitGpuMs; // additional time spent rendering by application (e.g. companion window)
        float m_flTotalRenderGpuMs; // time between work submitted immediately after present (ideally vsync) until the end of compositor submitted work
        float m_flCompositorRenderGpuMs; // time spend performing distortion correction, rendering chaperone, overlays, etc.
        float m_flCompositorRenderCpuMs; // time spent on cpu submitting the above work for this frame
        float m_flCompositorIdleCpuMs; // time spent waiting for running start (application could have used this much more time)

                                       /** Miscellaneous measured intervals. */
        float m_flClientFrameIntervalMs; // time between calls to WaitGetPoses
        float m_flPresentCallCpuMs; // time blocked on call to present (usually 0.0, but can go long)
        float m_flWaitForPresentCpuMs; // time spent spin-waiting for frame index to change (not near-zero indicates wait object failure)
        float m_flSubmitFrameMs; // time spent in IVRCompositor::Submit (not near-zero indicates driver issue)

                                 /** The following are all relative to this frame's SystemTimeInSeconds */
        float m_flWaitGetPosesCalledMs;
        float m_flNewPosesReadyMs;
        float m_flNewFrameReadyMs; // second call to IVRCompositor::Submit
        float m_flCompositorUpdateStartMs;
        float m_flCompositorUpdateEndMs;
        float m_flCompositorRenderStartMs;

        vr::TrackedDevicePose_t m_HmdPose; // pose used by app to render this frame
    };
    /** Cumulative stats for current application.  These are not cleared until a new app connects,
    * but they do stop accumulating once the associated app disconnects. */
    struct Compositor_CumulativeStats
    {
        uint32_t m_nPid; // Process id associated with these stats (may no longer be running).
        uint32_t m_nNumFramePresents; // total number of times we called present (includes reprojected frames)
        uint32_t m_nNumDroppedFrames; // total number of times an old frame was re-scanned out (without reprojection)
        uint32_t m_nNumReprojectedFrames; // total number of times a frame was scanned out a second time (with reprojection)

                                          /** Values recorded at startup before application has fully faded in the first time. */
        uint32_t m_nNumFramePresentsOnStartup;
        uint32_t m_nNumDroppedFramesOnStartup;
        uint32_t m_nNumReprojectedFramesOnStartup;

        /** Applications may explicitly fade to the compositor.  This is usually to handle level transitions, and loading often causes
        * system wide hitches.  The following stats are collected during this period.  Does not include values recorded during startup. */
        uint32_t m_nNumLoading;
        uint32_t m_nNumFramePresentsLoading;
        uint32_t m_nNumDroppedFramesLoading;
        uint32_t m_nNumReprojectedFramesLoading;

        /** If we don't get a new frame from the app in less than 2.5 frames, then we assume the app has hung and start
        * fading back to the compositor.  The following stats are a result of this, and are a subset of those recorded above.
        * Does not include values recorded during start up or loading. */
        uint32_t m_nNumTimedOut;
        uint32_t m_nNumFramePresentsTimedOut;
        uint32_t m_nNumDroppedFramesTimedOut;
        uint32_t m_nNumReprojectedFramesTimedOut;
    };
}