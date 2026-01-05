#include "SurfaceInfos_c.h"
#include <unordered_map>
#include <string>

uint32 SurfaceInfos_c::GetSurfaceIdFromName(SurfaceInfos_c* thiz, const char* surfaceName) {
    static const std::unordered_map<std::string, uint32> surfaceMap = {
            {"DEFAULT", 0}, {"TARMAC", 1}, {"TARMAC_FUCKED", 2}, {"TARMAC_REALLYFUCKED", 3},
            {"PAVEMENT", 4}, {"PAVEMENT_FUCKED", 5}, {"GRAVEL", 6}, {"FUCKED_CONCRETE", 7},
            {"PAINTED_GROUND", 8}, {"GRASS_SHORT_LUSH", 9}, {"GRASS_MEDIUM_LUSH", 10}, {"GRASS_LONG_LUSH", 11},
            {"GRASS_SHORT_DRY", 12}, {"GRASS_MEDIUM_DRY", 13}, {"GRASS_LONG_DRY", 14},
            {"GOLFGRASS_ROUGH", 15}, {"GOLFGRASS_SMOOTH", 16}, {"STEEP_SLIDYGRASS", 17}, {"STEEP_CLIFF", 18},
            {"FLOWERBED", 19}, {"MEADOW", 20}, {"WASTEGROUND", 21}, {"WOODLANDGROUND", 22}, {"VEGETATION", 23},
            {"MUD_WET", 24}, {"MUD_DRY", 25}, {"DIRT", 26}, {"DIRTTRACK", 27}, {"SAND_DEEP", 28},
            {"SAND_MEDIUM", 29}, {"SAND_COMPACT", 30}, {"SAND_ARID", 31}, {"SAND_MORE", 32}, {"SAND_BEACH", 33},
            {"CONCRETE_BEACH", 34}, {"ROCK_DRY", 35}, {"ROCK_WET", 36}, {"ROCK_CLIFF", 37}, {"WATER_RIVERBED", 38},
            {"WATER_SHALLOW", 39}, {"CORNFIELD", 40}, {"HEDGE", 41}, {"WOOD_CRATES", 42}, {"WOOD_SOLID", 43},
            {"WOOD_THIN", 44}, {"GLASS", 45}, {"GLASS_WINDOWS_LARGE", 46}, {"GLASS_WINDOWS_SMALL", 47},
            {"EMPTY1", 48}, {"EMPTY2", 49}, {"GARAGE_DOOR", 50}, {"THICK_METAL_PLATE", 51}, {"SCAFFOLD_POLE", 52},
            {"LAMP_POST", 53}, {"METAL_GATE", 54}, {"METAL_CHAIN_FENCE", 55}, {"GIRDER", 56}, {"FIRE_HYDRANT", 57},
            {"CONTAINER", 58}, {"NEWS_VENDOR", 59}, {"WHEELBASE", 60}, {"CARDBOARDBOX", 61}, {"PED", 62},
            {"CAR", 63}, {"CAR_PANEL", 64}, {"CAR_MOVINGCOMPONENT", 65}, {"TRANSPARENT_CLOTH", 66}, {"RUBBER", 67},
            {"PLASTIC", 68}, {"TRANSPARENT_STONE", 69}, {"WOOD_BENCH", 70}, {"CARPET", 71}, {"FLOORBOARD", 72},
            {"STAIRSWOOD", 73}, {"P_SAND", 74}, {"P_SAND_DENSE", 75}, {"P_SAND_ARID", 76}, {"P_SAND_COMPACT", 77},
            {"P_SAND_ROCKY", 78}, {"P_SANDBEACH", 79}, {"P_GRASS_SHORT", 80}, {"P_GRASS_MEADOW", 81}, {"P_GRASS_DRY", 82},
            {"P_WOODLAND", 83}, {"P_WOODDENSE", 84}, {"P_ROADSIDE", 85}, {"P_ROADSIDEDES", 86}, {"P_FLOWERBED", 87},
            {"P_WASTEGROUND", 88}, {"P_CONCRETE", 89}, {"P_OFFICEDESK", 90}, {"P_711SHELF1", 91}, {"P_711SHELF2", 92},
            {"P_711SHELF3", 93}, {"P_RESTUARANTTABLE", 94}, {"P_BARTABLE", 95}, {"P_UNDERWATERLUSH", 96},
            {"P_UNDERWATERBARREN", 97}, {"P_UNDERWATERCORAL", 98}, {"P_UNDERWATERDEEP", 99}, {"P_RIVERBED", 100},
            {"P_RUBBLE", 101}, {"P_BEDROOMFLOOR", 102}, {"P_KIRCHENFLOOR", 103}, {"P_LIVINGRMFLOOR", 104},
            {"P_CORRIDORFLOOR", 105}, {"P_711FLOOR", 106}, {"P_FASTFOODFLOOR", 107}, {"P_SKANKYFLOOR", 108},
            {"P_MOUNTAIN", 109}, {"P_MARSH", 110}, {"P_BUSHY", 111}, {"P_BUSHYMIX", 112}, {"P_BUSHYDRY", 113},
            {"P_BUSHYMID", 114}, {"P_GRASSWEEFLOWERS", 115}, {"P_GRASSDRYTALL", 116}, {"P_GRASSLUSHTALL", 117},
            {"P_GRASSGRNMIX", 118}, {"P_GRASSBRNMIX", 119}, {"P_GRASSLOW", 120}, {"P_GRASSROCKY", 121},
            {"P_GRASSSMALLTREES", 122}, {"P_DIRTROCKY", 123}, {"P_DIRTWEEDS", 124}, {"P_GRASSWEEDS", 125},
            {"P_RIVEREDGE", 126}, {"P_POOLSIDE", 127}, {"P_FORESTSTUMPS", 128}, {"P_FORESTSTICKS", 129},
            {"P_FORRESTLEAVES", 130}, {"P_DESERTROCKS", 131}, {"P_FORRESTDRY", 132}, {"P_SPARSEFLOWERS", 133},
            {"P_BUILDINGSITE", 134}, {"P_DOCKLANDS", 135}, {"P_INDUSTRIAL", 136}, {"P_INDUSTJETTY", 137},
            {"P_CONCRETELITTER", 138}, {"P_ALLEYRUBISH", 139}, {"P_JUNKYARDPILES", 140}, {"P_JUNKYARDGRND", 141},
            {"P_DUMP", 142}, {"P_CACTUSDENSE", 143}, {"P_AIRPORTGRND", 144}, {"P_CORNFIELD", 145}, {"P_GRASSLIGHT", 146},
            {"P_GRASSLIGHTER", 147}, {"P_GRASSLIGHTER2", 148}, {"P_GRASSMID1", 149}, {"P_GRASSMID2", 150},
            {"P_GRASSDARK", 151}, {"P_GRASSDARK2", 152}, {"P_GRASSDIRTMIX", 153}, {"P_RIVERBEDSTONE", 154},
            {"P_RIVERBEDSHALLOW", 155}, {"P_RIVERBEDWEEDS", 156}, {"P_SEAWEED", 157}, {"DOOR", 158},
            {"PLASTICBARRIER", 159}, {"PARKGRASS", 160}, {"STAIRSSTONE", 161}, {"STAIRSMETAL", 162},
            {"STAIRSCARPET", 163}, {"FLOORMETAL", 164}, {"FLOORCONCRETE", 165}, {"BIN_BAG", 166},
            {"THIN_METAL_SHEET", 167}, {"METAL_BARREL", 168}, {"PLASTIC_CONE", 169}, {"PLASTIC_DUMPSTER", 170},
            {"METAL_DUMPSTER", 171}, {"WOOD_PICKET_FENCE", 172}, {"WOOD_SLATTED_FENCE", 173}, {"WOOD_RANCH_FENCE", 174},
            {"UNBREAKABLE_GLASS", 175}, {"HAY_BALE", 176}, {"GORE", 177}, {"RAILTRACK", 178}
    };

    auto it = surfaceMap.find(surfaceName);
    return (it != surfaceMap.end()) ? it->second : 0;
}