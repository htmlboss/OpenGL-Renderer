#pragma once

#ifdef __linux__
    #include <sys/resource.h>
#endif

namespace Platform {
    long maxRSSKb() noexcept {
#ifdef __linux__
        // https://man7.org/linux/man-pages/man2/getrusage.2.html
        rusage ru;
        const auto result{ getrusage(RUSAGE_SELF, &ru) };

        if (result == -1) {
            return 0;
        }

        return ru.ru_maxrss;
#else
        #warning "Platform::maxRSSKb not implemented for this platform."
        return 0;
#endif
    }
};
