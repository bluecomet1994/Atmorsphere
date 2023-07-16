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

namespace ams::fssrv {

    namespace {

        constexpr inline const u8 HeaderSign1KeyModulusDev[fssystem::NcaCryptoConfiguration::Header1SignatureKeyGenerationMax + 1][fssystem::NcaCryptoConfiguration::Rsa2048KeyModulusSize] = {
            {
                0xD8, 0xF1, 0x18, 0xEF, 0x32, 0x72, 0x4C, 0xA7, 0x47, 0x4C, 0xB9, 0xEA, 0xB3, 0x04, 0xA8, 0xA4,
                0xAC, 0x99, 0x08, 0x08, 0x04, 0xBF, 0x68, 0x57, 0xB8, 0x43, 0x94, 0x2B, 0xC7, 0xB9, 0x66, 0x49,
                0x85, 0xE5, 0x8A, 0x9B, 0xC1, 0x00, 0x9A, 0x6A, 0x8D, 0xD0, 0xEF, 0xCE, 0xFF, 0x86, 0xC8, 0x5C,
                0x5D, 0xE9, 0x53, 0x7B, 0x19, 0x2A, 0xA8, 0xC0, 0x22, 0xD1, 0xF3, 0x22, 0x0A, 0x50, 0xF2, 0x2B,
                0x65, 0x05, 0x1B, 0x9E, 0xEC, 0x61, 0xB5, 0x63, 0xA3, 0x6F, 0x3B, 0xBA, 0x63, 0x3A, 0x53, 0xF4,
                0x49, 0x2F, 0xCF, 0x03, 0xCC, 0xD7, 0x50, 0x82, 0x1B, 0x29, 0x4F, 0x08, 0xDE, 0x1B, 0x6D, 0x47,
                0x4F, 0xA8, 0xB6, 0x6A, 0x26, 0xA0, 0x83, 0x3F, 0x1A, 0xAF, 0x83, 0x8F, 0x0E, 0x17, 0x3F, 0xFE,
                0x44, 0x1C, 0x56, 0x94, 0x2E, 0x49, 0x83, 0x83, 0x03, 0xE9, 0xB6, 0xAD, 0xD5, 0xDE, 0xE3, 0x2D,
                0xA1, 0xD9, 0x66, 0x20, 0x5D, 0x1F, 0x5E, 0x96, 0x5D, 0x5B, 0x55, 0x0D, 0xD4, 0xB4, 0x77, 0x6E,
                0xAE, 0x1B, 0x69, 0xF3, 0xA6, 0x61, 0x0E, 0x51, 0x62, 0x39, 0x28, 0x63, 0x75, 0x76, 0xBF, 0xB0,
                0xD2, 0x22, 0xEF, 0x98, 0x25, 0x02, 0x05, 0xC0, 0xD7, 0x6A, 0x06, 0x2C, 0xA5, 0xD8, 0x5A, 0x9D,
                0x7A, 0xA4, 0x21, 0x55, 0x9F, 0xF9, 0x3E, 0xBF, 0x16, 0xF6, 0x07, 0xC2, 0xB9, 0x6E, 0x87, 0x9E,
                0xB5, 0x1C, 0xBE, 0x97, 0xFA, 0x82, 0x7E, 0xED, 0x30, 0xD4, 0x66, 0x3F, 0xDE, 0xD8, 0x1B, 0x4B,
                0x15, 0xD9, 0xFB, 0x2F, 0x50, 0xF0, 0x9D, 0x1D, 0x52, 0x4C, 0x1C, 0x4D, 0x8D, 0xAE, 0x85, 0x1E,
                0xEA, 0x7F, 0x86, 0xF3, 0x0B, 0x7B, 0x87, 0x81, 0x98, 0x23, 0x80, 0x63, 0x4F, 0x2F, 0xB0, 0x62,
                0xCC, 0x6E, 0xD2, 0x46, 0x13, 0x65, 0x2B, 0xD6, 0x44, 0x33, 0x59, 0xB5, 0x8F, 0xB9, 0x4A, 0xA9
            },
            {
                0x9A, 0xBC, 0x88, 0xBD, 0x0A, 0xBE, 0xD7, 0x0C, 0x9B, 0x42, 0x75, 0x65, 0x38, 0x5E, 0xD1, 0x01,
                0xCD, 0x12, 0xAE, 0xEA, 0xE9, 0x4B, 0xDB, 0xB4, 0x5E, 0x36, 0x10, 0x96, 0xDA, 0x3D, 0x2E, 0x66,
                0xD3, 0x99, 0x13, 0x8A, 0xBE, 0x67, 0x41, 0xC8, 0x93, 0xD9, 0x3E, 0x42, 0xCE, 0x34, 0xCE, 0x96,
                0xFA, 0x0B, 0x23, 0xCC, 0x2C, 0xDF, 0x07, 0x3F, 0x3B, 0x24, 0x4B, 0x12, 0x67, 0x3A, 0x29, 0x36,
                0xA3, 0xAA, 0x06, 0xF0, 0x65, 0xA5, 0x85, 0xBA, 0xFD, 0x12, 0xEC, 0xF1, 0x60, 0x67, 0xF0, 0x8F,
                0xD3, 0x5B, 0x01, 0x1B, 0x1E, 0x84, 0xA3, 0x5C, 0x65, 0x36, 0xF9, 0x23, 0x7E, 0xF3, 0x26, 0x38,
                0x64, 0x98, 0xBA, 0xE4, 0x19, 0x91, 0x4C, 0x02, 0xCF, 0xC9, 0x6D, 0x86, 0xEC, 0x1D, 0x41, 0x69,
                0xDD, 0x56, 0xEA, 0x5C, 0xA3, 0x2A, 0x58, 0xB4, 0x39, 0xCC, 0x40, 0x31, 0xFD, 0xFB, 0x42, 0x74,
                0xF8, 0xEC, 0xEA, 0x00, 0xF0, 0xD9, 0x28, 0xEA, 0xFA, 0x2D, 0x00, 0xE1, 0x43, 0x53, 0xC6, 0x32,
                0xF4, 0xA2, 0x07, 0xD4, 0x5F, 0xD4, 0xCB, 0xAC, 0xCA, 0xFF, 0xDF, 0x84, 0xD2, 0x86, 0x14, 0x3C,
                0xDE, 0x22, 0x75, 0xA5, 0x73, 0xFF, 0x68, 0x07, 0x4A, 0xF9, 0x7C, 0x2C, 0xCC, 0xDE, 0x45, 0xB6,
                0x54, 0x82, 0x90, 0x36, 0x1F, 0x2C, 0x51, 0x96, 0xC5, 0x0A, 0x53, 0x5B, 0xF0, 0x8B, 0x4A, 0xAA,
                0x3B, 0x68, 0x97, 0x19, 0x17, 0x1F, 0x01, 0xB8, 0xED, 0xB9, 0x9A, 0x5E, 0x08, 0xC5, 0x20, 0x1E,
                0x6A, 0x09, 0xF0, 0xE9, 0x73, 0xA3, 0xBE, 0x10, 0x06, 0x02, 0xE9, 0xFB, 0x85, 0xFA, 0x5F, 0x01,
                0xAC, 0x60, 0xE0, 0xED, 0x7D, 0xB9, 0x49, 0xA8, 0x9E, 0x98, 0x7D, 0x91, 0x40, 0x05, 0xCF, 0xF9,
                0x1A, 0xFC, 0x40, 0x22, 0xA8, 0x96, 0x5B, 0xB0, 0xDC, 0x7A, 0xF5, 0xB7, 0xE9, 0x91, 0x4C, 0x49
            }
        };

        constexpr inline const u8 HeaderSign1KeyModulusProd[fssystem::NcaCryptoConfiguration::Header1SignatureKeyGenerationMax + 1][fssystem::NcaCryptoConfiguration::Rsa2048KeyModulusSize] = {
            {
                0xBF, 0xBE, 0x40, 0x6C, 0xF4, 0xA7, 0x80, 0xE9, 0xF0, 0x7D, 0x0C, 0x99, 0x61, 0x1D, 0x77, 0x2F,
                0x96, 0xBC, 0x4B, 0x9E, 0x58, 0x38, 0x1B, 0x03, 0xAB, 0xB1, 0x75, 0x49, 0x9F, 0x2B, 0x4D, 0x58,
                0x34, 0xB0, 0x05, 0xA3, 0x75, 0x22, 0xBE, 0x1A, 0x3F, 0x03, 0x73, 0xAC, 0x70, 0x68, 0xD1, 0x16,
                0xB9, 0x04, 0x46, 0x5E, 0xB7, 0x07, 0x91, 0x2F, 0x07, 0x8B, 0x26, 0xDE, 0xF6, 0x00, 0x07, 0xB2,
                0xB4, 0x51, 0xF8, 0x0D, 0x0A, 0x5E, 0x58, 0xAD, 0xEB, 0xBC, 0x9A, 0xD6, 0x49, 0xB9, 0x64, 0xEF,
                0xA7, 0x82, 0xB5, 0xCF, 0x6D, 0x70, 0x13, 0xB0, 0x0F, 0x85, 0xF6, 0xA9, 0x08, 0xAA, 0x4D, 0x67,
                0x66, 0x87, 0xFA, 0x89, 0xFF, 0x75, 0x90, 0x18, 0x1E, 0x6B, 0x3D, 0xE9, 0x8A, 0x68, 0xC9, 0x26,
                0x04, 0xD9, 0x80, 0xCE, 0x3F, 0x5E, 0x92, 0xCE, 0x01, 0xFF, 0x06, 0x3B, 0xF2, 0xC1, 0xA9, 0x0C,
                0xCE, 0x02, 0x6F, 0x16, 0xBC, 0x92, 0x42, 0x0A, 0x41, 0x64, 0xCD, 0x52, 0xB6, 0x34, 0x4D, 0xAE,
                0xC0, 0x2E, 0xDE, 0xA4, 0xDF, 0x27, 0x68, 0x3C, 0xC1, 0xA0, 0x60, 0xAD, 0x43, 0xF3, 0xFC, 0x86,
                0xC1, 0x3E, 0x6C, 0x46, 0xF7, 0x7C, 0x29, 0x9F, 0xFA, 0xFD, 0xF0, 0xE3, 0xCE, 0x64, 0xE7, 0x35,
                0xF2, 0xF6, 0x56, 0x56, 0x6F, 0x6D, 0xF1, 0xE2, 0x42, 0xB0, 0x83, 0x40, 0xA5, 0xC3, 0x20, 0x2B,
                0xCC, 0x9A, 0xAE, 0xCA, 0xED, 0x4D, 0x70, 0x30, 0xA8, 0x70, 0x1C, 0x70, 0xFD, 0x13, 0x63, 0x29,
                0x02, 0x79, 0xEA, 0xD2, 0xA7, 0xAF, 0x35, 0x28, 0x32, 0x1C, 0x7B, 0xE6, 0x2F, 0x1A, 0xAA, 0x40,
                0x7E, 0x32, 0x8C, 0x27, 0x42, 0xFE, 0x82, 0x78, 0xEC, 0x0D, 0xEB, 0xE6, 0x83, 0x4B, 0x6D, 0x81,
                0x04, 0x40, 0x1A, 0x9E, 0x9A, 0x67, 0xF6, 0x72, 0x29, 0xFA, 0x04, 0xF0, 0x9D, 0xE4, 0xF4, 0x03
            },
            {
                0xAD, 0xE3, 0xE1, 0xFA, 0x04, 0x35, 0xE5, 0xB6, 0xDD, 0x49, 0xEA, 0x89, 0x29, 0xB1, 0xFF, 0xB6,
                0x43, 0xDF, 0xCA, 0x96, 0xA0, 0x4A, 0x13, 0xDF, 0x43, 0xD9, 0x94, 0x97, 0x96, 0x43, 0x65, 0x48,
                0x70, 0x58, 0x33, 0xA2, 0x7D, 0x35, 0x7B, 0x96, 0x74, 0x5E, 0x0B, 0x5C, 0x32, 0x18, 0x14, 0x24,
                0xC2, 0x58, 0xB3, 0x6C, 0x22, 0x7A, 0xA1, 0xB7, 0xCB, 0x90, 0xA7, 0xA3, 0xF9, 0x7D, 0x45, 0x16,
                0xA5, 0xC8, 0xED, 0x8F, 0xAD, 0x39, 0x5E, 0x9E, 0x4B, 0x51, 0x68, 0x7D, 0xF8, 0x0C, 0x35, 0xC6,
                0x3F, 0x91, 0xAE, 0x44, 0xA5, 0x92, 0x30, 0x0D, 0x46, 0xF8, 0x40, 0xFF, 0xD0, 0xFF, 0x06, 0xD2,
                0x1C, 0x7F, 0x96, 0x18, 0xDC, 0xB7, 0x1D, 0x66, 0x3E, 0xD1, 0x73, 0xBC, 0x15, 0x8A, 0x2F, 0x94,
                0xF3, 0x00, 0xC1, 0x83, 0xF1, 0xCD, 0xD7, 0x81, 0x88, 0xAB, 0xDF, 0x8C, 0xEF, 0x97, 0xDD, 0x1B,
                0x17, 0x5F, 0x58, 0xF6, 0x9A, 0xE9, 0xE8, 0xC2, 0x2F, 0x38, 0x15, 0xF5, 0x21, 0x07, 0xF8, 0x37,
                0x90, 0x5D, 0x2E, 0x02, 0x40, 0x24, 0x15, 0x0D, 0x25, 0xB7, 0x26, 0x5D, 0x09, 0xCC, 0x4C, 0xF4,
                0xF2, 0x1B, 0x94, 0x70, 0x5A, 0x9E, 0xEE, 0xED, 0x77, 0x77, 0xD4, 0x51, 0x99, 0xF5, 0xDC, 0x76,
                0x1E, 0xE3, 0x6C, 0x8C, 0xD1, 0x12, 0xD4, 0x57, 0xD1, 0xB6, 0x83, 0xE4, 0xE4, 0xFE, 0xDA, 0xE9,
                0xB4, 0x3B, 0x33, 0xE5, 0x37, 0x8A, 0xDF, 0xB5, 0x7F, 0x89, 0xF1, 0x9B, 0x9E, 0xB0, 0x15, 0xB2,
                0x3A, 0xFE, 0xEA, 0x61, 0x84, 0x5B, 0x7D, 0x4B, 0x23, 0x12, 0x0B, 0x83, 0x12, 0xF2, 0x22, 0x6B,
                0xB9, 0x22, 0x96, 0x4B, 0x26, 0x0B, 0x63, 0x5E, 0x96, 0x57, 0x52, 0xA3, 0x67, 0x64, 0x22, 0xCA,
                0xD0, 0x56, 0x3E, 0x74, 0xB5, 0x98, 0x1F, 0x0D, 0xF8, 0xB3, 0x34, 0xE6, 0x98, 0x68, 0x5A, 0xAD
            }
        };

        constexpr inline const ::ams::fssystem::NcaCryptoConfiguration DefaultNcaCryptoConfigurationDev = {
            /* Header1 Signature Key Moduli */
            { HeaderSign1KeyModulusDev[0], HeaderSign1KeyModulusDev[1] },

            /* Header 1 Signature Key Public Exponent */
            { 0x01, 0x00, 0x01 },

            /* Key Area Encryption Key Sources */
            {
                /* Application */
                { 0x7F, 0x59, 0x97, 0x1E, 0x62, 0x9F, 0x36, 0xA1, 0x30, 0x98, 0x06, 0x6F, 0x21, 0x44, 0xC3, 0x0D },

                /* Ocean */
                { 0x32, 0x7D, 0x36, 0x08, 0x5A, 0xD1, 0x75, 0x8D, 0xAB, 0x4E, 0x6F, 0xBA, 0xA5, 0x55, 0xD8, 0x82 },

                /* System */
                { 0x87, 0x45, 0xF1, 0xBB, 0xA6, 0xBE, 0x79, 0x64, 0x7D, 0x04, 0x8B, 0xA6, 0x7B, 0x5F, 0xDA, 0x4A },
            },

            /* Header Encryption Key Source */
            { 0x1F, 0x12, 0x91, 0x3A, 0x4A, 0xCB, 0xF0, 0x0D, 0x4C, 0xDE, 0x3A, 0xF6, 0xD5, 0x23, 0x88, 0x2A },

            /* Encrypted Header Encryption Key */
            {
                { 0x5A, 0x3E, 0xD8, 0x4F, 0xDE, 0xC0, 0xD8, 0x26, 0x31, 0xF7, 0xE2, 0x5D, 0x19, 0x7B, 0xF5, 0xD0 },
                { 0x1C, 0x9B, 0x7B, 0xFA, 0xF6, 0x28, 0x18, 0x3D, 0x71, 0xF6, 0x4D, 0x73, 0xF1, 0x50, 0xB9, 0xD2 }
            },

            /* Key Generation Function */
            nullptr,

            /* Decrypt Aes Xts Eternal Function */
            nullptr,

            /* Encrypt Aes Xts Eternal Function */
            nullptr,

            /* Decrypt Aes Ctr Function */
            nullptr,

            /* Decrypt Aes Ctr External Function */
            nullptr,

            /* Verify Sign1 Function */
            nullptr,

            /* Plaintext Header Available */
            false,

            /* Software Key Available */
            true,
        };

        constexpr inline const ::ams::fssystem::NcaCryptoConfiguration DefaultNcaCryptoConfigurationProd = {
            /* Header1 Signature Key Moduli */
            { HeaderSign1KeyModulusProd[0], HeaderSign1KeyModulusProd[1] },

            /* Header 1 Signature Key Public Exponent */
            { 0x01, 0x00, 0x01 },

            /* Key Area Encryption Key Sources */
            {
                /* Application */
                { 0x7F, 0x59, 0x97, 0x1E, 0x62, 0x9F, 0x36, 0xA1, 0x30, 0x98, 0x06, 0x6F, 0x21, 0x44, 0xC3, 0x0D },

                /* Ocean */
                { 0x32, 0x7D, 0x36, 0x08, 0x5A, 0xD1, 0x75, 0x8D, 0xAB, 0x4E, 0x6F, 0xBA, 0xA5, 0x55, 0xD8, 0x82 },

                /* System */
                { 0x87, 0x45, 0xF1, 0xBB, 0xA6, 0xBE, 0x79, 0x64, 0x7D, 0x04, 0x8B, 0xA6, 0x7B, 0x5F, 0xDA, 0x4A },
            },

            /* Header Encryption Key Source */
            { 0x1F, 0x12, 0x91, 0x3A, 0x4A, 0xCB, 0xF0, 0x0D, 0x4C, 0xDE, 0x3A, 0xF6, 0xD5, 0x23, 0x88, 0x2A },

            /* Encrypted Header Encryption Key */
            {
                { 0x5A, 0x3E, 0xD8, 0x4F, 0xDE, 0xC0, 0xD8, 0x26, 0x31, 0xF7, 0xE2, 0x5D, 0x19, 0x7B, 0xF5, 0xD0 },
                { 0x1C, 0x9B, 0x7B, 0xFA, 0xF6, 0x28, 0x18, 0x3D, 0x71, 0xF6, 0x4D, 0x73, 0xF1, 0x50, 0xB9, 0xD2 }
            },

            /* Key Generation Function */
            nullptr,

            /* Decrypt Aes Xts Eternal Function */
            nullptr,

            /* Encrypt Aes Xts Eternal Function */
            nullptr,

            /* Decrypt Aes Ctr Function */
            nullptr,

            /* Decrypt Aes Ctr External Function */
            nullptr,

            /* Verify Sign1 Function */
            nullptr,

            /* Plaintext Header Available */
            false,

            /* Software Key Available */
            true,
        };

    }

    const ::ams::fssystem::NcaCryptoConfiguration *GetDefaultNcaCryptoConfiguration(bool prod) {
        return prod ? std::addressof(DefaultNcaCryptoConfigurationProd) : std::addressof(DefaultNcaCryptoConfigurationDev);
    }

}
