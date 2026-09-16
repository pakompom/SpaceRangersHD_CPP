/*License, info, etc
 ------------------
This implementation is made by me, Walied Othman, to contact me
mail to rainwolf@submanifold.be or triade@submanifold.be ,
always mention wether it 's about the FGInt or about the 6xs,
preferably in the subject line.
This source code is free, but only to other free software,
it's a two-way street, if you use this code in an application from which
you won't make any money of (e.g. software for the good of mankind)
then go right ahead, I won't stop you, I do demand acknowledgement for
my work.  However, if you're using this code in a commercial application,
an application from which you'll make money, then yes, I charge a
license-fee, as described in the license agreement for commercial use, see
the textfile in this zip-file.
If you 're going to use these implementations, let me know, so I ca, put a link
on my page if desired, I 'm always curious as to see where the spawn of my
mind ends up in.  If any algorithm is patented in your country, you should
acquire a license before using this software.  Modified versions of this
software must contain an acknowledgement of the original author (=me).

This implementation is available at
http://www.submanifold.be

copyright 2000, Walied Othman
This header may not be removed.*/
#pragma once
#include "runtime_support.hpp"

namespace FGInt {
    struct TFGInt;

    enum TSign : std::uint8_t {
        negative = 0,
        positive = 1,
    };

    // Native record RTTI.
    struct TFGInt {
        TSign Sign;
        std::uint8_t cpp_padding[3];
        // Count followed by 31-bit limbs.
        pas::DynArray<std::uint32_t> Number;
    };

} // namespace FGInt
