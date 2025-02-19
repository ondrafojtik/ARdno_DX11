// WARNING: Please don't edit this file. It was generated by C++/WinRT v2.0.220224.4

#pragma once
#ifndef WINRT_Windows_Graphics_Display_Core_0_H
#define WINRT_Windows_Graphics_Display_Core_0_H
WINRT_EXPORT namespace winrt::Windows::Foundation
{
    struct EventRegistrationToken;
    struct IAsyncAction;
    template <typename TResult> struct __declspec(empty_bases) IAsyncOperation;
    template <typename TSender, typename TResult> struct __declspec(empty_bases) TypedEventHandler;
}
WINRT_EXPORT namespace winrt::Windows::Foundation::Collections
{
    template <typename T> struct __declspec(empty_bases) IVectorView;
}
WINRT_EXPORT namespace winrt::Windows::Graphics::Display::Core
{
    enum class HdmiDisplayColorSpace : int32_t
    {
        RgbLimited = 0,
        RgbFull = 1,
        BT2020 = 2,
        BT709 = 3,
    };
    enum class HdmiDisplayHdrOption : int32_t
    {
        None = 0,
        EotfSdr = 1,
        Eotf2084 = 2,
        DolbyVisionLowLatency = 3,
    };
    enum class HdmiDisplayPixelEncoding : int32_t
    {
        Rgb444 = 0,
        Ycc444 = 1,
        Ycc422 = 2,
        Ycc420 = 3,
    };
    struct IHdmiDisplayInformation;
    struct IHdmiDisplayInformationStatics;
    struct IHdmiDisplayMode;
    struct IHdmiDisplayMode2;
    struct HdmiDisplayInformation;
    struct HdmiDisplayMode;
    struct HdmiDisplayHdr2086Metadata;
}
namespace winrt::impl
{
    template <> struct category<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation>{ using type = interface_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformationStatics>{ using type = interface_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode>{ using type = interface_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode2>{ using type = interface_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation>{ using type = class_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayMode>{ using type = class_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayColorSpace>{ using type = enum_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayHdrOption>{ using type = enum_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayPixelEncoding>{ using type = enum_category; };
    template <> struct category<winrt::Windows::Graphics::Display::Core::HdmiDisplayHdr2086Metadata>{ using type = struct_category<uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>; };
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation> = L"Windows.Graphics.Display.Core.HdmiDisplayInformation";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayMode> = L"Windows.Graphics.Display.Core.HdmiDisplayMode";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayColorSpace> = L"Windows.Graphics.Display.Core.HdmiDisplayColorSpace";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayHdrOption> = L"Windows.Graphics.Display.Core.HdmiDisplayHdrOption";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayPixelEncoding> = L"Windows.Graphics.Display.Core.HdmiDisplayPixelEncoding";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::HdmiDisplayHdr2086Metadata> = L"Windows.Graphics.Display.Core.HdmiDisplayHdr2086Metadata";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation> = L"Windows.Graphics.Display.Core.IHdmiDisplayInformation";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformationStatics> = L"Windows.Graphics.Display.Core.IHdmiDisplayInformationStatics";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode> = L"Windows.Graphics.Display.Core.IHdmiDisplayMode";
    template <> inline constexpr auto& name_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode2> = L"Windows.Graphics.Display.Core.IHdmiDisplayMode2";
    template <> inline constexpr guid guid_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation>{ 0x130B3C0A,0xF565,0x476E,{ 0xAB,0xD5,0xEA,0x05,0xAE,0xE7,0x4C,0x69 } }; // 130B3C0A-F565-476E-ABD5-EA05AEE74C69
    template <> inline constexpr guid guid_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformationStatics>{ 0x6CE6B260,0xF42A,0x4A15,{ 0x91,0x4C,0x7B,0x8E,0x2A,0x5A,0x65,0xDF } }; // 6CE6B260-F42A-4A15-914C-7B8E2A5A65DF
    template <> inline constexpr guid guid_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode>{ 0x0C06D5AD,0x1B90,0x4F51,{ 0x99,0x81,0xEF,0x5A,0x1C,0x0D,0xDF,0x66 } }; // 0C06D5AD-1B90-4F51-9981-EF5A1C0DDF66
    template <> inline constexpr guid guid_v<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode2>{ 0x07CD4E9F,0x4B3C,0x42B8,{ 0x84,0xE7,0x89,0x53,0x68,0x71,0x8A,0xF2 } }; // 07CD4E9F-4B3C-42B8-84E7-895368718AF2
    template <> struct default_interface<winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation>{ using type = winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation; };
    template <> struct default_interface<winrt::Windows::Graphics::Display::Core::HdmiDisplayMode>{ using type = winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode; };
    template <> struct abi<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation>
    {
        struct __declspec(novtable) type : inspectable_abi
        {
            virtual int32_t __stdcall GetSupportedDisplayModes(void**) noexcept = 0;
            virtual int32_t __stdcall GetCurrentDisplayMode(void**) noexcept = 0;
            virtual int32_t __stdcall SetDefaultDisplayModeAsync(void**) noexcept = 0;
            virtual int32_t __stdcall RequestSetCurrentDisplayModeAsync(void*, void**) noexcept = 0;
            virtual int32_t __stdcall RequestSetCurrentDisplayModeWithHdrAsync(void*, int32_t, void**) noexcept = 0;
            virtual int32_t __stdcall RequestSetCurrentDisplayModeWithHdrAndMetadataAsync(void*, int32_t, struct struct_Windows_Graphics_Display_Core_HdmiDisplayHdr2086Metadata, void**) noexcept = 0;
            virtual int32_t __stdcall add_DisplayModesChanged(void*, winrt::event_token*) noexcept = 0;
            virtual int32_t __stdcall remove_DisplayModesChanged(winrt::event_token) noexcept = 0;
        };
    };
    template <> struct abi<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformationStatics>
    {
        struct __declspec(novtable) type : inspectable_abi
        {
            virtual int32_t __stdcall GetForCurrentView(void**) noexcept = 0;
        };
    };
    template <> struct abi<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode>
    {
        struct __declspec(novtable) type : inspectable_abi
        {
            virtual int32_t __stdcall get_ResolutionWidthInRawPixels(uint32_t*) noexcept = 0;
            virtual int32_t __stdcall get_ResolutionHeightInRawPixels(uint32_t*) noexcept = 0;
            virtual int32_t __stdcall get_RefreshRate(double*) noexcept = 0;
            virtual int32_t __stdcall get_StereoEnabled(bool*) noexcept = 0;
            virtual int32_t __stdcall get_BitsPerPixel(uint16_t*) noexcept = 0;
            virtual int32_t __stdcall IsEqual(void*, bool*) noexcept = 0;
            virtual int32_t __stdcall get_ColorSpace(int32_t*) noexcept = 0;
            virtual int32_t __stdcall get_PixelEncoding(int32_t*) noexcept = 0;
            virtual int32_t __stdcall get_IsSdrLuminanceSupported(bool*) noexcept = 0;
            virtual int32_t __stdcall get_IsSmpte2084Supported(bool*) noexcept = 0;
            virtual int32_t __stdcall get_Is2086MetadataSupported(bool*) noexcept = 0;
        };
    };
    template <> struct abi<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode2>
    {
        struct __declspec(novtable) type : inspectable_abi
        {
            virtual int32_t __stdcall get_IsDolbyVisionLowLatencySupported(bool*) noexcept = 0;
        };
    };
    template <typename D>
    struct consume_Windows_Graphics_Display_Core_IHdmiDisplayInformation
    {
        WINRT_IMPL_AUTO(winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Graphics::Display::Core::HdmiDisplayMode>) GetSupportedDisplayModes() const;
        WINRT_IMPL_AUTO(winrt::Windows::Graphics::Display::Core::HdmiDisplayMode) GetCurrentDisplayMode() const;
        WINRT_IMPL_AUTO(winrt::Windows::Foundation::IAsyncAction) SetDefaultDisplayModeAsync() const;
        WINRT_IMPL_AUTO(winrt::Windows::Foundation::IAsyncOperation<bool>) RequestSetCurrentDisplayModeAsync(winrt::Windows::Graphics::Display::Core::HdmiDisplayMode const& mode) const;
        WINRT_IMPL_AUTO(winrt::Windows::Foundation::IAsyncOperation<bool>) RequestSetCurrentDisplayModeAsync(winrt::Windows::Graphics::Display::Core::HdmiDisplayMode const& mode, winrt::Windows::Graphics::Display::Core::HdmiDisplayHdrOption const& hdrOption) const;
        WINRT_IMPL_AUTO(winrt::Windows::Foundation::IAsyncOperation<bool>) RequestSetCurrentDisplayModeAsync(winrt::Windows::Graphics::Display::Core::HdmiDisplayMode const& mode, winrt::Windows::Graphics::Display::Core::HdmiDisplayHdrOption const& hdrOption, winrt::Windows::Graphics::Display::Core::HdmiDisplayHdr2086Metadata const& hdrMetadata) const;
        WINRT_IMPL_AUTO(winrt::event_token) DisplayModesChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation, winrt::Windows::Foundation::IInspectable> const& value) const;
        using DisplayModesChanged_revoker = impl::event_revoker<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation, &impl::abi_t<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation>::remove_DisplayModesChanged>;
        [[nodiscard]] DisplayModesChanged_revoker DisplayModesChanged(auto_revoke_t, winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation, winrt::Windows::Foundation::IInspectable> const& value) const;
        WINRT_IMPL_AUTO(void) DisplayModesChanged(winrt::event_token const& token) const noexcept;
    };
    template <> struct consume<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformation>
    {
        template <typename D> using type = consume_Windows_Graphics_Display_Core_IHdmiDisplayInformation<D>;
    };
    template <typename D>
    struct consume_Windows_Graphics_Display_Core_IHdmiDisplayInformationStatics
    {
        WINRT_IMPL_AUTO(winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation) GetForCurrentView() const;
    };
    template <> struct consume<winrt::Windows::Graphics::Display::Core::IHdmiDisplayInformationStatics>
    {
        template <typename D> using type = consume_Windows_Graphics_Display_Core_IHdmiDisplayInformationStatics<D>;
    };
    template <typename D>
    struct consume_Windows_Graphics_Display_Core_IHdmiDisplayMode
    {
        [[nodiscard]] WINRT_IMPL_AUTO(uint32_t) ResolutionWidthInRawPixels() const;
        [[nodiscard]] WINRT_IMPL_AUTO(uint32_t) ResolutionHeightInRawPixels() const;
        [[nodiscard]] WINRT_IMPL_AUTO(double) RefreshRate() const;
        [[nodiscard]] WINRT_IMPL_AUTO(bool) StereoEnabled() const;
        [[nodiscard]] WINRT_IMPL_AUTO(uint16_t) BitsPerPixel() const;
        WINRT_IMPL_AUTO(bool) IsEqual(winrt::Windows::Graphics::Display::Core::HdmiDisplayMode const& mode) const;
        [[nodiscard]] WINRT_IMPL_AUTO(winrt::Windows::Graphics::Display::Core::HdmiDisplayColorSpace) ColorSpace() const;
        [[nodiscard]] WINRT_IMPL_AUTO(winrt::Windows::Graphics::Display::Core::HdmiDisplayPixelEncoding) PixelEncoding() const;
        [[nodiscard]] WINRT_IMPL_AUTO(bool) IsSdrLuminanceSupported() const;
        [[nodiscard]] WINRT_IMPL_AUTO(bool) IsSmpte2084Supported() const;
        [[nodiscard]] WINRT_IMPL_AUTO(bool) Is2086MetadataSupported() const;
    };
    template <> struct consume<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode>
    {
        template <typename D> using type = consume_Windows_Graphics_Display_Core_IHdmiDisplayMode<D>;
    };
    template <typename D>
    struct consume_Windows_Graphics_Display_Core_IHdmiDisplayMode2
    {
        [[nodiscard]] WINRT_IMPL_AUTO(bool) IsDolbyVisionLowLatencySupported() const;
    };
    template <> struct consume<winrt::Windows::Graphics::Display::Core::IHdmiDisplayMode2>
    {
        template <typename D> using type = consume_Windows_Graphics_Display_Core_IHdmiDisplayMode2<D>;
    };
    struct struct_Windows_Graphics_Display_Core_HdmiDisplayHdr2086Metadata
    {
        uint16_t RedPrimaryX;
        uint16_t RedPrimaryY;
        uint16_t GreenPrimaryX;
        uint16_t GreenPrimaryY;
        uint16_t BluePrimaryX;
        uint16_t BluePrimaryY;
        uint16_t WhitePointX;
        uint16_t WhitePointY;
        uint16_t MaxMasteringLuminance;
        uint16_t MinMasteringLuminance;
        uint16_t MaxContentLightLevel;
        uint16_t MaxFrameAverageLightLevel;
    };
    template <> struct abi<Windows::Graphics::Display::Core::HdmiDisplayHdr2086Metadata>
    {
        using type = struct_Windows_Graphics_Display_Core_HdmiDisplayHdr2086Metadata;
    };
}
#endif
