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
#include <stratosphere/fs/fs_common.hpp>

namespace ams::fs {

    /* ACCURATE_TO_VERSION: Unknown */
    class IEventNotifier {
        public:
            virtual ~IEventNotifier() { /* ... */ }

            Result BindEvent(os::SystemEventType *out, os::EventClearMode clear_mode) {
                AMS_ASSERT(out != nullptr);
                R_RETURN(this->DoBindEvent(out, clear_mode));
            }
        private:
            virtual Result DoBindEvent(os::SystemEventType *out, os::EventClearMode clear_mode) = 0;
    };

}
