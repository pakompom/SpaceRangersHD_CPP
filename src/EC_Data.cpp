#include "layout/EC_Data.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/System.hpp"
#include "types/WindowsImports.hpp"
#include "units/CrcUnit.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Data.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Windows.hpp"

namespace EC_Data {
    std::uint8_t NextDataPathComponent(const pas::WideString& Path, std::int32_t& Position, std::int32_t& PathLength, std::int32_t& PartStart, std::int32_t& PartLength);

    const EC_Data::TResourceChecksumTable ResourceChecksums = EC_Data::TResourceChecksumTable{{
        {}, {}, {}, {},
        {.NameCrc = 0xddc6e004u, .FileCrc = 0xf42ed0e3u, .EncodedName = u"gdwd_txhvw_jhu_orjlfbjhu1tpp"_w}, {},
        {.NameCrc = 0x9352f406u, .FileCrc = 0x48e9edb1u, .EncodedName = u"gdwd_txhvw_hqj_pd}hbhqj1tpp"_w}, {},
        {.NameCrc = 0x0ed79c08u, .FileCrc = 0xb37f8a3cu, .EncodedName = u"gdwd_txhvw_uxv_erpehu1tpp"_w}, {},
        {.NameCrc = 0x8848d40au, .FileCrc = 0xfb928404u, .EncodedName = u"gdwd_txhvw_hqj_hohfwlrqbhqj1tpp"_w},
        {.NameCrc = 0x23a3b80bu, .FileCrc = 0xa9e77c9du, .EncodedName = u"gdwd_txhvw_uxv_skrwrurerw1tpp"_w},
        {.NameCrc = 0x02c3f00au, .FileCrc = 0x4a72d033u, .EncodedName = u"gdwd_txhvw_hqj_frgher{bhqj1tpp"_w},
        {.NameCrc = 0x5a840c0du, .FileCrc = 0x657f5078u, .EncodedName = u"gdwd_txhvw_hqj_sludwhvqhvwbhqj1tpp"_w},
        {.NameCrc = 0x73bab40eu, .FileCrc = 0xb572cb2du, .EncodedName = u"gdwd_depds_pdsb61rsw"_w}, {}, {}, {}, {},
        {.NameCrc = 0xebd7f013u, .FileCrc = 0x276a4098u, .EncodedName = u"gdwd_vfulsw_whvw1vfu"_w},
        {.NameCrc = 0x0f7d0c13u, .FileCrc = 0x254f56bcu, .EncodedName = u"gdwd_depds_pdsbervv1rsw"_w},
        {.NameCrc = 0xade23013u, .FileCrc = 0x2e4ce921u, .EncodedName = u"gdwd_txhvw_hqj_vleroxvrywbhqj1tpp"_w},
        {.NameCrc = 0xb70a2816u, .FileCrc = 0xec577bdfu, .EncodedName = u"gdwd_vfulsw_sfbsod4<1vfu"_w}, {}, {},
        {.NameCrc = 0x67406819u, .FileCrc = 0x8a69fdbeu, .EncodedName = u"gdwd_txhvw_uxv_vwtbdwdpdq51tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x59c49824u, .FileCrc = 0xc4eddde1u, .EncodedName = u"gdwd_txhvw_vsd_hylghqfhbvsd1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {}, {},
        {}, {.NameCrc = 0x9d88f430u, .FileCrc = 0x134b39dau, .EncodedName = u"gdwd_txhvw_vsd_plqlvwu|bvsd1tpp"_w}, {},
        {}, {.NameCrc = 0x774f6433u, .FileCrc = 0x5e6d10d0u, .EncodedName = u"gdwd_depds_pdsb41rsw"_w}, {},
        {.NameCrc = 0x73977035u, .FileCrc = 0x7410af29u, .EncodedName = u"gdwd_txhvw_hqj_urerwvbhqj1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x2abe6439u, .FileCrc = 0xc7c09903u, .EncodedName = u"gdwd_depds_pdowd1pds"_w},
        {.NameCrc = 0xc447703au, .FileCrc = 0xd1fe9395u, .EncodedName = u"gdwd_txhvw_jhu_sod|hubjhu1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x20bbe83eu, .FileCrc = 0xce83bcb7u, .EncodedName = u"gdwd_txhvw_jhu_vnlbjhu1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x7ea4c449u, .FileCrc = 0xaf3a1ab8u, .EncodedName = u"gdwd_depds_pdsb81rsw"_w}, {},
        {.NameCrc = 0xab80d84bu, .FileCrc = 0x17ece7fau, .EncodedName = u"gdwd_txhvw_hqj_vwtbedurq6bhqj1tpp"_w},
        {.NameCrc = 0x6bcc704cu, .FileCrc = 0xbca64246u, .EncodedName = u"gdwd_txhvw_hqj_ghswkbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x4e62584fu, .FileCrc = 0x6afc55f4u, .EncodedName = u"gdwd_txhvw_uxv_wd{lvw1tpp"_w}, {}, {}, {}, {},
        {}, {.NameCrc = 0x3ed66055u, .FileCrc = 0x3154b766u, .EncodedName = u"gdwd_txhvw_vsd_vwtbedurq5bvsd1tpp"_w},
        {.NameCrc = 0x45ada456u, .FileCrc = 0x3063358fu, .EncodedName = u"gdwd_depds_nuxjdgd1pds"_w}, {},
        {.NameCrc = 0x91fcb058u, .FileCrc = 0x3dc7ff75u, .EncodedName = u"gdwd_txhvw_jhu_glvnbjhu1tpp"_w}, {}, {},
        {.NameCrc = 0xf890185bu, .FileCrc = 0x734c1ceau, .EncodedName = u"gdwd_txhvw_hqj_eru}xnkdqbhqj1tpp"_w}, {}, {},
        {}, {.NameCrc = 0x6cf6cc5fu, .FileCrc = 0x0cd45da7u, .EncodedName = u"gdwd_txhvw_hqj_vkdvknlbhqj1tpp"_w}, {}, {},
        {}, {}, {},
        {.NameCrc = 0xea23c865u, .FileCrc = 0x4dafe272u, .EncodedName = u"gdwd_txhvw_jhu_hohfwlrqbjhu1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {.NameCrc = 0xdce02c6eu, .FileCrc = 0xc4b43da9u, .EncodedName = u"olerjj031goo"_w}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0xcb3e5475u, .FileCrc = 0x64f3921bu, .EncodedName = u"gdwd_txhvw_uxv_jdlgqhw1tpp"_w},
        {.NameCrc = 0xab572875u, .FileCrc = 0x049ae4a1u, .EncodedName = u"gdwd_txhvw_uxv_vnl1tpp"_w}, {}, {},
        {.NameCrc = 0x2caf6479u, .FileCrc = 0x1ad50e18u, .EncodedName = u"gdwd_vfulsw_sfbsod3<1vfu"_w}, {}, {}, {}, {},
        {}, {.NameCrc = 0xce87847fu, .FileCrc = 0xbc972677u, .EncodedName = u"gdwd_txhvw_vsd_sdfkydudvkbvsd1tpp"_w},
        {.NameCrc = 0x34656080u, .FileCrc = 0x62e9d3c4u, .EncodedName = u"gdwd_txhvw_jhu_vwtbdwdpdq5bjhu1tpp"_w}, {}, {},
        {}, {}, {}, {.NameCrc = 0xe5898086u, .FileCrc = 0x04d2b6beu, .EncodedName = u"gdwd_txhvw_uxv_ilvklqjfxs1tpp"_w},
        {}, {}, {}, {.NameCrc = 0x40f1488au, .FileCrc = 0x42739e94u, .EncodedName = u"gdwd_txhvw_uxv_sod|hu1tpp"_w}, {},
        {}, {.NameCrc = 0x3f78248du, .FileCrc = 0xebc64c53u, .EncodedName = u"gdwd_vfulsw_sfbsod341vfu"_w},
        {.NameCrc = 0x4ec1688du, .FileCrc = 0xf3b7fd4du, .EncodedName = u"gdwd_txhvw_hqj_ghdgrudolyhbhqj1tpp"_w},
        {.NameCrc = 0xcc77dc8fu, .FileCrc = 0xaf57f1e2u, .EncodedName = u"gdwd_txhvw_vsd_vwtbkhdgkxqwhubvsd1tpp"_w}, {},
        {}, {.NameCrc = 0x4117d492u, .FileCrc = 0x341252b1u, .EncodedName = u"gdwd_txhvw_jhu_vwtbkhdgkxqwhubjhu1tpp"_w},
        {.NameCrc = 0xdc90f893u, .FileCrc = 0x47882624u, .EncodedName = u"gdwd_txhvw_jhu_urerwvbjhu1tpp"_w}, {},
        {.NameCrc = 0xfd58e895u, .FileCrc = 0x39d1b40fu, .EncodedName = u"gdwd_vfulsw_sfbsduw91vfu"_w}, {}, {},
        {.NameCrc = 0xad36c898u, .FileCrc = 0xd0c851a6u, .EncodedName = u"gdwd_vfulsw_sfbsod481vfu"_w},
        {.NameCrc = 0x2a124c99u, .FileCrc = 0x9ed87cc1u, .EncodedName = u"gdwd_txhvw_hqj_orvwkhurbhqj1tpp"_w}, {},
        {.NameCrc = 0x2c4d8c9bu, .FileCrc = 0xc660870bu, .EncodedName = u"gdwd_txhvw_uxv_vwtbedurq61tpp"_w},
        {.NameCrc = 0x6b40f89cu, .FileCrc = 0xd1d71a15u, .EncodedName = u"gdwd_txhvw_hqj_sod|hubhqj1tpp"_w},
        {.NameCrc = 0xc56cc09cu, .FileCrc = 0xf66bb6a2u, .EncodedName = u"gdwd_txhvw_hqj_pdildbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x623300a0u, .FileCrc = 0x0e22b93cu, .EncodedName = u"gdwd_txhvw_jhu_px}rqbjhu1tpp"_w},
        {.NameCrc = 0x11fd84a1u, .FileCrc = 0xc54c33beu, .EncodedName = u"gdwd_txhvw_jhu_vwtbedurq4bjhu1tpp"_w}, {},
        {.NameCrc = 0x84acf0a3u, .FileCrc = 0x642e1db5u, .EncodedName = u"gdwd_depds_nuxjdgd1rsw"_w},
        {.NameCrc = 0xa99244a3u, .FileCrc = 0x17e2bf48u, .EncodedName = u"gdwd_txhvw_jhu_sl}}dbjhu1tpp"_w},
        {.NameCrc = 0xa9c318a5u, .FileCrc = 0x60b52c86u, .EncodedName = u"gdwd_vfulsw_sfbsod4:1vfu"_w},
        {.NameCrc = 0xe0a74ca3u, .FileCrc = 0xce293522u, .EncodedName = u"gdwd_txhvw_hqj_{hqrsdunbhqj1tpp"_w},
        {.NameCrc = 0x6d1d9ca3u, .FileCrc = 0xf1d129e1u, .EncodedName = u"gdwd_txhvw_hqj_sursurorjbhqj1tpp"_w},
        {.NameCrc = 0xf9ad38a8u, .FileCrc = 0xdf29620cu, .EncodedName = u"gdwd_vfulsw_sfbsduw71vfu"_w},
        {.NameCrc = 0x35bf04a5u, .FileCrc = 0x4f4f964du, .EncodedName = u"gdwd_txhvw_hqj_vwtbdwdpdq5bhqj1tpp"_w},
        {.NameCrc = 0x28b85ca6u, .FileCrc = 0x7671239fu, .EncodedName = u"gdwd_txhvw_uxv_vwtbedurq41tpp"_w}, {},
        {.NameCrc = 0x8a1c70acu, .FileCrc = 0xe9c82059u, .EncodedName = u"gdwd_txhvw_hqj_nlehuud}xpbhqj1tpp"_w}, {},
        {.NameCrc = 0x13ac04aeu, .FileCrc = 0x2fccd46eu, .EncodedName = u"gdwd_txhvw_uxv_vwtbkhdgkxqwhu1tpp"_w},
        {.NameCrc = 0x207168afu, .FileCrc = 0x2ee7314eu, .EncodedName = u"gdwd_txhvw_uxv_ro|psldgd1tpp"_w},
        {.NameCrc = 0x3b8df4b0u, .FileCrc = 0x5eb08ef0u, .EncodedName = u"gdwd_vfulsw_sfbsod361vfu"_w}, {}, {},
        {.NameCrc = 0xda7d58b3u, .FileCrc = 0xdbb44f8cu, .EncodedName = u"gdwd_txhvw_hqj_iruxpbhqj1tpp"_w}, {}, {}, {},
        {}, {.NameCrc = 0x29f1fcb8u, .FileCrc = 0xa52a3c50u, .EncodedName = u"gdwd_txhvw_vsd_ro|psldgdbvsd1tpp"_w},
        {.NameCrc = 0xb7ffc4b9u, .FileCrc = 0x50737e80u, .EncodedName = u"gdwd_txhvw_vsd_vkdvknlbvsd1tpp"_w}, {}, {},
        {.NameCrc = 0xbfa590bcu, .FileCrc = 0x027c08e8u, .EncodedName = u"gdwd_depds_pdsb81pds"_w}, {}, {}, {}, {}, {},
        {}, {}, {}, {}, {.NameCrc = 0xb64e30c6u, .FileCrc = 0x30e53515u, .EncodedName = u"gdwd_depds_pdsb41pds"_w},
        {.NameCrc = 0x52e188c6u, .FileCrc = 0x738d6016u, .EncodedName = u"gdwd_txhvw_hqj_sdunbhqj1tpp"_w},
        {.NameCrc = 0x9af320c7u, .FileCrc = 0xd0673e6bu, .EncodedName = u"gdwd_txhvw_uxv_hdv|zrun1tpp"_w}, {},
        {.NameCrc = 0x326654cau, .FileCrc = 0x5e65b093u, .EncodedName = u"gdwd_vfulsw_sfbsod3:1vfu"_w}, {},
        {.NameCrc = 0xebbf30ccu, .FileCrc = 0x58c47d40u, .EncodedName = u"gdwd_depds_pdowd1rsw"_w},
        {.NameCrc = 0x82cc50ccu, .FileCrc = 0x3b2591d6u, .EncodedName = u"gdwd_txhvw_jhu_{hqrsdunbjhu1tpp"_w},
        {.NameCrc = 0xe5e240cdu, .FileCrc = 0x56d5ade7u, .EncodedName = u"gdwd_txhvw_hqj_ilvklqjfxsbhqj1tpp"_w},
        {.NameCrc = 0xb7a4accdu, .FileCrc = 0x36f53283u, .EncodedName = u"gdwd_txhvw_hqj_yxondqbhqj1tpp"_w},
        {.NameCrc = 0x30aadcd0u, .FileCrc = 0x1c993b62u, .EncodedName = u"gdwd_txhvw_vsd_vwtbedurq7bvsd1tpp"_w},
        {.NameCrc = 0x118d78d1u, .FileCrc = 0x430f19f3u, .EncodedName = u"gdwd_txhvw_hqj_hdv|zrunbhqj1tpp"_w},
        {.NameCrc = 0xf04698d2u, .FileCrc = 0x7e751fa7u, .EncodedName = u"gdwd_vfulsw_sfbsduw31vfu"_w}, {}, {}, {}, {},
        {}, {.NameCrc = 0x763200d8u, .FileCrc = 0x1e16ee0du, .EncodedName = u"gdwd_txhvw_hqj_skrwrurerwbhqj1tpp"_w},
        {.NameCrc = 0xf3a874d8u, .FileCrc = 0x5ed68278u, .EncodedName = u"gdwd_txhvw_uxv_grrplqr1tpp"_w}, {}, {}, {},
        {.NameCrc = 0xc0c60cddu, .FileCrc = 0x8b6d6bacu, .EncodedName = u"gdwd_txhvw_vsd_vwtbdwdpdq5bvsd1tpp"_w}, {},
        {.NameCrc = 0xa028b8dfu, .FileCrc = 0x1501de7du, .EncodedName = u"gdwd_vfulsw_sfbsod461vfu"_w},
        {.NameCrc = 0x4a4634dfu, .FileCrc = 0xc1f569c9u, .EncodedName = u"gdwd_txhvw_vsd_slorwbvsd1tpp"_w},
        {.NameCrc = 0x61ab80dfu, .FileCrc = 0x08809345u, .EncodedName = u"gdwd_txhvw_hqj_vnlbhqj1tpp"_w},
        {.NameCrc = 0xa4dd68e2u, .FileCrc = 0x9c108f1au, .EncodedName = u"gdwd_vfulsw_sfbsod441vfu"_w},
        {.NameCrc = 0x35be98e1u, .FileCrc = 0x4a329fe9u, .EncodedName = u"gdwd_txhvw_jhu_vsdfholqhvbjhu1tpp"_w},
        {.NameCrc = 0x98a874e2u, .FileCrc = 0x64314c0bu, .EncodedName = u"gdwd_txhvw_hqj_glvnbhqj1tpp"_w}, {},
        {.NameCrc = 0xce7c58e6u, .FileCrc = 0x3c2f0b87u, .EncodedName = u"gdwd_depds_pdsbervv1pds"_w}, {}, {}, {},
        {.NameCrc = 0x658cc0eau, .FileCrc = 0x3db816a4u, .EncodedName = u"gdwd_txhvw_vsd_mxpshubvsd1tpp"_w}, {},
        {.NameCrc = 0xd9caf8ecu, .FileCrc = 0x928f2bf0u, .EncodedName = u"gdwd_txhvw_vsd_frgher{bvsd1tpp"_w}, {}, {},
        {.NameCrc = 0xf4b348efu, .FileCrc = 0x2b514452u, .EncodedName = u"gdwd_vfulsw_sfbsduw51vfu"_w}, {}, {},
        {.NameCrc = 0x12838cf2u, .FileCrc = 0x8686ac11u, .EncodedName = u"gdwd_txhvw_hqj_hylojhqlxvbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x67bf00f5u, .FileCrc = 0x440752cau, .EncodedName = u"gdwd_txhvw_hqj_udoo|bhqj1tpp"_w}, {},
        {.NameCrc = 0x369384f7u, .FileCrc = 0xba6ecb35u, .EncodedName = u"gdwd_vfulsw_sfbsod381vfu"_w}, {},
        {.NameCrc = 0xb5e364f9u, .FileCrc = 0xa01b4653u, .EncodedName = u"gdwd_txhvw_uxv_hghozhlvv1tpp"_w},
        {.NameCrc = 0x026708fau, .FileCrc = 0x1d319109u, .EncodedName = u"gdwd_txhvw_hqj_wrxulvwvbhqj1tpp"_w},
        {.NameCrc = 0xb2bbe0fbu, .FileCrc = 0x9f8476e9u, .EncodedName = u"gdwd_depds_pdsb61pds"_w}, {}, {}, {}, {}, {},
        {}, {}, {}, {},
        {.NameCrc = 0xfcfaf505u, .FileCrc = 0x0e7931a1u, .EncodedName = u"gdwd_txhvw_hqj_slorwbhqj1tpp"_w}, {},
        {.NameCrc = 0x78f94d07u, .FileCrc = 0xae9d1ea8u, .EncodedName = u"gdwd_txhvw_jhu_plqlvwu|bjhu1tpp"_w},
        {.NameCrc = 0x974b3108u, .FileCrc = 0x80a7790au, .EncodedName = u"gdwd_txhvw_vsd_edqnhwbvsd1tpp"_w},
        {.NameCrc = 0x12d0a109u, .FileCrc = 0x747714abu, .EncodedName = u"gdwd_depds_odelulqwblll1pds"_w}, {}, {}, {},
        {}, {}, {}, {}, {}, {},
        {.NameCrc = 0xbcb52113u, .FileCrc = 0xf7c13800u, .EncodedName = u"gdwd_txhvw_jhu_hylghqfhbjhu1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x14a5e11du, .FileCrc = 0xb40f490au, .EncodedName = u"gdwd_txhvw_jhu_frgher{bjhu1tpp"_w}, {}, {}, {},
        {},
        {.NameCrc = 0xdb54f122u, .FileCrc = 0x9dae1b4eu, .EncodedName = u"gdwd_txhvw_vsd_sludwhfodqsulvrqbvsd1tpp"_w},
        {}, {.NameCrc = 0x0c5dad24u, .FileCrc = 0xeb0de892u, .EncodedName = u"gdwd_txhvw_uxv_irqfhuv1tpp"_w}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0x1ef2512bu, .FileCrc = 0x151f72f0u, .EncodedName = u"gdwd_depds_pdsbvn|1rsw"_w},
        {.NameCrc = 0x3c81d12cu, .FileCrc = 0x309fd439u, .EncodedName = u"gdwd_vfulsw_sfbsod531vfu"_w},
        {.NameCrc = 0x1d5a312cu, .FileCrc = 0x8322b884u, .EncodedName = u"gdwd_txhvw_uxv_sdfkydudvk1tpp"_w},
        {.NameCrc = 0xa01af52cu, .FileCrc = 0xc99afcebu, .EncodedName = u"gdwd_txhvw_uxv_uyn1tpp"_w},
        {.NameCrc = 0xa270712bu, .FileCrc = 0xc0111a70u, .EncodedName = u"gdwd_txhvw_uxv_yxondq1tpp"_w},
        {.NameCrc = 0x4c8b0d2eu, .FileCrc = 0x9b513043u, .EncodedName = u"gdwd_txhvw_uxv_frpsoh{1tpp"_w},
        {.NameCrc = 0xd103c12fu, .FileCrc = 0x16ed62deu, .EncodedName = u"gdwd_txhvw_vsd_udoo|bvsd1tpp"_w}, {}, {}, {},
        {}, {}, {.NameCrc = 0xd450bd37u, .FileCrc = 0xc8d850d6u, .EncodedName = u"gdwd_txhvw_vsd_vwtbedurq4bvsd1tpp"_w},
        {}, {}, {.NameCrc = 0x5769d13au, .FileCrc = 0xbabc8e68u, .EncodedName = u"gdwd_txhvw_uxv_plqlvwu|1tpp"_w}, {},
        {}, {}, {}, {}, {.NameCrc = 0xd1647940u, .FileCrc = 0xd37c460bu, .EncodedName = u"gdwd_txhvw_uxv_skdudrq1tpp"_w},
        {}, {}, {.NameCrc = 0xcbb74943u, .FileCrc = 0x2ed7af28u, .EncodedName = u"gdwd_vfulsw_pvbve51vfu"_w}, {}, {},
        {.NameCrc = 0xf507e546u, .FileCrc = 0x8b1f22e8u, .EncodedName = u"gdwd_txhvw_jhu_vwtbedurq7bjhu1tpp"_w},
        {.NameCrc = 0x4d035d47u, .FileCrc = 0x9aa44ffcu, .EncodedName = u"gdwd_txhvw_hqj_prlbhqj1tpp"_w},
        {.NameCrc = 0x7a90dd48u, .FileCrc = 0x730166d1u, .EncodedName = u"gdwd_txhvw_jhu_vkdvknlbjhu1tpp"_w}, {}, {},
        {.NameCrc = 0xfda1fd4bu, .FileCrc = 0x70d7b8f2u, .EncodedName = u"gdwd_txhvw_vsd_edggd|bvsd1tpp"_w}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0xd5565d52u, .FileCrc = 0x361bc30au, .EncodedName = u"gdwd_txhvw_hqj_dpqhvldbhqj1tpp"_w},
        {.NameCrc = 0x0f527152u, .FileCrc = 0x43cb21fau, .EncodedName = u"gdwd_txhvw_vsd_hohfwlrqbvsd1tpp"_w}, {},
        {.NameCrc = 0xc2d02555u, .FileCrc = 0x21506ebau, .EncodedName = u"gdwd_txhvw_vsd_vnlbvsd1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0xb10fc15du, .FileCrc = 0xd9f4810fu, .EncodedName = u"gdwd_txhvw_hqj_suryrgdbhqj1tpp"_w},
        {.NameCrc = 0x7285fd5eu, .FileCrc = 0x532dd25au, .EncodedName = u"gdwd_txhvw_uxv_iruxp1tpp"_w},
        {.NameCrc = 0x24b38d5du, .FileCrc = 0xae54851fu, .EncodedName = u"gdwd_txhvw_jhu_sulvrqbjhu1tpp"_w}, {},
        {.NameCrc = 0x0c0b5d61u, .FileCrc = 0x14c8806du, .EncodedName = u"oleyruelv031goo"_w}, {}, {},
        {.NameCrc = 0x5e76c564u, .FileCrc = 0xcb0eb7c0u, .EncodedName = u"gdwd_txhvw_uxv_sludwhvqhvw1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x102f6568u, .FileCrc = 0xe0501010u, .EncodedName = u"gdwd_depds_nhoohubvslulwb351pds"_w},
        {.NameCrc = 0xf8cbe168u, .FileCrc = 0xdc5292c6u, .EncodedName = u"gdwd_depds_sludwhvbiodj1pds"_w},
        {.NameCrc = 0x1a925168u, .FileCrc = 0x6f8aa0c7u, .EncodedName = u"gdwd_txhvw_hqj_plqlvwu|bhqj1tpp"_w}, {}, {},
        {}, {.NameCrc = 0xd057d16eu, .FileCrc = 0xa35f5387u, .EncodedName = u"gdwd_depds_5vwduv1pds"_w},
        {.NameCrc = 0x13e45d6eu, .FileCrc = 0x519d4f88u, .EncodedName = u"gdwd_txhvw_hqj_erpehubhqj1tpp"_w},
        {.NameCrc = 0x875da96eu, .FileCrc = 0xd7a4a6a0u, .EncodedName = u"gdwd_txhvw_uxv_wrxulvwv1tpp"_w},
        {.NameCrc = 0x42d3e970u, .FileCrc = 0xc78f0f33u, .EncodedName = u"gdwd_txhvw_uxv_orvwkhur1tpp"_w},
        {.NameCrc = 0x3579c572u, .FileCrc = 0x2cba2313u, .EncodedName = u"gdwd_txhvw_uxv_phjdwhvw1tpp"_w},
        {.NameCrc = 0xadabbd73u, .FileCrc = 0x387bc314u, .EncodedName = u"gdwd_txhvw_vsd_eru}xnkdqbvsd1tpp"_w},
        {.NameCrc = 0x9d7ab574u, .FileCrc = 0x6f07d188u, .EncodedName = u"gdwd_txhvw_uxv_edqnhw1tpp"_w}, {}, {},
        {.NameCrc = 0xf013a177u, .FileCrc = 0xb6d6e4b5u, .EncodedName = u"gdwd_txhvw_vsd_vsdfholqhvbvsd1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0xdede3d7cu, .FileCrc = 0xacffb3dau, .EncodedName = u"gdwd_txhvw_hqj_hylghqfhbhqj1tpp"_w},
        {}, {.NameCrc = 0xd940f17eu, .FileCrc = 0xbc920906u, .EncodedName = u"gdwd_depds_dwrp1rsw"_w},
        {.NameCrc = 0xe6814d7eu, .FileCrc = 0x4e207124u, .EncodedName = u"gdwd_txhvw_jhu_hoxvbjhu1tpp"_w},
        {.NameCrc = 0xe847c180u, .FileCrc = 0xbc8fd416u, .EncodedName = u"gdwd_txhvw_uxv_hylojhqlxv1tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x1841a58bu, .FileCrc = 0x04a1794fu, .EncodedName = u"gdwd_depds_dwrp1pds"_w}, {}, {}, {}, {},
        {.NameCrc = 0x7cca5990u, .FileCrc = 0x309801d6u, .EncodedName = u"gdwd_txhvw_hqj_ro|psldgdbhqj1tpp"_w}, {}, {},
        {}, {}, {}, {.NameCrc = 0xdd70b196u, .FileCrc = 0xdd65462cu, .EncodedName = u"gdwd_txhvw_vsd_ghswkbvsd1tpp"_w},
        {}, {}, {}, {}, {.NameCrc = 0x1156859bu, .FileCrc = 0x2694d1f7u, .EncodedName = u"gdwd_depds_5vwduv1rsw"_w},
        {.NameCrc = 0x34c9199cu, .FileCrc = 0x4e0ee23bu, .EncodedName = u"gdwd_vfulsw_pvbehjlq1vfu"_w},
        {.NameCrc = 0xd12e319du, .FileCrc = 0xef95650au, .EncodedName = u"gdwd_depds_nhoohubvslulwb351rsw"_w},
        {.NameCrc = 0x39cab59du, .FileCrc = 0x327072bau, .EncodedName = u"gdwd_depds_sludwhvbiodj1rsw"_w}, {}, {}, {},
        {}, {}, {.NameCrc = 0xdfe7b5a4u, .FileCrc = 0x85b082f4u, .EncodedName = u"rnji1goo"_w}, {}, {}, {}, {}, {},
        {}, {}, {.NameCrc = 0x9a5731acu, .FileCrc = 0x32e13284u, .EncodedName = u"gdwd_txhvw_jhu_hghozhlvvbjhu1tpp"_w},
        {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x055ad9b3u, .FileCrc = 0x2257b552u, .EncodedName = u"gdwd_txhvw_jhu_sludwhfodqsulvrqbjhu1tpp"_w},
        {.NameCrc = 0x507b15b4u, .FileCrc = 0xaba7b7c2u, .EncodedName = u"gdwd_txhvw_uxv_vydurnrn1tpp"_w},
        {.NameCrc = 0xaeab99b5u, .FileCrc = 0xe09c4195u, .EncodedName = u"gdwd_txhvw_vsd_gulyhubvsd1tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0x6a6961beu, .FileCrc = 0x6f3e8572u, .EncodedName = u"gdwd_txhvw_uxv_glvn1tpp"_w},
        {.NameCrc = 0x2a40d1bfu, .FileCrc = 0xae314337u, .EncodedName = u"gdwd_txhvw_vsd_vwtbdwdpdq4bvsd1tpp"_w}, {}, {},
        {}, {.NameCrc = 0xfb7b59c3u, .FileCrc = 0xc278b686u, .EncodedName = u"gdwd_txhvw_jhu_vwtbedurq5bjhu1tpp"_w},
        {.NameCrc = 0xefd589c4u, .FileCrc = 0xc33caee2u, .EncodedName = u"gdwd_txhvw_hqj_hoxvbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0xe737d9c7u, .FileCrc = 0x396b4215u, .EncodedName = u"gdwd_txhvw_uxv_ohrqdugr1tpp"_w},
        {.NameCrc = 0xdf39d9c7u, .FileCrc = 0xce76a7bcu, .EncodedName = u"gdwd_txhvw_hqj_vwtbdwdpdq4bhqj1tpp"_w}, {}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0x789605d0u, .FileCrc = 0xcfc8f987u, .EncodedName = u"gdwd_txhvw_hqj_whvwlqjbhqj1tpp"_w}, {},
        {.NameCrc = 0x273e7dd2u, .FileCrc = 0x517da116u, .EncodedName = u"gdwd_txhvw_uxv_frorql}dwlrq1tpp"_w}, {},
        {.NameCrc = 0x14111dd4u, .FileCrc = 0x21918aaau, .EncodedName = u"gdwd_vfulsw_pvbwhuurq1vfu"_w}, {}, {}, {}, {},
        {.NameCrc = 0x1bb181d9u, .FileCrc = 0x42f24a83u, .EncodedName = u"gdwd_txhvw_hqj_glyhubhqj1tpp"_w}, {},
        {.NameCrc = 0x86ffaddbu, .FileCrc = 0x3659b625u, .EncodedName = u"{ylgfruh1goo"_w}, {},
        {.NameCrc = 0xc5d2cdddu, .FileCrc = 0xc9916113u, .EncodedName = u"gdwd_txhvw_uxv_eru}xnkdq1tpp"_w},
        {.NameCrc = 0xdff305deu, .FileCrc = 0x112f1af6u, .EncodedName = u"gdwd_depds_pdsbvn|1pds"_w},
        {.NameCrc = 0xfcd36ddeu, .FileCrc = 0x69ade472u, .EncodedName = u"gdwd_txhvw_uxv_gulyhu1tpp"_w}, {}, {},
        {.NameCrc = 0xdee3bde2u, .FileCrc = 0x56cd7493u, .EncodedName = u"gdwd_txhvw_jhu_vwtbdwdpdq4bjhu1tpp"_w}, {}, {},
        {}, {}, {}, {},
        {.NameCrc = 0x0b2abde9u, .FileCrc = 0xd0f2fe05u, .EncodedName = u"gdwd_txhvw_jhu_sdfkydudvkbjhu1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0x5a3a15eeu, .FileCrc = 0x6e7a206bu, .EncodedName = u"gdwd_txhvw_uxv_sl}}d1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {}, {},
        {}, {}, {.NameCrc = 0x67bde9fbu, .FileCrc = 0xb7aed871u, .EncodedName = u"gdwd_txhvw_vsd_{hqrsdunbvsd1tpp"_w},
        {.NameCrc = 0xd3d1f5fcu, .FileCrc = 0x092e8091u, .EncodedName = u"gdwd_depds_odelulqwblll1rsw"_w},
        {.NameCrc = 0x8bb405fbu, .FileCrc = 0xbb1b6da2u, .EncodedName = u"gdwd_txhvw_hqj_sulvrqbhqj1tpp"_w},
        {.NameCrc = 0xf08341feu, .FileCrc = 0x24fd2b48u, .EncodedName = u"gdwd_txhvw_uxv_sdun1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0xf2315e08u, .FileCrc = 0x824f98adu, .EncodedName = u"gdwd_txhvw_hqj_joxnlbhqj1tpp"_w}, {}, {}, {},
        {}, {}, {.NameCrc = 0x3461ee0eu, .FileCrc = 0x0c67c0aeu, .EncodedName = u"gdwd_vfulsw_pvbnhoohu1vfu"_w},
        {.NameCrc = 0x811afa0eu, .FileCrc = 0x5c2b78e7u, .EncodedName = u"gdwd_txhvw_hqj_edqnhwbhqj1tpp"_w},
        {.NameCrc = 0xe7815a10u, .FileCrc = 0x8470c206u, .EncodedName = u"gdwd_txhvw_jhu_irqfhuvbjhu1tpp"_w}, {},
        {.NameCrc = 0xd343ba12u, .FileCrc = 0x3d73b021u, .EncodedName = u"gdwd_vfulsw_sfbsod541vfu"_w}, {},
        {.NameCrc = 0x25c02214u, .FileCrc = 0x35e8a9a4u, .EncodedName = u"gdwd_txhvw_hqj_uynbhqj1tpp"_w},
        {.NameCrc = 0x17fdda15u, .FileCrc = 0xa1e32968u, .EncodedName = u"gdwd_txhvw_jhu_gulyhubjhu1tpp"_w}, {}, {},
        {.NameCrc = 0xebb7fe18u, .FileCrc = 0x7f7d8209u, .EncodedName = u"gdwd_txhvw_vsd_vruwlurynd4bvsd1tpp"_w}, {},
        {.NameCrc = 0xdfbfda1au, .FileCrc = 0xcd8c1561u, .EncodedName = u"gdwd_txhvw_hqj_vwhdowkbhqj1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x95cafa1eu, .FileCrc = 0xdae217ccu, .EncodedName = u"gdwd_txhvw_uxv_orjlf1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0xa387aa2au, .FileCrc = 0xa0ddaaaeu, .EncodedName = u"gdwd_txhvw_uxv_px}rq1tpp"_w}, {}, {}, {}, {},
        {.NameCrc = 0x4aaed22fu, .FileCrc = 0x80e2522bu, .EncodedName = u"gdwd_txhvw_hqj_vwtbedurq7bhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x9fe64e32u, .FileCrc = 0x4f42a7a2u, .EncodedName = u"gdwd_txhvw_vsd_ilvklqjfxsbvsd1tpp"_w}, {}, {},
        {.NameCrc = 0xda5d9a35u, .FileCrc = 0xa8369349u, .EncodedName = u"gdwd_txhvw_uxv_grprfodq1tpp"_w},
        {.NameCrc = 0x7e9d5236u, .FileCrc = 0x603cd386u, .EncodedName = u"gdwd_txhvw_vsd_glvnbvsd1tpp"_w},
        {.NameCrc = 0xc8f36e35u, .FileCrc = 0x89edd932u, .EncodedName = u"gdwd_txhvw_hqj_vwtbkhdgkxqwhubhqj1tpp"_w}, {},
        {}, {}, {}, {.NameCrc = 0xd7da523cu, .FileCrc = 0xb690e013u, .EncodedName = u"gdwd_txhvw_hqj_jodyuhgbhqj1tpp"_w},
        {}, {}, {}, {.NameCrc = 0x2080f240u, .FileCrc = 0xa72ee38cu, .EncodedName = u"gdwd_vfulsw_pvbve41vfu"_w}, {}, {},
        {}, {}, {.NameCrc = 0x1f149245u, .FileCrc = 0x6aa4252eu, .EncodedName = u"gdwd_txhvw_jhu_vruwlurynd4bjhu1tpp"_w},
        {}, {}, {}, {}, {.NameCrc = 0xd8c5564au, .FileCrc = 0x3bd4d698u, .EncodedName = u"gdwd_txhvw_uxv_vwhdowk1tpp"_w},
        {}, {}, {.NameCrc = 0xebf0364du, .FileCrc = 0x46388f17u, .EncodedName = u"gdwd_txhvw_hqj_edggd|bhqj1tpp"_w}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0xe0a98656u, .FileCrc = 0x10f06353u, .EncodedName = u"gdwd_txhvw_uxv_suryrgd1tpp"_w}, {}, {}, {}, {},
        {.NameCrc = 0x935f165bu, .FileCrc = 0x1e4472f6u, .EncodedName = u"gdwd_txhvw_hqj_phjdwhvwbhqj1tpp"_w}, {}, {},
        {}, {.NameCrc = 0xec4ada5fu, .FileCrc = 0xa3d1d615u, .EncodedName = u"gdwd_txhvw_vsd_ohrqdugrbvsd1tpp"_w},
        {.NameCrc = 0x1ecef660u, .FileCrc = 0xa115786bu, .EncodedName = u"gdwd_txhvw_hqj_vruwlurynd4bhqj1tpp"_w},
        {.NameCrc = 0xf22d065fu, .FileCrc = 0xe56a0d73u, .EncodedName = u"gdwd_txhvw_hqj_guxjvbhqj1tpp"_w}, {}, {}, {},
        {}, {}, {.NameCrc = 0x04dfae67u, .FileCrc = 0xe11a3420u, .EncodedName = u"gdwd_depds_xudqy461rsw"_w}, {}, {},
        {.NameCrc = 0x4cd88e6au, .FileCrc = 0x2f300933u, .EncodedName = u"gdwd_txhvw_hqj_jdlgqhwbhqj1tpp"_w},
        {.NameCrc = 0xd85eb66bu, .FileCrc = 0x44d2547du, .EncodedName = u"gdwd_vfulsw_sfbilqdo1vfu"_w}, {}, {}, {}, {},
        {}, {}, {.NameCrc = 0x956e8672u, .FileCrc = 0x9cc85e3au, .EncodedName = u"gdwd_txhvw_uxv_hohfwlrq1tpp"_w}, {},
        {.NameCrc = 0x9176b274u, .FileCrc = 0x1b55f497u, .EncodedName = u"gdwd_txhvw_uxv_slorw1tpp"_w}, {}, {}, {}, {},
        {.NameCrc = 0x610c3679u, .FileCrc = 0x4f0f1048u, .EncodedName = u"gdwd_txhvw_uxv_vsdfholqhv1tpp"_w}, {}, {},
        {.NameCrc = 0xe401c67cu, .FileCrc = 0xd8ac4e3du, .EncodedName = u"gdwd_txhvw_vsd_orjlfbvsd1tpp"_w}, {}, {}, {},
        {.NameCrc = 0xb4838a80u, .FileCrc = 0x38378814u, .EncodedName = u"gdwd_txhvw_hqj_sdfkydudvkbhqj1tpp"_w}, {}, {},
        {}, {.NameCrc = 0x26d99284u, .FileCrc = 0xe86a4c4eu, .EncodedName = u"gdwd_txhvw_uxv_vruwlurynd41tpp"_w}, {},
        {.NameCrc = 0x9ae4f686u, .FileCrc = 0xc364bdbcu, .EncodedName = u"gdwd_txhvw_uxv_ghdgrudolyh1tpp"_w}, {}, {}, {},
        {}, {}, {},
        {.NameCrc = 0x616bd68du, .FileCrc = 0x335fb4c7u, .EncodedName = u"gdwd_txhvw_hqj_hghozhlvvbhqj1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0xc5defa92u, .FileCrc = 0x7857100au, .EncodedName = u"gdwd_depds_xudqy461pds"_w}, {},
        {.NameCrc = 0xa3e42e94u, .FileCrc = 0x2ba7b9bcu, .EncodedName = u"gdwd_vfulsw_pvbeod}hu1vfu"_w},
        {.NameCrc = 0x5f3cb695u, .FileCrc = 0x74be3f82u, .EncodedName = u"gdwd_txhvw_hqj_sludwhfodqsulvrqbhqj1tpp"_w},
        {}, {}, {.NameCrc = 0x984f0298u, .FileCrc = 0x5e6cc392u, .EncodedName = u"gdwd_txhvw_hqj_vydurnrnbhqj1tpp"_w},
        {}, {}, {}, {}, {}, {}, {}, {},
        {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x738112a7u, .FileCrc = 0x3d089361u, .EncodedName = u"gdwd_txhvw_jhu_slorwbjhu1tpp"_w},
        {.NameCrc = 0x2e1d72a8u, .FileCrc = 0x7bb56b7du, .EncodedName = u"gdwd_txhvw_jhu_edqnhwbjhu1tpp"_w}, {},
        {.NameCrc = 0x44d26eaau, .FileCrc = 0xf93742d0u, .EncodedName = u"gdwd_txhvw_hqj_vwtbedurq5bhqj1tpp"_w}, {}, {},
        {}, {}, {}, {},
        {.NameCrc = 0x87f6beb1u, .FileCrc = 0xe53f3fb4u, .EncodedName = u"gdwd_txhvw_jhu_ro|psldgdbjhu1tpp"_w},
        {.NameCrc = 0xfc4b12b2u, .FileCrc = 0x0c1cadb6u, .EncodedName = u"gdwd_txhvw_uxv_vleroxvryw1tpp"_w},
        {.NameCrc = 0xb8fa52b3u, .FileCrc = 0xbbeaa04au, .EncodedName = u"gdwd_txhvw_hqj_gulyhubhqj1tpp"_w},
        {.NameCrc = 0xd184d6b4u, .FileCrc = 0xd6feceabu, .EncodedName = u"gdwd_txhvw_vsd_vwtbedurq6bvsd1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x9be6fabeu, .FileCrc = 0x5e8ac0afu, .EncodedName = u"gdwd_txhvw_uxv_hoxv1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0x01fecac8u, .FileCrc = 0x266b3facu, .EncodedName = u"gdwd_txhvw_uxv_sulvrq1tpp"_w}, {}, {}, {}, {},
        {}, {}, {}, {}, {}, {},
        {.NameCrc = 0x8c8bdad3u, .FileCrc = 0x26112504u, .EncodedName = u"gdwd_txhvw_uxv_{hqrorj1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x09f89ad7u, .FileCrc = 0xda56d2a1u, .EncodedName = u"gdwd_txhvw_uxv_vkdvknl1tpp"_w},
        {.NameCrc = 0xb6318ad8u, .FileCrc = 0xcdf5b20du, .EncodedName = u"gdwd_txhvw_hqj_iduxnbhqj1tpp"_w},
        {.NameCrc = 0x5bf426d8u, .FileCrc = 0x0b5a3eacu, .EncodedName = u"gdwd_txhvw_vsd_px}rqbvsd1tpp"_w}, {},
        {.NameCrc = 0x905562dbu, .FileCrc = 0xbbf28bcbu, .EncodedName = u"gdwd_txhvw_vsd_sl}}dbvsd1tpp"_w}, {},
        {.NameCrc = 0xbdecceddu, .FileCrc = 0x622a027eu, .EncodedName = u"gdwd_txhvw_hqj_ihlsv|fkrbhqj1tpp"_w},
        {.NameCrc = 0xbc228edeu, .FileCrc = 0x49c72061u, .EncodedName = u"vwhdpbdsl1goo"_w}, {}, {},
        {.NameCrc = 0xaf7eeae1u, .FileCrc = 0xea8e3fdeu, .EncodedName = u"gdwd_txhvw_uxv_urerwv1tpp"_w},
        {.NameCrc = 0x24b582e2u, .FileCrc = 0xced8ae1eu, .EncodedName = u"gdwd_txhvw_uxv_guxjv1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x05787ae6u, .FileCrc = 0xa4c2fbccu, .EncodedName = u"gdwd_txhvw_uxv_prl1tpp"_w}, {}, {}, {}, {},
        {.NameCrc = 0x44f7beebu, .FileCrc = 0x9956b9b4u, .EncodedName = u"gdwd_txhvw_jhu_edggd|bjhu1tpp"_w},
        {.NameCrc = 0x2866daecu, .FileCrc = 0x75c69db4u, .EncodedName = u"vwhdpbdfk1goo"_w},
        {.NameCrc = 0xd7e63eecu, .FileCrc = 0x791bb1e4u, .EncodedName = u"gdwd_txhvw_vsd_vleroxvrywbvsd1tpp"_w}, {}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0xfa241ef7u, .FileCrc = 0x1a741279u, .EncodedName = u"gdwd_txhvw_jhu_vydurnrnbjhu1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0x04b6d2fcu, .FileCrc = 0xb4290543u, .EncodedName = u"gdwd_txhvw_vsd_vwhdowkbvsd1tpp"_w},
        {.NameCrc = 0x9de5cefdu, .FileCrc = 0x6056d39au, .EncodedName = u"gdwd_txhvw_vsd_sulvrqbvsd1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x26e9a301u, .FileCrc = 0x6ead54c1u, .EncodedName = u"gdwd_txhvw_hqj_sl}}dbhqj1tpp"_w},
        {.NameCrc = 0xed48e702u, .FileCrc = 0x0a40d434u, .EncodedName = u"gdwd_txhvw_hqj_px}rqbhqj1tpp"_w},
        {.NameCrc = 0x179bcb01u, .FileCrc = 0x35e540ddu, .EncodedName = u"gdwd_txhvw_uxv_whvwlqj1tpp"_w},
        {.NameCrc = 0x4a0b1f02u, .FileCrc = 0x4d76164du, .EncodedName = u"gdwd_txhvw_uxv_nlehuud}xp1tpp"_w},
        {.NameCrc = 0x52903f04u, .FileCrc = 0xac34afc9u, .EncodedName = u"gdwd_txhvw_hqj_iloldobhqj1tpp"_w}, {},
        {.NameCrc = 0xf1e74b07u, .FileCrc = 0x18fed95du, .EncodedName = u"gdwd_txhvw_hqj_irqfhuvbhqj1tpp"_w},
        {.NameCrc = 0x6b507f07u, .FileCrc = 0x12114320u, .EncodedName = u"gdwd_txhvw_hqj_ohrqdugrbhqj1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0xc9d9cb0du, .FileCrc = 0x8884639eu, .EncodedName = u"gdwd_txhvw_jhu_vwhdowkbjhu1tpp"_w}, {},
        {.NameCrc = 0xec871b0fu, .FileCrc = 0x5a79c590u, .EncodedName = u"pdwul{jdph1goo"_w},
        {.NameCrc = 0x09e0af10u, .FileCrc = 0xe81cc99du, .EncodedName = u"gdwd_txhvw_vsd_hoxvbvsd1tpp"_w}, {}, {}, {},
        {}, {.NameCrc = 0x3d10cb15u, .FileCrc = 0xb5d3a37du, .EncodedName = u"gdwd_txhvw_uxv_joxnl1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x87b3af19u, .FileCrc = 0xc9c832ceu, .EncodedName = u"gdwd_txhvw_uxv_flwdghov1tpp"_w},
        {.NameCrc = 0x8c77d31au, .FileCrc = 0xd334a70cu, .EncodedName = u"gdwd_txhvw_uxv_vwtbdwdpdq41tpp"_w}, {}, {}, {},
        {}, {.NameCrc = 0xc3e08b1fu, .FileCrc = 0x57e43927u, .EncodedName = u"gdwd_txhvw_uxv_pdild1tpp"_w}, {}, {},
        {.NameCrc = 0x1429ef22u, .FileCrc = 0x8f7514c0u, .EncodedName = u"gdwd_txhvw_jhu_vwtbedurq6bjhu1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0xc5755727u, .FileCrc = 0x82cc6daau, .EncodedName = u"gdwd_vfulsw_sfbghvwur|hu1vfu"_w},
        {.NameCrc = 0x58c84328u, .FileCrc = 0x63cde2c8u, .EncodedName = u"gdwd_vfulsw_sfbsod4;1vfu"_w}, {}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0x9c78df30u, .FileCrc = 0xe41ca7ddu, .EncodedName = u"gdwd_depds_pdsb51rsw"_w}, {}, {},
        {.NameCrc = 0xdf817333u, .FileCrc = 0x06639a8fu, .EncodedName = u"gdwd_depds_dpxohw1rsw"_w},
        {.NameCrc = 0x65c6bb33u, .FileCrc = 0x028814bfu, .EncodedName = u"gdwd_txhvw_vsd_urerwvbvsd1tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {}, {}, {}, {},
        {}, {}, {},
        {.NameCrc = 0xc9a0fb43u, .FileCrc = 0x230269a2u, .EncodedName = u"gdwd_depds_eurqg5wkhuhyhqjhy441pds"_w}, {}, {},
        {}, {.NameCrc = 0xc36d0f47u, .FileCrc = 0xeae1e7f4u, .EncodedName = u"gdwd_vfulsw_sfbsod3;1vfu"_w}, {}, {},
        {.NameCrc = 0xdcda834au, .FileCrc = 0x1347cbacu, .EncodedName = u"gdwd_txhvw_jhu_mxpshubjhu1tpp"_w}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0x5807f751u, .FileCrc = 0x60d9ee56u, .EncodedName = u"gdwd_txhvw_hqj_{hqrorjbhqj1tpp"_w},
        {}, {}, {}, {}, {},
        {.NameCrc = 0xe8c4e757u, .FileCrc = 0xd79de9bbu, .EncodedName = u"gdwd_txhvw_jhu_udoo|bjhu1tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {.NameCrc = 0xddc9e75fu, .FileCrc = 0xb1e8c689u, .EncodedName = u"oleyruelviloh1goo"_w}, {},
        {.NameCrc = 0xc9a50b61u, .FileCrc = 0xa54694fau, .EncodedName = u"gdwd_txhvw_uxv_edggd|1tpp"_w}, {}, {}, {}, {},
        {}, {}, {.NameCrc = 0x093b6368u, .FileCrc = 0xc94aefbcu, .EncodedName = u"gdwd_txhvw_jhu_ohrqdugrbjhu1tpp"_w},
        {}, {}, {}, {}, {}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0x072b7b74u, .FileCrc = 0x9cda6630u, .EncodedName = u"gdwd_txhvw_uxv_pd}h1tpp"_w}, {},
        {}, {.NameCrc = 0x9166af77u, .FileCrc = 0x45077906u, .EncodedName = u"gdwd_depds_pdsb71rsw"_w},
        {.NameCrc = 0x1f006f77u, .FileCrc = 0x17f145fdu, .EncodedName = u"gdwd_txhvw_uxv_ghswk1tpp"_w}, {},
        {.NameCrc = 0x03acff7au, .FileCrc = 0xfe90485cu, .EncodedName = u"gdwd_txhvw_jhu_eru}xnkdqbjhu1tpp"_w},
        {.NameCrc = 0x124b077au, .FileCrc = 0x806f5e34u, .EncodedName = u"gdwd_txhvw_jhu_vleroxvrywbjhu1tpp"_w}, {}, {},
        {.NameCrc = 0x59f53b7eu, .FileCrc = 0xc4d2e00eu, .EncodedName = u"gdwd_txhvw_hqj_frorql}dwlrqbhqj1tpp"_w}, {},
        {.NameCrc = 0x6050cb80u, .FileCrc = 0x2ecd8611u, .EncodedName = u"gdwd_txhvw_uxv_jodyuhg1tpp"_w}, {},
        {.NameCrc = 0x5067fb82u, .FileCrc = 0x504ede55u, .EncodedName = u"gdwd_depds_pdsb71pds"_w}, {}, {},
        {.NameCrc = 0x49faa785u, .FileCrc = 0xfa7495f2u, .EncodedName = u"gdwd_txhvw_uxv_hylghqfh1tpp"_w}, {}, {},
        {.NameCrc = 0xc68e8f88u, .FileCrc = 0x1eb0c429u, .EncodedName = u"}ole1goo"_w},
        {.NameCrc = 0x8a17af88u, .FileCrc = 0x518832b6u, .EncodedName = u"gdwd_txhvw_hqj_vsdfholqhvbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x9577eb8cu, .FileCrc = 0x126cfcf8u, .EncodedName = u"gdwd_txhvw_uxv_iloldo1tpp"_w},
        {.NameCrc = 0x1a1af78du, .FileCrc = 0x1eaa8fc9u, .EncodedName = u"gdwd_txhvw_hqj_wd{lvwbhqj1tpp"_w},
        {.NameCrc = 0xd44f9f8eu, .FileCrc = 0x9eefe202u, .EncodedName = u"gdwd_vfulsw_sfbsod351vfu"_w},
        {.NameCrc = 0xa6d5a38fu, .FileCrc = 0x2bbc5e4du, .EncodedName = u"gdwd_vfulsw_pvbse1vfu"_w}, {}, {}, {}, {}, {},
        {}, {.NameCrc = 0x166f5396u, .FileCrc = 0x5d0ff6b0u, .EncodedName = u"gdwd_vfulsw_sfbsduw81vfu"_w}, {}, {},
        {.NameCrc = 0x04825799u, .FileCrc = 0xe2082079u, .EncodedName = u"gdwd_txhvw_uxv_sludwhfodqsulvrq1tpp"_w},
        {.NameCrc = 0x7d11339au, .FileCrc = 0x315a71f2u, .EncodedName = u"gdwd_txhvw_vsd_sod|hubvsd1tpp"_w},
        {.NameCrc = 0x4601739bu, .FileCrc = 0xd8adf2e0u, .EncodedName = u"gdwd_vfulsw_sfbsod491vfu"_w}, {},
        {.NameCrc = 0xa8da2f9du, .FileCrc = 0xc790a1a0u, .EncodedName = u"gdwd_txhvw_hqj_skdudrqbhqj1tpp"_w}, {}, {}, {},
        {}, {.NameCrc = 0x0a306fa2u, .FileCrc = 0x798ef5b6u, .EncodedName = u"gdwd_txhvw_uxv_sursurorj1tpp"_w}, {},
        {.NameCrc = 0x5a4b77a4u, .FileCrc = 0x393aa51fu, .EncodedName = u"gdwd_txhvw_jhu_ilvklqjfxsbjhu1tpp"_w},
        {.NameCrc = 0x345073a5u, .FileCrc = 0x979bafa0u, .EncodedName = u"gdwd_txhvw_vsd_hghozhlvvbvsd1tpp"_w},
        {.NameCrc = 0x42f4a3a6u, .FileCrc = 0x7774bff4u, .EncodedName = u"gdwd_vfulsw_sfbsod471vfu"_w},
        {.NameCrc = 0x52bd07a6u, .FileCrc = 0xeb0b57f9u, .EncodedName = u"gdwd_txhvw_hqj_orjlfbhqj1tpp"_w},
        {.NameCrc = 0xc38fe7a5u, .FileCrc = 0xc33088ffu, .EncodedName = u"gdwd_txhvw_uxv_vwtbedurq51tpp"_w}, {},
        {.NameCrc = 0x095a4baau, .FileCrc = 0x0665c841u, .EncodedName = u"gdwd_txhvw_uxv_frgher{1tpp"_w},
        {.NameCrc = 0x129a83abu, .FileCrc = 0xd2a23c21u, .EncodedName = u"gdwd_vfulsw_sfbsduw:1vfu"_w}, {}, {}, {},
        {.NameCrc = 0xb77ae7afu, .FileCrc = 0x9e80035cu, .EncodedName = u"gdwd_txhvw_hqj_nlgqdsshgbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x5f197bb2u, .FileCrc = 0x6747a1f6u, .EncodedName = u"gdwd_txhvw_uxv_iduxn1tpp"_w},
        {.NameCrc = 0xe84343b2u, .FileCrc = 0x0e8615d4u, .EncodedName = u"gdwd_txhvw_hqj_grrplqrbhqj1tpp"_w}, {}, {},
        {.NameCrc = 0x08a1afb6u, .FileCrc = 0x19212459u, .EncodedName = u"gdwd_depds_eurqg5wkhuhyhqjhy441rsw"_w},
        {.NameCrc = 0xc80107b7u, .FileCrc = 0xc099d269u, .EncodedName = u"gdwd_txhvw_uxv_ihlsv|fkr1tpp"_w}, {}, {}, {},
        {.NameCrc = 0xeb6cdfbbu, .FileCrc = 0x1d795f8au, .EncodedName = u"gdwd_txhvw_hqj_flwdghovbhqj1tpp"_w},
        {.NameCrc = 0xe7d147bbu, .FileCrc = 0x7fae8555u, .EncodedName = u"gdwd_txhvw_uxv_glyhu1tpp"_w}, {}, {}, {},
        {.NameCrc = 0x1f55a7c0u, .FileCrc = 0xa901f0d5u, .EncodedName = u"gdwd_txhvw_vsd_vydurnrnbvsd1tpp"_w}, {}, {},
        {}, {}, {.NameCrc = 0x5d798bc5u, .FileCrc = 0x16691b23u, .EncodedName = u"gdwd_depds_pdsb51pds"_w},
        {.NameCrc = 0x1e8027c6u, .FileCrc = 0x2e845170u, .EncodedName = u"gdwd_depds_dpxohw1pds"_w}, {},
        {.NameCrc = 0xae54b3c8u, .FileCrc = 0x5954dfb8u, .EncodedName = u"gdwd_txhvw_hqj_vwtbedurq4bhqj1tpp"_w},
        {.NameCrc = 0xd951efc9u, .FileCrc = 0x4646e623u, .EncodedName = u"gdwd_vfulsw_sfbsod371vfu"_w},
        {.NameCrc = 0x6ace53cau, .FileCrc = 0xdbeb1e0eu, .EncodedName = u"gdwd_txhvw_uxv_nlgqdsshg1tpp"_w}, {}, {}, {},
        {}, {}, {}, {.NameCrc = 0x1b7123d1u, .FileCrc = 0xf3ee6ae7u, .EncodedName = u"gdwd_vfulsw_sfbsduw61vfu"_w}, {},
        {}, {}, {}, {}, {}, {}, {},
        {.NameCrc = 0xbb1823dau, .FileCrc = 0x9cab9713u, .EncodedName = u"gdwd_txhvw_hqj_frpsoh{bhqj1tpp"_w}, {},
        {.NameCrc = 0x4b1f03dcu, .FileCrc = 0x07ef48b5u, .EncodedName = u"gdwd_vfulsw_sfbsod431vfu"_w},
        {.NameCrc = 0xc33debdcu, .FileCrc = 0xad28d94au, .EncodedName = u"gdwd_txhvw_uxv_{hqrsdun1tpp"_w}, {},
        {.NameCrc = 0x532353dfu, .FileCrc = 0x4d5446c2u, .EncodedName = u"gdwd_txhvw_hqj_grprfodqbhqj1tpp"_w}, {},
        {.NameCrc = 0x4fead3e1u, .FileCrc = 0x121c260au, .EncodedName = u"gdwd_vfulsw_sfbsod451vfu"_w},
        {.NameCrc = 0x2aee43e1u, .FileCrc = 0x0cd0f997u, .EncodedName = u"gdwd_txhvw_vsd_irqfhuvbvsd1tpp"_w},
        {.NameCrc = 0xce9197e2u, .FileCrc = 0xceea6befu, .EncodedName = u"gdwd_txhvw_uxv_vwtbedurq71tpp"_w}, {}, {}, {},
        {}, {}, {}, {}, {},
        {.NameCrc = 0x1f84f3ecu, .FileCrc = 0x60b2d128u, .EncodedName = u"gdwd_vfulsw_sfbsduw41vfu"_w},
        {.NameCrc = 0x28f62becu, .FileCrc = 0x30db11c5u, .EncodedName = u"gdwd_txhvw_uxv_mxpshu1tpp"_w},
        {.NameCrc = 0x73dd0becu, .FileCrc = 0x578630b1u, .EncodedName = u"gdwd_txhvw_hqj_mxpshubhqj1tpp"_w},
        {.NameCrc = 0xe4b797eeu, .FileCrc = 0xcaa7d4bbu, .EncodedName = u"gdwd_txhvw_jhu_ghswkbjhu1tpp"_w}, {},
        {.NameCrc = 0x307ae3f1u, .FileCrc = 0x3f3b7d43u, .EncodedName = u"gdwd_txhvw_uxv_dpqhvld1tpp"_w},
        {.NameCrc = 0x263033f2u, .FileCrc = 0x1cab4526u, .EncodedName = u"gdwd_vfulsw_sfbihpbudqjhuv1vfu"_w}, {},
        {.NameCrc = 0xdda43ff4u, .FileCrc = 0xc20289fau, .EncodedName = u"gdwd_vfulsw_sfbsod391vfu"_w}, {}, {}, {},
        {.NameCrc = 0x65d11ff8u, .FileCrc = 0xbeca3724u, .EncodedName = u"gdwd_txhvw_uxv_udoo|1tpp"_w}, {}, {}, {}, {},
        {}, {}, {},
    }};

    const std::uint32_t ResourceDatSeedKey = 0xea8f3f37u;

    const std::uint32_t ResourceDatCrcKey1 = 0x7db6c99du;

    const std::uint32_t ResourceDatCrcKey2 = 0xc83fcbf3u;

    void VerifyResourceFileChecksum(const pas::WideString& FileName) {
        pas::WideString LowerName{};
        std::int32_t CharacterIndex{};
        std::int32_t Index{};
        char16_t Ch{};
        EC_Buf::TBufEC* Buf{};
        std::int32_t NameLength = FileName.length();
        LowerName.set_length(NameLength);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, NameLength - 1); cpp_range.next(Index); ) {
            Ch = FileName.read(Index + 1);
            if (Ch >= u'A' && Ch <= u'Z') {
                Ch = Ch - 'A' + 'a';
            }
            LowerName.write(Index + 1) = Ch;
        }
        std::uint32_t NameCrc = CrcUnit::ComputeCrc32(LowerName.pchar(), NameLength * static_cast<std::int32_t>(sizeof(char16_t)));
        std::int32_t TableSize = 1024;
        Index = TableSize - 1 & NameCrc;
        while (ResourceChecksums[Index].NameCrc != 0) {
            if (ResourceChecksums[Index].NameCrc == NameCrc && ResourceChecksums[Index].EncodedName.length() == NameLength) {
                CharacterIndex = 0;
                while (CharacterIndex < NameLength) {
                    if (ResourceChecksums[Index].EncodedName.read(CharacterIndex + 1) != static_cast<char16_t>(LowerName.read(CharacterIndex + 1) + 3)) {
                        break;
                    }
                    ++CharacterIndex;
                }
                if (CharacterIndex >= NameLength) {
                    Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                    Buf->LoadFromWideFilePath(FileName.pchar());
                    if (Buf->ComputeCrc32() != ResourceChecksums[Index].FileCrc) {
                        GR_Main::CCInterface->SetResourceChecksumFailed(true);
                    }
                    pas::free(Buf);
                    break;
                }
            }
            ++Index;
            if (Index >= TableSize) {
                Index = 0;
            }
        }
    }

    void TDataFileEC_Create(TDataFileEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->FileRef = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
    }

    void TDataFileEC_Destroy(TDataFileEC* Self) {
        TDataFileEC::Clear();
        pas::free(Self->FileRef);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TDataFileEC::Clear() {
    }

    void TDataElEC_Create(TDataElEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TDataElEC_Destroy(TDataElEC* Self) {
        Self->ClearChildData();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TDataElEC::ClearChildData() {
        if (ChildData != nullptr) {
            pas::free(ChildData);
            ChildData = nullptr;
        }
    }

    void TDataEC_Create(TDataEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->FileLock = pas::make_critical_section<pas::CriticalSection>();
        Self->InternedFileListHeadRef = &Self->OwnedInternedFileListHead;
        Self->InternedFileListTailRef = &Self->OwnedInternedFileListTail;
    }

    void TDataEC_Destroy(TDataEC* Self) {
        Self->Clear();
        pas::free(Self->FileLock);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    std::uint8_t TDataEC::IsEmpty() {
        return FirstEntry == nullptr;
    }

    void TDataEC::Clear() {
        TDataFileEC* FileEntry{};
        TDataFileEC* RemovedFile{};
        TDataElEC* RemovedEntry{};
        if (!SharesInternedFileList) {
            FileEntry = pas::load_unaligned<TDataFileEC*>(InternedFileListHeadRef);
            while (FileEntry != nullptr) {
                RemovedFile = FileEntry;
                FileEntry = FileEntry->Next;
                pas::free(RemovedFile);
            }
        }
        TDataElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            RemovedEntry = Entry;
            Entry = Entry->Next;
            pas::free(RemovedEntry);
        }
        SharesInternedFileList = false;
        InternedFileListHeadRef = &OwnedInternedFileListHead;
        InternedFileListTailRef = &OwnedInternedFileListTail;
        IndexedEntries = nullptr;
        IndexedEntryCount = 0;
    }

    TDataElEC* TDataEC::AddEntry(TDataEntryKind EntryKind) {
        TDataElEC* Entry = pas::construct_call<TDataElEC>(TDataElEC_Create);
        if (LastEntry != nullptr) {
            LastEntry->Next = Entry;
        }
        Entry->Prev = LastEntry;
        Entry->Next = nullptr;
        LastEntry = Entry;
        if (FirstEntry == nullptr) {
            FirstEntry = Entry;
        }
        Entry->Kind = EntryKind;
        if (EntryKind != dekFile) {
            Entry->ChildData = pas::construct_call<TDataEC>(TDataEC_Create);
            Entry->ChildData->SharesInternedFileList = true;
            Entry->ChildData->InternedFileListHeadRef = InternedFileListHeadRef;
            Entry->ChildData->InternedFileListTailRef = InternedFileListTailRef;
        }
        return Entry;
    }

    TDataElEC* TDataEC::FindIndexedEntry(const pas::WideString& Name) {
        std::int32_t Mid{};
        std::int32_t Order{};
        TDataElEC* Entry{};
        if (IndexedEntryCount < 1) {
            return nullptr;
        }
        std::int32_t Lo = 0;
        std::int32_t Hi = IndexedEntryCount - 1;
        do {
            Mid = (Hi - Lo) / 2 + Lo;
            Entry = IndexedEntries[Mid];
            Order = EC_Str::CompareWideChars(Name.pchar(), Entry->Name.pchar());
            if (Order == 0) {
                return Entry;
            }
            if (Order < 0) {
                Hi = Mid - 1;
            } else {
                Lo = Mid + 1;
            }
        } while (!(Hi < Lo));
        return nullptr;
    }

    std::int32_t TDataEC::FindInsertionIndex(TDataElEC* Entry) {
        std::int32_t Mid{};
        std::int32_t Order{};
        TDataElEC* Existing{};
        if (IndexedEntryCount <= 0) {
            return 0;
        }
        std::int32_t Lo = 0;
        std::int32_t Hi = IndexedEntryCount - 1;
        do {
            Mid = pas::shr(Hi - Lo, 1) + Lo;
            Existing = IndexedEntries[Mid];
            Order = EC_Str::CompareWideChars(Entry->Name.pchar(), Existing->Name.pchar());
            if (Order == 0) {
                return Mid;
            }
            if (Order < 0) {
                Hi = Mid - 1;
            } else {
                Lo = Mid + 1;
            }
        } while (!(Hi < Lo));
        if (Order < 0) {
            return Mid;
        }
        return Mid + 1;
    }

    void TDataEC::InsertIntoIndex(TDataElEC* Entry) {
        std::int32_t Index{};
        IndexedEntries.set_length(IndexedEntryCount + 1);
        Index = FindInsertionIndex(Entry);
        if (Index >= IndexedEntryCount) {
            IndexedEntries[IndexedEntryCount] = Entry;
            ++IndexedEntryCount;
        } else {
            Windows::MoveMemory(&IndexedEntries[Index + 1], &IndexedEntries[Index], (IndexedEntryCount - Index) * static_cast<std::int32_t>(sizeof(TDataElEC*)));
            IndexedEntries[Index] = Entry;
            ++IndexedEntryCount;
        }
    }

    void TDataEC::RebuildIndex() {
        IndexedEntries.set_length(0);
        IndexedEntryCount = 0;
        TDataElEC* Entry = FirstEntry;
        while (Entry != nullptr) {
            InsertIntoIndex(Entry);
            Entry = Entry->Next;
        }
    }

    TDataFileEC* TDataEC::InternFileName(const pas::WideString& FileName) {
        TDataFileEC* Entry = pas::load_unaligned<TDataFileEC*>(InternedFileListHeadRef);
        while (Entry != nullptr) {
            if (Entry->FileRef->GetFileName() == FileName) {
                return Entry;
            }
            Entry = Entry->Next;
        }
        Entry = pas::construct_call<TDataFileEC>(TDataFileEC_Create);
        if (pas::load_unaligned<TDataFileEC*>(InternedFileListTailRef) != nullptr) {
            pas::load_unaligned<TDataFileEC*>(InternedFileListTailRef)->Next = Entry;
        }
        Entry->Prev = pas::load_unaligned<TDataFileEC*>(InternedFileListTailRef);
        Entry->Next = nullptr;
        pas::store_unaligned<TDataFileEC*>(InternedFileListTailRef, Entry);
        if (pas::load_unaligned<TDataFileEC*>(InternedFileListHeadRef) == nullptr) {
            pas::store_unaligned<TDataFileEC*>(InternedFileListHeadRef, Entry);
        }
        Entry->FileRef->SetFileName(FileName);
        return Entry;
    }

    TDataElEC* TDataEC::FindEntry(const pas::WideString& Name) {
        return FindIndexedEntry(Name);
    }

    TDataElEC* TDataEC::FindEntryByPath(const pas::WideString& Path) {
        std::int32_t PartStart{};
        std::int32_t PartLength{};
        TDataElEC* Entry{};
        std::int32_t PathLength = Path.length();
        std::int32_t Position = 0;
        TDataEC* Data = this;
        while (EC_Data::NextDataPathComponent(Path, Position, PathLength, PartStart, PartLength)) {
            Entry = Data->FindEntry(pas::copy(Path, PartStart + 1, PartLength));
            if (Entry == nullptr) {
                break;
            }
            if (Position >= PathLength) {
                return Entry;
            }
            if (Entry->Kind != dekSubtree) {
                break;
            }
            Data = Entry->ChildData;
        }
        return nullptr;
    }

    std::uint8_t NextDataPathComponent(const pas::WideString& Path, std::int32_t& Position, std::int32_t& PathLength, std::int32_t& PartStart, std::int32_t& PartLength) {
        char16_t Ch{};
        if (Position >= PathLength) {
            return false;
        }
        PartStart = Position;
        std::int32_t i = PartStart;
        while (PathLength > i) {
            Ch = Path.read(i + 1);
            if (Ch == u'.' || Ch == u'/' || Ch == u'\\') {
                break;
            }
            ++i;
        }
        PartLength = i - PartStart;
        Position = i + 1;
        return true;
    }

    void TDataEC::ReadEntryBuffer(TDataElEC* Entry, EC_Buf::TBufEC* Dest) {
        std::int32_t Size{};
        EC_Data::VerifyResourceFileChecksum(Entry->SharedFileRef->FileRef->FileName);
        pas::critical_enter(FileLock);
        Entry->SharedFileRef->FileRef->AcquireReadWriteHandle();
        {
            try {
                if (Entry->FileOffset != 0) {
                    Entry->SharedFileRef->FileRef->SetPointer(Entry->FileOffset, WindowsImports::FILE_BEGIN);
                }
                Size = Entry->ByteCount;
                if (Size < 0) {
                    std::uint32_t cpp_left = Entry->SharedFileRef->FileRef->GetSize();
                    Size = cpp_left - Entry->FileOffset;
                }
                Dest->LoadFromFileChunk(Entry->SharedFileRef->FileRef, Size);
            } catch (...) {
                Entry->SharedFileRef->FileRef->ReleaseHandle();
                pas::critical_leave(FileLock);
                throw;
            }
            Entry->SharedFileRef->FileRef->ReleaseHandle();
            pas::critical_leave(FileLock);
        }
    }

    TDataEC* TDataEC::GetData(const pas::WideString& Name) {
        TDataElEC* Entry = FindEntry(Name);
        if (Entry == nullptr || Entry->Kind != dekSubtree) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TDataEC.GetData. name=", Name}))));
        }
        return Entry->ChildData;
    }

    void TDataEC::ReadBufferByPath(const pas::WideString& Path, EC_Buf::TBufEC* Dest) {
        TDataElEC* Entry = FindEntryByPath(Path);
        if (Entry == nullptr || Entry->Kind != dekFile) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TDataEC.PathGetBuf. path=", Path}))));
        }
        ReadEntryBuffer(Entry, Dest);
    }

    std::uint8_t TDataEC::FileExistsByPath(const pas::WideString& Path) {
        TDataElEC* Entry = FindEntryByPath(Path);
        if (Entry == nullptr || Entry->Kind != dekFile) {
            return false;
        }
        if (Entry->SharedFileRef != nullptr && Entry->SharedFileRef->FileRef != nullptr && Entry->SharedFileRef->FileRef->TryAcquireReadHandle(false)) {
            Entry->SharedFileRef->FileRef->ReleaseHandle();
        } else {
            return false;
        }
        return true;
    }

    void TDataEC::AddMissingFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t i{};
        EC_BlockPar::TBlockParKind Kind{};
        TDataElEC* Entry{};
        std::int32_t Count = Block->GetEntryCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Kind = Block->GetEntryKindByIndex(i);
            if (Kind == EC_BlockPar::bpkString || Kind == EC_BlockPar::bpkBlock) {
                if (FindEntry(Block->GetEntryNameByIndex(i)) != nullptr) {
                    continue;
                }
            }
            if (Kind == EC_BlockPar::bpkString) {
                Entry = AddEntry(dekFile);
                Entry->Name = Block->GetEntryNameByIndex(i);
                Entry->SharedFileRef = InternFileName(Block->GetEntryStringByIndex(i));
                Entry->FileOffset = 0u;
                Entry->ByteCount = -1;
                InsertIntoIndex(Entry);
            } else if (Kind == EC_BlockPar::bpkBlock) {
                Entry = AddEntry(dekSubtree);
                Entry->Name = Block->GetEntryNameByIndex(i);
                InsertIntoIndex(Entry);
                Entry->ChildData->AddMissingFromBlock(Block->GetEntryBlockByIndex(i));
            }
        }
    }

    void TDataEC::WriteToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t i{};
        TDataElEC* Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, IndexedEntryCount - 1); cpp_range.next(i); ) {
            Entry = IndexedEntries[i];
            if (Entry->Kind == dekFile) {
                Block->AddParam(Entry->Name, Entry->SharedFileRef->FileRef->GetFileName());
            } else {
                EC_BlockPar::TBlockParEC* addChildBlock = Block->AddChildBlock(Entry->Name);
                TDataEC* childData = Entry->ChildData;
                childData->WriteToBlock(addChildBlock);
            }
        }
    }

    void TDataEC::MergeFrom(TDataEC* Source) {
        TDataElEC* Entry{};
        TDataElEC* Existing{};
        Entry = Source->FirstEntry;
        while (Entry != nullptr) {
            Existing = FindIndexedEntry(Entry->Name);
            if (Existing == nullptr) {
                Existing = AddEntry(Entry->Kind);
                Existing->Name = Entry->Name;
            } else if (Entry->Kind != Existing->Kind) {
                Entry = Entry->Next;
                continue;
            }
            if (static_cast<std::int32_t>(Entry->Kind) != 0) {
                if (Entry->Kind == dekFile) {
                    Existing->FileOffset = 0u;
                    Existing->ByteCount = -1;
                    Existing->SharedFileRef = InternFileName(Entry->SharedFileRef->FileRef->FileName);
                } else if (Entry->Kind == dekSubtree) {
                    Existing->ChildData->MergeFrom(Entry->ChildData);
                }
            }
            Entry = Entry->Next;
        }
        RebuildIndex();
    }

    void TDataEC::LoadFromDecodedBuffer(EC_Buf::TBufEC* Buf) {
        TDataElEC* Entry{};
        std::int32_t i{};
        TDataEntryKind Kind{};
        Clear();
        IndexedEntryCount = EC_Buf::TBufEC_GetInt32(Buf);
        IndexedEntries.set_length(IndexedEntryCount);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, IndexedEntryCount - 1); cpp_range.next(i); ) {
            Kind = static_cast<TDataEntryKind>(EC_Buf::TBufEC_GetByte(Buf));
            Entry = AddEntry(Kind);
            Entry->Name = Buf->ReadWideString();
            Entry->FileOffset = 0u;
            Entry->ByteCount = -1;
            if (Kind == dekFile) {
                Entry->SharedFileRef = InternFileName(Buf->ReadWideString());
            } else {
                Entry->ChildData->LoadFromDecodedBuffer(Buf);
            }
            IndexedEntries[i] = Entry;
        }
    }

    void TDataEC::LoadFromEncryptedDatFile(const pas::WideString& FileName) {
        EC_Buf::TBufEC* Buf{};
        std::uint32_t Crc{};
        std::int32_t Seed{};
        std::int32_t ByteCount{};
        std::uint32_t ExpectedOuter{};
        EC_File::TFileEC* FileObj = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileObj->SetFileName(static_cast<pas::WideString>(FileName.pchar()));
        FileObj->AcquireReadHandle(false);
        std::uint32_t Position = FileObj->GetPointer();
        FileObj->ReadBuffer(&ByteCount, static_cast<std::int32_t>(sizeof(std::int32_t)));
        FileObj->ReadBuffer(&ExpectedOuter, static_cast<std::int32_t>(sizeof(std::uint32_t)));
        ByteCount ^= ResourceDatCrcKey1 ^ ResourceDatCrcKey2;
        {
            std::uint32_t cpp_left_2 = FileObj->GetSize();
            std::uint32_t cpp_left = cpp_left_2 - FileObj->GetPointer();
            if (cpp_left == static_cast<std::uint32_t>(ByteCount)) {
                Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Buf->SetSize(ByteCount + static_cast<std::int32_t>(sizeof(std::uint32_t)));
                Position = FileObj->GetPointer();
                FileObj->ReadBuffer(static_cast<std::uint8_t*>(Buf->Data) + static_cast<std::int32_t>(sizeof(std::uint32_t)), Buf->DataSize - static_cast<std::int32_t>(sizeof(std::uint32_t)));
                Crc = Buf->ComputeCrc32Range(static_cast<std::int32_t>(sizeof(std::uint32_t)), Buf->DataSize) ^ ResourceDatCrcKey1;
                pas::store_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Buf->Data), Crc);
                Crc = Buf->ComputeCrc32() ^ ResourceDatCrcKey2;
                pas::free(Buf);
                if (Crc != ExpectedOuter) {
                    GR_Main::CCInterface->SetResourceChecksumFailed(true);
                }
            } else {
                GR_Main::CCInterface->SetResourceChecksumFailed(true);
            }
        }
        FileObj->SetPointer(Position, WindowsImports::FILE_BEGIN);
        {
            std::uint32_t cpp_left_3 = FileObj->GetSize();
            ByteCount = cpp_left_3 - FileObj->GetPointer();
        }
        FileObj->ReadBuffer(&Crc, static_cast<std::int32_t>(sizeof(std::uint32_t)));
        FileObj->ReadBuffer(&Seed, static_cast<std::int32_t>(sizeof(std::int32_t)));
        Seed ^= ResourceDatSeedKey;
        Buf = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buf->SetSize(ByteCount - static_cast<std::int32_t>(sizeof(std::uint32_t)) - static_cast<std::int32_t>(sizeof(std::int32_t)));
        FileObj->ReadBuffer(Buf->Data, Buf->DataSize);
        Buf->ApplyDatXorCipher(Seed);
        if (Buf->ComputeCrc32() == Crc) {
            Buf->ExpandZlibPayloadInPlace();
            Buf->SetPosition(0);
            LoadFromDecodedBuffer(Buf);
        }
        pas::free(Buf);
        pas::free(FileObj);
    }

    void TDataEC::p_destroy() {
        EC_Data::TDataEC_Destroy(this);
    }

    void TDataFileEC::p_destroy() {
        EC_Data::TDataFileEC_Destroy(this);
    }

    void TDataElEC::p_destroy() {
        EC_Data::TDataElEC_Destroy(this);
    }

} // namespace EC_Data
