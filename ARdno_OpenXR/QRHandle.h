#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

namespace qr_test
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Microsoft::MixedReality::QR;

    struct QRHandle
    {
        QRCodeWatcher watcher{ nullptr };
        // test
        xr::SessionHandle m_session;
        xr::ExtensionDispatchTable m_extensions;
        XrInstance m_instance;
        xr::SpaceHandle m_appSpace;
        XrTime time;

        void initialize();
        void OnAdded(const winrt::Windows::Foundation::IInspectable&, const QRCodeAddedEventArgs& args);
        void OnUpdated(const winrt::Windows::Foundation::IInspectable&, const QRCodeUpdatedEventArgs& args);
        void OnEnumerationComplete(const winrt::Windows::Foundation::IInspectable&, const winrt::Windows::Foundation::IInspectable&);
    };


}


/*

OpenXR - unresolved external symbol


Hello, I keep getting unresolved external symbol on xrCreateSpatialGraphNodeSpaceMSFT and at this point I just ran out of things I could try to make this thing work. I wouldn't call myself an experienced programmer by any means so I guess it is very possible I'm just doing something very stupid. I'll be very thankful for any kind of response. 

I've tried using headers from the NuGet package and the ones on Microsoft/OpenXR-MixedReality.
As for loader, I've tried the NuGet package one and building one myself from KhronosGroup/OpenXR-SDK.
I didn't have any luck with any of these (again, that might be because I did something wrong or juts plain stupid..).
Here's the code just for reference.

xr::SpaceHandle target_space{};
XrSpatialGraphNodeSpaceCreateInfoMSFT* create_info{};
create_info->type = XR_TYPE_SPATIAL_GRAPH_NODE_SPACE_CREATE_INFO_MSFT;
guid_ g_data{ code };
create_info->nodeType = XR_SPATIAL_GRAPH_NODE_TYPE_STATIC_MSFT;
memcpy(create_info->nodeId, (void*)g_data.Data1, sizeof(guid_));
xrCreateSpatialGraphNodeSpaceMSFT(m_session.Get(), create_info, target_space.Put());

*/