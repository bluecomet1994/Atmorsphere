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

namespace ams::time {

    Result GetStandardSteadyClockCurrentTimePoint(SteadyClockTimePoint *out) {
        #if defined(ATMOSPHERE_OS_HORIZON)
        static_assert(sizeof(*out) == sizeof(::TimeSteadyClockTimePoint));
        R_RETURN(::timeGetStandardSteadyClockTimePoint(reinterpret_cast<::TimeSteadyClockTimePoint *>(out)));
        #else
        AMS_UNUSED(out);
        AMS_ABORT("TODO");
        #endif
    }

    TimeSpan GetStandardSteadyClockInternalOffset() {
        TimeSpanType offset;

        #if defined(ATMOSPHERE_OS_HORIZON)
        static_assert(sizeof(TimeSpanType) == sizeof(s64));
        R_ABORT_UNLESS(::timeGetStandardSteadyClockInternalOffset(reinterpret_cast<s64 *>(std::addressof(offset))));
        #else
        AMS_UNUSED(offset);
        AMS_ABORT("TODO");
        #endif

        return offset;
    }

    Result StandardSteadyClock::GetCurrentTimePoint(SteadyClockTimePoint *out) {
        R_RETURN(GetStandardSteadyClockCurrentTimePoint(out));
    }

    StandardSteadyClock::time_point StandardSteadyClock::now() {
        SteadyClockTimePoint steady_clock_time_point = {0, util::InvalidUuid};
        if (R_FAILED(StandardSteadyClock::GetCurrentTimePoint(std::addressof(steady_clock_time_point)))) {
            steady_clock_time_point.value     = 0;
            steady_clock_time_point.source_id = util::InvalidUuid;
        }

        return StandardSteadyClock::time_point(StandardSteadyClock::duration(steady_clock_time_point.value));
    }

}
