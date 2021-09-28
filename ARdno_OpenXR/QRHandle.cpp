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

            uint8_t* get_array()
            {
                uint8_t d[16];
				for (int i = 0; i < 4; ++i)
					d[i] = ((uint8_t*)&Data1)[3 - i];

				for (int i = 4; i < 6; ++i)
					d[i] = ((uint8_t*)&Data2)[1 - i];

				for (int i = 6; i < 8; ++i)
					d[i] = ((uint8_t*)&Data3)[1 - i];

				d[8] = Data4[0];
				d[9] = Data4[1];
				d[10] = Data4[2];
				d[11] = Data4[3];
				d[12] = Data4[4];
				d[13] = Data4[5];
				d[14] = Data4[6];
				d[15] = Data4[7];

                return d;
            }

        };

        struct guid__
        {
            uint8_t data[16];

            guid__(QRCode code)
            {

				data[0] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data1 >> 24);
				data[1] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data1 >> 16);
				data[2] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data1 >> 8);
				data[3] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data1 >> 0);

				data[4] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data2 >> 8);
				data[5] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data2 >> 0);

				data[6] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data3 >> 8);
				data[7] = static_cast<uint8_t>(code.SpatialGraphNodeId().Data3 >> 0);

				data[8] = code.SpatialGraphNodeId().Data4[0];
				data[9] = code.SpatialGraphNodeId().Data4[1];
				data[10] = code.SpatialGraphNodeId().Data4[2];
				data[11] = code.SpatialGraphNodeId().Data4[3];
				data[12] = code.SpatialGraphNodeId().Data4[4];
				data[13] = code.SpatialGraphNodeId().Data4[5];
				data[14] = code.SpatialGraphNodeId().Data4[6];
				data[15] = code.SpatialGraphNodeId().Data4[7];

            }

        };


		xr::SpaceHandle target_space{};
        XrSpatialGraphNodeSpaceCreateInfoMSFT* create_info = new XrSpatialGraphNodeSpaceCreateInfoMSFT();
        create_info->type = XR_TYPE_SPATIAL_GRAPH_NODE_SPACE_CREATE_INFO_MSFT;
		guid_ g_data{ args.Code() };
        // note(Ondra): this may be set to XR_SPATIAL_GRAPH_NODE_TYPE_DYNAMIC_MSFT if we want it to move
		create_info->nodeType = XR_SPATIAL_GRAPH_NODE_TYPE_STATIC_MSFT;
        create_info->pose.orientation = xr::math::Quaternion::Identity();
        create_info->pose.position = { 0, 0, 0 };

        // TODO: test this?
        /*
		unsigned char const* GuidToByteArray(GUID const& g)
        {
        	return reinterpret_cast<unsigned char const*>(&g);
        }
        */
        //uint8_t __guid[16] = reinterpret_cast<unsigned char const*>(args.Code().SpatialGraphNodeId());
        //winrt::unbox_value<Collections::IVector<unsigned char>>(args.Code().SpatialGraphNodeId());
		
        //memcpy(create_info->nodeId, (void*)args.Code().SpatialGraphNodeId().Data1, sizeof(guid_));
        //memcpy(create_info->nodeId, (void*)g_data.Data1, sizeof(guid_));
        

        QRVersion version = args.Code().Version();
        winrt::guid g1 = args.Code().Id();
		winrt::guid g2 = args.Code().SpatialGraphNodeId();
        float side_len = args.Code().PhysicalSideLength();
        //winrt::Windows::Foundation::DateTime tim = args.Code().LastDetectedTime();
        //DateTime timee = winrt::unbox_value<DateTime>(args.Code().LastDetectedTime().IInspectable);
        winrt::Windows::Foundation::DateTime a = args.Code().LastDetectedTime();
        
        int64_t tim = args.Code().LastDetectedTime().time_since_epoch().count();
        
        // 200 (OK), 201 (Created), or 204 (No Content). 204 == 0xcc
        //uint8_t data__[16];
        //for (int i = 0; i < 16; i++)
        //    data__[i] = g_data.get_array()[i];
        //// the data might not be in the correct order (little endiang..) -> get_array method
        //std::copy(std::begin(data__), std::end(data__), std::begin(create_info->nodeId));
        
        //memcpy(create_info->nodeId, reinterpret_cast<void*>(&args.Code().SpatialGraphNodeId()), sizeof(uint8_t) * 16);
        
        //memcpy(create_info->nodeId, reinterpret_cast<void*>(&args.Code().SpatialGraphNodeId()), sizeof(uint8_t) * 16);
        // CHECK THIS!! VERY LIKELY IT WORKS!
        //memcpy(create_info->nodeId, (void*)g_data.Data1, sizeof(uint8_t)*16);
        
        memcpy(create_info->nodeId, reinterpret_cast<void*>(&g2), sizeof(uint8_t)*16);
        XrResult res = m_extensions.xrCreateSpatialGraphNodeSpaceMSFT(m_session.Get(), create_info, target_space.Put());

        // tmp
		XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
		res = xrLocateSpace(target_space.Get(), m_appSpace.Get(), time, &spaceLocation);
        
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

/*


*/