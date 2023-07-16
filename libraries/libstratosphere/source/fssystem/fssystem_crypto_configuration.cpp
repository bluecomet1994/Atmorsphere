/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stratosphere.hpp>
#include "fssystem_key_slot_cache.hpp"

namespace ams::fssystem {

    namespace {

        constexpr inline const size_t KeySize = crypto::AesDecryptor128::KeySize;

        constexpr inline const size_t AcidSignatureKeyGenerationMax = 1;

        constexpr inline const u8 AcidSignatureKeyModulusDev[AcidSignatureKeyGenerationMax + 1][AcidSignatureKeyModulusSize] = {
            {
                0xD6, 0x34, 0xA5, 0x78, 0x6C, 0x68, 0xCE, 0x5A, 0xC2, 0x37, 0x17, 0xF3, 0x82, 0x45, 0xC6, 0x89,
                0xE1, 0x2D, 0x06, 0x67, 0xBF, 0xB4, 0x06, 0x19, 0x55, 0x6B, 0x27, 0x66, 0x0C, 0xA4, 0xB5, 0x87,
                0x81, 0x25, 0xF4, 0x30, 0xBC, 0x53, 0x08, 0x68, 0xA2, 0x48, 0x49, 0x8C, 0x3F, 0x38, 0x40, 0x9C,
                0xC4, 0x26, 0xF4, 0x79, 0xE2, 0xA1, 0x85, 0xF5, 0x5C, 0x7F, 0x58, 0xBA, 0xA6, 0x1C, 0xA0, 0x8B,
                0x84, 0x16, 0x14, 0x6F, 0x85, 0xD9, 0x7C, 0xE1, 0x3C, 0x67, 0x22, 0x1E, 0xFB, 0xD8, 0xA7, 0xA5,
                0x9A, 0xBF, 0xEC, 0x0E, 0xCF, 0x96, 0x7E, 0x85, 0xC2, 0x1D, 0x49, 0x5D, 0x54, 0x26, 0xCB, 0x32,
                0x7C, 0xF6, 0xBB, 0x58, 0x03, 0x80, 0x2B, 0x5D, 0xF7, 0xFB, 0xD1, 0x9D, 0xC7, 0xC6, 0x2E, 0x53,
                0xC0, 0x6F, 0x39, 0x2C, 0x1F, 0xA9, 0x92, 0xF2, 0x4D, 0x7D, 0x4E, 0x74, 0xFF, 0xE4, 0xEF, 0xE4,
                0x7C, 0x3D, 0x34, 0x2A, 0x71, 0xA4, 0x97, 0x59, 0xFF, 0x4F, 0xA2, 0xF4, 0x66, 0x78, 0xD8, 0xBA,
                0x99, 0xE3, 0xE6, 0xDB, 0x54, 0xB9, 0xE9, 0x54, 0xA1, 0x70, 0xFC, 0x05, 0x1F, 0x11, 0x67, 0x4B,
                0x26, 0x8C, 0x0C, 0x3E, 0x03, 0xD2, 0xA3, 0x55, 0x5C, 0x7D, 0xC0, 0x5D, 0x9D, 0xFF, 0x13, 0x2F,
                0xFD, 0x19, 0xBF, 0xED, 0x44, 0xC3, 0x8C, 0xA7, 0x28, 0xCB, 0xE5, 0xE0, 0xB1, 0xA7, 0x9C, 0x33,
                0x8D, 0xB8, 0x6E, 0xDE, 0x87, 0x18, 0x22, 0x60, 0xC4, 0xAE, 0xF2, 0x87, 0x9F, 0xCE, 0x09, 0x5C,
                0xB5, 0x99, 0xA5, 0x9F, 0x49, 0xF2, 0xD7, 0x58, 0xFA, 0xF9, 0xC0, 0x25, 0x7D, 0xD6, 0xCB, 0xF3,
                0xD8, 0x6C, 0xA2, 0x69, 0x91, 0x68, 0x73, 0xB1, 0x94, 0x6F, 0xA3, 0xF3, 0xB9, 0x7D, 0xF8, 0xE0,
                0x72, 0x9E, 0x93, 0x7B, 0x7A, 0xA2, 0x57, 0x60, 0xB7, 0x5B, 0xA9, 0x84, 0xAE, 0x64, 0x88, 0x69
            },
            {
                0xBC, 0xA5, 0x6A, 0x7E, 0xEA, 0x38, 0x34, 0x62, 0xA6, 0x10, 0x18, 0x3C, 0xE1, 0x63, 0x7B, 0xF0,
                0xD3, 0x08, 0x8C, 0xF5, 0xC5, 0xC4, 0xC7, 0x93, 0xE9, 0xD9, 0xE6, 0x32, 0xF3, 0xA0, 0xF6, 0x6E,
                0x8A, 0x98, 0x76, 0x47, 0x33, 0x47, 0x65, 0x02, 0x70, 0xDC, 0x86, 0x5F, 0x3D, 0x61, 0x5A, 0x70,
                0xBC, 0x5A, 0xCA, 0xCA, 0x50, 0xAD, 0x61, 0x7E, 0xC9, 0xEC, 0x27, 0xFF, 0xE8, 0x64, 0x42, 0x9A,
                0xEE, 0xBE, 0xC3, 0xD1, 0x0B, 0xC0, 0xE9, 0xBF, 0x83, 0x8D, 0xC0, 0x0C, 0xD8, 0x00, 0x5B, 0x76,
                0x90, 0xD2, 0x4B, 0x30, 0x84, 0x35, 0x8B, 0x1E, 0x20, 0xB7, 0xE4, 0xDC, 0x63, 0xE5, 0xDF, 0xCD,
                0x00, 0x5F, 0x81, 0x5F, 0x67, 0xC5, 0x8B, 0xDF, 0xFC, 0xE1, 0x37, 0x5F, 0x07, 0xD9, 0xDE, 0x4F,
                0xE6, 0x7B, 0xF1, 0xFB, 0xA1, 0x5A, 0x71, 0x40, 0xFE, 0xBA, 0x1E, 0xAE, 0x13, 0x22, 0xD2, 0xFE,
                0x37, 0xA2, 0xB6, 0x8B, 0xAB, 0xEB, 0x84, 0x81, 0x4E, 0x7C, 0x1E, 0x02, 0xD1, 0xFB, 0xD7, 0x5D,
                0x11, 0x84, 0x64, 0xD2, 0x4D, 0xBB, 0x50, 0x00, 0x67, 0x54, 0xE2, 0x77, 0x89, 0xBA, 0x0B, 0xE7,
                0x05, 0x57, 0x9A, 0x22, 0x5A, 0xEC, 0x76, 0x1C, 0xFD, 0xE8, 0xA8, 0x18, 0x16, 0x41, 0x65, 0x03,
                0xFA, 0xC4, 0xA6, 0x31, 0x5C, 0x1A, 0x7F, 0xAB, 0x11, 0xC8, 0x4A, 0x99, 0xB9, 0xE6, 0xCF, 0x62,
                0x21, 0xA6, 0x72, 0x47, 0xDB, 0xBA, 0x96, 0x26, 0x4E, 0x2E, 0xD4, 0x8C, 0x46, 0xD6, 0xA7, 0x1A,
                0x6C, 0x32, 0xA7, 0xDF, 0x85, 0x1C, 0x03, 0xC3, 0x6D, 0xA9, 0xE9, 0x68, 0xF4, 0x17, 0x1E, 0xB2,
                0x70, 0x2A, 0xA1, 0xE5, 0xE1, 0xF3, 0x8F, 0x6F, 0x63, 0xAC, 0xEB, 0x72, 0x0B, 0x4C, 0x4A, 0x36,
                0x3C, 0x60, 0x91, 0x9F, 0x6E, 0x1C, 0x71, 0xEA, 0xD0, 0x78, 0x78, 0xA0, 0x2E, 0xC6, 0x32, 0x6B
            }
        };

        constexpr inline const u8 AcidSignatureKeyModulusProd[AcidSignatureKeyGenerationMax + 1][AcidSignatureKeyModulusSize] = {
            {
                0xDD, 0xC8, 0xDD, 0xF2, 0x4E, 0x6D, 0xF0, 0xCA, 0x9E, 0xC7, 0x5D, 0xC7, 0x7B, 0xAD, 0xFE, 0x7D,
                0x23, 0x89, 0x69, 0xB6, 0xF2, 0x06, 0xA2, 0x02, 0x88, 0xE1, 0x55, 0x91, 0xAB, 0xCB, 0x4D, 0x50,
                0x2E, 0xFC, 0x9D, 0x94, 0x76, 0xD6, 0x4C, 0xD8, 0xFF, 0x10, 0xFA, 0x5E, 0x93, 0x0A, 0xB4, 0x57,
                0xAC, 0x51, 0xC7, 0x16, 0x66, 0xF4, 0x1A, 0x54, 0xC2, 0xC5, 0x04, 0x3D, 0x1B, 0xFE, 0x30, 0x20,
                0x8A, 0xAC, 0x6F, 0x6F, 0xF5, 0xC7, 0xB6, 0x68, 0xB8, 0xC9, 0x40, 0x6B, 0x42, 0xAD, 0x11, 0x21,
                0xE7, 0x8B, 0xE9, 0x75, 0x01, 0x86, 0xE4, 0x48, 0x9B, 0x0A, 0x0A, 0xF8, 0x7F, 0xE8, 0x87, 0xF2,
                0x82, 0x01, 0xE6, 0xA3, 0x0F, 0xE4, 0x66, 0xAE, 0x83, 0x3F, 0x4E, 0x9F, 0x5E, 0x01, 0x30, 0xA4,
                0x00, 0xB9, 0x9A, 0xAE, 0x5F, 0x03, 0xCC, 0x18, 0x60, 0xE5, 0xEF, 0x3B, 0x5E, 0x15, 0x16, 0xFE,
                0x1C, 0x82, 0x78, 0xB5, 0x2F, 0x47, 0x7C, 0x06, 0x66, 0x88, 0x5D, 0x35, 0xA2, 0x67, 0x20, 0x10,
                0xE7, 0x6C, 0x43, 0x68, 0xD3, 0xE4, 0x5A, 0x68, 0x2A, 0x5A, 0xE2, 0x6D, 0x73, 0xB0, 0x31, 0x53,
                0x1C, 0x20, 0x09, 0x44, 0xF5, 0x1A, 0x9D, 0x22, 0xBE, 0x12, 0xA1, 0x77, 0x11, 0xE2, 0xA1, 0xCD,
                0x40, 0x9A, 0xA2, 0x8B, 0x60, 0x9B, 0xEF, 0xA0, 0xD3, 0x48, 0x63, 0xA2, 0xF8, 0xA3, 0x2C, 0x08,
                0x56, 0x52, 0x2E, 0x60, 0x19, 0x67, 0x5A, 0xA7, 0x9F, 0xDC, 0x3F, 0x3F, 0x69, 0x2B, 0x31, 0x6A,
                0xB7, 0x88, 0x4A, 0x14, 0x84, 0x80, 0x33, 0x3C, 0x9D, 0x44, 0xB7, 0x3F, 0x4C, 0xE1, 0x75, 0xEA,
                0x37, 0xEA, 0xE8, 0x1E, 0x7C, 0x77, 0xB7, 0xC6, 0x1A, 0xA2, 0xF0, 0x9F, 0x10, 0x61, 0xCD, 0x7B,
                0x5B, 0x32, 0x4C, 0x37, 0xEF, 0xB1, 0x71, 0x68, 0x53, 0x0A, 0xED, 0x51, 0x7D, 0x35, 0x22, 0xFD
            },
            {
                0xE7, 0xAA, 0x25, 0xC8, 0x01, 0xA5, 0x14, 0x6B, 0x01, 0x60, 0x3E, 0xD9, 0x96, 0x5A, 0xBF, 0x90,
                0xAC, 0xA7, 0xFD, 0x9B, 0x5B, 0xBD, 0x8A, 0x26, 0xB0, 0xCB, 0x20, 0x28, 0x9A, 0x72, 0x12, 0xF5,
                0x20, 0x65, 0xB3, 0xB9, 0x84, 0x58, 0x1F, 0x27, 0xBC, 0x7C, 0xA2, 0xC9, 0x9E, 0x18, 0x95, 0xCF,
                0xC2, 0x73, 0x2E, 0x74, 0x8C, 0x66, 0xE5, 0x9E, 0x79, 0x2B, 0xB8, 0x07, 0x0C, 0xB0, 0x4E, 0x8E,
                0xAB, 0x85, 0x21, 0x42, 0xC4, 0xC5, 0x6D, 0x88, 0x9C, 0xDB, 0x15, 0x95, 0x3F, 0x80, 0xDB, 0x7A,
                0x9A, 0x7D, 0x41, 0x56, 0x25, 0x17, 0x18, 0x42, 0x4D, 0x8C, 0xAC, 0xA5, 0x7B, 0xDB, 0x42, 0x5D,
                0x59, 0x35, 0x45, 0x5D, 0x8A, 0x02, 0xB5, 0x70, 0xC0, 0x72, 0x35, 0x46, 0xD0, 0x1D, 0x60, 0x01,
                0x4A, 0xCC, 0x1C, 0x46, 0xD3, 0xD6, 0x35, 0x52, 0xD6, 0xE1, 0xF8, 0x3B, 0x5D, 0xEA, 0xDD, 0xB8,
                0xFE, 0x7D, 0x50, 0xCB, 0x35, 0x23, 0x67, 0x8B, 0xB6, 0xE4, 0x74, 0xD2, 0x60, 0xFC, 0xFD, 0x43,
                0xBF, 0x91, 0x08, 0x81, 0xC5, 0x4F, 0x5D, 0x16, 0x9A, 0xC4, 0x9A, 0xC6, 0xF6, 0xF3, 0xE1, 0xF6,
                0x5C, 0x07, 0xAA, 0x71, 0x6C, 0x13, 0xA4, 0xB1, 0xB3, 0x66, 0xBF, 0x90, 0x4C, 0x3D, 0xA2, 0xC4,
                0x0B, 0xB8, 0x3D, 0x7A, 0x8C, 0x19, 0xFA, 0xFF, 0x6B, 0xB9, 0x1F, 0x02, 0xCC, 0xB6, 0xD3, 0x0C,
                0x7D, 0x19, 0x1F, 0x47, 0xF9, 0xC7, 0x40, 0x01, 0xFA, 0x46, 0xEA, 0x0B, 0xD4, 0x02, 0xE0, 0x3D,
                0x30, 0x9A, 0x1A, 0x0F, 0xEA, 0xA7, 0x66, 0x55, 0xF7, 0xCB, 0x28, 0xE2, 0xBB, 0x99, 0xE4, 0x83,
                0xC3, 0x43, 0x03, 0xEE, 0xDC, 0x1F, 0x02, 0x23, 0xDD, 0xD1, 0x2D, 0x39, 0xA4, 0x65, 0x75, 0x03,
                0xEF, 0x37, 0x9C, 0x06, 0xD6, 0xFA, 0xA1, 0x15, 0xF0, 0xDB, 0x17, 0x47, 0x26, 0x4F, 0x49, 0x03
            }
        };

        static_assert(sizeof(AcidSignatureKeyModulusProd) == sizeof(AcidSignatureKeyModulusDev));

        constexpr inline const u8 AcidSignatureKeyPublicExponent[] = {
            0x01, 0x00, 0x01
        };

        NcaCryptoConfiguration g_nca_crypto_configuration_dev;
        NcaCryptoConfiguration g_nca_crypto_configuration_prod;

        constexpr inline s32 KeySlotCacheEntryCount = 3;
        constinit KeySlotCache g_key_slot_cache;
        constinit util::optional<KeySlotCacheEntry> g_key_slot_cache_entry[KeySlotCacheEntryCount];

        spl::AccessKey &GetNcaKekAccessKey(s32 key_type) {
            AMS_FUNCTION_LOCAL_STATIC_CONSTINIT(spl::AccessKey, s_nca_kek_access_key_array[KeyAreaEncryptionKeyCount]);
            AMS_FUNCTION_LOCAL_STATIC_CONSTINIT(spl::AccessKey, s_nca_header_kek_access_key);
            AMS_FUNCTION_LOCAL_STATIC_CONSTINIT(spl::AccessKey, s_invalid_nca_kek_access_key);

            if (key_type > static_cast<s32>(KeyType::NcaHeaderKey2) || IsInvalidKeyTypeValue(key_type)) {
                return s_invalid_nca_kek_access_key;
            } else if (key_type == static_cast<s32>(KeyType::NcaHeaderKey1) || key_type == static_cast<s32>(KeyType::NcaHeaderKey2)) {
                return s_nca_header_kek_access_key;
            } else {
                return s_nca_kek_access_key_array[key_type];
            }
        }

        void GenerateNcaKey(void *dst, size_t dst_size, const void *src, size_t src_size, s32 key_type) {
            R_ABORT_UNLESS(spl::GenerateAesKey(dst, dst_size, GetNcaKekAccessKey(key_type), src, src_size));
        }

        void ComputeCtr(void *dst, size_t dst_size, int key_slot_idx, const void *src, size_t src_size, const void *iv, size_t iv_size) {
            if (dst == src) {
                /* If the destination and source are the same, we'll use an intermediate buffer. */
                constexpr size_t MinimumSizeToRequireLocking = 256_KB;
                constexpr size_t MinimumWorkBufferSize       = 16_KB;

                /* If the request is large enough, acquire a lock to prevent too many large requests in flight simultaneously. */
                static constinit os::SdkMutex s_large_work_buffer_mutex;
                util::optional<std::scoped_lock<os::SdkMutex>> lk = util::nullopt;
                if (dst_size >= MinimumSizeToRequireLocking) {
                    lk.emplace(s_large_work_buffer_mutex);
                }

                /* Allocate a pooled buffer. */
                PooledBuffer pooled_buffer;
                pooled_buffer.AllocateParticularlyLarge(dst_size, MinimumWorkBufferSize);

                /* Copy the iv locally. */
                AMS_ASSERT(iv_size == crypto::Aes128CtrEncryptor::IvSize);
                u8 work_iv[crypto::Aes128CtrEncryptor::IvSize];
                std::memcpy(work_iv, iv, sizeof(work_iv));

                /* Process all data. */
                size_t processed = 0;
                while (processed < dst_size) {
                    /* Determine the currently processable size. */
                    const size_t cur_size = std::min<size_t>(dst_size - processed, pooled_buffer.GetSize());

                    /* Process. */
                    R_ABORT_UNLESS(spl::ComputeCtr(pooled_buffer.GetBuffer(), cur_size, key_slot_idx, static_cast<const u8 *>(src) + processed, cur_size, work_iv, sizeof(work_iv)));

                    /* Copy to dst. */
                    std::memcpy(static_cast<u8 *>(dst) + processed, pooled_buffer.GetBuffer(), cur_size);

                    /* Advance. */
                    processed += cur_size;

                    /* Increment the counter. */
                    fssystem::AddCounter(work_iv, sizeof(work_iv), cur_size / crypto::Aes128CtrEncryptor::BlockSize);
                }
            } else {
                /* If the destination and source are different, we can just call ComputeCtr directly. */
                R_ABORT_UNLESS(spl::ComputeCtr(dst, dst_size, key_slot_idx, src, src_size, iv, iv_size));
            }
        }

        void DecryptAesCtr(void *dst, size_t dst_size, u8 key_index, u8 key_generation, const void *enc_key, size_t enc_key_size, const void *iv, size_t iv_size, const void *src, size_t src_size) {
            std::unique_ptr<KeySlotCacheAccessor> accessor;

            const s32 key_type = GetKeyTypeValue(key_index, key_generation);

            R_TRY_CATCH(g_key_slot_cache.Find(std::addressof(accessor), enc_key, enc_key_size, key_type)) {
                R_CATCH(fs::ResultTargetNotFound) {
                    R_ABORT_UNLESS(g_key_slot_cache.AllocateHighPriority(std::addressof(accessor), enc_key, enc_key_size, key_type));
                    R_ABORT_UNLESS(spl::LoadAesKey(accessor->GetKeySlotIndex(), GetNcaKekAccessKey(key_type), enc_key, enc_key_size));
                }
            } R_END_TRY_CATCH_WITH_ABORT_UNLESS;

            ComputeCtr(dst, dst_size, accessor->GetKeySlotIndex(), src, src_size, iv, iv_size);
        }

        void DecryptAesCtrForPreparedKey(void *dst, size_t dst_size, u8 key_index, u8 key_generation, const void *enc_key, size_t enc_key_size, const void *iv, size_t iv_size, const void *src, size_t src_size) {
            std::unique_ptr<KeySlotCacheAccessor> accessor;

            AMS_UNUSED(key_index, key_generation);
            const s32 key_type = static_cast<s32>(KeyType::NcaExternalKey);

            R_TRY_CATCH(g_key_slot_cache.Find(std::addressof(accessor), enc_key, enc_key_size, key_type)) {
                R_CATCH(fs::ResultTargetNotFound) {
                    R_ABORT_UNLESS(g_key_slot_cache.AllocateHighPriority(std::addressof(accessor), enc_key, enc_key_size, key_type));

                    spl::AccessKey access_key;
                    AMS_ABORT_UNLESS(enc_key_size == sizeof(access_key));
                    std::memcpy(std::addressof(access_key), enc_key, sizeof(access_key));
                    R_ABORT_UNLESS(spl::LoadPreparedAesKey(accessor->GetKeySlotIndex(), access_key));
                }
            } R_END_TRY_CATCH_WITH_ABORT_UNLESS;

            ComputeCtr(dst, dst_size, accessor->GetKeySlotIndex(), src, src_size, iv, iv_size);
        }

        bool VerifySign1Prod(const void *sig, size_t sig_size, const void *data, size_t data_size, u8 generation) {
            const u8 *mod         = g_nca_crypto_configuration_prod.header_1_sign_key_moduli[generation];
            const size_t mod_size = NcaCryptoConfiguration::Rsa2048KeyModulusSize;
            const u8 *exp         = g_nca_crypto_configuration_prod.header_1_sign_key_public_exponent;
            const size_t exp_size = NcaCryptoConfiguration::Rsa2048KeyPublicExponentSize;

            return crypto::VerifyRsa2048PssSha256(sig, sig_size, mod, mod_size, exp, exp_size, data, data_size);
        }

        bool VerifySign1Dev(const void *sig, size_t sig_size, const void *data, size_t data_size, u8 generation) {
            const u8 *mod         = g_nca_crypto_configuration_dev.header_1_sign_key_moduli[generation];
            const size_t mod_size = NcaCryptoConfiguration::Rsa2048KeyModulusSize;
            const u8 *exp         = g_nca_crypto_configuration_dev.header_1_sign_key_public_exponent;
            const size_t exp_size = NcaCryptoConfiguration::Rsa2048KeyPublicExponentSize;

            return crypto::VerifyRsa2048PssSha256(sig, sig_size, mod, mod_size, exp, exp_size, data, data_size);
        }

    }

    const ::ams::fssystem::NcaCryptoConfiguration *GetNcaCryptoConfiguration(bool prod) {
        /* Decide which configuration to use. */
        NcaCryptoConfiguration * const cfg = prod ? std::addressof(g_nca_crypto_configuration_prod) : std::addressof(g_nca_crypto_configuration_dev);
        std::memcpy(cfg, fssrv::GetDefaultNcaCryptoConfiguration(prod), sizeof(NcaCryptoConfiguration));

        /* Set the key generation functions. */
        cfg->generate_key                  = GenerateNcaKey;
        cfg->decrypt_aes_xts_external      = nullptr;
        cfg->encrypt_aes_xts_external      = nullptr;
        cfg->decrypt_aes_ctr               = DecryptAesCtr;
        cfg->decrypt_aes_ctr_external      = DecryptAesCtrForPreparedKey;
        cfg->verify_sign1                  = prod ? VerifySign1Prod : VerifySign1Dev;
        cfg->is_plaintext_header_available = !prod;
        cfg->is_available_sw_key           = true;

        /* TODO: Should this default to false for host tools with api to set explicitly? */
        #if !defined(ATMOSPHERE_BOARD_NINTENDO_NX)
        cfg->is_unsigned_header_available_for_host_tool = true;
        #endif

        return cfg;
    }

    void SetUpKekAccessKeys(bool prod) {
        /* Get the crypto configuration. */
        const NcaCryptoConfiguration *nca_crypto_cfg = GetNcaCryptoConfiguration(prod);

        /* Setup the nca keys. */
        {
            constexpr s32 Option = 0;

            /* Setup the key area encryption keys. */
            for (u8 i = 0; i < NcaCryptoConfiguration::KeyGenerationMax; ++i) {
                spl::GenerateAesKek(std::addressof(GetNcaKekAccessKey(GetKeyTypeValue(0, i))), nca_crypto_cfg->key_area_encryption_key_source[0], KeySize, i, Option);
                spl::GenerateAesKek(std::addressof(GetNcaKekAccessKey(GetKeyTypeValue(1, i))), nca_crypto_cfg->key_area_encryption_key_source[1], KeySize, i, Option);
                spl::GenerateAesKek(std::addressof(GetNcaKekAccessKey(GetKeyTypeValue(2, i))), nca_crypto_cfg->key_area_encryption_key_source[2], KeySize, i, Option);
            }

            /* Setup the header encryption key. */
            R_ABORT_UNLESS(spl::GenerateAesKek(std::addressof(GetNcaKekAccessKey(static_cast<s32>(KeyType::NcaHeaderKey1))), nca_crypto_cfg->header_encryption_key_source, KeySize, 0, Option));
        }

        /* TODO FS-REIMPL: Save stuff. */

        /* Setup the keyslot cache. */
        for (s32 i = 0; i < KeySlotCacheEntryCount; i++) {
            s32 slot_index = -1;
            R_ABORT_UNLESS(spl::AllocateAesKeySlot(std::addressof(slot_index)));
            g_key_slot_cache_entry[i].emplace(slot_index);
            g_key_slot_cache.AddEntry(std::addressof(g_key_slot_cache_entry[i].value()));
        }
    }

    void InvalidateHardwareAesKey() {
        constexpr u8 InvalidKey[KeySize] = {};
        for (s32 i = 0; i < KeySlotCacheEntryCount; ++i) {
            std::unique_ptr<KeySlotCacheAccessor> accessor;
            R_ABORT_UNLESS(g_key_slot_cache.AllocateHighPriority(std::addressof(accessor), InvalidKey, KeySize, -1 - i));
        }
    }

    const u8 *GetAcidSignatureKeyModulus(bool prod, size_t key_generation) {
        AMS_ASSERT(key_generation <= AcidSignatureKeyGenerationMax);
        const size_t used_keygen = (key_generation % (AcidSignatureKeyGenerationMax + 1));
        return prod ? AcidSignatureKeyModulusProd[used_keygen] : AcidSignatureKeyModulusDev[used_keygen];
    }

    const u8 *GetAcidSignatureKeyPublicExponent() {
        return AcidSignatureKeyPublicExponent;
    }

}
