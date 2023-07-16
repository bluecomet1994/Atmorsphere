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

#pragma once
#include <vapours/svc/svc_common.hpp>

#if defined(ATMOSPHERE_BOARD_NINTENDO_NX)

    #include <vapours/svc/board/nintendo/nx/svc_hardware_constants.hpp>
    namespace ams::svc {
        using namespace ams::svc::board::nintendo::nx;
    }

#elif defined(ATMOSPHERE_BOARD_QEMU_VIRT)

    #include <vapours/svc/board/qemu/virt/svc_hardware_constants.hpp>
    namespace ams::svc {
        using namespace ams::svc::board::qemu::virt;
    }

#else

    #error "Unknown board for svc Hardware Constants"

#endif
