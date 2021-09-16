#include "pch.h"
#include "QRHandle.h"

#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

namespace qr_test 
{
	using namespace winrt::Windows::Foundation;
	using namespace winrt::Microsoft::MixedReality::QR;

    void qr_test::QRHandle::initialize()
	{
		if (watcher.IsSupported())
		{
			QRCodeWatcherAccessStatus status = QRCodeWatcher::RequestAccessAsync().get();
			if (status == QRCodeWatcherAccessStatus::Allowed)
			{
				watcher = QRCodeWatcher();
				watcher.Added({ this, &QRHandle::OnAdded });
				watcher.Updated({ this, &QRHandle::OnUpdated });
				watcher.EnumerationCompleted({ this, &QRHandle::OnEnumerationComplete });
				watcher.Start();
			}

		}
	}

	void qr_test::QRHandle::OnAdded(const winrt::Windows::Foundation::IInspectable&, const QRCodeAddedEventArgs& args)
	{
        using namespace winrt::Windows::Perception::Spatial;
        using namespace winrt::Windows::Perception::Spatial::Preview;
        using namespace winrt::Windows::Foundation::Numerics;
        using namespace winrt::Windows::Perception::Spatial;

        struct guid_
        {
            uint32_t Data1;
            uint16_t Data2;
            uint16_t Data3;
            uint8_t  Data4[8];

            guid_(QRCode code)
            {
                uint32_t Data1 = code.SpatialGraphNodeId().Data1;
                uint16_t Data2 = code.SpatialGraphNodeId().Data2;
                uint16_t Data3 = code.SpatialGraphNodeId().Data3;
                uint8_t  Data4[8];
                Data4[0] = code.SpatialGraphNodeId().Data4[0];
                Data4[1] = code.SpatialGraphNodeId().Data4[1];
                Data4[2] = code.SpatialGraphNodeId().Data4[2];
                Data4[3] = code.SpatialGraphNodeId().Data4[3];
                Data4[4] = code.SpatialGraphNodeId().Data4[4];
                Data4[5] = code.SpatialGraphNodeId().Data4[5];
                Data4[6] = code.SpatialGraphNodeId().Data4[6];
                Data4[7] = code.SpatialGraphNodeId().Data4[7];

            }
        };


        QRCode code = args.Code();
        SpatialCoordinateSystem qr_coords = SpatialGraphInteropPreview::CreateCoordinateSystemForNode(code.SpatialGraphNodeId());
        SpatialCoordinateSystem null_space = SpatialGraphInteropPreview::CreateCoordinateSystemForNode(code.SpatialGraphNodeId(), { 10, 0, 0 });
        // winrt::guid const& nodeId, winrt::Windows::Foundation::Numerics::float3 const& relativePosition

        //XrSpace target_space{};
		// note(Ondra): this may be set to XR_SPATIAL_GRAPH_NODE_TYPE_DYNAMIC_MSFT if we want it to move

        xr::SpaceHandle target_space{};
        XrSpatialGraphNodeSpaceCreateInfoMSFT* create_info = new XrSpatialGraphNodeSpaceCreateInfoMSFT();
        create_info->type = XR_TYPE_SPATIAL_GRAPH_NODE_SPACE_CREATE_INFO_MSFT;
        guid_ g_data{ code };
        create_info->nodeType = XR_SPATIAL_GRAPH_NODE_TYPE_STATIC_MSFT;
        memcpy(create_info->nodeId, (void*)g_data.Data1, sizeof(guid_));
        xrGetInstanceProcAddr(m_instance, XR_MSFT_SPATIAL_GRAPH_BRIDGE_EXTENSION_NAME, );
        //xrCreateSpatialGraphNodeSpaceMSFT(m_session.Get(), create_info, target_space.Put());

        //XrSceneObserverMSFT scene_obs{};
        //xr::su::Scene* scene = new xr::su::Scene(m_extensions, scene_obs);
        //std::vector<xr::su::SceneObject> objects_ = scene->GetObjects();
        //for (xr::su::SceneObject o : objects_)
        //{
        //    //if (o.id == code.SpatialGraphNodeId()) // TypedUuid<SceneObject> : guid
        //}

        /*
        xr::su::SceneObserver m_sceneObserver(m_extensions, m_session.Get());
            static const std::vector<xr::su::SceneObject::Kind> kindFilter{ XR_SCENE_OBJECT_KIND_BACKGROUND_MSFT,
                                                                            XR_SCENE_OBJECT_KIND_WALL_MSFT,
                                                                            XR_SCENE_OBJECT_KIND_FLOOR_MSFT,
                                                                            XR_SCENE_OBJECT_KIND_CEILING_MSFT,
                                                                            XR_SCENE_OBJECT_KIND_PLATFORM_MSFT,
                                                                            XR_SCENE_OBJECT_KIND_INFERRED_MSFT };
            //std::vector<xr::su::SceneObject> objects_ = m_sceneObserver.CreateScene().get()->GetObjects(kindFilter);
            xr::SceneBounds bounds{};
            bounds.space = m_appSpace.Get();
            //m_sceneObserver.ComputeNewScene();
            xr::su::Scene* m_scene = m_sceneObserver.CreateScene().get();

            xr::SpaceHandle new_space;
            m_appSpace = new_space;

        */

        /*
        winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::Numerics::float4x4> m = qr_coords.TryGetTransformTo(null_space);
        winrt::Windows::Foundation::Numerics::float4x4 vec = winrt::unbox_value<winrt::Windows::Foundation::Numerics::float4x4>(m);



        XrVector3f position{};
        position.x = vec.m14;
        position.y = vec.m24;
        position.z = vec.m34;

        XrVector3f scale{};
        scale.x = vec.m11;
        scale.y = vec.m22;
        scale.z = vec.m33;
        */

	}


    void qr_test::QRHandle::OnUpdated(const winrt::Windows::Foundation::IInspectable&, const QRCodeUpdatedEventArgs& args)
    {
        ;
    }

    void qr_test::QRHandle::OnEnumerationComplete(const winrt::Windows::Foundation::IInspectable&, const winrt::Windows::Foundation::IInspectable&)
    {
        ;
    }

}