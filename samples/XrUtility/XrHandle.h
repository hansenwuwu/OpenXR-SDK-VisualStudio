//*********************************************************
//    Copyright (c) Microsoft. All rights reserved.
//
//    Apache 2.0 License
//
//    You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//*********************************************************
#pragma once

namespace xr {
    template <typename HandleType, XrResult(XRAPI_PTR* DestroyFunction)(HandleType)>
    class UniqueHandle {
    public:
        UniqueHandle() = default;
        UniqueHandle(const UniqueHandle&) = delete;
        UniqueHandle(UniqueHandle&& other) noexcept {
            *this = std::move(other);
        }

        ~UniqueHandle() noexcept {
            Reset();
        }

        UniqueHandle& operator=(const UniqueHandle&) = delete;
        UniqueHandle& operator=(UniqueHandle&& other) noexcept {
            if (m_handle != other.m_handle) {
                Reset();

                m_handle = other.m_handle;
                other.m_handle = XR_NULL_HANDLE;
            }
            return *this;
        }

        HandleType Get() const noexcept {
            return m_handle;
        }

        HandleType* Put() noexcept {
            Reset();
            return &m_handle;
        }

        void Reset() noexcept {
            if (m_handle != XR_NULL_HANDLE) {
                DestroyFunction(m_handle);
                m_handle = XR_NULL_HANDLE;
            }
        }

    private:
        HandleType m_handle{XR_NULL_HANDLE};
    };

    // Extension functions cannot be statically linked, so the creator must pass in the destroy function.
    template <typename HandleType>
    class UniqueExtHandle {
        using PFN_DestroyFunction = XrResult(XRAPI_PTR*)(HandleType);

    public:
        UniqueExtHandle() = default;
        UniqueExtHandle(const UniqueExtHandle&) = delete;
        UniqueExtHandle(UniqueExtHandle&& other) noexcept {
            *this = std::move(other);
        }

        ~UniqueExtHandle() noexcept {
            Reset();
        }

        UniqueExtHandle& operator=(const UniqueExtHandle&) = delete;
        UniqueExtHandle& operator=(UniqueExtHandle&& other) noexcept {
            if (m_handle != other.m_handle || m_destroyer != other.m_destroyer) {
                Reset();

                m_handle = other.m_handle;
                m_destroyer = other.m_destroyer;

                other.m_handle = XR_NULL_HANDLE;
                other.m_destroyer = nullptr;
            }
            return *this;
        }

        HandleType Get() const noexcept {
            return m_handle;
        }

        HandleType* Put(PFN_DestroyFunction destroyFunction) noexcept {
            assert(destroyFunction != nullptr);
            Reset();
            m_destroyer = destroyFunction;
            return &m_handle;
        }

        void Reset() noexcept {
            if (m_handle != XR_NULL_HANDLE) {
                m_destroyer(m_handle);
                m_handle = XR_NULL_HANDLE;
            }

            m_destroyer = nullptr;
        }

    private:
        HandleType m_handle{XR_NULL_HANDLE};
        PFN_DestroyFunction m_destroyer{nullptr};
    };

    class ActionHandle : public UniqueHandle<XrAction, xrDestroyAction> {};
    class ActionSetHandle : public UniqueHandle<XrActionSet, xrDestroyActionSet> {};
    class InstanceHandle : public UniqueHandle<XrInstance, xrDestroyInstance> {};
    class SessionHandle : public UniqueHandle<XrSession, xrDestroySession> {};
    class SpaceHandle : public UniqueHandle<XrSpace, xrDestroySpace> {};
    class SwapchainHandle : public UniqueHandle<XrSwapchain, xrDestroySwapchain> {};
    class SpatialAnchorHandle : public UniqueExtHandle<XrSpatialAnchorMSFT> {};
} // namespace xr
